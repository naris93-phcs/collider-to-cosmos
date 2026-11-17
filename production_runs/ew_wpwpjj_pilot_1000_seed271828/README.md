# Phase 2A: LO EW W+W+jj pilot

Status: matrix-element gate PASSED; 1,000-event central LHE pilot PASSED.
STOPPED before shower/analysis because the configured systematics step could
not load the Python LHAPDF interface. No environment repair was attempted.
No W-W- production, 50k production, analysis rebuild or selection change occurred.

## Process and matrix-element provenance

Installed MG5_aMC version 3.8.0 was copied to an isolated directory under this
pilot. Automatic updates were disabled in the copy. The existing installation
and existing samples were not used as output targets.

```text
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
define lp = e+ mu+
define vlp = ve vm
generate p p > w+ w+ j j QCD=0 QED=4, w+ > lp vlp
```

Exact command file: `cards/process.mg5` (lines 45-48 for flavours/process/output).
The production restriction is before the comma. Generated Fortran headers
record `QCD=0 QED<=4`. MG's single equality is a maximum-order restriction;
the actual exported diagrams establish the intended order, not syntax alone.

One grouped subprocess directory contains 14 matrix-element subprocess routines.
There are 860 exported diagrams: six routines with 86 diagrams and eight with
43. All 860 diagram labels read `QCD=0, QED=6`. Every coupling referenced in
the original matrix Fortran is EW; none carries QCD order. See
`matrix_element_verification.json`, `SubProcesses/P1_qq_wpwpqq_wp_lvl_wp_lvl/`
inside the new process, and `process.log`.
The earlier generation-stage log count of 776 is a different intermediate
count; 860 is the exported decay-combined diagram count used here.
Flavour-equivalent processes are grouped: these are not 14 distinct final-state
flavour measurements or a count of only literal VBS diagrams.

Two EW decay vertices add to the four production vertices: amplitude order
QCD=0,QED=6, LO cross-section order alpha^6. There is no QCD-induced production
or EW-QCD interference. EW non-VBS, gauge/Higgs and permitted triboson diagrams
are retained. No VBS-only diagram filter was imposed.

## Controlled configuration

Archived positive run_04 banner was the source of run and parameter cards.
`run_card_changes.diff` confirms only nevents=1000 and iseed=271828 changed.
The parameter card was retained. Beams: pp at 6500+6500 GeV. PDF: nn23lo1;
dynamic scale choice -1; event_norm=average; cut_decays=False; bwcutoff=15.
Generator jet cuts remain ptj=20 GeV, etaj=5, drjj=0.4; maxjetflavor=4.
The e/mu decay chains retain matrix-element spin correlations and exclude
ME tau decays. This is a resonant decay-chain approximation, not a demonstrated
full off-shell six-fermion equivalence. Generator-cut independence is untested.

Executed from the pilot directory:
```bash
python3 -B MG5_aMC_v3_8_0/bin/mg5_aMC cards/process.mg5 > process.log 2>&1
```
After the matrix-element gate and copying the run/parameter cards, executed
from `sm_ew_wpwpjj_leptonic`:
```bash
python3 -B bin/generate_events ew_wpwp_1000_seed271828 --laststep=parton --nb_core=1 -f > ../generation.log 2>&1
```
Generation logs confirm a single worker. These commands document the completed
run; do not rerun them over existing outputs. Reproduction must use a new path.

## LHE validation

File:
`sm_ew_wpwpjj_leptonic/Events/ew_wpwp_1000_seed271828/unweighted_events.lhe.gz`

- Exactly 1,000 events.
- Cross section: 0.009830870 pb = 9.830870 fb.
- Integration uncertainty: 0.00007025674 pb = 0.07025674 fb (~0.715%).
- This is generator-level, with generation cuts and e/mu decays, NOT fiducial.
- IDWTUP=-4; all 1,000 nominal XWGTUP values are +0.0098308702 in the
  average normalization convention. The tiny difference from XSECUP is rounding.
- Do not sum these nominal weights as cross-section contributions. For this
  constant-weight pilot each contribution is nominal_weight/1000 pb.
- No additional W branching-ratio factor should be applied.
- Flavours: 276 ee, 499 e-mu, 225 mu-mu; all charged leptons are positive.
- Each event has two status-2 W+ parents of the requested leptons/neutrinos.
- 157 events also have a status-2 W- with two light-quark daughters. This is a
  permitted hadronic resonance history in the full EW set, not a negative-charge
  leptonic sample or an event-by-event gauge-invariant VBS/non-VBS decomposition.
- No ME tau decays. All flavour and W-parent checks passed.
- The corrected process command is present in the LHE header.
- No blank line immediately after the event header in these 1,000 records.
  This does not establish Pythia compatibility; no shower was attempted.

Audit: `verify_lhe.py`, `lhe_verification.json` (includes compressed-file SHA256).
Banner: `sm_ew_wpwpjj_leptonic/Events/ew_wpwp_1000_seed271828/ew_wpwp_1000_seed271828_tag_1_banner.txt`.

## Stop condition and warnings

`generation.log` lines 92-94 reports failure to access Python LHAPDF and skips
systematics. Central generation completed and stored the LHE, but requested
PDF/scale variation processing did not complete. The central integration error
is not a PDF/scale/theory uncertainty. No installation, PYTHONPATH repair or
systematics disablement was attempted after this failure.

Prepared but NOT executed: `cards/pythia8.cmd` and `shower/pythia8.cmd`.
These retain archived baseline physics settings, set 1000 events, fresh HepMC
filename, original new LHE input, and explicit Random:setSeed=on,
Random:seed=271829. HEPMCoutput:scaling=1e9. A future serial run must verify
its actual Weight and Weight_MERGING conventions; no HepMC weights are claimed.
No HepMC, ROOT output, particle-level cutflow or selected count exists for this
pilot yet. The existing C++ executable was not run or rebuilt.

Preparation diagnostics retained for transparency:
1. Initial extraction of the process XML block retained CDATA delimiters.
   MG ignored these two unknown commands. The extracted original is preserved;
   cleaned cards remove only XML wrappers. All generated amplitudes passed
   independent checks before generation.
2. An optional attempt to append local execution preferences used a duplicated
   directory path and failed before writing anything. Generation used the
   generated/default configuration with explicit --nb_core=1. A missing browser
   warning and a bypass_check=[] parser warning were nonfatal. No claim of an
   applied run_mode override is made.
3. The first LHE checker assumed exactly two W records of either sign. The full
   EW set legitimately permits an additional hadronic W-. The checker now
   requires exactly two positive leptonic W parents and explicitly verifies
   additional W- light-quark daughters; no generated event was altered.

## Preservation and remaining physics work

`preservation_check.json` verifies unchanged hashes of particle_level.cpp,
reconstruction.h, CMakeLists.txt and build/particle_level. Existing QCD LHE,
HepMC and ROOT files were not output targets. Git status before/after and the
pre-existing analysis/CMake diff are recorded. New work is confined to this
pilot directory. `pilot_artifacts_sha256.json` records key artifact hashes.

The setup retains the baseline PDF, scales and shower settings, not the ATLAS
nominal generator configuration. The previously audited prompt-object, MET,
b-hadron and veto-definition limitations remain. Even successful showering
would only test the existing selection, not establish ATLAS agreement.

Recommendation: do NOT scale to 50,000 per charge yet. First resolve the
LHAPDF configuration with approval, explicitly decide the systematics workflow,
then shower and analyse this same pilot without changing the selection. Verify
counts, weight conventions, cutflow, selected kinematics and warnings. A later
separate W-W- pilot and production approval are needed before either 50k run.
