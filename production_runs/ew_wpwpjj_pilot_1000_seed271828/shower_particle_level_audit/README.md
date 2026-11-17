# EW W+W+jj pilot shower and particle-level audit

The central sample is complete through particle level. The existing shower
artifact was reused after matching the LHE path, Pythia 8.317, seed 271829 and
HepMC scaling to the prepared card. Both copies of the card are identical.
The compressed LHE checksum matches the independently verified pilot.
Pythia reported 1,000 accepted events and a string fragmentation failure
followed by a retry message. The HepMC 2.06.09 listing has 1,000 sequential
events (0–999), physical units GEV/MM and a closing marker.

## LHAPDF

The generation log reports the import failure after the central LO run summary
and before storing parton-level results. The central run card uses built-in
`nn23lo1`. The standalone shower executable runs without the LHAPDF Python
interface. MG5's configured `lhapdf-config` correctly points to LHAPDF 6.5.5.
The pilot ran under system Python 3.12.3, while the extension is installed for
CPython 3.14 and requires `libpython3.14.so.1.0` and `libLHAPDF.so`. With the
package on the module path, the system interpreter still fails to find
`libpython3.14.so.1.0`. With the matching interpreter and package path but
without the library path, the import fails to find `libLHAPDF.so`.

Run `bash python_with_lhapdf.sh <python arguments>` from this directory to use
the existing `hep` Python 3.14.7 with process-local search paths. The smoke test
used MG5's actual `import_python_lhapdf` helper and evaluated PDF ID 247000,
the set identified in the LHE header, at x=0.1 and Q=100 GeV. The optional
PDF/scale weights remain uncomputed. The smoke test covers binding and PDF
access. Full systematics reweighting remains unverified.

## Analysis and normalization

From the repository root, the existing `build/particle_level` executable was run
in `lifetime` mode with the HepMC file in the sibling `shower/` directory and
`ew_wpwp_1000_seed271828_lifetime.root` in this directory. The exact input and
output paths are recorded in the logs. ROOT reopened successfully and all 14
objects were readable. The final cutflow count and tree agree at 209, the event
indices are unique and in range, and all stored selected kinematics are finite
and pass the thresholds testable from the tree.

- Generator LHE `XSECUP`: 0.009830870 ± 0.00007025674 pb (9.830870 ±
  0.07025674 fb, integration uncertainty).
- LHE `IDWTUP=-4`, `event_norm=average`. HepMC `Weight` carries the constant
  nominal value 0.0098308702. The per-event cross section is
  `Weight_MERGING=0.000` = 9.8308702e-6 pb. The sum over 1,000 equals
  0.0098308702 pb and matches both the last cumulative HepMC C record and
  the generator normalization within rounding. The Pythia inclusive result
  is 9.83087020e-12 mb, consistent after multiplying by 1e9 pb/mb.
- Fiducial fraction: 209/1,000 = 20.9%. The selected contribution sums to
  0.0020546519 pb (2.05465 fb). The independent-event binomial approximation
  gives 0.12640 fb from acceptance statistics. The integration uncertainty
  propagated at fixed acceptance is 0.01468 fb. Neither term includes theory
  variations. The interface's inclusive quadrature of event weights in the
  shower log is a separate convention from the generator integration error.
- ROOT histograms contain event counts. Cross-section normalization was
  applied in this audit, without an additional W branching-ratio factor.

The cutflow and checksums are in `verification.json`. The source, executable,
CMake settings and input LHE were verified against the previous audit's hashes.
The fiducial result is preliminary and specific to the existing implementation
and generator cuts. The prior ATLAS definition audit and generator-cut
dependence still limit any comparison.
