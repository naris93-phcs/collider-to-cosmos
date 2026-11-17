#!/bin/bash
set -euo pipefail
cd /home/naris/projects/collider_to_cosmos
D=validation/lifetime_integration
E=/home/naris/miniforge3/envs/hep
H=$PWD/MG5_aMC_v3_8_0/HEPTools
export LD_LIBRARY_PATH="$H/pythia8/lib:$H/hepmc/lib:$E/lib"
export PYTHIA8DATA="$H/pythia8/share/Pythia8/xmldoc"
input=sm_wpwpjj_leptonic/Events/run_04/tag_1_pythia8_events.hepmc
if [ "$#" -eq 0 ]; then set -- status1 lifetime; fi
for mode; do
  case "$mode" in status1|lifetime) ;; *) exit 1 ;; esac
  test ! -e "$D/$mode.root"
  test ! -e "$D/$mode.log"
  test ! -e "$D/$mode.verify.json"
  "$D/build/particle_level" "$mode" "$input" "$D/$mode.root" > "$D/$mode.log" 2>&1
  "$E/bin/python" -B "$D/verify.py" "$mode" > "$D/$mode.verify.json"
done
