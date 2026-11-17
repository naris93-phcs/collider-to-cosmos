# Reproducibility

The checked comparison can be reconstructed from preserved event files and
ROOT trees. The commands below describe reproduction in new output locations.
The curation pass did not execute them.

## Environment and local dependency layout

The validated stack uses MadGraph5_aMC@NLO 3.8.0, Pythia 8.317 with gzip
support, HepMC 2.06.09, FastJet 3.5.1, and ROOT 6.40.04/PyROOT from the
existing `hep` Conda environment. The local curation host reports CMake
3.28.3 and GCC 13.3.0. CMake requires version 3.28 or newer. The installed
MG5 HEPTools directory supplies the HepMC/Pythia libraries, the Pythia XML
data, and the compiled standalone shower interface.

From the repository root, to configure a separate build:

```bash
source "$HOME/miniforge3/etc/profile.d/conda.sh"
conda activate hep
replay_build=$(mktemp -d /tmp/collider-build.XXXXXX)
cmake -S "$PWD" -B "$replay_build" -DCMAKE_PREFIX_PATH="$CONDA_PREFIX"
cmake --build "$replay_build" --target particle_level -j 4
```

The final comparison used the existing `build/particle_level` binary.
Capture the commit plus dirty diff and untracked reconstruction header when
transferring it. The executable's rpath and compile-time XML path make
relocation dependent on the HEPTools layout. The [checksum manifest](../results/SHA256SUMS)
captures the validated inputs and software files used by this record.

## Generator records and LHE preparation

- EW W⁺W⁺ parent. The [export card](../production_runs/ew_wpwpjj_50000_seed314159/cards/process.mg5)
  contains the exact process, with the original output path. Export used
  `python3 -B MG5_aMC_v3_8_0/bin/mg5_aMC cards/process.mg5` from the isolated
  run directory. After copying the preserved run and parameter cards into
  the process, generation used
  `python3 -B bin/generate_events ew_wpwp_50000_seed314159 --laststep=parton --nb_core=8 -f`
  from the generated process directory. These historical commands point to
  the archived production identity. A replay would require a fresh export
  path and run name.
- Positive shower input. [prepare_lhe.py](../production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/prepare_lhe.py)
  verifies the parent hash, removes only the header gaps, and selects
  10,000 indices uniformly without replacement using seed 314161, in source
  order. It updates both subset event-count fields and preserves average
  weights. In an isolated copy of the script directory without the output
  files, use `python3 -B prepare_lhe.py /absolute/path/to/parent.lhe.gz`.
- EW W⁻W⁻ manual run. The generated [process directory](../production_runs/ew_wmwmjj_manual/Cards/proc_card_mg5.dat)
  and [banner](../production_runs/ew_wmwmjj_manual/Events/run_01/run_01_tag_1_banner.txt)
  establish the process and parameters. The exact interactive MadGraph
  invocation has not been reconstructed. Use the preserved LHE for replay.
  [prepare_lhe.py](../production_runs/ew_wmwmjj_manual_analysis_seed161805/prepare_lhe.py)
  performs count, flavour and numeric checks and produces the formatting-only
  copy when needed.
- QCD reuse. For exact reproduction from the shower stage, use the archived
  HepMC files. The available QCD cards, logs and known seeds are in the
  [results index](RESULTS_INDEX.md). The effective positive-charge chunk
  overrides and unavailable shower seeds limit exact regeneration.

## Shower and particle-level execution

The negative EW [postprocess.sh](../production_runs/ew_wmwmjj_manual_analysis_seed161805/postprocess.sh)
is the recorded end-to-end command. It validates and repairs the LHE,
compiles the 8.317 reader probe, verifies all records, writes the
run-specific card, showers, checks event counts/weights, and runs lifetime
analysis. To replay with the same seed and the current layout, copy it and
its dependencies into a new sibling of the source run, under
`production_runs/`:

```bash
src=production_runs/ew_wmwmjj_manual_analysis_seed161805
work=$(mktemp -d "$PWD/production_runs/ew_wmwmjj_replay.XXXXXX")
mkdir "$work"/{cards,shower,analysis}
cp "$src"/{postprocess.sh,prepare_lhe.py,verify_hepmc.py,verify_pythia_reader.cpp} "$work/"
bash "$work/postprocess.sh"
```

If replaying an individual shower stage, run the following from a fresh
working directory after preparing a copy of the archived card with the
verified input and new output path. The card contains the seed and event
count. On the same software stack, retain the seed to replay the existing
shower. Check the reader, accepted count, HepMC framing and weight sums
against the sample verification JSON. The executable exit status alone
cannot distinguish an early EOF from success.

```bash
"$repo/MG5_aMC_v3_8_0/HEPTools/MG5aMC_PY8_interface/MG5aMC_PY8_interface" "$card" > shower.log 2>&1
"$repo/build/particle_level" lifetime "$hepmc" "$new_root" > particle_level.log 2>&1
```

Here `repo`, `card`, `hepmc` and `new_root` are absolute paths. The
analysis uses the same direct-W prompt e/μ, dressed leptons, same-sign
pair, overlap removal, MET and veto logic and anti-kT R=0.4 jet selection
for all four samples. The final cumulative cut requires strict tagging
jet pT >65/35 GeV, mjj >500 GeV, and |Δyjj| >2. The
[cutflow](../results/cutflow.csv) has all stages and
[particle_level.cpp](../analysis/particle_level.cpp) is authoritative.

## Replot from preserved ROOT trees

The plot script uses PyROOT and opens existing selected-event trees.
Stage the script and link the four original output directories inside a
new temporary tree, preserving the sibling names that it resolves:

```bash
repo=$PWD
src="$repo/production_runs/ew_wmwmjj_manual_analysis_seed161805"
stage=$(mktemp -d /tmp/collider-plots.XXXXXX)
mkdir "$stage/replay"
ln -s "$src/analysis" "$stage/replay/analysis"
ln -s "$repo/production_runs/ew_wpwpjj_50000_seed314159" "$stage/ew_wpwpjj_50000_seed314159"
ln -s "$repo/production_runs/ew_wmwmjj_10000_seed161803" "$stage/ew_wmwmjj_10000_seed161803"
cp "$src/compare.py" "$stage/replay/compare.py"
"$HOME/miniforge3/envs/hep/bin/python" -B "$stage/replay/compare.py"
```

The original ROOT files remain the references. Plot checks include
matching ROOT tree/cutflow counts and width-weighted integrals of unity or
the expected fiducial rate. Compare tables and numerical histograms for
scientific equivalence; PDF bytes can depend on rendering metadata.
