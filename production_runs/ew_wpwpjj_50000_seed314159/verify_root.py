"""Cross-check the existing analysis output and calculate the fiducial rate."""
import hashlib
import json
import math
from pathlib import Path
import sys
import ROOT

production = Path(__file__).resolve().parent
lhe = json.loads((production / "lhe_verification.json").read_text())
hep = json.loads((production / "hepmc_verification.json").read_text())
path = Path(sys.argv[1])
root_file = ROOT.TFile.Open(str(path), "READ")
assert root_file and not root_file.IsZombie() and not root_file.TestBit(ROOT.TFile.kRecovered)
keys = [key.GetName() for key in root_file.GetListOfKeys()]
assert len(keys) == len(set(keys)) == 14
assert all(root_file.Get(name) for name in keys)
hist = root_file.Get("h_cutflow")
assert hist.GetNbinsX() == 12
cutflow = [(hist.GetXaxis().GetBinLabel(i), int(hist.GetBinContent(i)))
           for i in range(1, hist.GetNbinsX() + 1)]
assert cutflow[0][1] == lhe["events"] == hep["events"]
assert all(a[1] >= b[1] for a, b in zip(cutflow, cutflow[1:]))
tree = root_file.Get("fiducial_events")
selected = int(tree.GetEntries())
assert selected == cutflow[-1][1] and selected > 0
cuts = dict(mjj=500, dy_jj=2, mll=20, lepton1_pt=27, lepton2_pt=27,
            met=30, jet1_pt=65, jet2_pt=35)
indices = set()
for row in tree:
    for field, threshold in cuts.items():
        value = getattr(row, field)
        assert math.isfinite(value) and value > threshold, (row.event_index, field, value)
    index = int(row.event_index)
    assert 1 <= index <= hep["events"] and index not in indices
    indices.add(index)
root_file.Close()
acceptance = selected / lhe["events"]
sigma = lhe["cross_section_pb"]
weight = hep["minimum_weight_merging_pb"]
assert weight == hep["maximum_weight_merging_pb"]
fiducial = selected * weight
assert math.isclose(fiducial, acceptance * sigma, rel_tol=1e-6)
print(json.dumps({
    "root_path": str(path.resolve()),
    "root_sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
    "root_keys": keys,
    "cutflow": cutflow,
    "selected_events": selected,
    "acceptance": acceptance,
    "fiducial_cross_section_pb": fiducial,
    "fiducial_mc_acceptance_stat_pb": sigma * math.sqrt(acceptance * (1 - acceptance) / lhe["events"]),
    "fiducial_propagated_integration_error_pb": acceptance * lhe["integration_error_pb"],
}, indent=2))
