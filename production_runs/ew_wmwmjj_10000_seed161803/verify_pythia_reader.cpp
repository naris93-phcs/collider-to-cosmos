#include "Pythia8/Pythia.h"
#include <fstream>
#include <cmath>
#include <iostream>
int main(int, char** argv) {
    static_assert(PYTHIA_VERSION_INTEGER == 8317);
    Pythia8::Reader reader(argv[1]);
    std::ifstream expected(argv[2]);
    int index, n, process, matched = 0;
    double weight;
    while (expected >> index >> n >> process >> weight) {
        const bool parsed = reader.readEvent();
        bool valid = parsed && reader.hepeup.NUP == n && reader.hepeup.IDUP.size() == static_cast<unsigned>(n)
            && std::abs(reader.hepeup.XWGTUP - weight) < 1e-12
            && reader.hepeup.IDPRUP == process;
        for (int i = 1; i <= n; ++i) {
            int pdg;
            expected >> pdg;
            if (parsed && i <= static_cast<int>(reader.hepeup.IDUP.size()) && reader.hepeup.IDUP[i-1] != pdg) valid = false;
        }
        std::cout << "source " << index << " NUP " << n
                  << " parsed " << parsed << " read "
                  << (parsed ? reader.hepeup.NUP : -1)
                  << " match " << valid << '\n';
        matched += valid;
    }
    std::cout << "Matched " << matched << "/expected\n";
    return !expected.eof() || matched != std::stoi(argv[3]);
}
