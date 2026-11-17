# W−W−jj run_01: Pythia 8.317 shower

The original gzip, MadGraph banner and shower card were retained. The
zero-byte HepMC file from the failed attempt is not the production output.
The W+W+ run_04 and C++ particle-level analysis were not used or modified.

## Root cause and controlled probes

The bundled Pythia 8.317 LHE reader failed on a blank line between the
numeric event header (NUP, IDPRUP, XWGTUP, ...) and the first particle
record. This is an input-format compatibility issue for this reader, not
a gzip failure or a failed hard process. The same bundled MG5–Pythia
driver, same XML and a one-event cap reproduced EOF with both the original
gzip and its plain decompression (gz/probe.log and plain/probe.log). A
plain Reader probe accepted the no-blank variant; changing the version
label alone did not help. The decisive control was the driver successfully
showering the gzip after removing **only** post-header blank lines
(formatted_probe/probe.log).

The production copy changes whitespace in 9,642 events; the other 358
already had no such gap. All nonblank lines, including LHE version, init,
weights, event/particle records and reweighting metadata, are byte-identical
line by line. The original gzip was never rewritten. The production card
differs from the existing run_01 card only in Beams:LHEF, retaining its
explicit 10000-event cap. This was a single serial driver invocation, not
the positive sample's split/merge workflow. Shower/hadronization settings
and the default Pythia seed policy were retained.

## Commands and paths

All shower commands ran from the stated directories with the bundled
8.317 XML explicitly selected and LD_LIBRARY_PATH unset for the subprocess.
For independent reproduction, use a fresh directory with the same layout;
the scripts/commands below are the recorded successful sequence, not a
request to overwrite outputs.

```bash
R=/home/naris/projects/collider_to_cosmos
H="$R/MG5_aMC_v3_8_0/HEPTools"
cd "$R/sm_wmwmjj_leptonic/Events/run_01/shower_8317"
sha256sum ../unweighted_events.lhe.gz ../run_01_tag_1_banner.txt ../tag_1_pythia8.cmd > original.sha256
gzip -t ../unweighted_events.lhe.gz
gzip -cd ../unweighted_events.lhe.gz > input.lhe
ln -s ../unweighted_events.lhe.gz input.lhe.gz

for kind in gz plain; do
  mkdir "$kind"
  infile=../input.lhe
  if [ "$kind" = gz ]; then infile=../input.lhe.gz; fi
  sed -E "s|^Main:numberOfEvents.*|Main:numberOfEvents = 1|;s|^Beams:LHEF=.*|Beams:LHEF=$infile|" ../tag_1_pythia8.cmd > "$kind/probe.cmd"
  (cd "$kind"; env -u LD_LIBRARY_PATH PYTHIA8DATA="$H/pythia8/share/Pythia8/xmldoc" "$H/MG5aMC_PY8_interface/MG5aMC_PY8_interface" probe.cmd > probe.log 2>&1)
done

python3 -B prepare_lhe.py > prepare.log
mkdir formatted_probe
sed -E 's|^Main:numberOfEvents.*|Main:numberOfEvents = 1|;s|^Beams:LHEF=.*|Beams:LHEF=../prepared.lhe.gz|' ../tag_1_pythia8.cmd > formatted_probe/probe.cmd
(cd formatted_probe; env -u LD_LIBRARY_PATH PYTHIA8DATA="$H/pythia8/share/Pythia8/xmldoc" "$H/MG5aMC_PY8_interface/MG5aMC_PY8_interface" probe.cmd > probe.log 2>&1)
test "$(rg -c '^E ' formatted_probe/tag_1_pythia8_events.hepmc)" -eq 1

sed 's|^Beams:LHEF=.*|Beams:LHEF=prepared.lhe.gz|' ../tag_1_pythia8.cmd > production.cmd
test ! -e tag_1_pythia8_events.hepmc
/usr/bin/time -p -o production.time env -u LD_LIBRARY_PATH PYTHIA8DATA="$H/pythia8/share/Pythia8/xmldoc" "$H/MG5aMC_PY8_interface/MG5aMC_PY8_interface" production.cmd > production.log 2>&1

g++ -std=c++17 -O2 -I "$H/hepmc/include" verify_hepmc.cpp "$H/hepmc/lib/libHepMC.so" -Wl,-rpath,"$H/hepmc/lib" -o verify_hepmc > verifier_build.log 2>&1
tail -n 3 tag_1_pythia8_events.hepmc | rg '^HepMC::IO_GenEvent-END_EVENT_LISTING$'
./verify_hepmc tag_1_pythia8_events.hepmc > integrity.log 2>&1
sha256sum -c original.sha256 > originals_verified.log
test ! -e ../tag_1_pythia8_events_verified.hepmc
test ! -L ../tag_1_pythia8_events_verified.hepmc
mv -n tag_1_pythia8_events.hepmc ../tag_1_pythia8_events_verified.hepmc
```

## Statistics and warnings

Pythia tried / selected / accepted: **10000 / 10000 / 10000**.
Wall time 136.87 s. The driver's final inclusive cross section is
3.870572e-12 mb = 0.003870572 pb, matching the MadGraph central value.
The driver's reported ±3.870572e-14 mb is not the MadGraph integration
error (9.298090e-06 pb).

The complete warnings are in production.log. Pythia emitted one
space-shower stuck-loop error, eleven energy–momentum precision warnings,
six ISR and one MPI weight-above-unity warnings, plus initialization/SLHA
messages. The run finished without an EOF abort or loss in accepted counts.
A successful serialization check does not remove these generator warnings.

Integrity check passed: HepMC 2.06.09 decoded all 10,000 events with unique
event numbers, valid records, GeV/mm units, finite momenta, and two hard
status-22 W− records in each. The file has its closing HepMC marker.
Final output: ../tag_1_pythia8_events_verified.hepmc (2,240,132,169 bytes).

The mean of the LHE-convention Weight field is 0.003870572 pb; do not sum
that field as event contributions. Each Weight_MERGING=0.000 is
3.870572e-7 pb and their sum is 0.003870572 pb. The C record is cumulative,
with final value 0.003870572 pb (rounding only); its last error field is
zero and does not replace the archived MadGraph integration uncertainty.
The original gzip, banner and cmd passed SHA256 comparison.
