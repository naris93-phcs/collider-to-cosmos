# Configuration index

The authoritative generator settings are in the LHE banners. The separate
cards and effective shower logs are linked per sample in the
[results index](../docs/RESULTS_INDEX.md).

- EW positive generator [process](../production_runs/ew_wpwpjj_50000_seed314159/cards/process.mg5),
  [run](../production_runs/ew_wpwpjj_50000_seed314159/cards/run_card.dat) and
  [parameter](../production_runs/ew_wpwpjj_50000_seed314159/cards/param_card.dat) cards.
- EW negative manual [banner](../production_runs/ew_wmwmjj_manual/Events/run_01/run_01_tag_1_banner.txt),
  [process](../production_runs/ew_wmwmjj_manual/Cards/proc_card_mg5.dat),
  [run](../production_runs/ew_wmwmjj_manual/Cards/run_card.dat) and
  [parameter](../production_runs/ew_wmwmjj_manual/Cards/param_card.dat) cards.
- EW 10k Pythia cards for [W⁺W⁺](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/shower/pythia8.cmd)
  and [W⁻W⁻](../production_runs/ew_wmwmjj_manual_analysis_seed161805/cards/pythia8.cmd).
- Archived QCD Pythia [positive reference card](../sm_wpwpjj_leptonic/Events/run_04/tag_1_pythia8.cmd)
  and [negative effective production card](../sm_wmwmjj_leptonic/Events/run_01/shower_8317/production.cmd).
  The positive log records chunk-level overrides.

Central convention for the four files is LHE `event_norm=average`,
`IDWTUP=-4`, GeV kinematics and generator cross sections in pb.

## Card file links

- EW positive parent [process](ew_plus_parent/process.mg5), [run](ew_plus_parent/run_card.dat), [parameters](ew_plus_parent/param_card.dat)
- EW negative manual [process](ew_minus_manual/process.mg5), [run](ew_minus_manual/run_card.dat), [parameters](ew_minus_manual/param_card.dat)
- Pythia [EW positive](shower/ew_plus_10k.cmd), [EW negative](shower/ew_minus_10k.cmd), [QCD positive nominal](shower/qcd_plus_nominal_reference.cmd), [QCD negative production](shower/qcd_minus_archived_production.cmd)

The links expose the preserved cards. Replays use a fresh copy with their
own output paths and the working directory expected by the card.
