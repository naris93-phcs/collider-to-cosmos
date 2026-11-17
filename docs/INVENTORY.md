# Inventory snapshot

[Full file inventory (compressed TSV)](FILE_INVENTORY.tsv.gz). Logical bytes from lstat; symlinks are listed without following them. Git internals, protected agent/configuration directories and new curation directories are excluded. Git-tracked files, local ignored installations and run files are otherwise included. Directory allocation from du differs from these totals.

| Location | Entries | Logical bytes |
|---|---:|---:|
| `production_runs` | 45,583 | 6,427,131,379 |
| `sm_wpwpjj_leptonic` | 1,095 | 3,201,428,309 |
| `sm_wmwmjj_leptonic` | 992 | 2,338,458,751 |
| `MG5_aMC_v3_8_0` | 13,673 | 568,749,638 |
| `analysis` | 1,401 | 118,884,010 |
| `validation` | 153 | 78,461,094 |
| `sm_wpwpjj` | 1,046 | 38,281,236 |
| `(root)` | 14 | 33,544,006 |
| `references` | 5 | 4,190,526 |
| `build` | 29 | 2,523,369 |
| `plots` | 8 | 117,526 |
| `.vscode` | 2 | 552 |
| **Total** | **64,001** | **12,811,770,396** |

## Files over 100 MiB

| Path | Logical bytes |
|---|---:|
| `sm_wpwpjj_leptonic/Events/run_04/tag_1_pythia8_events.hepmc` | 2,294,641,869 |
| `sm_wmwmjj_leptonic/Events/run_01/tag_1_pythia8_events_verified.hepmc` | 2,240,132,169 |
| `production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/shower/ew_wpwpjj_10000_pythia8317.hepmc` | 2,162,421,555 |
| `production_runs/ew_wmwmjj_manual_analysis_seed161805/shower/ew_wmwmjj_10000_pythia8317.hepmc` | 2,097,060,666 |
| `sm_wpwpjj_leptonic/Events/run_04/tag_1_pythia8_events.hepmc.gz` | 833,569,192 |
| `production_runs/ew_wpwpjj_pilot_1000_seed271828/shower/ew_wpwp_1000_seed271828_pythia8317.hepmc` | 217,275,904 |
