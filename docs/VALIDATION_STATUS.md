# Validation status

## Completed for this baseline

- The four HepMC inputs each supply 10,000 read events, and the release
  comparison checked the 12-stage ROOT cutflows against the selected-event
  trees. Count, weight and sample provenance are in the [results index](RESULTS_INDEX.md).
- EW exports verify QCD=0, QED=4 in production and QCD=0, QED=6 after both
  direct leptonic decays. The archived QCD exports have QCD=2, QED=4 after
  decays. Both charges are represented.
- [Manual EW W⁻W⁻ LHE validation](../production_runs/ew_wmwmjj_manual_analysis_seed161805/lhe_verification.json)
  records original SHA-256
  `5420336fbc0e887adad5c785df5dff584eee21d5aa13c803fef1ba6f1cb9091e`.
  The separate shower copy removes 5,038 blank gaps between event headers
  and particle records. Particle data, weights and cross-section metadata
  are preserved. The installed 8.317 reader matched all 10,000 records.
- The [strict tagging-jet regression](../validation/strict_tagging_pt/README.md)
  documents the change to strict tagging thresholds. Earlier validation
  notes should be read with their source version and sample identity.
- The curation pass compared stored artifacts, paths and checksums. It
  performed no simulation, event analysis, or physics-definition changes.

## Open issues and limits

1. **EW W⁺W⁺ normalization.** Pilot 9.830870 fb versus parent 10.425760 fb
   is a +6.05% shift, 8.37 times the quadrature sum of the reported
   integration errors. Cards and exported physics agree. The 50k run
   integrated more phase-space points and ran a second refinement; pilot
   underconvergence is plausible and unproven. Absolute EW positive-charge
   and charge-summed rates are provisional. [Pilot comparison](../production_runs/ew_wpwpjj_50000_seed314159/pilot_comparison.json)
2. **ATLAS fiducial audit incomplete.** Quantities are particle-level and
   ATLAS-inspired. The existing §7.1 comparison records a different MET
   construction and remaining prompt-object, overlap, third-lepton and
   b-hadron definition questions. The strict tagging pT regression addresses
   one historical item. Experimental extraction, detector response and
   acceptance corrections are outside this baseline. [Local fiducial audit](../validation/atlas_fiducial_audit/README.md) · [ATLAS JHEP 04 (2024) 026](https://doi.org/10.1007/JHEP04(2024)026)
3. **Theory coverage.** LO separately generated EW and QCD-induced direct
   e/μ samples with generation cuts. EW–QCD interference and PDF/scale
   systematics are missing. Numerical integration and MC acceptance errors
   are reported separately. Generator-cut independence and any equivalence
   to a full off-shell leptonic process remain unverified.
4. **Archived shower provenance.** The QCD samples are validated for reuse
   from existing HepMC. Effective QCD shower seeds are unverified. QCD
   W⁺W⁺ was assembled from 12 numbering sequences with archived scaling;
   the checked-in-place nominal card alone does not reconstruct the actual
   chunk invocations. Preserve the archived outputs and logs.
5. **Historical failures.** Retain the incomplete [3,031-event EW W⁻W⁻](../production_runs/ew_wmwmjj_10000_seed161803/README.md)
   run, the blank-line parser probes and the [pilot LHAPDF diagnostic](../production_runs/ew_wpwpjj_pilot_1000_seed271828/shower_particle_level_audit/README.md).
   The pilot workaround verifies the LHAPDF import and PDF access only.
