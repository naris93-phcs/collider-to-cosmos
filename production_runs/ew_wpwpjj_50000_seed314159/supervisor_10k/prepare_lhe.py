from pathlib import Path
import gzip
import hashlib
import json
import math
import random
import re
import sys

out = Path(__file__).resolve().parent
original = Path(sys.argv[1])
expected_hash = "e53b4c8a94fb49a310e6ff3b6000735ae8671cd77cd981a8c4e374306cc35df4"
assert hashlib.sha256(original.read_bytes()).hexdigest() == expected_hash
with gzip.open(original, "rb") as f:
    raw = f.read()
events = list(re.finditer(rb"(?m)^<event>[^\S\n]*\n([^\n]+)\n((?:[^\S\n]*\n)*)[\s\S]*?^</event>[^\S\n]*\n", raw))
assert len(events) == raw.count(b"<event>") == raw.count(b"</event>") == 50000
header, footer = raw[:events[0].start()], raw[events[-1].end():]
assert re.fullmatch(rb"\s*</LesHouchesEvents>\s*", footer)
assert re.search(rb"<init>\s+2212\s+2212.*?\n\s*1.042576e-02", header, re.S)
assert all(not raw[a.end():b.start()].strip() for a, b in zip(events, events[1:]))
selected = sorted(random.Random(314161).sample(range(1, len(events) + 1), 10000))
subset_ids = set(selected)
subset_header, n1 = re.subn(rb"(?m)^(\s*)50000(?=\s*=\s*nevents\b)", rb"\g<1>10000", header)
subset_header, n2 = re.subn(rb"(?m)^(#\s*Number of Events\s*:\s*)50000\b", rb"\g<1>10000", subset_header)
assert n1 == n2 == 1
full_path = out / "ew_wpwpjj_50000_format_corrected.lhe.gz"
subset_path = out / "ew_wpwpjj_10000_subset.lhe.gz"
removed = 0
full_digest = hashlib.sha256()
subset_digest = hashlib.sha256()
with gzip.open(full_path, "xb") as full, gzip.open(subset_path, "xb") as subset, \
     (out / "full_expected.txt").open("x") as full_expected, (out / "subset_expected.txt").open("x") as sub_expected:
    full.write(header)
    subset.write(subset_header)
    full_digest.update(header)
    subset_digest.update(subset_header)
    previous = events[0].start()
    for index, record in enumerate(events, 1):
        count, process, weight, *_ = record[1].split()
        count = int(count)
        assert count > 0 and int(process) == 1 and float(weight) == 0.010425765
        particles = raw[record.end(2):record.end()].splitlines()[:count]
        assert len(particles) == count and all(len(row.split()) == 13 for row in particles)
        blank_start, blank_end = record.span(2)
        gap = raw[previous:record.start()]
        event = raw[record.start():blank_start] + raw[blank_end:record.end()]
        data = gap + event
        full.write(data)
        full_digest.update(data)
        removed += int(blank_end > blank_start)
        ids = [int(row.split()[0]) for row in particles]
        print(index, count, 1, float(weight), *ids, file=full_expected)
        if index in subset_ids:
            subset.write(event)
            subset_digest.update(event)
            print(index, count, 1, float(weight), *ids, file=sub_expected)
        previous = record.end()
    full.write(footer)
    subset.write(footer)
    full_digest.update(footer)
    subset_digest.update(footer)
assert removed == 48338
for path, digest in ((full_path, full_digest), (subset_path, subset_digest)):
    with gzip.open(path, "rb") as stream:
        assert hashlib.file_digest(stream, "sha256").hexdigest() == digest.hexdigest()
assert hashlib.sha256(original.read_bytes()).hexdigest() == expected_hash
print(json.dumps({"original_sha256": expected_hash, "full_events": 50000, "blank_gaps_removed": removed,
    "corrected_sha256": hashlib.sha256(full_path.read_bytes()).hexdigest(), "subset_events": len(selected),
    "subset_sha256": hashlib.sha256(subset_path.read_bytes()).hexdigest(), "subset_seed": 314161,
    "nominal_weight_pb": 0.010425765, "subset_contribution_pb": 0.010425765 / len(selected)}, indent=2))
