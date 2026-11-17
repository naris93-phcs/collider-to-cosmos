# Targeted lifetime jet-constituent validation

Processed only one-based sequential indices 4619, 9905, 2673, 3298, 8879,
628, 4403, in each mode. Other input records were read and discarded before
reconstruction to reach the requested positions. Stopped after index 9905.
No generation or main-analysis changes. Counts printed by the inherited
source use the input ordinal and are not counts of reconstructed events.

From this directory, on fresh output paths: `python3 -B prepare.py`,
`bash run.sh`, `python3 -B verify.py`, `python3 -B match.py > matches.txt`.
The preparation script instruments the validated comparison snapshot and
uses its unchanged reconstruction helper. Jet metadata do not change
momenta, clustering, or ghost flavour labels.

The two jets.tsv files contain: sequential index, stage, rank, px, py, pz,
E, pT, eta, y, phi, and constituent tuples barcode:PDG:E:ghost, in GeV
for momenta/energies. Stages: clustered (inclusive 20 GeV), pre (after
eta acceptance), post (after overlap removal), vbs (after 25 GeV threshold).
matches.txt ranks candidate matches by shared physical constituent energy,
gives common energy and its fractions relative to each jet, plus wrapped
eta–phi DeltaR. Ghosts are recorded but excluded from matching. A large
shared fraction from multiple jets is treated as a merge, not forced into
a one-to-one match. verify.py passed in both modes: all seven event rows
match the previous checkpoint, including input identities and cut flags;
numerical observables agree within 1e-9 and constituent energy sums close
within 1e-9 GeV with no duplicate constituents per event/stage.

| Index | VBS jets B→L | mjj B→L (GeV) | abs(dy) B→L | Explanation |
|---|---:|---:|---:|---|
| 4619 | 5→5 | 423.376→1144.074 | 1.808→4.069 | Baseline rank 3 becomes rank 2, with 693.54 GeV of shared constituent energy and DeltaR 0.002. Rank 2 drops from 136.441 to 135.744 GeV, while rank 3 rises from 136.125 to 136.319. Both modes remove the same rank-5 jet in overlap. |
| 9905 | 3→3 | 243.612→646.274 | 3.444→5.427 | Both remove pre rank 1 in overlap and drop one sub-25 GeV jet. Baseline VBS rank 3 becomes rank 2 (33.237→34.417 GeV, DeltaR 0.022); the old rank 2 remains at 33.310. Still fails tagging pT. |
| 2673 | 5→5 | 159.105→545.027 | 1.699→3.999 | Both remove pre rank 2 in overlap. Baseline VBS rank 3 is unchanged but is promoted to rank 2 because the former rank 2 falls 47.819→46.158 GeV. The promoted jet has identical physical constituents (262.189 GeV shared). |
| 3298 | 2→2 | 499.073→500.180 | 0.40152→0.40162 | Same pair and same overlap removals. Subleading pT 209.712→210.086 GeV; its matched jet shares 228.713 GeV and DeltaR is 0.002. This is a constituent change near the mass threshold, not a rank swap. |
| 8879 | 5→5 | 535.595→450.482 | 1.891→0.705 | Baseline rank 3 (185.986 GeV) rises to 186.848 GeV and overtakes unchanged rank 2 (186.092 GeV). Match shares 189.227 GeV at DeltaR 0.002. Same overlap removal and 25 GeV filtering pattern. |
| 628 | 3→2 | 680.279→784.917 | 1.144→1.184 | Already before overlap, lifetime leading jet contains 533.143 and 150.513 GeV shared with baseline ranks 1 and 3 respectively; those are 84.2% and 91.0% of the original jets' physical energies. Leading pT rises 354.581→456.752 GeV. No overlap removal in either mode. |
| 4403 | 3→2 | 343.359→455.994 | 2.802→2.748 | Already before overlap, lifetime pre rank 2 contains 501.604 and 402.746 GeV shared with baseline pre ranks 3 and 4 (88.4% and 96.3% of their energies). The matched electron-overlap jet is removed in both modes (B pre rank 2, L pre rank 3). Subleading pT rises 50.848→90.734 GeV. |

The 3298 mass-stage entry and 8879 exit reproduce the prior checkpoint;
neither passes final selection in either mode. No final flag changes among
these seven. Event 8879 also illustrates why identity overlap alone can be
low for a physically matched jet: its leading axis is unchanged, while
the lost constituent set includes 200.904 GeV in two charged pions and
the added set includes a K_S with 200.904 GeV.

Conclusion: the large mass changes are explained by tagging-rank swaps
following constituent replacement. The large pT changes at 628 and 4403
show merging already at clustering, not differential overlap removal.
No inconsistency was found in these targeted events. Recommend controlled
integration, with the status-1 mode retained for regression and no claim
of ATLAS equivalence. No integration was performed here.
