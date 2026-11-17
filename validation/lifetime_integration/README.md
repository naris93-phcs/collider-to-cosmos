# Lifetime reconstruction integration

The default stable-particle mode is **lifetime**. The explicit **status1**
mode keeps the original HepMC status-1 collection and does not load the
Pythia particle table at runtime. Both modes use the same fiducial cuts,
jet clustering, b ghosts, overlap removal, prompt-lepton ancestry, dressing
rules and neutrino-sum MET convention. Only collection membership changes.
Original HepMC particles are never mutated. The main analysis directly includes
the already validated ../lifetime_impact/reconstruction.h; keep that source
header with the analysis when packaging/checking out the code. No second
implementation of the algorithm is maintained.

## Build and run

From the repository root, with the existing local dependencies:

```bash
export CONDA_PREFIX=/home/naris/miniforge3/envs/hep
cmake -S . -B validation/lifetime_integration/build \
  -DCMAKE_PREFIX_PATH="$CONDA_PREFIX" -DCMAKE_BUILD_TYPE=Release
cmake --build validation/lifetime_integration/build
```

Executable syntax: `particle_level [lifetime|status1 [input.hepmc [output.root]]]`.
With no arguments, run from the analysis directory so the historical relative
run_04 input resolves. Default output in the working directory is
particle_level_lifetime.root (or particle_level_status1.root with status1).
For runs from another directory, pass both paths explicitly. Outputs are
opened in ROOT CREATE mode and existing destinations are refused.
The selected-event tree adds event_index, the one-based sequential ordinal in
the input (not the potentially repeated HepMC event number).

CMake prioritizes the bundled Pythia 8.317 headers/library and uses its XML
path. If another Pythia is on the process loader path or PYTHIA8DATA is set,
use the explicit process-local settings in run.sh; do not mix Conda 8.312 and
the bundled 8.317.

## Regression on archived run_04

`bash validation/lifetime_integration/run.sh` runs both modes against the same
10,000-event input with fresh status1.root and lifetime.root destinations here.
The script stops at the first nonzero analysis or verification exit. The
verifier compares all 12 cutflow bins, the 339 selected sequential indices
and eight selected observables (1e-9 absolute tolerance), plus exact bin
contents/entries including flow bins for all reference histograms. Reference
inputs are the independent lifetime_impact ROOT/CSV/JSON artifacts.
Do not rerun over existing outputs; use fresh destinations.

## Scope and ATLAS differences

The lifetime mode uses the validated Pythia particle-table threshold
c*tau0 > 8.99377 mm and ancestor-aware replacement of recorded physical decays.
This is a sample-validated interpretation of the stable-particle criterion,
not full ATLAS fiducial equivalence. The MET is still the vector sum of
stable neutrinos, whereas ATLAS 2024 section 7.1 describes visible final-state
objects. Prompt leptons are identified through the local direct hard-W
lineage rule; prompt photons and b-hadron ghosts use generator-history
predicates. These are implementation-specific approximations, not validated
as identical to the experiment. The sample covers EW W+W+jj only, not both
charge channels. The histograms/cutflows are unweighted counts, not cross
sections. No generator, physics cuts or detector assumptions changed.

## Recorded integration result

Both runs processed 10,000 events and passed. In order, the 12 cutflow
counts are:

- status1: 10000, 3258, 3258, 3225, 3225, 3115, 2729, 2648, 2540, 2246, 772, 339.
- lifetime: 10000, 3258, 3258, 3225, 3225, 3115, 2729, 2648, 2539, 2248, 772, 339.

Both agree with their independent checkpoint, including the same 339
indices across modes, all eight selected observables, and exact bin
contents of all 13 histograms. The selected lifetime-vs-status1 shifts
match selected_summary.json; see lifetime.verify.json. The overwrite
guard rejected the historical ROOT path without changing its checksum.

The initial verifier stopped on unsupported PyROOT TFile attribute access,
before testing any data. It was switched to TFile.Get and rerun against the
already produced status1.root; no analysis changes or rerun were needed.
The lifetime run then proceeded with run.sh lifetime. No physics regression
failed. The build produced deprecation warnings from HepMC2 headers.
