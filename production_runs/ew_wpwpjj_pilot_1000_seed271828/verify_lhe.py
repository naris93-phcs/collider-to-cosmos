"""Read-only LHE audit; emit JSON on stdout. No third-party dependencies."""
import collections
import gzip
import hashlib
import json
import re
import sys
from pathlib import Path

path = Path(sys.argv[1])
with gzip.open(path, "rt") if path.suffix == ".gz" else path.open() as stream:
    text = stream.read()
init = re.search(r"<init>\s*(.*?)</init>", text, re.S)[1].splitlines()
beam = init[0].split()
processes = [line.split() for line in init[1:1 + int(beam[-1])]]
events = re.findall(r"<event>\s*(.*?)</event>", text, re.S)
weights = collections.Counter()
flavours = collections.Counter()
w_patterns = collections.Counter()
blank_after_header = 0
for event in events:
    lines = event.splitlines()
    header = lines[0].split()
    blank_after_header += int(not lines[1].strip())
    rows = [line.split() for line in lines[1:] if line.strip()][:int(header[0])]
    assert len(rows) == int(header[0])
    weights[float(header[2])] += 1
    ws = [row for row in rows if abs(int(row[0])) == 24]
    positive_ws = [row for row in ws if row[0] == "24"]
    assert len(positive_ws) == 2 and all(row[1] == "2" for row in ws)
    w_patterns[tuple(sorted(int(row[0]) for row in ws))] += 1
    # The full EW production includes hadronic W- resonance histories.
    # Do not mistake these for wrong-sign leptonic W production.
    for i, row in enumerate(rows, start=1):
        if row[0] == "-24":
            daughters = [x for x in rows if int(x[2]) == i and int(x[3]) == i]
            assert len(daughters) == 2 and all(1 <= abs(int(x[0])) <= 4 for x in daughters)
    leptons = [row for row in rows if row[1] == "1" and abs(int(row[0])) in (11, 13, 15)]
    neutrinos = [row for row in rows if row[1] == "1" and abs(int(row[0])) in (12, 14, 16)]
    assert len(leptons) == len(neutrinos) == 2
    assert all(int(row[0]) in (-11, -13) for row in leptons)
    assert all(int(row[0]) in (12, 14) for row in neutrinos)
    assert sorted(-int(row[0]) + 1 for row in leptons) == sorted(int(row[0]) for row in neutrinos)
    for row in leptons + neutrinos:
        assert row[2] == row[3] and rows[int(row[2]) - 1][0] == "24"
    flavours[tuple(sorted(int(row[0]) for row in leptons))] += 1
assert len(events) == 1000
assert beam[:4] == ["2212", "2212", "6.500000e+03", "6.500000e+03"]
assert len(weights) == 1 and min(weights) > 0
assert "generate p p > w+ w+ j j QCD=0 QED=4, w+ > lp vlp" in text
result = {
    "file": str(path.resolve()),
    "sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
    "events": len(events),
    "init_beams": beam,
    "init_processes": processes,
    "cross_section_pb": sum(float(row[0]) for row in processes),
    "integration_error_pb": (sum(float(row[1])**2 for row in processes))**0.5,
    "nominal_weights": dict(weights),
    "flavour_counts": {str(key): count for key, count in flavours.items()},
    "W_record_patterns": {str(key): count for key, count in w_patterns.items()},
    "two_positive_intermediate_Ws_per_event": True,
    "no_ME_tau_decays": True,
    "production_command_in_header_verified": True,
    "events_with_blank_line_after_header": blank_after_header,
}
print(json.dumps(result, indent=2))
