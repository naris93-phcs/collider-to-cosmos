#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"

#include "fastjet/ClusterSequence.hh"

#include "TH1D.h"
#include "TFile.h"


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


    // -------------------------------------------------------------------------
    // Counters
    // -------------------------------------------------------------------------

    int event_count = 0;

    // Prompt-lepton multiplicity sanity check before fiducial cuts.
    int events_with_0_prompt_leptons = 0;
    int events_with_1_prompt_lepton = 0;
    int events_with_2_prompt_leptons = 0;
    int events_with_more_prompt_leptons = 0;

    // Dressed-lepton / MET cutflow.
    int events_pass_two_leptons = 0;
    int events_pass_same_sign = 0;
    int events_pass_mll = 0;
    int events_pass_met = 0;

    // Jet/VBS cutflow, independent of the lepton cutflow.
    int events_with_two_jets = 0;
    int events_pass_jet_pt = 0;
    int events_pass_mjj = 0;
    int events_pass_dy = 0;

    // Full cumulative baseline selection.
    int events_pass_full_baseline = 0;


    // -------------------------------------------------------------------------
    // Event loop
    // -------------------------------------------------------------------------

    HepMC::GenEvent* event = nullptr;

    while ((event = ascii_in.read_next_event())) {

        ++event_count;

        std::vector<fastjet::PseudoJet> particles;
        std::vector<HepMC::GenParticle*> prompt_leptons;
        std::vector<HepMC::GenParticle*> final_state_photons;

        double met_px = 0.0;
        double met_py = 0.0;


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
            if ((absId == 11 || absId == 13) &&
                hasHardWAncestor(*p)) {
                prompt_leptons.push_back(*p);
            }

            // Baseline dressing uses stable final-state photons.
            // The exact ATLAS prompt-photon semantics will be refined later.
            if (absId == 22) {
                final_state_photons.push_back(*p);
            }

            // Particle-level MET from stable neutrinos.
            if (absId == 12 || absId == 14 || absId == 16) {
                met_px += mom.px();
                met_py += mom.py();
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


        // ---------------------------------------------------------------------
        // Photon dressing: p_l,dressed = p_l + sum p_gamma for DeltaR < 0.1
        // ---------------------------------------------------------------------

        std::vector<fastjet::PseudoJet> dressed_leptons;
        std::vector<int> dressed_lepton_pdgids;

        for (const auto* lep : prompt_leptons) {

            const HepMC::FourVector& lmom = lep->momentum();

            fastjet::PseudoJet dressed(
                lmom.px(),
                lmom.py(),
                lmom.pz(),
                lmom.e()
            );

            for (const auto* photon : final_state_photons) {

                const HepMC::FourVector& gmom = photon->momentum();

                fastjet::PseudoJet gamma(
                    gmom.px(),
                    gmom.py(),
                    gmom.pz(),
                    gmom.e()
                );

                if (dressed.delta_R(gamma) < 0.1)
                    dressed += gamma;
            }

            // Fiducial dressed-lepton acceptance.
            if (dressed.pt() <= 27.0)
                continue;

            if (std::abs(dressed.eta()) >= 2.5)
                continue;

            dressed_leptons.push_back(dressed);
            dressed_lepton_pdgids.push_back(lep->pdg_id());
        }

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

        const double met = std::sqrt(
            met_px * met_px + met_py * met_py
        );

        const bool pass_met = (pass_mll && met > 30.0);

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


        // ---------------------------------------------------------------------
        // Run anti-kT clustering
        // ---------------------------------------------------------------------

        fastjet::ClusterSequence cluster_seq(
            particles,
            jet_def
        );

        std::vector<fastjet::PseudoJet> jets =
            fastjet::sorted_by_pt(
                cluster_seq.inclusive_jets(25.0)
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


        // ---------------------------------------------------------------------
        // Full cumulative ATLAS-inspired baseline
        // ---------------------------------------------------------------------

        const bool pass_full_baseline =
            pass_met &&
            pass_jet_pt &&
            pass_mjj &&
            pass_dy;

        if (pass_full_baseline)
            ++events_pass_full_baseline;


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

    h_lepton1_pt.Write();
    h_lepton2_pt.Write();
    h_mll.Write();
    h_met.Write();

    output.Close();


    // -------------------------------------------------------------------------
    // Summary
    // -------------------------------------------------------------------------

    std::cout << "\n========================================" << std::endl;
    std::cout << "Particle-level analysis done!" << std::endl;
    std::cout << "Events processed: " << event_count << "\n";

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
    std::cout << "  + MET > 30 GeV: " << events_pass_met << "\n";

    std::cout << "\nJet/VBS cutflow (independent):\n";
    std::cout << "  >= 2 jets: " << events_with_two_jets << "\n";
    std::cout << "  + jet pT cuts: " << events_pass_jet_pt << "\n";
    std::cout << "  + mjj > 500 GeV: " << events_pass_mjj << "\n";
    std::cout << "  + |dy_jj| > 2: " << events_pass_dy << "\n";

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
