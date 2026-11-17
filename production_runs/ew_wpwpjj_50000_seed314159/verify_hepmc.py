"""Audit a HepMC2 ASCII file in one pass without storing its particle records."""
import hashlib
import json
import math
from pathlib import Path
import sys

root = Path(__file__).resolve().parent
lhe = json.loads((root / "lhe_verification.json").read_text())
hepmc = Path(sys.argv[1])
expected_count = lhe["events"]
expected_nominal = float(next(iter(lhe["nominal_weights"])))
expected_contribution = expected_nominal / expected_count

digest = hashlib.sha256()
count = names = units = xsecs = starts = ends = 0
minimum_weight = math.inf
maximum_weight = 0.0
weights = []
last_cross_section = 0.0
last_record = b""
with hepmc.open("rb") as stream:
    for raw in stream:
        digest.update(raw)
        prefix = raw[:2]
        if prefix == b"E ":
            fields = raw.split()
            assert int(fields[1]) == count
            assert int(fields[8]) > 0
            first_weight = 13 + int(fields[11])
            assert int(fields[first_weight - 1]) == 2
            nominal, weight = map(float, fields[first_weight:])
            assert math.isclose(nominal, expected_nominal, rel_tol=1e-12)
            assert math.isclose(weight, expected_contribution, rel_tol=1e-12)
            minimum_weight = min(minimum_weight, weight)
            maximum_weight = max(maximum_weight, weight)
            weights.append(weight)
            count += 1
        elif prefix == b"N ":
            assert raw.split() == [b"N", b"2", b'"Weight"', b'"Weight_MERGING=0.000"']
            names += 1
        elif prefix == b"U ":
            assert raw.split() == [b"U", b"GEV", b"MM"]
            units += 1
        elif prefix == b"C ":
            last_cross_section, error = map(float, raw.split()[1:])
            assert math.isclose(last_cross_section, count * expected_contribution, rel_tol=1e-10)
            assert math.isfinite(error) and error >= 0
            xsecs += 1
        elif raw.startswith(b"HepMC::IO_GenEvent-"):
            if raw.strip().endswith(b"START_EVENT_LISTING"):
                starts += 1
            elif raw.strip().endswith(b"END_EVENT_LISTING"):
                ends += 1
        if raw != b"\n":
            last_record = raw
assert count == names == units == xsecs == expected_count, (count, names, units, xsecs)
assert starts == ends == 1 and last_record.strip().endswith(b"END_EVENT_LISTING")
summed = math.fsum(weights)
assert math.isclose(summed, lhe["cross_section_pb"], rel_tol=1e-6)
assert math.isclose(summed, last_cross_section, rel_tol=1e-10)
print(json.dumps({
    "path": str(hepmc.resolve()),
    "sha256": digest.hexdigest(),
    "events": count,
    "weight_nominal_average_pb": expected_nominal,
    "minimum_weight_merging_pb": minimum_weight,
    "maximum_weight_merging_pb": maximum_weight,
    "weight_merging_sum_pb": summed,
    "last_hepmc_cross_section_pb": last_cross_section,
}, indent=2))
