#!/usr/bin/env bash
set -euo pipefail
set -o noclobber

production=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
repo=$(cd -- "$production/../.." && pwd)
run=ew_wmwm_10000_seed161803
source="$production/sm_ew_wmwmjj_leptonic/Events/$run/unweighted_events.lhe.gz"
shower="$production/shower"
analysis="$production/analysis"
hepmc="$shower/ew_wmwmjj_10000_pythia8317.hepmc"
root="$analysis/ew_minus_lifetime.root"
tools="$repo/MG5_aMC_v3_8_0/HEPTools"
positive="$repo/production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k"
qcd="$repo/sm_wmwmjj_leptonic/Events/run_01"

test ! -e "$hepmc"
test ! -e "$root"
test ! -e "$shower/djrs.dat"
test ! -e "$shower/pts.dat"
python3 -B "$production/prepare_lhe.py" "$source" > "$production/lhe_verification.json"
target=$(python3 -B -c 'import json, sys; print(json.load(open(sys.argv[1]))["shower_lhe"])' "$production/lhe_verification.json")

g++ -std=c++17 -DGZIP -I"$tools/pythia8/include" -I"$tools/zlib/include" \
    "$production/verify_pythia_reader.cpp" -L"$tools/pythia8/lib" -L"$tools/zlib/lib" \
    -Wl,-rpath,"$tools/pythia8/lib:$tools/zlib/lib" -lpythia8 -lz -o "$shower/verify_reader"
"$shower/verify_reader" "$target" "$production/lhe_reader_expected.txt" 10000 > "$shower/reader.log"
if rg -i 'error|abort' "$shower/reader.log"; then exit 1; fi

sed -E "s|^HEPMCoutput:file = .*|HEPMCoutput:file = ${hepmc##*/}|; s|^Beams:LHEF=.*|Beams:LHEF=$target|; s|^Random:seed = .*|Random:seed = 161804|" \
    "$positive/shower/pythia8.cmd" > "$production/cards/pythia8.cmd"
printf '%s\n' 'LHE and reader validated. Starting Pythia8.'
(
    cd "$shower"
    "$tools/MG5aMC_PY8_interface/MG5aMC_PY8_interface" "$production/cards/pythia8.cmd" > shower.log 2>&1
)
python3 -B "$production/verify_hepmc.py" "$production" "$qcd" ew > "$analysis/ew_minus_hepmc_verification.json"
printf '%s\n' '10,000 HepMC records and normalization validated. Starting lifetime analysis.'
"$repo/build/particle_level" lifetime "$hepmc" "$root" > "$analysis/ew_minus_particle_level.log" 2>&1
