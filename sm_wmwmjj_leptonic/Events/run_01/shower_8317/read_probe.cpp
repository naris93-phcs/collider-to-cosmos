#include "Pythia8/LHEF3.h"
#include <iostream>
int main(int argc,char** argv) {
 for(int i=1;i<argc;++i) {
   Pythia8::Reader r(argv[i]);
   std::cout << argv[i] << " first_event=" << r.readEvent() << " NUP=" << r.hepeup.NUP << '\n';
 }
}
