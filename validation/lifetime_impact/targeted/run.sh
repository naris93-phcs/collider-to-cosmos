#!/bin/bash
set -euo pipefail
cd /home/naris/projects/collider_to_cosmos
D=validation/lifetime_impact/targeted
for name in baseline lifetime; do test ! -e "$D/$name.root"; test ! -e "$D/$name.csv"; done
E=/home/naris/miniforge3/envs/hep
H=$PWD/MG5_aMC_v3_8_0/HEPTools
export LD_LIBRARY_PATH="$H/pythia8/lib:$H/hepmc/lib:$E/lib"
export PYTHIA8DATA="$H/pythia8/share/Pythia8/xmldoc"
g++ -std=c++17 -O2 "$D/targeted.cpp" -I "$D/.." -I "$H/pythia8/include" -I "$H/hepmc/include" -I "$E/include" -L "$H/pythia8/lib" -L "$H/hepmc/lib" -L "$E/lib" -Wl,-rpath,"$E/lib" -Wl,-rpath,"$H/hepmc/lib" -Wl,-rpath,"$H/pythia8/lib" -lCore -lHist -lRIO -lTree -lfastjettools -lfastjet -lHepMC -lpythia8 -o "$D/comparison" > "$D/build.log" 2>&1
cd "$D"
./comparison baseline > baseline.log 2>&1
./comparison lifetime > lifetime.log 2>&1
