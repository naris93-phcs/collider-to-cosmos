#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>
#include <utility>
#include <set>
#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"
#include "fastjet/ClusterSequence.hh"
#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"


// -----------------------------------------------------------------------------
// Generator-history helpers
// -----------------------------------------------------------------------------

bool isHardProcessW(const HepMC::GenParticle* particle)
{
    if (particle == nullptr)
        return false;

    if (std::abs(particle->pdg_id()) != 24)
        return false;

    const int status = std::abs(particle->status());

    // Pythia hard-process status range.
    return (status >= 21 && status <= 29);
}


bool hasHardWAncestor(const HepMC::GenParticle* particle)
{
    if (particle == nullptr)
        return false;

    const HepMC::GenVertex* vertex = particle->production_vertex();

    if (vertex == nullptr)
        return false;

    for (auto parent = vertex->particles_in_const_begin();
         parent != vertex->particles_in_const_end();
         ++parent) {

        if (isHardProcessW(*parent))
            return true;

        if (hasHardWAncestor(*parent))
            return true;
    }

    return false;
}


// Signal-specific ancestry audit. Follow W->W copies back to a hard W,
// and lepton->same-lepton copies back to a W decay. Do not traverse through
// hadrons, photons, taus or initial-state protons. Leave isHardProcessW intact.
bool isHardWCopyLine(const HepMC::GenParticle* p,
                     std::set<const HepMC::GenParticle*>& seen)
{
    if (p == nullptr || !seen.insert(p).second || std::abs(p->pdg_id()) != 24)
        return false;
    if (isHardProcessW(p)) return true;
    const auto* v = p->production_vertex();
    if (!v) return false;
    for (auto it = v->particles_in_const_begin();
         it != v->particles_in_const_end(); ++it) {
        if ((*it)->pdg_id() == p->pdg_id() && isHardWCopyLine(*it, seen))
            return true;
    }
    return false;
}

bool hasDirectHardWLeptonLineageImpl(
    const HepMC::GenParticle* p, int leptonPdgId,
    std::set<const HepMC::GenParticle*>& seenLeptons)
{
    if (!p || !seenLeptons.insert(p).second) return false;
    const auto* v = p->production_vertex();
    if (!v) return false;
    for (auto it = v->particles_in_const_begin();
         it != v->particles_in_const_end(); ++it) {
        const auto* parent = *it;
        if (!parent) continue;
        if (std::abs(parent->pdg_id()) == 24) {
            std::set<const HepMC::GenParticle*> seenW;
            if (isHardWCopyLine(parent, seenW)) return true;
        }
        if (parent->pdg_id() == leptonPdgId &&
            hasDirectHardWLeptonLineageImpl(parent, leptonPdgId, seenLeptons))
            return true;
    }
    return false;
}

bool hasDirectHardWLeptonLineage(const HepMC::GenParticle* p)
{
    if (!p) return false;
    const int id = p->pdg_id();
    if (std::abs(id) != 11 && std::abs(id) != 13) return false;
    std::set<const HepMC::GenParticle*> seenLeptons;
    return hasDirectHardWLeptonLineageImpl(p, id, seenLeptons);
}


// Diagnostic only: PDG hadron flavour digits. This is NOT yet the
// weakly-decaying b-hadron definition required for ATLAS ghost matching.
bool isBottomFlavouredHadronCandidate(int pdgId)
{
    const long long id = std::llabs(static_cast<long long>(pdgId));
    if (id < 100 || id >= 1000000000LL) return false;
    const int q1 = static_cast<int>((id / 1000) % 10);
    const int q2 = static_cast<int>((id / 100) % 10);
    const int q3 = static_cast<int>((id / 10) % 10);
    // Reject bottomonium (b bbar): not a weakly decaying open-bottom hadron.
    if (q1 == 0) return (q2 == 5) != (q3 == 5);
    return q1 == 5 || q2 == 5 || q3 == 5;
}

// Follow the decay chain: only the last open-bottom hadron is ghosted.
// This avoids double-counting excited B states and B* -> B transitions.
bool isLastBottomHadron(const HepMC::GenParticle* p)
{
    if (!isBottomFlavouredHadronCandidate(p->pdg_id())) return false;
    const HepMC::GenVertex* v = p->end_vertex();
    if (v == nullptr) return false;
    for (auto it = v->particles_out_const_begin();
         it != v->particles_out_const_end(); ++it) {
        if (isBottomFlavouredHadronCandidate((*it)->pdg_id()))
            return false;
    }
    return true;
}

// Validation-only: inspect the immediate parent(s) of a final-state photon.
// This is NOT a complete ATLAS prompt-photon classifier: a hadron can occur
// earlier in the decay chain, and beam protons must not be treated as decays.
bool hasImmediateHadronParent(const HepMC::GenParticle* p)
{
    if (!p || !p->production_vertex()) return false;
    const auto* v = p->production_vertex();
    for (auto it = v->particles_in_const_begin();
         it != v->particles_in_const_end(); ++it) {
        const auto* parent = *it;
        if (!parent) continue;
        const long long id = std::llabs(static_cast<long long>(parent->pdg_id()));
        if (id >= 100 && id < 1000000000LL && parent->production_vertex())
            return true;
    }
    return false;
}

bool containsBGhost(const fastjet::PseudoJet& jet)
{
    for (const auto& constituent : jet.constituents()) {
        if (constituent.user_index() == 500) return true;
    }
    return false;
}
// Signal-sample approximation to prompt photons: reject photons with a
// decaying hadron anywhere in their generator ancestry. Stop at the beam /
// hard-process boundary, so incoming protons are not called hadron decays.
bool hasDecayHadronAncestorImpl(const HepMC::GenParticle* p,
    std::set<const HepMC::GenParticle*>& visited)
{
    if (!p || !visited.insert(p).second) return false;
    const auto* vertex = p->production_vertex();
    if (!vertex) return false;
    for (auto it = vertex->particles_in_const_begin();
         it != vertex->particles_in_const_end(); ++it) {
        const auto* parent = *it;
        if (!parent) continue;
        const int status = std::abs(parent->status());
        if (status == 4 || (status >= 21 && status <= 29) ||
            !parent->production_vertex()) continue;
        const long long id = std::llabs(static_cast<long long>(parent->pdg_id()));
        if (id >= 100 && id < 1000000000LL && parent->end_vertex())
            return true;
        if (hasDecayHadronAncestorImpl(parent, visited)) return true;
    }
    return false;
}

