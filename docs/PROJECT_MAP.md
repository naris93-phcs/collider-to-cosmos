# Project map

```text
collider_to_cosmos/
├── analysis/             Maintained particle-level target and historical utilities
├── scripts/              Index of validated, in-place workflow scripts
├── configs/              Index of cards and effective shower configurations
├── results/              Four-sample index, checksums, plots and tables
├── docs/                 Methodology, validation, reproduction and cleanup proposal
├── production_runs/      EW provenance, simulations and run-local tooling
├── sm_* /                Original MadGraph process and QCD event directories
├── validation/           Regression evidence and live reconstruction header
├── references/           Local papers and reference index
├── MG5_aMC_v3_8_0/       Local installation and linked HEPTools
└── build/                Validated local executable and CMake outputs
```

An optional future `archive/` may hold retired tools and exploratory material
once their references and runtime dependencies have been checked. Current
paths remain authoritative.

| Location | Classification and dependencies |
|---|---|
| [CMakeLists.txt](../CMakeLists.txt) | C++17; builds only `particle_level`; finds ROOT and uses FastJet in `CONDA_PREFIX`. Pythia 8.317, its XML data, and HepMC2 are resolved under the top-level `MG5_aMC_v3_8_0/HEPTools`. |
| [particle_level.cpp](../analysis/particle_level.cpp) | Maintained entry point, with `lifetime` as default and `status1` for regressions. Includes [reconstruction.h](../validation/lifetime_impact/reconstruction.h) by relative path. |
| [lhe_reader.cpp](../analysis/lhe_reader.cpp), [ROOT macros](../analysis/macros/fiducial_shapes.C) | Historical LHE and plotting utilities. The LHE reader is not a CMake target. The release comparison reads the four selected ROOT trees via the production Python script. |
| [Pilot](../production_runs/ew_wpwpjj_pilot_1000_seed271828/README.md) | 1k EW positive-charge sample and LHAPDF diagnosis. Scientific evidence for the unresolved comparison with the 50k parent. |
| [EW W⁺W⁺ parent](../production_runs/ew_wpwpjj_50000_seed314159/README.md) and [10k subset](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/README.md) | Parent LHE, formatting correction, reproducible subset, and two positive-charge ROOT results. |
| [Incomplete EW W⁻W⁻](../production_runs/ew_wmwmjj_10000_seed161803/README.md) | Preserves the 3,031-event failed production and the **valid** archived QCD W⁻W⁻ analysis under the same directory. |
| [Manual EW process](../production_runs/ew_wmwmjj_manual/Events/run_01/run_01_tag_1_banner.txt) and [analysis](../production_runs/ew_wmwmjj_manual_analysis_seed161805/README.md) | Original manual input and separate corrected LHE, HepMC, ROOT and final comparison. |
| [Archived QCD W⁺W⁺ banner](../sm_wpwpjj_leptonic/Events/run_04/run_04_tag_1_banner.txt), [QCD W⁻W⁻ banner](../sm_wmwmjj_leptonic/Events/run_01/run_01_tag_1_banner.txt) | Original QCD provenance. The local names and old banner syntax can be misleading. Use the coupling-order evidence in [results index](RESULTS_INDEX.md). |
| [Validation](../validation/atlas_fiducial_audit/README.md) | Stable-particle, lifetime/jet and tagging-threshold checks, cutflow regressions, and ATLAS audit notes. Preserve source, logs, input identities and outputs together. |
| [References](../references/README.md), [workflow](../workflow.txt), [ROOT quick reference](../ROOT_QUICK_REFERENCE_Collider_to_Cosmos_v0.2.txt) | Papers and historical documentation. Some historical sample labels and output pointers predate the curated index. |
| [Results](../results/README.md) | Relative links to the validated plots and tables, sample metadata, and checksums. |

Top-level legacy plots, generic ROOT names in the root/build/analysis
directories, and `plots/` belong to earlier studies. Resolve their provenance
before citing or moving them. The legacy `sm_wpwpjj/` tree contains an earlier
production study. Run-local cards, logs, generated Fortran, repair scripts,
integration tables, and LHE/HepMC/ROOT versions are all provenance-bearing.

## Implemented organization

`scripts/{analysis,lhe,validation}/` and
`configs/{ew_plus_parent,ew_minus_manual,shower}/` expose relative links to
the original utilities and cards. The [legacy charge study](../plots/legacy_charge_study/README.md)
contains the two historical figures moved from the repository root. The
root [README](../README.md) starts with the current baseline and retains the
previous notes below it. The historical workflow and quick reference retain
their original paths and current edits.
