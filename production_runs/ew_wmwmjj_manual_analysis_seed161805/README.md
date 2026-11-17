# Preliminary four-sample same-sign WWjj comparison at 13 TeV

The manual EW W-W- LHE contains 10,000 complete events. Its SHA-256 is
`5420336fbc0e887adad5c785df5dff584eee21d5aa13c803fef1ba6f1cb9091e`.
The original remains untouched. The format-only copy removes 5,038 blank
gaps after event headers. The Pythia 8.317 reader matches all 10,000 particle
counts, PDG sequences, process IDs and nominal weights. The exported manual
MadGraph diagrams all carry QCD=0, QED=6 (860 diagrams).

The shower card differs from the successful EW positive-charge 10k card
only in LHE path, output filename and shower seed (161805). Pythia reported
10,000 tried, selected and accepted, and the HepMC2 audit confirmed 10,000
complete sequential records with standard units and named weights. The nominal
weight is 0.0040766554 pb and the per-event contribution is
4.0766554e-7 pb, summing to the LHE central cross section within rounding.
The shower log includes SLHA, ISR/MPI and one energy-momentum conservation
warning, with the accepted count and normalization verified independently.

All four samples use the existing lifetime-mode particle-level selection.
The EW W-W- ROOT file reopens cleanly, and its 1,848 selected tree entries
agree with the final cutflow bin. The differential distributions use the
fiducial trees, common bin edges, and bin-width normalization.

| Sample | Selected / 10,000 | Acceptance | Generator [fb] ± integration | Fiducial [fb] ± MC stat |
|---|---:|---:|---:|---:|
| EW W+W+ | 1,971 | 19.71% | 10.42576 ± 0.01079 | 2.05492 ± 0.04147 |
| EW W-W- | 1,848 | 18.48% | 4.076655 ± 0.010757 | 0.75337 ± 0.01582 |
| QCD W+W+ | 339 | 3.39% | 8.95104 ± 0.02036 | 0.30344 ± 0.01620 |
| QCD W-W- | 291 | 2.91% | 3.870572 ± 0.009298 | 0.11263 ± 0.00651 |

Charge-summed fiducial cross sections are 2.80828 ± 0.04439 fb (EW) and
0.41607 ± 0.01746 fb (QCD), with independent sample MC errors added in
quadrature. MC errors are binomial acceptance uncertainties at fixed generator
cross section. The propagated generator integration component is separately
tabulated (0.00199 fb for EW W-W-). The charge-sum integration terms assume
independent numerical errors and do not incorporate model systematics.

Absolute EW normalization is preliminary because the EW W+W+ pilot-to-parent
generator discrepancy (+6.05%) is unresolved. The ATLAS fiducial audit is
incomplete. EW–QCD interference and PDF/scale systematics are absent.

The meeting outputs are four_sample_fiducial.pdf/png, cross_sections.csv,
cutflow.csv, distributions.csv and comparison.json. The new EW W-W- ROOT
is analysis/ew_minus_lifetime.root. The other three ROOT inputs remain in
their original analysis directories as recorded in compare.py.
