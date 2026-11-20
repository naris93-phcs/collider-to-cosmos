# Four-sample results index

Preliminary Standard Model LO same-sign WWjj baseline at 13 TeV, using resonant W decay chains to direct e/μ and the common `lifetime` particle-level selection. The curated figures and tables are exposed in [results/](../results/README.md). The [comparison code](../production_runs/ew_wmwmjj_manual_analysis_seed161805/compare.py) uses the four selected-event trees.

| Sample | Generated parent | Read in ROOT | Selected | Acceptance ± MC stat. (percentage points) | Generator ± integration (fb) | Fiducial ± MC stat. (fb) | Fiducial propagated integration (fb) |
|---|---:|---:|---:|---:|---:|---:|---:|
| EW W⁺W⁺jj | 50,000 | 10,000 | 1,971 | 19.71% ± 0.40 | 10.425760 ± 0.010794 | 2.054917 ± 0.041475 | 0.002127 |
| EW W⁻W⁻jj | 10,000 | 10,000 | 1,848 | 18.48% ± 0.39 | 4.076655 ± 0.010757 | 0.753366 ± 0.015823 | 0.001988 |
| QCD W⁺W⁺jj | 10,000 | 10,000 | 339 | 3.39% ± 0.18 | 8.951040 ± 0.020357 | 0.303440 ± 0.016199 | 0.000690 |
| QCD W⁻W⁻jj | 10,000 | 10,000 | 291 | 2.91% ± 0.17 | 3.870572 ± 0.009298 | 0.112634 ± 0.006506 | 0.000271 |

| Charge sum | Fiducial (fb) | MC stat. (fb) | Propagated generator integration (fb) |
|---|---:|---:|---:|
| EW | 2.808283 | 0.044390 | 0.002912 |
| QCD | 0.416074 | 0.017457 | 0.000741 |

Acceptance is selected/analyzed. The MC uncertainty is binomial at fixed generator cross section. The propagated numerical integration uncertainty is acceptance times the generator integration error. The charge sums add the independent sample uncertainties in quadrature per component. These uncertainties exclude PDF/scale and modeling systematics.

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

## Sample provenance

### EW W⁺W⁺jj

- Production `QCD=0 QED=4`; decay-combined amplitudes `QCD=0 QED=6`. MadGraph seed `314159` (banner). Subset selection seed `314161`. Pythia seed `314160`.
- [original lhe](../production_runs/ew_wpwpjj_50000_seed314159/sm_ew_wpwpjj_leptonic/Events/ew_wpwp_50000_seed314159/unweighted_events.lhe.gz) · [banner](../production_runs/ew_wpwpjj_50000_seed314159/sm_ew_wpwpjj_leptonic/Events/ew_wpwp_50000_seed314159/ew_wpwp_50000_seed314159_tag_1_banner.txt) · [shower lhe](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/ew_wpwpjj_10000_subset.lhe.gz) · [shower card](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/shower/pythia8.cmd) · [shower log](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/shower/shower.log) · [hepmc](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/shower/ew_wpwpjj_10000_pythia8317.hepmc) · [root](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/analysis/ew_wpwpjj_10000_lifetime.root) · [validation](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/README.md).

### EW W⁻W⁻jj

- Production `QCD=0 QED=4`; decay-combined amplitudes `QCD=0 QED=6`. MadGraph seed `161804` (banner). Pythia seed `161805`.
- [original lhe](../production_runs/ew_wmwmjj_manual/Events/run_01/unweighted_events.lhe.gz) · [banner](../production_runs/ew_wmwmjj_manual/Events/run_01/run_01_tag_1_banner.txt) · [shower lhe](../production_runs/ew_wmwmjj_manual_analysis_seed161805/ew_wmwmjj_10000_format_corrected.lhe.gz) · [shower card](../production_runs/ew_wmwmjj_manual_analysis_seed161805/cards/pythia8.cmd) · [shower log](../production_runs/ew_wmwmjj_manual_analysis_seed161805/shower/shower.log) · [hepmc](../production_runs/ew_wmwmjj_manual_analysis_seed161805/shower/ew_wmwmjj_10000_pythia8317.hepmc) · [root](../production_runs/ew_wmwmjj_manual_analysis_seed161805/analysis/ew_minus_lifetime.root) · [validation](../production_runs/ew_wmwmjj_manual_analysis_seed161805/README.md).

