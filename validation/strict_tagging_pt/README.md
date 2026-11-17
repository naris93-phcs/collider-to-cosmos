# Strict tagging-jet pT regression

Only the active cumulative selection changes: leading pT > 65 GeV and
subleading pT > 35 GeV (ATLAS 2024 §7.1, p. 17). The historical
oldDressingPasses diagnostic retains its original selection. No other cut
or object definition changes.

Commands used from the repository root, with fresh output paths:

```bash
D=validation/strict_tagging_pt
export CONDA_PREFIX=/home/naris/miniforge3/envs/hep
H=$PWD/MG5_aMC_v3_8_0/HEPTools
export LD_LIBRARY_PATH="$H/pythia8/lib:$H/hepmc/lib:$CONDA_PREFIX/lib"
export PYTHIA8DATA="$H/pythia8/share/Pythia8/xmldoc"
cmake -S . -B "$D/build" -DCMAKE_PREFIX_PATH="$CONDA_PREFIX" -DCMAKE_BUILD_TYPE=Release > "$D/build.log" 2>&1
cmake --build "$D/build" >> "$D/build.log" 2>&1
"$D/build/particle_level" lifetime sm_wpwpjj_leptonic/Events/run_04/tag_1_pythia8_events.hepmc "$D/lifetime.root" > "$D/run.log" 2>&1
"$CONDA_PREFIX/bin/python" -B "$D/compare.py" > "$D/comparison.json"
```

The comparator checks all 12 cutflow bins and ordered sequential event indices
against lifetime_integration/lifetime.root. It also checks the old full-sample
lifetime CSV at its 17-digit precision for exact equality to either threshold
among events with at least two VBS jets, irrespective of earlier cuts, and
predicts the only allowed cutflow and index changes.

Result: PASS on 10,000 events. No tagging pair was exactly at either endpoint.
All 12 cutflow bins and all 339 ordered selected indices are identical to the
previous lifetime run; no entries or exits. See comparison.json.
