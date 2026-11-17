#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenCrossSection.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
int main(int argc,char** argv) {
 int n=0;
 try {
  if(argc!=2) throw std::runtime_error("expected one HepMC filename");
  HepMC::IO_GenEvent input(argv[1],std::ios::in);
  constexpr double xs=0.003870572;
  long double lhe=0, normalized=0;
  double lastC=0,lastErr=0;
  std::set<int> indices;
  while(std::unique_ptr<HepMC::GenEvent> ev{input.read_next_event()}) {
   ++n;
   if(!indices.insert(ev->event_number()).second || !ev->is_valid() ||
      ev->momentum_unit()!=HepMC::Units::GEV || ev->length_unit()!=HepMC::Units::MM)
     throw std::runtime_error("invalid event identity, record or units");
   int nw=0;
   for(auto p=ev->particles_begin();p!=ev->particles_end();++p) {
    auto v=(*p)->momentum();
    if(!std::isfinite(v.px()) || !std::isfinite(v.py()) ||
       !std::isfinite(v.pz()) || !std::isfinite(v.e()))
      throw std::runtime_error("non-finite particle momentum");
    if(std::abs((*p)->pdg_id())==24 && (*p)->status()==22) {
     if((*p)->pdg_id()!=-24) throw std::runtime_error("positive hard W in negative sample");
     ++nw;
    }
   }
   if(nw!=2) throw std::runtime_error("expected two hard W-");
   const auto& w=ev->weights();
   if(!w.has_key("Weight") || !w.has_key("Weight_MERGING=0.000"))
     throw std::runtime_error("missing named weights");
   if(!(std::abs(w["Weight"]-xs)<1e-12 &&
        std::abs(w["Weight_MERGING=0.000"]-xs/10000)<1e-15))
     throw std::runtime_error("unexpected weight normalization");
   lhe+=w["Weight"]; normalized+=w["Weight_MERGING=0.000"];
   auto c=ev->cross_section();
   if(!c || !(std::abs(c->cross_section()-normalized)<1e-11))
     throw std::runtime_error("cross-section record does not match cumulative normalized weight");
   lastC=c->cross_section();lastErr=c->cross_section_error();
  }
  if(n!=10000 || input.error_type()!=0) throw std::runtime_error("unexpected event count or HepMC parser error: "+input.error_message());
  std::cout << std::setprecision(15) << "events=" << n << "\n"
            << "mean_Weight_pb=" << lhe/n << "\n"
            << "sum_Weight_MERGING=0.000_pb=" << normalized << "\n"
            << "last_C_pb=" << lastC << " last_C_err_pb=" << lastErr << "\n";
 } catch(const std::exception& e) {
  std::cerr << "FAIL at event ordinal " << n << ": " << e.what() << "\n";
  return 1;
 }
}
