"""Check one integrated mode against the independently validated checkpoint."""
import csv
import json
import math
from pathlib import Path
import sys
import ROOT

mode = sys.argv[1]
assert mode in ('status1', 'lifetime')
directory = Path(__file__).resolve().parent
reference = directory.parent / 'lifetime_impact'
name = 'baseline' if mode == 'status1' else 'lifetime'
cutflow = json.loads((reference / 'summary.json').read_text())['cutflow']
with (reference / (name + '.csv')).open() as f:
    rows = list(csv.DictReader(f))
assert len(rows) == 10000
indices = [int(r['index']) for r in rows if r['cuts'][-1] == '1']

actual = ROOT.TFile.Open(str(directory / (mode + '.root')), 'READ')
expected = ROOT.TFile.Open(str(reference / (name + '.root')), 'READ')
assert actual and expected and not actual.IsZombie() and not expected.IsZombie()
counts = [int(actual.Get('h_cutflow').GetBinContent(i)) for i in range(1, 13)]
assert counts == [stage[name] for stage in cutflow], counts

tree, ref = actual.Get('fiducial_events'), expected.Get('fiducial_events')
assert tree.GetEntries() == ref.GetEntries() == len(indices) == 339
branches = [b.GetName() for b in ref.GetListOfBranches()]
assert len(branches) == 8
maxima = {key: 0. for key in branches}
for i, index in enumerate(indices):
    tree.GetEntry(i)
    ref.GetEntry(i)
    assert tree.event_index == index, (i, tree.event_index, index)
    for key in branches:
        delta = abs(getattr(tree, key) - getattr(ref, key))
        assert math.isfinite(delta) and delta < 1e-9, (i, key, delta)
        maxima[key] = max(maxima[key], delta)

histograms = []
for key in expected.GetListOfKeys():
    h = expected.Get(key.GetName())
    if not h.InheritsFrom('TH1'):
        continue
    new = actual.Get(key.GetName())
    assert new and h.GetNcells() == new.GetNcells()
    assert h.GetEntries() == new.GetEntries()
    assert all(h.GetBinContent(i) == new.GetBinContent(i) for i in range(h.GetNcells())), key.GetName()
    histograms.append(key.GetName())

result = dict(mode=mode, cutflow=counts, selected_indices=indices,
              max_abs_difference=maxima, exact_histogram_bins=histograms)
if mode == 'lifetime':
    status1 = json.loads((directory / 'status1.verify.json').read_text())
    assert indices == status1['selected_indices']
    baseline = ROOT.TFile.Open(str(directory / 'status1.root'), 'READ')
    old = baseline.Get('fiducial_events')
    checks = {'met': 'met', 'pt1': 'jet1_pt', 'pt2': 'jet2_pt',
              'mjj': 'mjj', 'dy': 'dy_jj'}
    differences = {key: [] for key in checks}
    for i in range(len(indices)):
        tree.GetEntry(i)
        old.GetEntry(i)
        assert old.event_index == tree.event_index
        for key, branch in checks.items():
            differences[key].append(abs(getattr(tree, branch) - getattr(old, branch)))
    shifts = {key: dict(mean_abs_delta=math.fsum(values) / len(values),
                        max_abs_delta=max(values))
              for key, values in differences.items()}
    expected_shifts = json.loads((reference / 'selected_summary.json').read_text())
    for key, stats in shifts.items():
        for field, value in stats.items():
            assert abs(value - expected_shifts[key][field]) < 1e-9
    result['selected_shifts_lifetime_vs_status1'] = shifts
print(json.dumps(result, indent=2))
