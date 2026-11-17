# Preliminary mjj checkpoint

Baseline: unchanged current analysis/particle_level.cpp status==1 selection.
Lifetime reconstruction is NOT integrated. checkpoint.cpp is a snapshot;
source.diff lists all changes: absolute input, three cumulative histograms,
and a local ROOT output filename. No selection or event-weight code changed.
Original h_mjj is filled at source line 984 without cumulative cuts; the
existing selected-event tree contains only full-baseline events. Hence neither
provides the requested pre-VBS distribution. Existing plotting macros use
that tree or cutflow and cannot recover missing stages.

All checkpoint histograms: 130 identical bins, 0--13000 GeV (100 GeV/bin).
Raw unit fills, no cross-section, luminosity or unit-area normalization.
These are not weighted yields; the existing baseline does not apply HepMC
weights to these fills. Input weights/event records remain unchanged.
Selections: pre = pass_b_cumulative && pass_jet_pt; mass adds pass_mjj;
full adds pass_dy. Tagging jets and all cuts follow the snapshot exactly.

Commands, from repository root (new build/output paths only):
```bash
ROOT_PREFIX=/home/naris/miniforge3/envs/hep
g++ -std=c++17 -O2 validation/physics_checkpoints/mjj/checkpoint.cpp -I "$ROOT_PREFIX/include" -I MG5_aMC_v3_8_0/HEPTools/hepmc/include -L "$ROOT_PREFIX/lib" -L MG5_aMC_v3_8_0/HEPTools/hepmc/lib -Wl,-rpath,/home/naris/miniforge3/envs/hep/lib -Wl,-rpath,/home/naris/projects/collider_to_cosmos/MG5_aMC_v3_8_0/HEPTools/hepmc/lib -lCore -lHist -lRIO -lTree -lfastjettools -lfastjet -lHepMC -o validation/physics_checkpoints/mjj/checkpoint > validation/physics_checkpoints/mjj/build.log 2>&1
cd validation/physics_checkpoints/mjj
./checkpoint > analysis_run.log 2>&1
/home/naris/miniforge3/envs/hep/bin/python -B plot.py > plot.log 2>&1
```
An initial incorrect executable path failed before execution; run.log preserves
that error. The corrected command above was used. Do not rerun over outputs.
The main analysis was not executed or modified: only its isolated snapshot.

Outcome: processed 10000, pre=2246, mass=772, full=339. Full-range
means (GeV): 491.6024, 910.8163, 966.0288. All flow bins empty.
Final display zooms to 0--4000 GeV after asserting all bins above 4000
are empty; full histograms remain 0--13000 GeV. plot.py was rerun from
this directory (plot_display.log), preserving initial counts.json.
A mistaken plot invocation from repository root failed before plotting;
its log was moved here as failed_plot_cwd.log. No files remain outside
this diagnostic directory from that attempt.
