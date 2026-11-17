# Workflow script index

Scripts live with the runs they describe, since several resolve inputs and
exclusive output filenames relative to their own location.

| Stage | Script |
|---|---|
| MadGraph export / generation provenance | [Positive parent process card](../production_runs/ew_wpwpjj_50000_seed314159/cards/process.mg5), [generation log](../production_runs/ew_wpwpjj_50000_seed314159/generation.log) |
| Positive EW LHE repair and fixed subset | [prepare_lhe.py](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/prepare_lhe.py) |
| Manual negative EW LHE validation and repair | [prepare_lhe.py](../production_runs/ew_wmwmjj_manual_analysis_seed161805/prepare_lhe.py) |
| Pythia reader, shower, HepMC and lifetime analysis | [postprocess.sh](../production_runs/ew_wmwmjj_manual_analysis_seed161805/postprocess.sh), [C++ reader probe](../production_runs/ew_wmwmjj_manual_analysis_seed161805/verify_pythia_reader.cpp), [HepMC verifier](../production_runs/ew_wmwmjj_manual_analysis_seed161805/verify_hepmc.py) |
| Final four-sample comparison | [compare.py](../production_runs/ew_wmwmjj_manual_analysis_seed161805/compare.py) |
| Lifetime regression | [run.sh](../validation/lifetime_integration/run.sh), [verify.py](../validation/lifetime_integration/verify.py) |

See [reproducibility](../docs/REPRODUCIBILITY.md) for path and output guards.

## Tool links

- LHE preparation for [EW negative](lhe/prepare_ew_minus_lhe.py) and [EW positive](lhe/prepare_ew_plus_subset.py)
- Validation for [HepMC](validation/verify_hepmc.py) and the [Pythia reader](validation/verify_pythia_reader.cpp)
- [Four-sample comparison](analysis/compare_four_samples.py)

These are relative links to the original scripts. For a replay, follow the
staging commands in the reproducibility guide. Several scripts resolve
paths relative to their own file and write into a run-specific directory.
