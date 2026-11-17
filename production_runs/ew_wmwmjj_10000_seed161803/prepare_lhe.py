"""Validate the central W-W- sample and, if necessary, make a format-only copy."""
from collections import Counter
from pathlib import Path
import gzip
import hashlib
import json
import math
import re
import sys

source = Path(sys.argv[1]).resolve()
out = Path(__file__).resolve().parent
checksum = hashlib.sha256(source.read_bytes()).hexdigest()
with gzip.open(source, "rb") as stream:
    raw = stream.read()

events = list(re.finditer(
    rb"(?m)^<event>[^\S\n]*\n([^\n]+)\n((?:[^\S\n]*\n)*)[\s\S]*?^</event>[^\S\n]*\n", raw))
assert len(events) == raw.count(b"<event>") == raw.count(b"</event>") == 10000
header = raw[:events[0].start()]
assert raw[events[-1].end():].strip() == b"</LesHouchesEvents>"
assert all(not raw[a.end():b.start()].strip() for a, b in zip(events, events[1:]))
assert re.search(rb"(?m)^\s*10000\s*=\s*nevents\b", header)
assert re.search(rb"(?m)^\s*161803\s*=\s*iseed\b", header)
assert re.search(rb"(?m)^\s*none\s*=\s*systematics_program\b", header)
assert re.search(rb"(?m)^\s*average\s*=\s*event_norm\b", header)
assert b"generate p p > w- w- j j QCD=0 QED=4, w- > lm vlm" in header
assert not re.search(rb"<(?:initrwgt|rwgt|weights)\b", raw)
init = re.search(rb"<init>\s*([^\n]+)\n\s*([^\n]+)", header)
beam = init[1].split()
assert beam == b"2212 2212 6.500000e+03 6.500000e+03 0 0 247000 247000 -4 1".split()
sigma, error, maxweight, code = map(float, init[2].split())
assert all(math.isfinite(x) and x > 0 for x in (sigma, error, maxweight)) and code == 1

weights, flavours, multiplicities, resonance_patterns = (Counter() for _ in range(4))
blank_gaps = []
with (out / "lhe_reader_expected.txt").open("x") as expected:
    for index, record in enumerate(events, 1):
        event_header = record[1].split()
        assert len(event_header) == 6
        count, process = map(int, event_header[:2])
        weight, scale, aqed, aqcd = map(float, event_header[2:])
        assert all(math.isfinite(x) and x > 0 for x in (weight, scale, aqed, aqcd))
        assert math.isclose(weight, sigma, rel_tol=1e-6) and process == int(code)
        weights[weight] += 1
        particle_lines = raw[record.end(2):record.end()].splitlines()[:count]
        assert len(particle_lines) == count
        particles = []
        for line in particle_lines:
            fields = line.split()
            assert len(fields) == 13
            identifiers = tuple(map(int, fields[:6]))
            assert all(math.isfinite(float(v)) for v in fields[6:])
            assert all(0 <= m <= count for m in identifiers[2:4])
            particles.append(identifiers)
        assert sum(row[1] == -1 for row in particles) == 2
        ws = {i: row[0] for i, row in enumerate(particles, 1) if abs(row[0]) == 24}
        negative = {i for i, pdg in ws.items() if pdg == -24}
        assert len(negative) == 2 and all(particles[i - 1][1] == 2 for i in ws)
        for parent, pdg in ws.items():
            daughters = {row[0] for row in particles if row[2] == row[3] == parent}
            if pdg == -24:
                assert daughters in ({11, -12}, {13, -14}), (index, daughters)
            else:
                assert len(daughters) == 2 and all(1 <= abs(d) <= 4 for d in daughters)
        leptons = [row for row in particles if row[1] == 1 and abs(row[0]) in (11, 13, 15)]
        neutrinos = [row for row in particles if row[1] == 1 and abs(row[0]) in (12, 14, 16)]
        assert len(leptons) == len(neutrinos) == 2
        assert all(row[2] == row[3] and row[2] in negative for row in leptons + neutrinos)
        assert all(row[0] in (11, 13) for row in leptons)
        assert all(row[0] in (-12, -14) for row in neutrinos)
        flavours[str(tuple(sorted(row[0] for row in leptons)))] += 1
        resonance_patterns[str(tuple(sorted(ws.values())))] += 1
        multiplicities[count] += 1
        print(index, count, process, weight, *(row[0] for row in particles), file=expected)
        if record.start(2) < record.end(2):
            blank_gaps.append(record.span(2))
assert len(weights) == 1

target = source
if blank_gaps:
    chunks, cursor = [], 0
    for start, end in blank_gaps:
        assert start >= cursor and not raw[start:end].strip()
        chunks.append(raw[cursor:start])
        cursor = end
    chunks.append(raw[cursor:])
    corrected = b"".join(chunks)
    target = out / "ew_wmwmjj_10000_format_corrected.lhe.gz"
    with gzip.open(target, "xb") as stream:
        stream.write(corrected)
    with gzip.open(target, "rb") as stream:
        assert stream.read() == corrected

assert hashlib.sha256(source.read_bytes()).hexdigest() == checksum
print(json.dumps({"original_lhe": str(source), "original_sha256": checksum, "shower_lhe": str(target),
                  "shower_lhe_sha256": hashlib.sha256(target.read_bytes()).hexdigest(),
                  "blank_gaps_removed": len(blank_gaps), "events": len(events),
                  "generator_sigma_pb": sigma, "integration_error_pb": error,
                  "nominal_weights": weights, "flavour_counts": flavours,
                  "NUP_counts": multiplicities, "W_record_patterns": resonance_patterns}, indent=2))
