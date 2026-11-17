# Two-event stable-particle reconstruction diagnostic

Scope: archived run_04 event numbers 0 and 2 only. Event 1 is read to reach
event 2 but is not inspected. No generation, showering, ROOT output or physics
selection is performed. The existing analysis and HepMC are unchanged.

The executable loads the bundled Pythia 8.317 XML via its constructor and uses
ParticleData::tau0(), in mm/c, with c*tau0 > 8.99377 mm. It never calls init()
or next(). Zero tau0 is accepted for an original status-1 particle only when
both its width and its available decay table are absent (intrinsically stable
species). All PDG IDs must be known. The XML and library hashes are recorded
in provenance.txt.

Candidates must have HepMC status 2, lifetime above threshold, production and
end vertices, one incoming parent and nonempty daughters. Same-PDG daughters
cause failure rather than interpretation as a physical decay. This uses the
bundled Pythia manual's statusHepMC definition: status 2 denotes a normal SM
hadron/tau/muon decay, excluding virtual intermediate/shower states:
MG5_aMC_v3_8_0/HEPTools/pythia8/share/Pythia8/xmldoc/ParticleProperties.xml,
lines 845-866. Status 4 beams and generator-specific history statuses are
not eligible replacements.

Only outermost candidates are retained; their entire descendant subtrees are
excluded, including intermediate long-lived daughters, photons and neutrinos.
Original status-1 particles outside those subtrees are retained. Original
HepMC pointers/ancestry are not modified. Duplicate barcodes, shared descendant
subtrees, ambiguous descendant vertices, nonfinal leaves and retained
ancestor-descendant pairs cause failure. Four-momenta use long-double sums;
each component must close within an absolute 1e-8 GeV, both locally for candidate
decays and globally for the two collections. No momentum adjustments are made.

## Commands executed

Working directory for all commands: /home/naris/projects/collider_to_cosmos.
The directory was absent before creation; no pre-existing files were replaced.

```bash
mkdir -p validation
mkdir validation/stable_particles
g++ -std=c++17 -O2 -Wall -Wextra -I MG5_aMC_v3_8_0/HEPTools/hepmc/include -I MG5_aMC_v3_8_0/HEPTools/pythia8/include validation/stable_particles/diagnostic.cpp -L MG5_aMC_v3_8_0/HEPTools/hepmc/lib -L MG5_aMC_v3_8_0/HEPTools/pythia8/lib -Wl,-rpath,/home/naris/projects/collider_to_cosmos/MG5_aMC_v3_8_0/HEPTools/hepmc/lib -Wl,-rpath,/home/naris/projects/collider_to_cosmos/MG5_aMC_v3_8_0/HEPTools/pythia8/lib -lHepMC -lpythia8 -o validation/stable_particles/diagnostic > validation/stable_particles/build.log 2>&1
env PYTHIA8DATA=/home/naris/projects/collider_to_cosmos/MG5_aMC_v3_8_0/HEPTools/pythia8/share/Pythia8/xmldoc validation/stable_particles/diagnostic sm_wpwpjj_leptonic/Events/run_04/tag_1_pythia8_events.hepmc MG5_aMC_v3_8_0/HEPTools/pythia8/share/Pythia8/xmldoc > validation/stable_particles/results.txt 2>&1
```

Do not rerun these redirections over the preserved outputs. For independent
reproduction, use fresh executable/log filenames in this diagnostic directory.
Compilation succeeded; warnings in build.log concern deprecated std::iterator
in the existing HepMC headers. results.txt contains both events and all PASS
markers. The difference is reconstructed minus original, ordered px,py,pz,E.

| Event | Original | Decayed candidates | Nested | Roots | Removed status-1 | Reconstructed |
|---|---:|---:|---:|---:|---:|---:|
| 0 | 362 | 8 | 0 | 8 | 18 | 352 |
| 2 | 575 | 26 | 2 | 24 | 59 | 540 |

Limitations: only two events tested. This is not an ATLAS fiducial analysis,
does not validate MET or other object prescriptions, and does not measure
acceptance changes. A same-PDG continuation or an unsupported topology in a
different event will require explicit investigation, not automatic handling.

## Full-sample extension (2026-10-29)

The previous source is preserved as diagnostic_two_event.cpp; diagnostic,
results.txt, build.log and provenance.txt are unchanged. The active source
now inspects every event, catches and reports each failure, and requires
exactly 10000 input events. Reconstruction and the 1e-8 GeV tolerance are
unchanged. Failure particle context is printed with PDG/barcode; event and
condition follow on the same line. Totals labelled passed exclude failed
events. Component maxima cover events reaching the global closure check.
New executable: diagnostic_full; logs: build_full.log, results_full.txt,
failures_full.txt. No selection, ROOT output, generation or shower is run.

Build uses the same include/library/rpath arguments above, with output
`-o validation/stable_particles/diagnostic_full`, logging to build_full.log
(the full build used -std=c++17 -O2). Execution uses the same two arguments
and PYTHIA8DATA above, replacing the executable with diagnostic_full and
redirecting stdout to results_full.txt and stderr to failures_full.txt.

This validates event-record reconstruction, not ATLAS fiducial equivalence
or the impact on selected events, jets or MET.

Full-sample outcome (exit 0; empty failures_full.txt):
```text
SUMMARY processed=10000 passed=10000 failed=0 original_passed=4654857 reconstructed_passed=4390656 excluded_status1_passed=433021 excluded_all_descendants_passed=527218
MAX_ABS_RESIDUAL_GEV 6.68562427641461454e-14 9.4035890185750759e-14 1.09301456774346661e-12 7.13207271019200562e-13
```
Event numbers in the merged input repeat; EVENT labels are not globally unique.
Use the ordinal EVENT entry as well as the stored event number for identification.
