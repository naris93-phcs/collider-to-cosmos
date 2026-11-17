# EW W+W+jj, 50,000 events, MG5 seed 314159

Status: parton-level generation completed. Shower and ROOT analysis were held
after two unexpected findings at the LHE validation gate.

- The generator cross section is 0.01042576 ± 0.00001079378 pb
  (10.42576 ± 0.01079378 fb). The error is the MG5 integration error.
  The shift from the pilot is +0.59489 fb (+6.05%), or 8.37 times the
  quadrature sum of the two reported integration errors. The cause of this
  discrepancy is undetermined.
- 48,338 of 50,000 event records have a blank line between the event header
  and first particle row. The pilot had zero. The LHE field checks can read
  these records with blank-line handling. Compatibility of this file with
  the validated Pythia reader remains unverified. The original LHE is
  preserved byte-for-byte.

## Configuration and checks

An isolated copy of MG5_aMC 3.8.0 exported the pilot process with a new output
path. All 14 original Fortran matrix routines agreed with the pilot after
accounting for permutations of independent declarations and SAVE statements.
The production headers show QCD=0, QED<=4 and all 860 exported decay-combined
diagrams have QCD=0, QED=6. The LHE parameter block is identical to the pilot.
The LHE run block differs in event count, seed and `systematics_program=none`.
PDF, beam conditions, scales, cuts and decays agree with the pilot. The
Pythia 8.317 card was prepared with seed 314160.

The parton run used Python 3.12 and eight local MG5 workers.

```bash
python3 -B bin/generate_events ew_wpwp_50000_seed314159 --laststep=parton --nb_core=8 -f
```

The producer finished both refinements and combined 50,000 events. The
compressed stream and closing LHE tag were read. The final leptonic
composition is 12,444 ee, 25,116 eµ and 12,440 µµ. Each event has two positive
leptonic W parents and no ME tau decay. The 8,209 additional W− resonance
records have light-quark daughters.

The positive constant nominal XWGTUP is 0.010425765 pb in average
normalization with IDWTUP=-4. Dividing by 50,000 would give 2.085153e-7 pb
per event for the central prediction. Actual HepMC weights, fiducial
counts, acceptance and MC statistical error remain pending.

MG5 logged a Python escape-sequence warning, noninteractive terminal and
viewer messages, and an unchanged empty bypass_check parser warning.
PDF/scale reweighting was disabled explicitly and the LHE contains no
variation weights. The production retains the pilot's LO EW resonant decay
chain and generator cuts. The existing particle-level definition still has
the documented limitations relative to ATLAS, and dependence on generator
cuts remains untested.

`lhe_verification.json` records the LHE path and hash. `pilot_comparison.json`
records the banner and normalization comparisons. All monitored pilot
artifacts, analysis sources and the existing executable match their
pre-production hashes.
