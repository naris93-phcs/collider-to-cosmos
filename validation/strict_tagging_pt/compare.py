"""Compare the strict tagging-pT run with the previous lifetime output."""
import csv
import json
from pathlib import Path
import ROOT

directory = Path(__file__).resolve().parent
refdir = directory.parent
with (refdir / 'lifetime_impact/lifetime.csv').open() as f:
    rows = list(csv.DictReader(f))
assert len(rows) == 10000
boundaries = [
    dict(index=int(r['index']), event=int(r['event']), pt1=float(r['pt1']),
         pt2=float(r['pt2']), previous_flags=r['cuts'])
    for r in rows if int(r['njets']) >= 2
    and (float(r['pt1']) == 65.0 or float(r['pt2']) == 35.0)
]
removed = {r['index'] for r in boundaries if r['previous_flags'][9] == '1'}
expected = [
    sum(r['cuts'][i] == '1' and (i < 9 or int(r['index']) not in removed)
        for r in rows)
    for i in range(12)
]

def read(path):
    f = ROOT.TFile.Open(str(path), 'READ')
    assert f and not f.IsZombie(), path
    h, tree = f.Get('h_cutflow'), f.Get('fiducial_events')
    return ([h.GetXaxis().GetBinLabel(i) for i in range(1, 13)],
            [int(h.GetBinContent(i)) for i in range(1, 13)],
            [int(row.event_index) for row in tree])

labels, before, old_ids = read(refdir / 'lifetime_integration/lifetime.root')
new_labels, after, new_ids = read(directory / 'lifetime.root')
assert before == [sum(r['cuts'][i] == '1' for r in rows) for i in range(12)]
assert old_ids == [int(r['index']) for r in rows if r['cuts'][-1] == '1']
result = dict(
    events_at_exact_thresholds=boundaries, removed_at_tagging_pt=sorted(removed),
    cutflow=[dict(stage=l, before=b, after=a, expected=e, delta=a-b)
             for l, b, a, e in zip(labels, before, after, expected)],
    selected_before=len(old_ids), selected_after=len(new_ids),
    entering=sorted(set(new_ids)-set(old_ids)), leaving=sorted(set(old_ids)-set(new_ids)),
    identical_ordered_indices=new_ids == old_ids)
print(json.dumps(result, indent=2), flush=True)
assert labels == new_labels and after == expected
assert new_ids == [i for i in old_ids if i not in removed]
