# EW W-W-jj at 13 TeV — production stopped at the event-count check

MG5 produced 3,031 unweighted events for a target of 10,000. The generator
logged the shortfall during event combination and exited with status zero.
The compressed LHE independently contains 3,031 complete event blocks and
a closing LHE tag. The event-count gate stopped the run before Pythia.
No EW HepMC or EW ROOT output was created.

The partial run reports XSECUP = 0.004085351 pb and XERRUP = 0.000008687279 pb
(4.085351 ± 0.008687279 fb, integration error). All 3,031 nominal weights
are +0.0040853512 pb with IDWTUP=-4. These are partial-run generator figures.
The source of the event shortfall is undetermined. The log suggests changes
to integration settings, but none were applied. MG5 skipped the automated
second refinement after its cross-section stability check, before discovering
that the event supply was below target.

The new process used `p p > w- w- j j QCD=0 QED=4, w- > lm vlm`, with
`lm = e- mu-` and `vlm = ve~ vm~`. The 14 matrix routines carry the production
restriction QCD=0 QED<=4 and all 860 exported decay-combined diagrams have
QCD=0 QED=6. The run reused the reference parameter card and central 13 TeV
beam/PDF/scale/cut settings, with MG5 seed 161803 and eight local workers.
The prepared downstream workflow specifies shower seed 161804.
PDF/scale reweighting was disabled for this central run.

The verified archived QCD W-W-jj sample was reanalyzed with the existing
lifetime binary and reproduced 291/10,000 selected (2.91%). Its LHE and
HepMC normalization agree at 3.870572 fb, giving a preliminary fiducial
0.112634 ± 0.006506 fb (MC acceptance statistics). The four-sample comparison
and the EW combined-charge result are pending a complete EW negative-charge
sample. The unresolved EW W+W+ normalization discrepancy, incomplete ATLAS
fiducial audit, absent interference and missing systematic variations still
apply. Existing analysis code and monitored reference files match their
initial checksums.