bool hasDecayHadronAncestor(const HepMC::GenParticle* p)
{
    std::set<const HepMC::GenParticle*> visited;
    return hasDecayHadronAncestorImpl(p, visited);
}

// Evaluate the original dressing selection on the same event for a paired
// cutflow comparison. Jets are copied because overlap removal mutates them.
std::pair<bool,bool> oldDressingPasses(
    const std::vector<fastjet::PseudoJet>& oldLeptons,
    const std::vector<int>& oldIds,
    std::vector<fastjet::PseudoJet> jets,
    double met, bool thirdVeto)
{
    auto leptons = oldLeptons;
    auto ids = oldIds;
    // Reproduce the original baseline exactly: sort by dressed pT before
    // electron-jet overlap removal, keeping the PDG IDs aligned.
    std::vector<size_t> order(leptons.size());
    for (size_t i = 0; i < order.size(); ++i) order[i] = i;
    std::sort(order.begin(), order.end(), [&](size_t a, size_t b) {
        return leptons[a].pt() > leptons[b].pt();
    });
    std::vector<fastjet::PseudoJet> sortedLeptons;
    std::vector<int> sortedIds;
    for (size_t i : order) {
        sortedLeptons.push_back(leptons[i]);
        sortedIds.push_back(ids[i]);
    }
    leptons = std::move(sortedLeptons);
    ids = std::move(sortedIds);
    for (size_t i = 0; i < leptons.size();) {
        if (std::abs(ids[i]) != 11) { ++i; continue; }
        bool removed = false;
        while (true) {
            size_t closest = jets.size();
            double drmin = 0.4;
            for (size_t j = 0; j < jets.size(); ++j) {
                if (jets[j].pt() < 25.0) continue;
                double dr = leptons[i].delta_R(jets[j]);
                if (dr < drmin) { drmin = dr; closest = j; }
            }
            if (closest == jets.size()) break;
            if (leptons[i].pt() / jets[closest].pt() < 0.5) {
                leptons.erase(leptons.begin() + i);
                ids.erase(ids.begin() + i);
                removed = true;
                break;
            }
            jets.erase(jets.begin() + closest);
        }
        if (!removed) ++i;
    }
    const bool two = leptons.size() == 2;
    if (!two || ids[0]*ids[1] <= 0) return {two, false};
    const double mll = (leptons[0]+leptons[1]).m();
    if (mll <= 20.0 || !thirdVeto) return {two, false};
    if (std::abs(ids[0]) == 11 && std::abs(ids[1]) == 11 &&
        std::abs(mll-91.1876) < 15.0) return {two, false};
    if (met <= 30.0) return {two, false};
    for (const auto& j : jets)
        if (j.pt() > 20.0 && std::abs(j.eta()) < 2.5 && containsBGhost(j))
            return {two, false};
    std::vector<fastjet::PseudoJet> vbs;
    for (const auto& j : jets) if (j.pt() >= 25.0) vbs.push_back(j);
    if (vbs.size() < 2 || vbs[0].pt() < 65.0 || vbs[1].pt() < 35.0)
        return {two, false};
    if ((vbs[0]+vbs[1]).m() <= 500.0 ||
        std::abs(vbs[0].rap()-vbs[1].rap()) <= 2.0)
        return {two, false};
    return {two, true};
}


