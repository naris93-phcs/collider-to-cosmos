# Collider to Cosmos

MSc research software for preliminary Standard Model same-sign WWjj
particle-level predictions at 13 TeV, with future Georgi–Machacek studies.

## Daily navigation

- Active C++ analysis: [analysis/particle_level.cpp](analysis/particle_level.cpp).
- Required reconstruction header:
  [validation/lifetime_impact/reconstruction.h](validation/lifetime_impact/reconstruction.h).
- Four HepMC inputs: [data/sm/hepmc/](data/sm/hepmc/).
- Four final ROOT references: [data/sm/root/](data/sm/root/).
- Sample names: `ew_plus`, `ew_minus`, `qcd_plus`, `qcd_minus`.

These eight paths are relative symlinks to preserved production files.
A Git checkout alone does not supply the ignored original datasets. Keep the
canonical ROOT files as references; write new analysis outputs elsewhere.

## Build and run

Use the existing `hep` environment, CMake >=3.28, C++17 and the bundled
Pythia 8.317/HepMC2 dependencies. ROOT/PyROOT and FastJet come from `hep`.
From the repository root:

```bash
source /home/naris/miniforge3/etc/profile.d/conda.sh
conda activate hep
export PYTHIA8DATA="$PWD/MG5_aMC_v3_8_0/HEPTools/pythia8/share/Pythia8/xmldoc"
export LD_LIBRARY_PATH="$PWD/MG5_aMC_v3_8_0/HEPTools/pythia8/lib:$PWD/MG5_aMC_v3_8_0/HEPTools/hepmc/lib:$CONDA_PREFIX/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
cmake -S . -B build/local -DCMAKE_PREFIX_PATH="$CONDA_PREFIX"
cmake --build build/local --target particle_level -j 4
mkdir -p results/replays
./build/local/particle_level lifetime \
    data/sm/hepmc/ew_plus.hepmc \
    results/replays/ew_plus_lifetime.root
```

Choose a fresh output filename; existing outputs are refused. This separate
build leaves the recorded `build/particle_level` executable in place.
Viewing existing ROOT results requires no build:

```bash
root -l data/sm/root/ew_plus.root
```

## Final results

- [Cross sections](results/cross_sections.csv), [cutflow](results/cutflow.csv),
  [binned distributions](results/distributions.csv) and [comparison](results/comparison.json).
- [Four-sample figure](results/four_sample_fiducial.pdf) and
  [physics gallery](results/physics_gallery/README.md).
- [Sample manifest](results/samples.json), [checksums](results/SHA256SUMS) and
  [provenance index](docs/RESULTS_INDEX.md).

Each sample has 10,000 analyzed events. Selected counts are 1,971 (EW+),
1,848 (EW−), 339 (QCD+) and 291 (QCD−). Absolute EW+ normalization remains
provisional because of the +6.05% pilot/parent discrepancy. The ATLAS
fiducial audit is incomplete; interference and PDF/scale systematics are
absent. See [validation status](docs/VALIDATION_STATUS.md).

## Provenance and history

Original `production_runs/` and `sm_*` directories stay in place.
[Configs](configs/README.md) and [workflow scripts](scripts/README.md) point
to their original cards and tools. MG5 installations and generated process
trees are software/provenance, not extra active SM datasets.

Use the [project map](docs/PROJECT_MAP.md),
[reproduction guide](docs/REPRODUCIBILITY.md) and
[historical results index](docs/HISTORICAL_RESULTS.md). The latter preserves
the previous README and distinguishes historical plots, regression
references and unidentified outputs. No historical artifacts were removed.
