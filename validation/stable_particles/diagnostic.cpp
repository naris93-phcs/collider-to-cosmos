// Two-event feasibility diagnostic, not the particle-level physics analysis.
// Lifetimes: exact bundled Pythia 8.317 ParticleData, in mm/c.
#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "Pythia8/Pythia.h"
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

using Particle = const HepMC::GenParticle*;
using Set = std::set<Particle>;
constexpr double threshold = 8.99377; // c * 3e-11 seconds, mm
constexpr long double tolerance = 1e-8L; // absolute GeV per component
Particle context=nullptr;
std::array<long double,4> maxima{};
unsigned long long totalOriginal=0,totalReconstructed=0,totalRemoved=0,totalDescendants=0;
void require(bool ok, const char* message) {
  if (!ok) {
    if(context) std::cerr << " PDG=" << context->pdg_id() << " barcode=" << context->barcode() << " ";
    throw std::runtime_error(message);
  }
}
Set descendants(Particle parent) {
  Set seen;
  std::vector<Particle> pending{parent};
  while (!pending.empty()) {
    auto p = pending.back(); pending.pop_back();
    if (!p->end_vertex()) continue;
    for (auto it = p->end_vertex()->particles_out_const_begin();
         it != p->end_vertex()->particles_out_const_end(); ++it) {
      require(*it != parent, "Cycle back to decay root");
      if (seen.insert(*it).second) pending.push_back(*it);
    }
  }
  return seen;
}
std::array<long double,4> sum(const Set& particles) {
  std::array<long double,4> total{};
  for (auto p : particles) {
    auto m = p->momentum();
    total[0] += m.px(); total[1] += m.py();
    total[2] += m.pz(); total[3] += m.e();
  }
  return total;
}
void inspect(const HepMC::GenEvent& event, Pythia8::ParticleData& data) {
  require(event.momentum_unit() == HepMC::Units::GEV, "Expected GeV");
  Set original, candidates;
  std::set<int> identities;
  for (auto it=event.particles_begin(); it!=event.particles_end(); ++it) {
    auto p=*it; context=p;
    require(identities.insert(p->barcode()).second, "Duplicate barcode");
    require(data.isParticle(p->pdg_id()), "Unknown PDG lifetime lookup");
    const double ct=data.tau0(p->pdg_id());
    if (p->status()==1) {
      require(!p->end_vertex(), "Status-1 particle has end vertex");
      // Zero lifetime encodes genuinely stable species only if there is
      // neither a width nor a decay table. Never treat all tau0==0 as stable.
      const bool intrinsicStable = ct==0 && data.mWidth(p->pdg_id())==0
                                   && !data.canDecay(p->pdg_id());
      require(ct>threshold || intrinsicStable,
              "Final-state particle fails lifetime criterion");
      original.insert(p);
    }
    if (p->status()!=2 || ct<=threshold) continue;
    auto v=p->end_vertex();
    require(p->production_vertex() && v, "Missing candidate vertex");
    require(v->particles_in_size()==1 && v->particles_out_size()>0,
            "Ambiguous decay vertex");
    Set daughters;
    for (auto d=v->particles_out_const_begin();
         d!=v->particles_out_const_end(); ++d) {
      require((*d)->pdg_id()!=p->pdg_id(),
              "Same-PDG continuation: possible history copy");
      daughters.insert(*d);
    }
    auto before=sum(Set{p}), after=sum(daughters);
    for (int k=0;k<4;++k)
      require(std::abs(before[k]-after[k])<tolerance,
              "Decay vertex fails momentum closure");
    candidates.insert(p);
  }
  Set nested;
  for (auto p:candidates)
    for (auto d:descendants(p))
      if (candidates.count(d)) nested.insert(d);
  Set roots, removed;
  for (auto p:candidates) if (!nested.count(p)) {
    context=p;
    roots.insert(p);
    for (auto d:descendants(p)) {
      require(removed.insert(d).second, "Shared descendant between roots");
      if (auto v=d->end_vertex())
        require(v->particles_in_size()==1 && v->particles_out_size()>0,
                "Ambiguous descendant decay vertex");
      else require(d->status()==1, "Nonfinal descendant leaf");
    }
  }
  Set reconstructed=roots;
  unsigned removedFinal=0;
  for (auto p:original) {
    if (removed.count(p)) ++removedFinal;
    else require(reconstructed.insert(p).second, "Duplicate retained particle");
  }
  // Check ancestors through the original graph, including history vertices.
  for (auto p:reconstructed) {
    context=p;
    Set visited;
    std::vector<Particle> pending{p};
    while (!pending.empty()) {
      auto current=pending.back(); pending.pop_back();
      if (!current->production_vertex()) continue;
      auto v=current->production_vertex();
      for (auto a=v->particles_in_const_begin();
           a!=v->particles_in_const_end(); ++a) {
        require(!reconstructed.count(*a), "Retained ancestor-descendant pair");
        if (visited.insert(*a).second) pending.push_back(*a);
      }
    }
  }
  auto oldSum=sum(original), newSum=sum(reconstructed);
  std::cout << "EVENT " << event.event_number()
            << " original=" << original.size()
            << " candidates=" << candidates.size()
            << " nested=" << nested.size() << " roots=" << roots.size()
            << " removed_final=" << removedFinal
            << " reconstructed=" << reconstructed.size() << '\n';
  std::cout << "residual reconstructed-original [px,py,pz,E] GeV:";
  for (int k=0;k<4;++k) {
    std::cout << ' ' << newSum[k]-oldSum[k];
    maxima[k]=std::max(maxima[k],std::abs(newSum[k]-oldSum[k]));
    require(std::abs(newSum[k]-oldSum[k])<tolerance,
            "Final collections fail momentum closure");
  }
  std::cout << "\nidentities_unique=PASS ancestry=PASS closure=PASS\n";
  totalOriginal+=original.size(); totalReconstructed+=reconstructed.size();
  totalRemoved+=removedFinal; totalDescendants+=removed.size();
  // Print one root per relevant species, with its immediate daughters.
  std::set<int> printed;
  for (auto p:roots) {
    int id=std::abs(p->pdg_id());
    if ((id!=310 && id!=3122 && id!=3312 && id!=3322)
        || !printed.insert(id).second) continue;
    std::cout << "CHAIN barcode=" << p->barcode() << " PDG=" << p->pdg_id()
              << " ct_mm=" << data.tau0(p->pdg_id()) << " daughters:";
    auto v=p->end_vertex();
    for (auto d=v->particles_out_const_begin();
         d!=v->particles_out_const_end(); ++d)
      std::cout << " [" << (*d)->barcode() << ',' << (*d)->pdg_id()
                << ",status=" << (*d)->status() << ']';
    std::cout << '\n';
  }
}
int main(int argc,char** argv) {
  try {
    require(argc==3,"Usage: diagnostic INPUT.hepmc PYTHIA_XML_DIRECTORY");
    static_assert(PYTHIA_VERSION_INTEGER==8317,"Require Pythia 8.317 headers");
    Pythia8::Pythia pythia(argv[2],false); // Load tables only; no init/next.
    HepMC::IO_GenEvent input(argv[1],std::ios::in);
    std::cout << std::setprecision(18);
    int tested=0,passed=0,failed=0;
    while (std::unique_ptr<HepMC::GenEvent> event{input.read_next_event()}) {
      ++tested; context=nullptr;
      try { inspect(*event,pythia.particleData); ++passed; }
      catch(const std::exception& e) {
        ++failed; std::cerr << "FAIL event=" << event->event_number() << " condition=" << e.what() << "\n";
      }
      context=nullptr;
    }
    std::cout << "SUMMARY processed=" << tested << " passed=" << passed << " failed=" << failed
              << " original_passed=" << totalOriginal << " reconstructed_passed=" << totalReconstructed
              << " excluded_status1_passed=" << totalRemoved << " excluded_all_descendants_passed=" << totalDescendants << "\nMAX_ABS_RESIDUAL_GEV";
    for(auto r:maxima) std::cout << " " << r;
    std::cout << "\n";
    require(tested==10000,"Expected exactly 10000 events");
    return failed ? 1 : 0;
  } catch (const std::exception& e) {
    std::cerr << "FAIL: " << e.what() << '\n'; return 1;
  }
}