int main()
{
    // -------------------------------------------------------------------------
    // Input HepMC file
    // -------------------------------------------------------------------------

    HepMC::IO_GenEvent ascii_in(
        "../sm_wpwpjj_leptonic/Events/run_04/tag_1_pythia8_events.hepmc",
        std::ios::in
    );


    // -------------------------------------------------------------------------
    // FastJet definition
    // -------------------------------------------------------------------------

    const double R = 0.4;

    fastjet::JetDefinition jet_def(
        fastjet::antikt_algorithm,
        R
    );


    // -------------------------------------------------------------------------
    // ROOT histograms
    // -------------------------------------------------------------------------

    TH1D h_njets(
        "h_njets",
        "Jet multiplicity;N_{jets};Events",
        10, -0.5, 9.5
    );

    TH1D h_jet1_pt(
        "h_jet1_pt",
        "Leading jet p_{T};p_{T}^{j1} [GeV];Events",
        50, 0.0, 500.0
    );

    TH1D h_jet2_pt(
        "h_jet2_pt",
        "Subleading jet p_{T};p_{T}^{j2} [GeV];Events",
        50, 0.0, 500.0
    );

    TH1D h_jet1_eta(
        "h_jet1_eta",
        "Leading jet #eta;#eta_{j1};Events",
        50, -5.0, 5.0
    );

    TH1D h_jet2_eta(
        "h_jet2_eta",
        "Subleading jet #eta;#eta_{j2};Events",
        50, -5.0, 5.0
    );

    TH1D h_mjj(
        "h_mjj",
        "Dijet invariant mass;m_{jj} [GeV];Events",
        60, 0.0, 3000.0
    );

    TH1D h_deta_jj(
        "h_deta_jj",
        "Jet separation;|#Delta#eta_{jj}|;Events",
        50, 0.0, 10.0
    );

    TH1D h_dy_jj(
        "h_dy_jj",
        "Jet rapidity separation;|#Deltay_{jj}|;Events",
        50, 0.0, 10.0
    );

    TH1D h_lepton1_pt(
        "h_lepton1_pt",
        "Leading dressed lepton p_{T};p_{T}^{l1} [GeV];Events",
        50, 0.0, 250.0
    );

    TH1D h_lepton2_pt(
        "h_lepton2_pt",
        "Subleading dressed lepton p_{T};p_{T}^{l2} [GeV];Events",
        50, 0.0, 250.0
    );

    TH1D h_mll(
        "h_mll",
        "Dilepton invariant mass;m_{ll} [GeV];Events",
        60, 0.0, 600.0
    );

    TH1D h_met(
        "h_met",
        "Particle-level missing transverse momentum;E_{T}^{miss} [GeV];Events",
        50, 0.0, 300.0
    );

    TH1D h_cutflow(
        "h_cutflow",
        "SM same-sign WWjj cumulative cutflow;Selection stage;Events",
        12, 0.5, 12.5
    );


    // Selected-event ntuple for future fiducial differential comparisons.
    // One entry per event passing the full current ATLAS-inspired baseline.
    TTree fiducial_events("fiducial_events", "Selected W+W+jj particle-level observables");
    double tree_mjj = 0., tree_dy_jj = 0., tree_mll = 0.;
    double tree_lepton1_pt = 0., tree_lepton2_pt = 0., tree_met = 0.;
    double tree_jet1_pt = 0., tree_jet2_pt = 0.;
    fiducial_events.Branch("mjj", &tree_mjj);
    fiducial_events.Branch("dy_jj", &tree_dy_jj);
    fiducial_events.Branch("mll", &tree_mll);
    fiducial_events.Branch("lepton1_pt", &tree_lepton1_pt);
    fiducial_events.Branch("lepton2_pt", &tree_lepton2_pt);
    fiducial_events.Branch("met", &tree_met);
    fiducial_events.Branch("jet1_pt", &tree_jet1_pt);
    fiducial_events.Branch("jet2_pt", &tree_jet2_pt);

    // -------------------------------------------------------------------------
    // Counters
    // -------------------------------------------------------------------------

    int event_count = 0;
    long long photons_rejected_hadron_ancestry = 0;
    long long photons_assigned = 0;
    long long photons_with_multiple_candidates = 0;
    int old_two_count = 0, old_final_count = 0;
    int jets_pre_overlap_two = 0;
    int changed_two_events = 0, changed_final_events = 0;
    int old_only_final = 0, new_only_final = 0;

    // MET closure diagnostic only; never used in event selection.
    // All stable non-neutrino particles enter the visible momentum sum.
    double met_closure_sum = 0.0;
    double met_closure_max = 0.0;
    int met_closure_above_1mev = 0;
    double met_magnitude_difference_sum = 0.0;

    // Prompt-lepton multiplicity sanity check before fiducial cuts.
    long long old_hardW_ancestry_leptons = 0;
    long long new_directW_lineage_leptons = 0;
    long long rejected_broad_ancestry_leptons = 0;
    int events_with_prompt_definition_difference = 0;
    int events_with_0_prompt_leptons = 0;
    int events_with_1_prompt_lepton = 0;
    int events_with_2_prompt_leptons = 0;
    int events_with_more_prompt_leptons = 0;

    // Dressed-lepton / MET cutflow.
    int events_pass_two_leptons = 0;
    int events_pass_same_sign = 0;
    int events_pass_mll = 0;
    int events_pass_met = 0;
    int events_pass_third_lepton_veto = 0;
    int events_pass_z_veto = 0;
    int events_pass_b_veto = 0;
    int events_rejected_b_veto = 0;
    int events_with_tagged_bjets = 0;
    long long total_last_b_hadrons = 0;
    int events_rejected_z_veto = 0;
    int events_with_third_prompt_lepton = 0;

    // Jet/VBS cutflow, independent of the lepton cutflow.
    int events_with_two_jets = 0;
    int events_pass_jet_pt = 0;
    int events_pass_mjj = 0;
    int events_pass_dy = 0;

    // Unified cumulative cutflow: VBS cuts after lepton/MET selection.
    int full_pass_two_jets = 0;
    int full_pass_jet_pt = 0;
    int full_pass_mjj = 0;
    int full_pass_dy = 0;

    // Diagnostic event counts for electron-jet overlap removal.
    int events_overlap_electron_removed = 0;
    int events_overlap_jet_removed = 0;

    // Non-invasive b-hadron record diagnostic.
    int events_with_b_hadron_candidates = 0;
    long long total_b_hadron_candidates = 0;
    std::map<int, int> b_hadron_ids;
    std::map<int, int> b_hadron_statuses;

    // Non-invasive photon-dressing and bottom-hadron audits.
    long long stable_photons_total = 0;
    long long stable_photons_immediate_hadron_parent = 0;
    long long photons_in_bare_lepton_cones = 0;
    long long hadron_parent_photons_in_bare_cones = 0;
    long long photons_in_multiple_bare_cones = 0;
    int events_with_hadron_parent_photon_in_cone = 0;
    int events_with_shared_photon_in_cones = 0;
    long long ghosted_b_hadrons_status2 = 0;
    long long ghosted_b_hadrons_other_status = 0;
    long long ghosted_b_hadrons_without_end_vertex = 0;

    // Full cumulative baseline selection.
    int events_pass_full_baseline = 0;


    // -------------------------------------------------------------------------
    // Event loop
    // -------------------------------------------------------------------------

    HepMC::GenEvent* event = nullptr;

    while ((event = ascii_in.read_next_event())) {

        ++event_count;

        // Scan ALL HepMC particles, including unstable intermediates.
        bool found_b_candidate = false;
        for (auto it = event->particles_begin();
             it != event->particles_end(); ++it) {
            const HepMC::GenParticle* p = *it;
            if (!isBottomFlavouredHadronCandidate(p->pdg_id())) continue;
            found_b_candidate = true;
            ++total_b_hadron_candidates;
            ++b_hadron_ids[p->pdg_id()];
            ++b_hadron_statuses[p->status()];
        }
        if (found_b_candidate) ++events_with_b_hadron_candidates;

        std::vector<fastjet::PseudoJet> particles;
        // Ghosts have negligible momentum but retain their flavour label.
        for (auto it = event->particles_begin();
             it != event->particles_end(); ++it) {
            const HepMC::GenParticle* hadron = *it;
            if (!isLastBottomHadron(hadron)) continue;
            const HepMC::FourVector& m = hadron->momentum();
            if (m.e() <= 0.0 || std::hypot(m.px(), m.py()) == 0.0) continue;
            const double scale = 1e-20 / m.e();
            fastjet::PseudoJet ghost(m.px()*scale, m.py()*scale,
                                     m.pz()*scale, m.e()*scale);
            ghost.set_user_index(500);
            particles.push_back(ghost);
            ++total_last_b_hadrons;
            if (hadron->status() == 2) ++ghosted_b_hadrons_status2;
            else ++ghosted_b_hadrons_other_status;
            if (!hadron->end_vertex()) ++ghosted_b_hadrons_without_end_vertex;
        }

        std::vector<HepMC::GenParticle*> prompt_leptons;
        bool event_prompt_definition_difference = false;
        std::vector<HepMC::GenParticle*> final_state_photons;

        double met_px = 0.0;
        double met_py = 0.0;
        double visible_px = 0.0;
        double visible_py = 0.0;


        // ---------------------------------------------------------------------
        // Select stable final-state objects
        // ---------------------------------------------------------------------

        for (auto p = event->particles_begin();
             p != event->particles_end();
             ++p) {

            if ((*p)->status() != 1)
                continue;

            const int absId = std::abs((*p)->pdg_id());
            const HepMC::FourVector& mom = (*p)->momentum();

            // Stable e/mu descending from a hard-process W.
            // This is our current signal-sample prompt-lepton approximation.
            if (absId == 11 || absId == 13) {
                const bool broad = hasHardWAncestor(*p);
                const bool direct = hasDirectHardWLeptonLineage(*p);
                if (broad) ++old_hardW_ancestry_leptons;
                if (direct) {
                    ++new_directW_lineage_leptons;
                    prompt_leptons.push_back(*p);
                }
                if (broad != direct) {
                    event_prompt_definition_difference = true;
                    if (broad && !direct) ++rejected_broad_ancestry_leptons;
                }
            }

            // Baseline dressing uses stable final-state photons.
            // The exact ATLAS prompt-photon semantics will be refined later.
            if (absId == 22) {
                final_state_photons.push_back(*p);
                ++stable_photons_total;
                if (hasImmediateHadronParent(*p))
                    ++stable_photons_immediate_hadron_parent;
            }

            // Particle-level MET from stable neutrinos.
            if (absId == 12 || absId == 14 || absId == 16) {
                met_px += mom.px();
                met_py += mom.py();
            }

            // Independent visible-particle momentum sum for MET closure.
            // No acceptance cuts: this tests full-event momentum conservation,
            // not the ATLAS selected-object MET definition.
            if (absId != 12 && absId != 14 && absId != 16) {
                visible_px += mom.px();
                visible_py += mom.py();
            }

            // ATLAS-like particle-level jet inputs:
            // exclude muons and neutrinos; electrons and photons remain.
            if (absId == 12 || absId == 14 ||
                absId == 16 || absId == 13)
                continue;

            particles.emplace_back(
                mom.px(),
                mom.py(),
                mom.pz(),
                mom.e()
            );
        }


        // ---------------------------------------------------------------------
        if (event_prompt_definition_difference)
            ++events_with_prompt_definition_difference;

        // Prompt-lepton multiplicity sanity check
        // ---------------------------------------------------------------------

        if (prompt_leptons.size() == 0)
            ++events_with_0_prompt_leptons;
        else if (prompt_leptons.size() == 1)
            ++events_with_1_prompt_lepton;
        else if (prompt_leptons.size() == 2)
            ++events_with_2_prompt_leptons;
        else
            ++events_with_more_prompt_leptons;


        // Photon-cone audit using BARE lepton directions. No selection changes.
        bool event_hadron_parent_photon_in_cone = false;
        bool event_shared_photon_in_cones = false;
        for (const auto* photon : final_state_photons) {
            const auto& pm = photon->momentum();
            const fastjet::PseudoJet gamma(pm.px(), pm.py(), pm.pz(), pm.e());
            int matches = 0;
            for (const auto* lep : prompt_leptons) {
                const auto& lm = lep->momentum();
                const fastjet::PseudoJet bare(lm.px(), lm.py(), lm.pz(), lm.e());
                if (bare.delta_R(gamma) < 0.1) ++matches;
            }
            if (matches > 0) {
                ++photons_in_bare_lepton_cones;
                if (hasImmediateHadronParent(photon)) {
                    ++hadron_parent_photons_in_bare_cones;
                    event_hadron_parent_photon_in_cone = true;
                }
            }
            if (matches > 1) {
                ++photons_in_multiple_bare_cones;
                event_shared_photon_in_cones = true;
            }
        }
        if (event_hadron_parent_photon_in_cone)
            ++events_with_hadron_parent_photon_in_cone;
        if (event_shared_photon_in_cones)
            ++events_with_shared_photon_in_cones;

        // ---------------------------------------------------------------------
        // Photon dressing: p_l,dressed = p_l + sum p_gamma for DeltaR < 0.1
        // ---------------------------------------------------------------------

        // Save the historical algorithm for a paired, event-by-event comparison.
        std::vector<fastjet::PseudoJet> old_dressed_leptons;
        std::vector<int> old_dressed_ids;
        for (const auto* lep : prompt_leptons) {
            const auto& lm = lep->momentum();
            fastjet::PseudoJet dressed(lm.px(),lm.py(),lm.pz(),lm.e());
            for (const auto* photon : final_state_photons) {
                const auto& gm = photon->momentum();
                fastjet::PseudoJet gamma(gm.px(),gm.py(),gm.pz(),gm.e());
                if (dressed.delta_R(gamma) < 0.1) dressed += gamma;
            }
            if (dressed.pt() > 27.0 && std::abs(dressed.eta()) < 2.5) {
                old_dressed_leptons.push_back(dressed);
                old_dressed_ids.push_back(lep->pdg_id());
            }
        }

        // New dressing: fixed bare-lepton cones, only photons without a
        // hadron-decay ancestor, unique assignment to nearest bare lepton.
        std::vector<fastjet::PseudoJet> all_dressed;
        for (const auto* lep : prompt_leptons) {
            const auto& lm = lep->momentum();
            all_dressed.emplace_back(lm.px(),lm.py(),lm.pz(),lm.e());
        }
        for (const auto* photon : final_state_photons) {
            const auto& gm = photon->momentum();
            fastjet::PseudoJet gamma(gm.px(),gm.py(),gm.pz(),gm.e());
            size_t closest = prompt_leptons.size();
            double best_dr = 0.1;
            int candidates = 0;
            for (size_t i = 0; i < prompt_leptons.size(); ++i) {
                const auto& lm = prompt_leptons[i]->momentum();
                const fastjet::PseudoJet bare(lm.px(),lm.py(),lm.pz(),lm.e());
                const double dr = bare.delta_R(gamma);
                if (dr < 0.1) ++candidates;
                if (dr < best_dr) { best_dr = dr; closest = i; }
            }
            if (candidates > 1) ++photons_with_multiple_candidates;
            if (closest == prompt_leptons.size()) continue;
            if (hasDecayHadronAncestor(photon)) {
                ++photons_rejected_hadron_ancestry;
                continue;
            }
            all_dressed[closest] += gamma;
            ++photons_assigned;
        }
        std::vector<fastjet::PseudoJet> dressed_leptons;
        std::vector<int> dressed_lepton_pdgids;
        for (size_t i = 0; i < all_dressed.size(); ++i) {
            if (all_dressed[i].pt() <= 27.0 ||
                std::abs(all_dressed[i].eta()) >= 2.5) continue;
            dressed_leptons.push_back(all_dressed[i]);
            dressed_lepton_pdgids.push_back(prompt_leptons[i]->pdg_id());
        }

        // Third prompt-lepton veto diagnostic: additional prompt e/mu with
        // pT > 3 GeV and |eta| < 2.5. The two signal leptons are already
        // identified above, but the low-pT veto must use the unfiltered list.
        // Current approximation: hard-W ancestry; no low-pT overlap removal.
        int n_prompt_leptons_above_3 = 0;
        for (const auto* lep : prompt_leptons) {
            const auto& mom = lep->momentum();
            fastjet::PseudoJet candidate(mom.px(), mom.py(), mom.pz(), mom.e());
            if (candidate.pt() > 3.0 && std::abs(candidate.eta()) < 2.5)
                ++n_prompt_leptons_above_3;
        }
        const bool pass_third_lepton_veto = (n_prompt_leptons_above_3 <= 2);
        if (!pass_third_lepton_veto)
            ++events_with_third_prompt_lepton;

        // Sort dressed leptons by pT while keeping PDG IDs aligned.
        std::vector<size_t> lepton_order(dressed_leptons.size());
        for (size_t i = 0; i < lepton_order.size(); ++i)
            lepton_order[i] = i;

        std::sort(
            lepton_order.begin(),
            lepton_order.end(),
            [&](size_t a, size_t b) {
                return dressed_leptons[a].pt() > dressed_leptons[b].pt();
            }
        );

        std::vector<fastjet::PseudoJet> sorted_dressed_leptons;
        std::vector<int> sorted_dressed_lepton_pdgids;

        for (size_t index : lepton_order) {
            sorted_dressed_leptons.push_back(dressed_leptons[index]);
            sorted_dressed_lepton_pdgids.push_back(dressed_lepton_pdgids[index]);
        }

        dressed_leptons = sorted_dressed_leptons;
        dressed_lepton_pdgids = sorted_dressed_lepton_pdgids;


        // ---------------------------------------------------------------------
        // ATLAS-inspired electron-jet overlap removal (first validation step)
        // IMPORTANT: use the SAME reconstructed jets for overlap and VBS cuts.
        // ---------------------------------------------------------------------

        // ---------------------------------------------------------------------
        // Run anti-kT clustering
        // ---------------------------------------------------------------------

        fastjet::ClusterSequence cluster_seq(
            particles,
            jet_def
        );

        std::vector<fastjet::PseudoJet> jets =
            fastjet::sorted_by_pt(
                cluster_seq.inclusive_jets(20.0)
            );


        // ---------------------------------------------------------------------
        // Jet eta acceptance
        // ---------------------------------------------------------------------

        std::vector<fastjet::PseudoJet> selected_jets;

        for (size_t j = 0; j < jets.size(); ++j) {

            if (std::abs(jets[j].eta()) >= 4.5)
                continue;

            selected_jets.push_back(jets[j]);
        }

        // Raw clustered jets before any dressing-dependent overlap removal.
        int raw_vbs_jets = 0;
        for (const auto& j : selected_jets)
            if (j.pt() >= 25.0) ++raw_vbs_jets;
        if (raw_vbs_jets >= 2) ++jets_pre_overlap_two;

        const double paired_met = std::hypot(met_px, met_py);
        const auto old_result = oldDressingPasses(
            old_dressed_leptons, old_dressed_ids, selected_jets,
            paired_met, pass_third_lepton_veto);
        if (old_result.first) ++old_two_count;
        if (old_result.second) ++old_final_count;

        // Keep the old >=25 GeV threshold for electron-jet overlap removal.
        // The 20-25 GeV jets are needed only for the b-jet veto.
        // The selected dressed leptons and their PDG IDs remain index-aligned.
        // Only electrons overlap with jets in this particle-level prescription.
        // The closest overlapping jet is considered first for each electron.
        bool event_removed_electron = false;
        bool event_removed_jet = false;

        for (size_t i = 0; i < dressed_leptons.size(); ) {
            if (std::abs(dressed_lepton_pdgids[i]) != 11) {
                ++i;
                continue;
            }

            bool electron_removed = false;
            while (true) {
                size_t closest = selected_jets.size();
                double smallest_dr = 0.4;
                for (size_t j = 0; j < selected_jets.size(); ++j) {
                    if (selected_jets[j].pt() < 25.0) continue;
                    const double dr = dressed_leptons[i].delta_R(selected_jets[j]);
                    if (dr < smallest_dr) {
                        smallest_dr = dr;
                        closest = j;
                    }
                }
                if (closest == selected_jets.size())
                    break;

                const double ratio = dressed_leptons[i].pt() / selected_jets[closest].pt();
                if (ratio < 0.5) {
                    dressed_leptons.erase(dressed_leptons.begin() + i);
                    dressed_lepton_pdgids.erase(dressed_lepton_pdgids.begin() + i);
                    electron_removed = true;
                    event_removed_electron = true;
                    break;
                }

                selected_jets.erase(selected_jets.begin() + closest);
                event_removed_jet = true;
            }
            if (!electron_removed)
                ++i;
        }

        if (event_removed_electron) ++events_overlap_electron_removed;
        if (event_removed_jet) ++events_overlap_jet_removed;

        // ---------------------------------------------------------------------
        // Dressed-lepton fiducial selection
        // ---------------------------------------------------------------------

        const bool pass_two_leptons = (dressed_leptons.size() == 2);

        if (pass_two_leptons)
            ++events_pass_two_leptons;

        bool pass_same_sign = false;
        bool pass_mll = false;
        double mll = 0.0;

        if (pass_two_leptons) {

            // e-/mu- have positive PDG IDs; e+/mu+ have negative IDs.
            // Same sign therefore means the product of the two IDs is positive.
            pass_same_sign =
                (dressed_lepton_pdgids[0] * dressed_lepton_pdgids[1] > 0);

            if (pass_same_sign) {
                ++events_pass_same_sign;

                const fastjet::PseudoJet dilepton =
                    dressed_leptons[0] + dressed_leptons[1];

                mll = dilepton.m();
                pass_mll = (mll > 20.0);

                if (pass_mll)
                    ++events_pass_mll;
            }
        }

        // ATLAS dielectron Z-mass window veto, using dressed four-vectors.
        // This is applied only to the selected ee channel, not e-mu or mu-mu.
        const double z_mass = 91.1876; // GeV
        const bool is_dielectron = pass_two_leptons &&
            std::abs(dressed_lepton_pdgids[0]) == 11 &&
            std::abs(dressed_lepton_pdgids[1]) == 11;
        const bool in_z_window = is_dielectron &&
            std::abs(mll - z_mass) < 15.0;
        const bool pass_z_veto = !in_z_window;
        if (pass_mll && pass_third_lepton_veto && in_z_window)
            ++events_rejected_z_veto;

        const double met = std::sqrt(
            met_px * met_px + met_py * met_py
        );

        // The two vector definitions agree if total stable transverse
        // momentum closes: pT(neutrinos) + pT(visible) = 0.
        const double met_visible = std::hypot(visible_px, visible_py);
        const double closure = std::hypot(met_px + visible_px,
                                          met_py + visible_py);
        met_closure_sum += closure;
        met_closure_max = std::max(met_closure_max, closure);
        met_magnitude_difference_sum += std::abs(met - met_visible);
        if (closure > 1e-3) ++met_closure_above_1mev;

        const bool pass_veto_cumulative = pass_mll && pass_third_lepton_veto;
        if (pass_veto_cumulative)
            ++events_pass_third_lepton_veto;

        const bool pass_z_cumulative = pass_veto_cumulative && pass_z_veto;
        if (pass_z_cumulative)
            ++events_pass_z_veto;

        const bool pass_met = (pass_z_cumulative && met > 30.0);

        if (pass_met)
            ++events_pass_met;

        // Fill lepton/MET histograms after the two-lepton requirement.
        if (pass_two_leptons) {
            h_lepton1_pt.Fill(dressed_leptons[0].pt());
            h_lepton2_pt.Fill(dressed_leptons[1].pt());
            h_met.Fill(met);

            if (pass_same_sign)
                h_mll.Fill(mll);
        }


        // Ghost matching AFTER electron-jet overlap removal.
        // A tagged b-jet with pT > 20 GeV and |eta| < 2.5 vetoes the event.
        bool has_bjet = false;
        for (const auto& jet : selected_jets) {
            if (jet.pt() > 20.0 && std::abs(jet.eta()) < 2.5 &&
                containsBGhost(jet)) {
                has_bjet = true;
                break;
            }
        }
        if (has_bjet) ++events_with_tagged_bjets;
        const bool pass_b_veto = !has_bjet;
        if (pass_met && has_bjet) ++events_rejected_b_veto;
        const bool pass_b_cumulative = pass_met && pass_b_veto;
        if (pass_b_cumulative) ++events_pass_b_veto;

        // VBS jets retain their original 25 GeV threshold.
        std::vector<fastjet::PseudoJet> vbs_jets;
        for (const auto& jet : selected_jets)
            if (jet.pt() >= 25.0) vbs_jets.push_back(jet);
        selected_jets = std::move(vbs_jets);

        h_njets.Fill(selected_jets.size());


        // ---------------------------------------------------------------------
        // VBS dijet observables and cuts
        // ---------------------------------------------------------------------

        bool pass_jet_pt = false;
        bool pass_mjj = false;
        bool pass_dy = false;

        if (selected_jets.size() >= 2) {

            ++events_with_two_jets;

            const fastjet::PseudoJet& j1 = selected_jets[0];
            const fastjet::PseudoJet& j2 = selected_jets[1];

            pass_jet_pt = (j1.pt() >= 65.0 && j2.pt() >= 35.0);

            if (pass_jet_pt)
                ++events_pass_jet_pt;

            h_jet1_pt.Fill(j1.pt());
            h_jet2_pt.Fill(j2.pt());
            h_jet1_eta.Fill(j1.eta());
            h_jet2_eta.Fill(j2.eta());

            const fastjet::PseudoJet dijet = j1 + j2;
            const double mjj = dijet.m();

            pass_mjj = (pass_jet_pt && mjj > 500.0);

            if (pass_mjj)
                ++events_pass_mjj;

            const double deta = std::abs(j1.eta() - j2.eta());
            const double dy = std::abs(j1.rap() - j2.rap());

            pass_dy = (pass_mjj && dy > 2.0);

            if (pass_dy)
                ++events_pass_dy;

            h_mjj.Fill(mjj);
            h_deta_jj.Fill(deta);
            h_dy_jj.Fill(dy);
        }


        // Unified cutflow: apply jet/VBS cuts after lepton/MET selection.
        if (pass_b_cumulative) {

            if (selected_jets.size() >= 2) {
                ++full_pass_two_jets;
            }

            if (pass_jet_pt) {
                ++full_pass_jet_pt;
            }

            if (pass_mjj) {
                ++full_pass_mjj;
            }

            if (pass_dy) {
                ++full_pass_dy;
            }
        }


        // ---------------------------------------------------------------------
        // Full cumulative ATLAS-inspired baseline
        // ---------------------------------------------------------------------

        const bool pass_full_baseline =
            pass_b_cumulative &&
            pass_jet_pt &&
            pass_mjj &&
            pass_dy;

        if (pass_full_baseline) {
            ++events_pass_full_baseline;
            const auto& j1 = selected_jets[0];
            const auto& j2 = selected_jets[1];
            tree_mjj = (j1 + j2).m();
            tree_dy_jj = std::abs(j1.rap() - j2.rap());
            tree_mll = mll;
            tree_lepton1_pt = dressed_leptons[0].pt();
            tree_lepton2_pt = dressed_leptons[1].pt();
            tree_met = met;
            tree_jet1_pt = j1.pt();
            tree_jet2_pt = j2.pt();
            fiducial_events.Fill();
        }
        if (old_result.first != pass_two_leptons) ++changed_two_events;
        if (old_result.second != pass_full_baseline) ++changed_final_events;
        if (old_result.second && !pass_full_baseline) ++old_only_final;
        if (!old_result.second && pass_full_baseline) ++new_only_final;



        delete event;

        // Small progress indicator.
        if (event_count % 1000 == 0) {
            std::cout
                << "Processed "
                << event_count
                << " events"
                << std::endl;
        }
    }

    // Fill unified cumulative cutflow histogram.
    const char* cut_labels[12] = {
        "Generated",
        "2 dressed leptons",
        "Same sign",
        "mll > 20",
        "3rd lepton veto",
        "Z veto (ee)",
        "MET > 30",
        "b-jet veto",
        ">= 2 jets",
        "Jet pT",
        "mjj > 500",
        "|dy_jj| > 2"
    };

    const int cut_counts[12] = {
        event_count,
        events_pass_two_leptons,
        events_pass_same_sign,
        events_pass_mll,
        events_pass_third_lepton_veto,
        events_pass_z_veto,
        events_pass_met,
        events_pass_b_veto,
        full_pass_two_jets,
        full_pass_jet_pt,
        full_pass_mjj,
        full_pass_dy
    };

    for (int i = 0; i < 12; ++i) {
        h_cutflow.GetXaxis()->SetBinLabel(i + 1, cut_labels[i]);
        h_cutflow.SetBinContent(i + 1, cut_counts[i]);
    }
    // -------------------------------------------------------------------------
    // Save histograms
    // -------------------------------------------------------------------------

    TFile output(
        "particle_level.root",
        "RECREATE"
    );

    h_njets.Write();
    h_jet1_pt.Write();
    h_jet2_pt.Write();
    h_jet1_eta.Write();
    h_jet2_eta.Write();
    h_mjj.Write();
    h_deta_jj.Write();
    h_dy_jj.Write();
    h_cutflow.Write();
    h_lepton1_pt.Write();
    h_lepton2_pt.Write();
    h_mll.Write();
    h_met.Write();
    fiducial_events.Write();

    output.Close();


    // -------------------------------------------------------------------------
    // Summary
    // -------------------------------------------------------------------------

    std::cout << "\n========================================" << std::endl;
    std::cout << "Particle-level analysis done!" << std::endl;
    std::cout << "Events processed: " << event_count << "\n";

    std::cout << "\nPROMPT-LEPTON LINEAGE COMPARISON:\n";
    std::cout << "  Old broad hard-W ancestry leptons: " << old_hardW_ancestry_leptons << "\n";
    std::cout << "  New direct-W lepton-lineage leptons: " << new_directW_lineage_leptons << "\n";
    std::cout << "  Old-only candidates rejected: " << rejected_broad_ancestry_leptons << "\n";
    std::cout << "  Events with definition difference: " << events_with_prompt_definition_difference << "\n";
    std::cout << "  Note: this run uses the new definition; compare the cutflow with baseline 339.\n";

    std::cout << "\nMET MOMENTUM-CLOSURE DIAGNOSTIC (no selection changes):\n";
    if (event_count > 0) {
        std::cout << "  Mean |pT(nu) + pT(visible)| [GeV]: "
                  << met_closure_sum / event_count << "\n";
        std::cout << "  Max  |pT(nu) + pT(visible)| [GeV]: "
                  << met_closure_max << "\n";
        std::cout << "  Mean ||pT(nu)| - |pT(visible)|| [GeV]: "
                  << met_magnitude_difference_sum / event_count << "\n";
        std::cout << "  Events with closure > 0.001 GeV: "
                  << met_closure_above_1mev << " / " << event_count << "\n";
    }
    std::cout << "  Visible sum uses all stable non-neutrino particles,"
              << " without object cuts.\n";

    std::cout << "\nPAIRED OLD VS NEW PHOTON DRESSING (same events):\n";
    std::cout << "  New assigned prompt-like photons: " << photons_assigned << "\n";
    std::cout << "  Rejected in-cone hadron-ancestry photons: " << photons_rejected_hadron_ancestry << "\n";
    std::cout << "  Shared-cone photons uniquely assigned: " << photons_with_multiple_candidates << "\n";
    std::cout << "  Old two-lepton cut: " << old_two_count << "\n";
    std::cout << "  New two-lepton cut: " << events_pass_two_leptons << "\n";
    std::cout << "  Events changing two-lepton pass/fail: " << changed_two_events << "\n";
    std::cout << "  Old final baseline: " << old_final_count << "\n";
    std::cout << "  New final baseline: " << events_pass_full_baseline << "\n";
    std::cout << "  Events changing final pass/fail: " << changed_final_events << "\n";
    std::cout << "  Old-only final: " << old_only_final << "\n";
    std::cout << "  New-only final: " << new_only_final << "\n";
    std::cout << "  Photon prompt definition is a generator-history approximation, not an official ATLAS truth label.\n";

    std::cout << "\nPHOTON DRESSING AUDIT (historical cone diagnostic):\n";
    std::cout << "  All stable photons: " << stable_photons_total << "\n";
    std::cout << "  Stable photons with immediate hadron parent: "
              << stable_photons_immediate_hadron_parent << "\n";
    std::cout << "  Photons within DR<0.1 of a bare selected-lineage lepton: "
              << photons_in_bare_lepton_cones << "\n";
    std::cout << "  Of these, immediate hadron-parent photons: "
              << hadron_parent_photons_in_bare_cones << "\n";
    std::cout << "  Events with such hadron-parent photons in cone: "
              << events_with_hadron_parent_photon_in_cone << "\n";
    std::cout << "  Photons within DR<0.1 of multiple bare leptons: "
              << photons_in_multiple_bare_cones << "\n";
    std::cout << "  Events with shared-cone photons: "
              << events_with_shared_photon_in_cones << "\n";
    std::cout << "  Caveat: immediate hadron parent != full prompt-photon classification.\n";
    std::cout << "  Caveat: diagnostic uses bare lepton cones; baseline dressing\n"
              << "          uses an iteratively updated dressed-lepton direction.\n";

    std::cout << "\nB-GHOST RECORD AUDIT (diagnostic only):\n";
    std::cout << "  Ghosted last-open-bottom hadrons, status 2: "
              << ghosted_b_hadrons_status2 << "\n";
    std::cout << "  Ghosted last-open-bottom hadrons, other statuses: "
              << ghosted_b_hadrons_other_status << "\n";
    std::cout << "  Ghosted last-open-bottom hadrons without end vertex: "
              << ghosted_b_hadrons_without_end_vertex << "\n";
    std::cout << "  Caveat: this does not by itself prove weak-decay status.\n";

    std::cout << "\nB-HADRON RECORD DIAGNOSTIC (candidate IDs; no veto):\n";
    std::cout << "  Events with candidates: "
              << events_with_b_hadron_candidates << "\n";
    std::cout << "  Total candidate entries: "
              << total_b_hadron_candidates << "\n";
    std::cout << "  PDG IDs (signed):\n";
    for (const auto& kv : b_hadron_ids)
        std::cout << "    " << kv.first << " : " << kv.second << "\n";
    std::cout << "  HepMC statuses:\n";
    for (const auto& kv : b_hadron_statuses)
        std::cout << "    " << kv.first << " : " << kv.second << "\n";

    std::cout << "\nB-JET GHOST MATCHING:\n";
    std::cout << "  Last open-bottom hadrons ghosted: " << total_last_b_hadrons << "\n";
    std::cout << "  Events with tagged b-jets: " << events_with_tagged_bjets << "\n";
    std::cout << "  Events vetoed after lepton/MET cuts: " << events_rejected_b_veto << "\n";

    std::cout << "\nElectron-jet overlap removal (events):\n";
    std::cout << "  Electron removed: " << events_overlap_electron_removed << "\n";
    std::cout << "  Jet removed     : " << events_overlap_jet_removed << "\n";

    std::cout << "\nEvents with >=3 prompt leptons (pT>3, |eta|<2.5): "
              << events_with_third_prompt_lepton << "\n";
    std::cout << "\nPrompt-lepton multiplicity (pre-cuts):\n";
    std::cout << "  0 leptons : " << events_with_0_prompt_leptons << "\n";
    std::cout << "  1 lepton  : " << events_with_1_prompt_lepton << "\n";
    std::cout << "  2 leptons : " << events_with_2_prompt_leptons << "\n";
    std::cout << "  >2 leptons: " << events_with_more_prompt_leptons << "\n";

    std::cout << "\nLepton/MET cumulative cutflow:\n";
    std::cout << "  2 dressed leptons (pT>27, |eta|<2.5): "
              << events_pass_two_leptons << "\n";
    std::cout << "  + same sign: " << events_pass_same_sign << "\n";
    std::cout << "  + mll > 20 GeV: " << events_pass_mll << "\n";
    std::cout << "  + third prompt-lepton veto: " << events_pass_third_lepton_veto << "\n";
    std::cout << "  + ee Z-veto: " << events_pass_z_veto << "\n";
    std::cout << "  Z-veto rejected at this stage: " << events_rejected_z_veto << "\n";
    std::cout << "  + MET > 30 GeV: " << events_pass_met << "\n";

    std::cout << "\nJet/VBS cutflow (after dressing-dependent overlap removal):\n";
    std::cout << "  >= 2 jets BEFORE overlap removal: " << jets_pre_overlap_two << "\n";
    std::cout << "  >= 2 jets: " << events_with_two_jets << "\n";
    std::cout << "  + jet pT cuts: " << events_pass_jet_pt << "\n";
    std::cout << "  + mjj > 500 GeV: " << events_pass_mjj << "\n";
    std::cout << "  + |dy_jj| > 2: " << events_pass_dy << "\n";
    std::cout << "\n========================================\n";
    std::cout << "UNIFIED CUMULATIVE CUTFLOW\n";
    std::cout << "========================================\n";

    std::cout << "Generated events: "
            << event_count << "\n";

    std::cout << "+ Two dressed leptons: "
            << events_pass_two_leptons << "\n";

    std::cout << "+ Same sign: "
            << events_pass_same_sign << "\n";

    std::cout << "+ mll > 20 GeV: "
            << events_pass_mll << "\n";

    std::cout << "+ third prompt-lepton veto: "
              << events_pass_third_lepton_veto << "\n";

    std::cout << "+ ee Z-veto: " << events_pass_z_veto << "\n";

    std::cout << "+ MET > 30 GeV: "
            << events_pass_met << "\n";

    std::cout << "+ b-jet veto: " << events_pass_b_veto << "\n";

    std::cout << "+ >= 2 jets: "
            << full_pass_two_jets << "\n";

    std::cout << "+ Jet pT cuts: "
            << full_pass_jet_pt << "\n";

    std::cout << "+ mjj > 500 GeV: "
            << full_pass_mjj << "\n";

    std::cout << "+ |dy_jj| > 2: "
            << full_pass_dy << "\n";

    std::cout << "\nFull ATLAS-inspired baseline:\n";
    std::cout << "  leptons + MET + VBS jets: "
              << events_pass_full_baseline << "\n";

    if (event_count > 0) {
        std::cout << "  baseline efficiency: "
                  << 100.0 * events_pass_full_baseline / event_count
                  << " %\n";
    }

    std::cout << "\nOutput: particle_level.root" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
