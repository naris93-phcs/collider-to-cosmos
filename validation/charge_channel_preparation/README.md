# SM EW W−W−jj channel preparation

**GO for the first 10,000-event production after approval.** No generation,
showering, build or analysis was executed for this preparation. Only this
report and four new cards were created. The existing positive sample and
all prior outputs are unchanged.

## Charge audit

| Item | Existing implementation and implication |
|---|---|
| Hard W and ancestry | [particle_level.cpp:30](../../analysis/particle_level.cpp#L30) accepts abs(PDG ID)=24 with hard statuses 21–29. [Lines 72–117](../../analysis/particle_level.cpp#L72) follow same-signed W and same-PDG lepton copies; either W sign works. The direct lineage excludes tau intermediates, as for the positive sample. |
| Charged leptons and sign | [Lines 575–591](../../analysis/particle_level.cpp#L575) accept abs(ID)=11 or 13. PDG +11/+13 are e−/μ−, whereas −11/−13 are e+/μ+. [Lines 876–891](../../analysis/particle_level.cpp#L876) require exactly two and a positive PDG-ID product; this accepts both ++ and −−. The executable does not distinguish or reject mixed input samples by channel. |
| MET and jet inputs | [Lines 603–628](../../analysis/particle_level.cpp#L603) use abs(ID)=12/14/16 in the neutrino sum, including antineutrinos with the same kinematic treatment. Jet exclusions are sign-independent. No ATLAS MET correction is implied. |
| Dressing, overlap, b veto | [Lines 703–735](../../analysis/particle_level.cpp#L703) dress either sign with eligible photons; electron overlap and ee veto use abs(ID), and open-bottom flavour checks use abs(PDG ID). Anti-kT, ghosts, vetoes and VBS cuts are unchanged. |
| Stable mode | [Lines 272–293](../../analysis/particle_level.cpp#L272) retain default lifetime and explicit status1. Lifetime lookup/reconstruction already handled signed PDG IDs in the positive sample and makes no W-sign selection. |
| Weights | Histograms and cutflow are raw counts; the analysis does not use HepMC weights to estimate a cross section. Do not infer equal rates for W−W− and W+W+ in pp collisions. |
| Naming | The no-argument HepMC path points to positive run_04; default ROOT names encode mode only. The TTree title at line 402 is a historical W+W+ string, not a channel classifier. Use **explicit negative input and charge-qualified output** as below; identify the channel by the process card, run directory and filename, never by that title. |

**Production code changes: none.** The existing physics logic already
supports W−W−. No new positive-channel regression was needed; the latest
[strict-tagging lifetime regression](../strict_tagging_pt/comparison.json)
processed 10,000 events, retained all 12 cutflow counts and the same 339
ordered indices. No dedicated W−W− HepMC fixture was found in the local
sample inventory. Negative execution and generation remain untested.

## Prepared cards

The source is the [archived run_04 banner](../../sm_wpwpjj_leptonic/Events/run_04/run_04_tag_1_banner.txt)
(MG 3.8.0), rather than today's working Cards directory.

- [proc_card.mg5](cards/proc_card.mg5): archived MG5ProcCard with the same
  SM imports, subprocess options and p/j definitions. The generation line
  is exactly `generate p p > w- w- j j, w- > lm vlm QED=4 QCD=0`,
  with `lm = e- mu-` and `vlm = ve~ vm~`. This mirrors the full original
  decay-chain syntax and both explicit coupling-order restrictions
  (banner lines 80–83), not merely a standalone QCD restriction.
- [run_card.dat](cards/run_card.dat): archived MGRunCard, with only the
  integration/unweighting seed changed (42 → 314161). 10,000 requested
  events, 6500 GeV per proton beam, active PDF nn23lo1, dynamic scales
  with choice −1 and scale factor 1, generator cuts and systematics
  settings remain unchanged. In particular ptj=20 GeV, etaj=5, drjj=0.4,
  cut_decays=False; the nominal decay-product cut parameters must not
  be mistaken for the analysis fiducial cuts. event_norm=average remains.
- [param_card.dat](cards/param_card.dat): archived SLHA content, including
  the same masses and widths.
- [pythia8.cmd](cards/pythia8.cmd): byte-identical to archived run_04
  tag_1_pythia8.cmd, verified against its banner block. Use the bundled
  Pythia **8.317**, not the Conda 8.312. No tune, decay, MPI, shower,
  hadronization or random setting was added. The archived MPI-off line
  is a comment. The driver reads the gzipped LHE and writes HepMC2.

**Normalization caveat:** archived card conventions are retained, including
HEPMCoutput:scaling, but the commands below use a single direct shower
invocation rather than run_04's parallel split/merge. The MadEvent parallel
wrapper adjusts per-split scaling (madevent_interface.py:5082–5093);
do not assume identical raw per-event HepMC weights between workflows.
Retain the banner, LHE weights and shower statistics and check the
normalization before any weighted comparison. No such comparison is required
to produce or inspect the first unweighted cutflow.

## Commands for the first run — execute only after approval

Run in Bash with the existing local software. Start in the repository root.
The commands below write only into the new W−W− production directory.
The archived Pythia command writes a generic filename **inside that run**;
the final ROOT filename is explicitly charge-qualified. Do not resume by
rerunning over an existing directory.

```bash
set -euo pipefail
R=/home/naris/projects/collider_to_cosmos
cd "$R"
C="$R/validation/charge_channel_preparation/cards"
W="$R/production_runs/wmwmjj_10k_seed314161"
P="$W/sm_wmwmjj_leptonic"
NAME=wmwmjj_10k_seed314161
E=/home/naris/miniforge3/envs/hep

test ! -e "$W"
test ! -L "$W"
for card in proc_card.mg5 run_card.dat param_card.dat pythia8.cmd; do
  test -r "$C/$card"
done
command -v python3 gfortran cmake
df -h "$R"
df -i "$R"
# Check free space before continuing: raw HepMC alone was ~2.3 GB in run_04.
mkdir -p "$R/production_runs"
mkdir "$W"
cp -a "$R/MG5_aMC_v3_8_0" "$W/"
MG="$W/MG5_aMC_v3_8_0"
H="$R/MG5_aMC_v3_8_0/HEPTools"

cd "$W"
{ printf 'set auto_update 0\n'; cat "$C/proc_card.mg5"; } > production.mg5
"$MG/bin/mg5_aMC" production.mg5 > process.log 2>&1
test -x "$P/bin/generate_events"
cp "$C/run_card.dat" "$C/param_card.dat" "$P/Cards/"
cd "$P"
test ! -e "Events/$NAME"
./bin/generate_events "$NAME" --laststep=parton --nb_core=1 -f > "$W/generation.log" 2>&1

cd "$P/Events/$NAME"
test -s unweighted_events.lhe.gz
test "$(gzip -cd unweighted_events.lhe.gz | rg -c '^\s*<event(?:\s|>)')" -eq 10000
# Before showering: check the generated banner for the W− decay process,
# 10000 events, seed 314161, and the archived physics settings.
cp "$C/pythia8.cmd" .
test ! -e tag_1_pythia8_events.hepmc
export PYTHIA8DATA="$H/pythia8/share/Pythia8/xmldoc"
export LD_LIBRARY_PATH="$H/pythia8/lib:$H/hepmc/lib:$H/zlib/lib:$E/lib"
ldd "$H/MG5aMC_PY8_interface/MG5aMC_PY8_interface" > "$W/shower_libraries.log"
"$H/MG5aMC_PY8_interface/MG5aMC_PY8_interface" pythia8.cmd > "$W/shower.log" 2>&1
rg -q 'PYTHIA version 8\.317' "$W/shower.log"
test "$(rg -c '^E ' tag_1_pythia8_events.hepmc)" -eq 10000
tail -n 3 tag_1_pythia8_events.hepmc | rg 'END_EVENT_LISTING'

cd "$R"
export CONDA_PREFIX="$E"
cmake -S "$R" -B "$W/build" -DCMAKE_PREFIX_PATH="$E" -DCMAKE_BUILD_TYPE=Release > "$W/build.log" 2>&1
cmake --build "$W/build" >> "$W/build.log" 2>&1
"$W/build/particle_level" lifetime \
  "$P/Events/$NAME/tag_1_pythia8_events.hepmc" \
  "$W/wmwmjj_10k_seed314161_lifetime.root" > "$W/analysis.log" 2>&1
rg '^Events processed: 10000$' "$W/analysis.log"
```

Stop if the generated banner, software version, counts or output paths do
not match expectations. Inspect Pythia Tried/Selected/Accepted and warnings,
and verify that hard W− records and direct e−/μ− daughters are present before
interpreting the cutflow. The 339 positive-channel count is a regression
reference, not a target for this different physics sample.
