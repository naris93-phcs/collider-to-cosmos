# From Collider Physics to the Early Universe

MSc research software for Standard Model same-sign WWjj production at 13 TeV,
with a later research direction in extended Higgs sectors and the electroweak
phase transition. The current baseline contains separately generated EW and
QCD-induced W⁺W⁺jj and W⁻W⁻jj, with 10,000 analyzed events per sample.

Start with the [curated results](../results/README.md) and
[four-sample provenance index](RESULTS_INDEX.md). The common analysis is the
existing particle-level `lifetime` selection. Rates are preliminary because
the EW positive generator discrepancy and the ATLAS fiducial audit remain
open. The baseline excludes interference and PDF/scale systematics.
[Validation status](VALIDATION_STATUS.md)

## Build and use

The workflow uses MadGraph5_aMC@NLO 3.8.0, Pythia 8.317, HepMC2, FastJet,
ROOT and C++17. With the local `hep` Conda environment active and HEPTools
in place, CMake 3.28 or newer can build the `particle_level` target.
[Environment, exact commands and isolated replay](REPRODUCIBILITY.md)

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="$CONDA_PREFIX"
cmake --build build --target particle_level
./build/particle_level lifetime /absolute/path/to/input.hepmc /absolute/path/to/new.root
```

See the [project map](PROJECT_MAP.md) and the [cleanup proposal](CLEANUP_REPORT.md).
This draft lives under `docs/` pending approval for a merge with the existing
root README. Its relative links are written for its present location.
