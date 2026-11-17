# EW vs QCD W+W+jj at 13 TeV — supervisor comparison

Both samples use the existing `build/particle_level lifetime` executable and
its 12-stage particle-level selection. The distributions use the final
`fiducial_events` trees.

| Sample | Analyzed | Selected | Acceptance ± MC stat | Generator σ ± integration [fb] | Fiducial σ ± MC stat [fb] |
|---|---:|---:|---:|---:|---:|
| EW subset | 10,000 | 1,971 | 19.71 ± 0.40 percentage points | 10.42576 ± 0.01079 | 2.0549 ± 0.0415 |
| Archived QCD run_04 | 10,000 | 339 | 3.39 ± 0.18 percentage points | 8.95104 ± 0.02036 | 0.3034 ± 0.0162 |

The cited MC errors are binomial acceptance uncertainties at fixed generator
cross section. The propagated integration terms are 0.00213 fb (EW) and
0.00069 fb (QCD). The two-column figure shows unit-area densities and
differential fiducial cross sections. Bin-width-weighted integrals equal unity
or the reported fiducial cross section. Plot error bars are per-bin MC
statistics. The generator integration errors, theory uncertainties and
bin-to-bin correlations are outside the plot error bars.

The EW 50k generator result still differs from the EW pilot by **+6.05%**.
The cause remains unresolved, so the EW cross-section normalization should be
treated as provisional. No PDF/scale variations were included. The comparison
uses the two LO samples, without EW–QCD interference, and the existing
generator cuts and particle-level definition. It is a sample comparison
for the meeting, without an ATLAS validation claim.

## Provenance and checks

- The original compressed 50k LHE retained SHA-256
  `e53b4c8a94fb49a310e6ff3b6000735ae8671cd77cd981a8c4e374306cc35df4`.
  The corrected full copy removes 48,338 blank gaps after event headers
  while preserving the banner, init and the remaining bytes of each record.
  The installed Pythia 8.317 reader parsed all 50,000 records and matched
  each declared particle count, PDG sequence, process ID and nominal weight.
  The initial preparation check assumed 10–11 particles based on the earlier
  small probe. There are also 164 valid 12-particle records in the full file.
  The final validation follows the actual NUP in every event.
- The standalone subset is a uniform draw without replacement using Python
  seed 314161, stored in source order. Its two event-count fields say 10000.
  The integrated cross section and nominal average weight are inherited from
  the 50k parent. Pythia 8.317 with seed 314160 accepted all 10,000 events and
  the HepMC2 file contains 10,000 sequential records. The nominal weight is
  0.010425765 pb and the sum of per-event merging-slot weights is 0.010425765 pb.
  Pythia logged 13 ISR stuck-loop messages, one fragmentation failure with
  retry, MPI/ISR weight warnings and SLHA warnings. Final counts and
  normalization checks passed.
- The QCD archive is `sm_wpwpjj_leptonic/Events/run_04`. Its original command
  places the order restriction after the comma, which makes its label
  misleading. The 160 archived exported decay-combined diagrams have QCD=2,
  QED=4. The archived HepMC2 output contains 12 event number sequences, totaling
  10,000 records. Both weight slots repeat the constant nominal LHE value,
  so the QCD contribution used here is `XSECUP/10000`, independently
  checked against the terminal HepMC cross-section field.
- Both ROOT files have 14 readable objects. Final cutflow and tree counts
  match, entries have distinct sequential input indices, and the saved
  kinematics pass the fiducial checks available from the tree.

## Files for the meeting

- `ew_vs_qcd_fiducial.pdf` and `ew_vs_qcd_fiducial.png`
- `cutflow.csv`, `comparison.json`, and `distributions.csv`
- `analysis/ew_wpwpjj_10000_lifetime.root` and
  `analysis/qcd_wpwpjj_archived_run04_lifetime.root`
