#!/usr/bin/env bash
set -euo pipefail
set -o noclobber

production=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
repo=$(cd -- "$production/../.." && pwd)
run=ew_wpwp_50000_seed314159
lhe="$production/sm_ew_wpwpjj_leptonic/Events/$run/unweighted_events.lhe.gz"
shower="$production/shower"
hepmc="$shower/${run}_pythia8317.hepmc"
root="$production/analysis/${run}_lifetime.root"
pythia="$repo/MG5_aMC_v3_8_0/HEPTools/MG5aMC_PY8_interface/MG5aMC_PY8_interface"

cd "$repo"
sha256sum -c "$production/preserved.sha256" > "$production/preserved_before_shower.log"
test ! -e "$hepmc"
test ! -e "$root"
test ! -e "$shower/djrs.dat"
test ! -e "$shower/pts.dat"

python3 -B "$production/verify_lhe.py" "$lhe" > "$production/lhe_verification.partial.json"
mv --no-clobber "$production/lhe_verification.partial.json" "$production/lhe_verification.json"
printf '%s\n' 'LHE verified. Starting standalone Pythia8/HepMC2.'
(
    cd "$shower"
    "$pythia" "$production/cards/pythia8.cmd" > shower.log 2>&1
)
python3 -B "$production/verify_hepmc.py" "$hepmc" > "$production/hepmc_verification.partial.json"
mv --no-clobber "$production/hepmc_verification.partial.json" "$production/hepmc_verification.json"
python3 -B - "$production" <<'PY'
from pathlib import Path
import json
import math
import re
import sys

production = Path(sys.argv[1])
log = (production / "shower/shower.log").read_text()
verified = json.loads((production / "hepmc_verification.json").read_text())
assert "This is PYTHIA version 8.317" in log
match = re.search(r"Les Houches User Process\(es\).*?\|\s*(\d+)\s+(\d+)\s+(\d+)\s*\|", log)
assert match and tuple(map(int, match.groups())) == (verified["events"],) * 3
match = re.search(r"Inclusive cross section:\s+(\S+)\s+\+-\s+\S+\s+mb", log)
assert match and math.isclose(float(match[1]) * 1e9, verified["weight_merging_sum_pb"], rel_tol=1e-8)
assert not re.search(r"(?i)abort run|end of lhef|normalisation will be decreased|unknown (?:setting|command)", log)
PY
printf '%s\n' 'HepMC2 verified. Starting existing lifetime analysis.'
"$repo/build/particle_level" lifetime "$hepmc" "$root" > "$production/analysis/particle_level.log" 2>&1
"$HOME/miniforge3/envs/hep/bin/python" -B "$production/verify_root.py" "$root" > "$production/analysis/verification.partial.json"
mv --no-clobber "$production/analysis/verification.partial.json" "$production/analysis/verification.json"
sha256sum -c "$production/preserved.sha256" > "$production/preserved_after_analysis.log"
git status --short > "$production/git_status_after.txt"
git diff --binary | cmp "$production/existing_changes.patch"
git diff --check
printf '%s\n' 'ROOT output, cutflow, normalization and preserved inputs verified.'