### QCD W⁺W⁺jj

- Production `QCD=2 QED=2`; decay-combined amplitudes `QCD=2 QED=4`. MadGraph seed `42` (banner). Effective Pythia seed unverified in the retained card/log.
- [original lhe](../sm_wpwpjj_leptonic/Events/run_04/unweighted_events.lhe.gz) · [banner](../sm_wpwpjj_leptonic/Events/run_04/run_04_tag_1_banner.txt) · [shower card](../sm_wpwpjj_leptonic/Events/run_04/tag_1_pythia8.cmd) · [shower log](../sm_wpwpjj_leptonic/Events/run_04/tag_1_pythia8.log) · [hepmc](../sm_wpwpjj_leptonic/Events/run_04/tag_1_pythia8_events.hepmc) · [root](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/analysis/qcd_wpwpjj_archived_run04_lifetime.root) · [validation](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/README.md).

### QCD W⁻W⁻jj

- Production `QCD=2 QED=2`; decay-combined amplitudes `QCD=2 QED=4`. MadGraph seed `314161` (banner). Effective Pythia seed unverified in the retained card/log.
- [original lhe](../sm_wmwmjj_leptonic/Events/run_01/unweighted_events.lhe.gz) · [banner](../sm_wmwmjj_leptonic/Events/run_01/run_01_tag_1_banner.txt) · [shower lhe](../sm_wmwmjj_leptonic/Events/run_01/shower_8317/prepared.lhe.gz) · [shower card](../sm_wmwmjj_leptonic/Events/run_01/shower_8317/production.cmd) · [shower log](../sm_wmwmjj_leptonic/Events/run_01/shower_8317/production.log) · [hepmc](../sm_wmwmjj_leptonic/Events/run_01/tag_1_pythia8_events_verified.hepmc) · [root](../production_runs/ew_wmwmjj_10000_seed161803/analysis/qcd_minus_lifetime.root) · [validation](../production_runs/ew_wmwmjj_10000_seed161803/analysis/qcd_minus_hepmc_verification.json).

## Validation and interpretation

- **EW W⁺W⁺** uses a reproducible 10,000-event draw from the 50,000-event parent. Formatting-only repair and reader checks are recorded in the production audit. Its parent normalization remains provisional because of the unresolved +6.05% pilot difference.
- **EW W⁻W⁻** uses the successful manual run. The analysis made a format-only LHE copy and removed 5,038 blank gaps. The original digest and the reader validation are recorded. The preceding 3,031-event run is excluded.
- **QCD W⁺W⁺** and **QCD W⁻W⁻** use archived events. Although the banner text puts the EW order restriction after the decay-chain comma, each export has 160 diagrams labeled `QCD=2, QED=4` with leptonic decays. The corresponding production order follows after subtracting the two EW decay vertices. The exact per-chunk cards/seeds of QCD W⁺W⁺ remain unverified. The retained card is a reference and the log records the actual chunk settings.
- The EW and negative QCD files have sum-normalized per-event HepMC contributions. In the archived positive QCD file both slots repeat the nominal average weight, and event numbering restarts across 12 chunks. Here its per-event contribution is the verified LHE `XSECUP/10000`, checked against the terminal HepMC cross-section field.
- The existing comparison verified ROOT readability, all 12 cumulative cutflow bins, agreement with the selected tree, and stored kinematic thresholds. Legacy ROOT names and older figures have no role in the current release. See [validation status](VALIDATION_STATUS.md) for limits on interpreting this baseline.
