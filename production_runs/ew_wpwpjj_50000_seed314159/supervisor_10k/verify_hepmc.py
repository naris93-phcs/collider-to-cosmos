"""Check event framing and the distinct EW and archived QCD weight conventions."""
from collections import Counter
from pathlib import Path
import gzip
import hashlib
import json
import math
import re
import sys

out, qcd = map(Path, sys.argv[1:3])
with gzip.open(qcd / "unweighted_events.lhe.gz", "rb") as stream:
    qcd_lhe = stream.read()
init = re.search(rb"<init>\s*([^\n]+)\n\s*([^\n]+)", qcd_lhe)
assert init[1].split()[-2:] == [b"-4", b"1"]
qcd_sigma, qcd_error, _, _ = map(float, init[2].split())
qcd_weights = Counter(float(header.split()[2]) for header in re.findall(rb"<event>\s*([^\n]+)", qcd_lhe))
assert qcd_weights == {qcd_sigma: 10000}
assert re.search(rb"(?m)^\s*average\s*=\s*event_norm", qcd_lhe)

def inspect(file, sigma, error, archive=False):
    sha = hashlib.sha256()
    events = starts = ends = weights = cross_sections = units = 0
    sequences = []
    last_sigma = None
    with file.open("rb") as stream:
        for raw in stream:
            sha.update(raw)
            prefix = raw[:2]
            if prefix == b"E ":
                fields = raw.split()
                index = int(fields[1])
                if index == 0:
                    sequences.append(0)
                assert index == sequences[-1]
                sequences[-1] += 1
                assert int(fields[8]) > 0
                first = 13 + int(fields[11])
                assert int(fields[first - 1]) == 2
                nominal, merging = map(float, fields[first:])
                assert math.isclose(nominal, sigma, rel_tol=1e-6)
                if archive:
                    assert math.isclose(merging, nominal, rel_tol=1e-10)
                else:
                    assert math.isclose(merging, nominal / 10000, rel_tol=1e-10)
                events += 1
            elif prefix == b"N ":
                assert raw.split() == [b"N", b"2", b'"Weight"', b'"Weight_MERGING=0.000"']
                weights += 1
            elif prefix == b"U ":
                assert raw.split() == [b"U", b"GEV", b"MM"]
                units += 1
            elif prefix == b"C ":
                last_sigma, err = map(float, raw.split()[1:])
                assert math.isfinite(err) and err >= 0
                if not archive:
                    assert math.isclose(last_sigma, events * merging, rel_tol=1e-9)
                cross_sections += 1
            elif raw.startswith(b"HepMC::IO_GenEvent-"):
                starts += b"START" in raw
                ends += b"END" in raw
            if raw.strip():
                last = raw
    assert events == units == weights == cross_sections == 10000
    assert starts == ends == 1 and last.strip().endswith(b"END_EVENT_LISTING")
    assert math.isclose(last_sigma, sigma, rel_tol=1e-6)
    return {"events": events, "generator_sigma_pb": sigma, "integration_error_pb": error,
            "per_event_pb": sigma / events, "hepmc_nominal_weight": nominal,
            "hepmc_merging_weight": merging, "hepmc_last_xsec_pb": last_sigma,
            "event_number_sequence_sizes": sequences, "hepmc_sha256": sha.hexdigest(),
            "hepmc_file": str(file.resolve())}

kind = sys.argv[3]
if kind == "ew":
    ew = inspect(out / "shower/ew_wpwpjj_10000_pythia8317.hepmc", 0.01042576, 1.079378e-5)
    assert ew["event_number_sequence_sizes"] == [10000]
    assert math.isclose(ew["hepmc_merging_weight"] * ew["events"], ew["generator_sigma_pb"], rel_tol=1e-6)
    log = (out / "shower/shower.log").read_text()
    stats = re.findall(r"Les Houches User Process\(es\).*?\|\s*(\d+)\s+(\d+)\s+(\d+)\s*\|", log)
    assert stats and stats[-1] == ("10000",) * 3
    inclusive = re.search(r"Inclusive cross section:\s+(\S+)\s+\+-\s+\S+\s+mb", log)
    assert inclusive and math.isclose(float(inclusive[1]) * 1e9, ew["generator_sigma_pb"], rel_tol=1e-6)
    assert "This is PYTHIA version 8.317" in log
    assert not re.search(r"(?i)pythia abort|reached end of lhef|normalisation will be decreased", log)
    print(json.dumps(ew, indent=2))
elif kind == "qcd":
    archived = inspect(qcd / "tag_1_pythia8_events.hepmc", qcd_sigma, qcd_error, archive=True)
    archived["lhe_sha256"] = hashlib.sha256((qcd / "unweighted_events.lhe.gz").read_bytes()).hexdigest()
    print(json.dumps(archived, indent=2))
else:
    raise ValueError(kind)
