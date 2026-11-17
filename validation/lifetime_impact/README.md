# Preliminary lifetime impact checkpoint

Independent snapshot of the current analysis, not an integration or an ATLAS
fiducial-equivalence claim. Main analysis, earlier validation artifacts and
archived input remain unchanged. All counts are unweighted; the same input
weight is recorded and verified between modes, never used as a cross section.

Run from repository root: `bash validation/lifetime_impact/run.sh`, then
`/home/naris/miniforge3/envs/hep/bin/python -B validation/lifetime_impact/summarize.py`.
run.sh contains exact compilation/library/rpath/input commands and refuses
existing CSV/ROOT destinations. For fresh reproduction use a fresh checkpoint
copy; do not overwrite these artifacts. prepare.py documents the exact source
extraction/transformation; source.diff and source.sha256 identify the snapshot.
Do not rerun prepare.py over this checkpoint's generated sources.

reconstruction.h is extracted from the validated diagnostic (not a fresh
algorithm): retains lifetime lookup, physical-decay/history safeguards,
duplicate identities, full ancestor checks and local/global 1e-8 GeV closure.
Only reporting and return type change to return the reconstructed pointer set.
Pythia 8.317 XML is loaded without init/next; original HepMC graph is untouched.
Collection filtering follows the original event-record order in both modes,
not pointer-set iteration order. The input is streamed twice independently;
CSV sequence, stored event number and weight must match for every pair.

All downstream logic comes from analysis/particle_level.cpp unchanged:
anti-kT R=0.4, exclusion of muons/neutrinos, electron/photon inputs, all-record
last-b-hadron ghosts, electron-jet overlap removal and cumulative cuts.
Prompt direct-hard-W lineage and photon hadron-ancestry predicates are fixed.
Photon and neutrino availability reflects each stable collection; photons
removed under long-lived hadrons were excluded by the dressing ancestry filter
already. Thus reconstructed jet overlaps can still affect selected leptons.
The legacy oldDressing diagnostic is copied but is not the selection used here.

baseline.csv and lifetime.csv contain every event (one-based sequential index,
possibly repeated stored event number, weight, physical jet-input count and
barcodes excluding ghosts, MET components and magnitude, post-overlap/25-GeV
jet multiplicity, ordered leading jet pT, mjj and dy, twelve cumulative flags).
Missing leading/subleading jets have zero placeholders; summary.py excludes
undefined observables using multiplicity masks. Changed jet ordering is allowed;
leading/subleading are rank comparisons, not matched physical jets.

summary.json reports both cutflows, entering/leaving counts by stage, paired
mean and maximum differences. transitions.csv lists every event with any
cumulative flag change; full CSVs permit investigation of large observable
changes without a selection transition. The baseline is checked against the
previous mjj checkpoint: 339 selected entries and all eight tree observables.
impact.png/pdf show raw counts, MET and multiplicity for all events, mjj/dy
for each mode's >=2-jet subset. Plot ranges may have overflow; aggregate CSV
statistics include the full range. Nested selections are correlated.

Limitations: the MET remains the baseline all-stable-neutrino sum, not a
resolution of ATLAS MET semantics. Lifetime validation does not prove ATLAS
acceptance equivalence, generator correctness or uncertainty coverage. Closure
alone does not validate jets: this checkpoint measures their separate changes.
No main-analysis integration is authorized or performed.

Execution note: the first build found Conda Pythia 8.312 headers/libraries
before the bundled 8.317 and the lifetime lookup stopped at event 1 on XML
version mismatch. A loader-only retry exposed ABI incompatibility and stopped.
No results from these attempts are used. attempt1 preserves their surviving
artifacts. run.sh now places BOTH bundled Pythia headers and libraries before
Conda and explicitly sets the process-local loader path. Both branches are
rebuilt/rerun consistently. No persistent environment configuration changed.

Successful outcome: both branches processed exactly 10000 events; no lifetime
consistency failure or nonfinite reported observable. Baseline cutflow matched
all 12 expected stages, and its selected TTree matched the previous checkpoint
entry by entry for all eight observables (1e-9 tolerance). Final 339 sequential
indices are identical between modes, despite earlier-stage migrations.
selected_summary.json gives paired differences restricted to those 339 events.
significant_changes.csv records the top 20 absolute changes per observable;
the corresponding reproducible postprocessing is included in summarize.py.
Those supplementary outputs were initially computed after summarize.py and
then the identical postprocessing was appended for fresh reproduction.
