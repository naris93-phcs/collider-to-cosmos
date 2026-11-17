# Repository cleanup proposal

## Audit outcome

The audit identified one maintained CMake target and four validated event-to-ROOT
chains. The working tree already contained modifications to CMake, the root
README, the ROOT quick reference, the analysis source and workflow notes,
plus untracked production, validation and plotting files. The curation adds
documentation, a manifest and relative result links. The pre-existing files
and tracked diff are preserved. See the [inventory](INVENTORY.md) for the
full file census and byte sizes.

| Class | Examples | Proposed disposition |
|---|---|---|
| Essential, live | `analysis/particle_level.cpp`, `CMakeLists.txt`, `validation/lifetime_impact/reconstruction.h`, top-level HEPTools, validated binary | Keep at existing paths. The validation header is a compile dependency and the HEPTools location is compiled into the executable. |
| Essential, scientific evidence | Four original LHEs and final HepMC/ROOT, banners, cards, exported diagrams, run logs and checks, manual formatting copy, positive 50k parent and 10k subset | Keep originals. Publish curated links and checksums. Retain archived per-run inputs even when a downstream representation can be reproduced. |
| Failed or exploratory, scientifically useful | EW negative 3,031-event run, pilot EW positive, QCD negative parser probes, `supervisor_10k/initial_preparation/` | Document and retain. The failed EW negative directory also contains the accepted QCD negative ROOT and metadata. Moving the whole directory would break the release. |
| Redundant representation, byte-verified | QCD positive `run_04/tag_1_pythia8_events.hepmc.gz` and uncompressed `tag_1_pythia8_events.hepmc` | The gzip stream decompresses byte-for-byte to the canonical plain input. Preserve both for now. A future archive policy could retain the gzip and materialize the plain file at its expected path when needed. |
| Candidates for retirement or external archive | `analysis/pythia8312/`, exploratory `analysis/{lhe_reader,pythia_test,reader_test,stream_test,nano stream_test.cpp}`, `mg_clean_test.lhe`, `test_one_event.lhe`, `test_no_mgrwt.lhe`, historical plots and generic `particle_level.root` files | The current CMake target uses Pythia 8.317 and the current release bypasses these older paths. The LHE fixtures and legacy outputs can be evidence. Check references and identities before proposing exact moves. |
| Apparent duplicates or rebuildable files | Three run-local MG5 installations, build object files, Python caches, AppleDouble and Zone.Identifier files, generated PLY tables `py.py` | Stage a manifest-led review. Run-local installs include differing configuration and absolute paths. Rebuildability of a binary or table alone does not establish that its surrounding directory is disposable. |

Two negative QCD input probes, `input.lhe` and `v1_probe.lhe`, have the same
size and differ byte-for-byte. Their similar names or sizes are insufficient
grounds for deduplication. The earlier failed reader probes and empty HepMC
outputs record how the parser problem was diagnosed.

## Storage estimate and strategy

- Measured allocation was approximately **13 GiB** excluding Git. The four
  final plain HepMC inputs total **8,794,256,259 bytes** (8.79 GB).
- A verified compressed archive and on-demand materialization could recover
  **2,294,641,869 logical bytes** locally from the QCD positive plain copy
  after readers, references and materialization have been tested. The
  existing compressed copy is 833,569,192 bytes. Deleting only the compressed
  copy instead would save its size while leaving the larger local working
  representation with no compressed duplicate. These options are mutually
  exclusive.
- The old Pythia 8.312 tree occupies approximately **104 MB** on disk.
  Conditional total for retiring that tree and materializing the QCD plain
  input on demand is approximately **2.40 GB** (2.23 GiB). No recovery is
  established until relocation, integrity and dependency checks pass.
- The three isolated MG5 copies occupy approximately **1.84 GB** in total.
  This is a footprint, excluded from recoverable space because redundancy
  and exact rebuild/relocation have not been established. Compression
  savings for the other HepMC files have not been measured.
- Suggested long-term policy: retain versioned source/cards and small
  result metadata in Git after review; store raw and compressed large
  artifacts in a backed-up research-data store with SHA-256 and a path
  restoration manifest. Keep an active local working copy where the current
  reader needs one. Verify round-trip decompression and check provenance
  before any locally retained representation is retired.

## Operations requiring approval

1. **README merge.** Merge [the draft](README_DRAFT.md) into the existing
   root README, preserving its local changes and migrating useful older
   notes with intact links. Rewrite the draft's documentation-relative
   paths for the root. The root README already has uncommitted edits.
2. **Optional legacy archive.** Once references are audited, approve an
   explicit source-to-destination manifest for selected exploratory files
   such as `analysis/pythia8312/`. Keep fixtures, historical logs and
   data provenance attached. An optional `archive/` can then be added.
3. **Optional large-data migration.** Approve storage destination, backup,
   checksums and restoration/rollback procedure before moving anything.
   Separately approve local retirement only after a read-back and audited
   replay. No scientific inputs or failed-run evidence are proposed for
   irreversible deletion.
4. **Git changes.** Any change to ignore policies, tracked-file migration,
   commit or push remains a separate review. The local ignored tools and
   raw inputs require provisioning beyond a Git checkout.

The initial implementation uses links and indices at the preferred locations
rather than breaking the established process and build layouts. The selected
comparison and its ROOT inputs resolve at the original paths.

## Approved follow-up implemented

The root README now opens with the current baseline and preserves the
previous README bytes after the new introduction. Two standalone PNGs
moved from the root into `plots/legacy_charge_study/`, with byte identity
checked. Their names appear in historical Git status snapshots, which
preserve the paths that applied at the time. No active code or link
referenced either original path.

`scripts/` and `configs/` now provide relative symlinks to the original
path-dependent tools and cards. `.gitignore` gained rules for local
generated process scaffolding and integration work areas, event formats,
known probe executables, parser tables and scratch suffixes. These rules
affect discovery of untracked files by Git. The underlying files and
scientific evidence remain in their run directories, and there is no
blanket ignore rule for `production_runs/` or its logs.

Archive and external storage proposals above remain unapplied. The inventory
describes the pre-organization snapshot. A future public release still
needs a reviewed data distribution mechanism for local and ignored files.

The historical README's `plots/cutflow.png` link was already unresolved.
The expected path now contains a byte-identical copy of the existing
`analysis/cutflow.png` historical figure. A separate copy keeps the
documented plot-export command from writing through a symlink to the
source image.
