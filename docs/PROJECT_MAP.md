# Project map

## Canonical SM navigation

Only these four sample identities are active. Each path is a relative
symlink to the original artifact; production cards, inputs and logs remain
at their existing locations. `results/samples.json` keeps the original
`artifacts` references and adds `canonical_paths` for navigation.

| Sample | HepMC input | Final ROOT reference |
|---|---|---|
| EW + | [ew_plus.hepmc](../data/sm/hepmc/ew_plus.hepmc) | [ew_plus.root](../data/sm/root/ew_plus.root) |
| EW - | [ew_minus.hepmc](../data/sm/hepmc/ew_minus.hepmc) | [ew_minus.root](../data/sm/root/ew_minus.root) |
| QCD + | [qcd_plus.hepmc](../data/sm/hepmc/qcd_plus.hepmc) | [qcd_plus.root](../data/sm/root/qcd_plus.root) |
| QCD - | [qcd_minus.hepmc](../data/sm/hepmc/qcd_minus.hepmc) | [qcd_minus.root](../data/sm/root/qcd_minus.root) |

The QCD negative link resolves to `tag_1_pythia8_events_verified.hepmc`;
the empty file without `_verified` is not an input. Original data are local
and ignored by Git; tracking the links does not distribute those data.
New analysis outputs must use fresh destinations outside the canonical links.
See [historical results](HISTORICAL_RESULTS.md) for other event/ROOT files.

```text
collider_to_cosmos/
├── data/sm/              Eight canonical HepMC/ROOT navigation links
├── analysis/             Active particle-level source and retained ROOT macros
├── archive/              Selected historical studies, diagnostics and snapshots
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

`archive/` now contains only the individually reviewed historical files listed
below. Original production directories and canonical data targets remain in
place. `analysis/pythia8312/` remains untouched and is not an active build
dependency; its future disposition is unresolved.

| Location | Classification and dependencies |
|---|---|
| [CMakeLists.txt](../CMakeLists.txt) | C++17; builds only `particle_level`; finds ROOT and uses FastJet in `CONDA_PREFIX`. Pythia 8.317, its XML data, and HepMC2 are resolved under the top-level `MG5_aMC_v3_8_0/HEPTools`. |
| [particle_level.cpp](../analysis/particle_level.cpp) | Maintained entry point, with `lifetime` as default and `status1` for regressions. Includes [reconstruction.h](../validation/lifetime_impact/reconstruction.h) by relative path. |
| [archived lhe_reader.cpp](../archive/lhe_study/lhe_reader.cpp), [ROOT macros](../analysis/macros/fiducial_shapes.C) | Historical LHE and plotting utilities. The LHE reader is not a CMake target. The release comparison reads the four selected ROOT trees via the production Python script. |
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

## Active analysis and reviewed historical archive

```text
analysis/
├── particle_level.cpp
├── plot_cutflow.C
├── macros/fiducial_shapes.C
└── pythia8312/                 Unchanged historical software tree; HOLD
archive/
├── lhe_study/                 LHE source and seven nonempty historical plots
├── lhe_reader_diagnostics/    Three probe sources, three LHE fixtures and diff
└── particle_level_snapshot/  Historical executable and ROOT; pairing unverified
```

See each archive README and [historical results](HISTORICAL_RESULTS.md) for
original paths and provenance limits. The active CMake target still compiles
`analysis/particle_level.cpp` and includes
`validation/lifetime_impact/reconstruction.h`. No CMake or analysis change
was required. Archived source files were moved byte-for-byte, not rewritten.
The archived stream probe retains an obsolete relative input path; its new
location is not a ready-to-run reproduction environment.

Use explicit input paths for `analysis/macros/fiducial_shapes.C`. The retained
`analysis/plot_cutflow.C` still uses a working-directory-relative input; use
the documented isolated temporary-link workflow. Both macros remain in place.
`plots/cutflow.png` retains the exact historical image whose duplicate in
`analysis/` was removed. No active workflow uses the removed test binaries.
