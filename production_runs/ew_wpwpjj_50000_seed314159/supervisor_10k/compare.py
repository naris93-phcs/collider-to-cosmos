"""Make the supervisor comparison from selected, same-mode ROOT trees."""
from array import array
from bisect import bisect_right
import csv
import json
import math
from pathlib import Path
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetTitleBorderSize(0)
output = Path(__file__).resolve().parent
analysis = output / "analysis"
paths = {"EW": analysis / "ew_wpwpjj_10000_lifetime.root",
         "QCD": analysis / "qcd_wpwpjj_archived_run04_lifetime.root"}
data = {}
for label, path in paths.items():
    meta = json.loads((analysis / f"{label.lower()}_hepmc_verification.json").read_text())
    source = ROOT.TFile.Open(str(path), "READ")
    assert source and not source.IsZombie() and not source.TestBit(ROOT.TFile.kRecovered)
    keys = [key.GetName() for key in source.GetListOfKeys()]
    assert len(keys) == len(set(keys)) == 14 and all(source.Get(key) for key in keys)
    flow = source.Get("h_cutflow")
    assert flow.GetNbinsX() == 12
    stages = [(flow.GetXaxis().GetBinLabel(i), int(flow.GetBinContent(i))) for i in range(1, 13)]
    tree = source.Get("fiducial_events")
    n, selected = stages[0][1], int(tree.GetEntries())
    assert n == meta["events"] == 10000
    assert selected == stages[-1][1] and 0 < selected < n
    assert all(a[1] >= b[1] for a, b in zip(stages, stages[1:]))
    thresholds = {"mjj": 500, "dy_jj": 2, "jet1_pt": 65, "jet2_pt": 35,
                  "mll": 20, "met": 30, "lepton1_pt": 27, "lepton2_pt": 27}
    observables = {"mjj": [], "dy_jj": []}
    indices = set()
    for row in tree:
        idx = int(row.event_index)
        assert 1 <= idx <= n and idx not in indices
        indices.add(idx)
        for name, threshold in thresholds.items():
            value = getattr(row, name)
            assert math.isfinite(value) and value > threshold
        for name in observables:
            observables[name].append(getattr(row, name))
    source.Close()
    acceptance = selected / n
    stat = math.sqrt(acceptance * (1 - acceptance) / n)
    sigma_fb = meta["generator_sigma_pb"] * 1000
    data[label] = {"generated": n, "selected": selected, "acceptance": acceptance,
                   "acceptance_mc_stat": stat, "generator_fb": sigma_fb,
                   "generator_integration_error_fb": meta["integration_error_pb"] * 1000,
                   "fiducial_fb": sigma_fb * acceptance, "fiducial_mc_stat_fb": sigma_fb * stat,
                   "fiducial_propagated_integration_error_fb": 1000 * meta["integration_error_pb"] * acceptance,
                   "cutflow": stages, "observables": observables}

assert [name for name, _ in data["EW"]["cutflow"]] == [name for name, _ in data["QCD"]["cutflow"]]
with (output / "cutflow.csv").open("x") as f:
    writer = csv.writer(f)
    writer.writerow(["stage", "EW_events", "QCD_events", "EW_fraction_of_generated", "QCD_fraction_of_generated"])
    for (name, ew), (_, qcd) in zip(data["EW"]["cutflow"], data["QCD"]["cutflow"]):
        writer.writerow([name, ew, qcd, ew / data["EW"]["generated"], qcd / data["QCD"]["generated"]])

def make_bins(name, edges, step):
    maximum = max(max(item["observables"][name]) for item in data.values())
    while edges[-1] <= maximum:
        edges.append(edges[-1] + step)
    return edges

variables = [
    ("mjj", "m_{jj} [GeV]", "GeV", make_bins("mjj", [500, 650, 800, 1000, 1250, 1600, 2000, 2500, 3500, 5000], 1500)),
    ("dy_jj", "|#Delta y_{jj}|", "", make_bins("dy_jj", [2 + 0.5 * i for i in range(15)], 0.5)),
]
palette = {"EW": ROOT.TColor.GetColor("#166d84"), "QCD": ROOT.TColor.GetColor("#ac4820")}
fig = ROOT.TCanvas("comparison", "Fiducial same-sign W+W+jj at 13 TeV", 1440, 1010)
fig.Divide(2, 2, 0.001, 0.001)
keep = []
with (output / "distributions.csv").open("x") as f:
    writer = csv.writer(f)
    writer.writerow(["observable", "sample", "edge_low", "edge_high", "events", "unit_area_density",
                     "unit_area_density_mc_stat", "differential_fiducial_fb", "differential_fiducial_mc_stat_fb"])
    for row, (name, xlabel, unit, edges) in enumerate(variables):
        histograms = {"shape": {}, "physical": {}}
        for label, item in data.items():
            counts = [0] * (len(edges) - 1)
            for value in item["observables"][name]:
                counts[bisect_right(edges, value) - 1] += 1
            assert sum(counts) == item["selected"]
            for norm in histograms:
                hist = ROOT.TH1D(f"{name}_{label}_{norm}", "", len(counts), array("d", edges))
                hist.SetDirectory(0)
                hist.SetLineColor(palette[label])
                hist.SetLineWidth(2)
                hist.SetMarkerColor(palette[label])
                hist.SetMarkerStyle(20 if label == "EW" else 25)
                hist.SetMarkerSize(0.85)
                histograms[norm][label] = hist
            for idx, (low, high, count) in enumerate(zip(edges, edges[1:], counts), 1):
                width = high - low
                shape_fraction = count / item["selected"]
                absolute_fraction = count / item["generated"]
                shape = shape_fraction / width
                shape_stat = math.sqrt(shape_fraction * (1 - shape_fraction) / item["selected"]) / width
                absolute = item["generator_fb"] * absolute_fraction / width
                absolute_stat = item["generator_fb"] * math.sqrt(absolute_fraction * (1 - absolute_fraction) / item["generated"]) / width
                for norm, val, err in (("shape", shape, shape_stat), ("physical", absolute, absolute_stat)):
                    histograms[norm][label].SetBinContent(idx, val)
                    histograms[norm][label].SetBinError(idx, err)
                writer.writerow([name, label, low, high, count, shape, shape_stat, absolute, absolute_stat])
            assert math.isclose(histograms["shape"][label].Integral("width"), 1, rel_tol=1e-12)
            assert math.isclose(histograms["physical"][label].Integral("width"), item["fiducial_fb"], rel_tol=1e-12)
        for col, norm in enumerate(("shape", "physical")):
            pad = fig.cd(row * 2 + col + 1)
            pad.SetLeftMargin(0.16)
            pad.SetBottomMargin(0.17)
            pad.SetTopMargin(0.12)
            pad.SetRightMargin(0.06)
            pad.SetTicks(1, 1)
            hists = histograms[norm]
            first = hists["EW"]
            first.SetTitle("Unit-area comparison" if norm == "shape" else "Fiducial cross-section normalization")
            first.GetXaxis().SetTitle(xlabel)
            first.GetYaxis().SetTitle(("Unit-area density" + (" [1/GeV]" if unit else "")) if norm == "shape"
                                      else "d#sigma_{fid}/dx [" + ("fb/GeV" if unit else "fb") + "]")
            first.GetYaxis().SetTitleOffset(1.55)
            if name == "mjj":
                pad.SetLogx()
                first.GetXaxis().SetMoreLogLabels()
                first.GetXaxis().SetNoExponent()
            first.SetMinimum(0)
            first.SetMaximum(1.32 * max(h.GetMaximum() for h in hists.values()))
            first.Draw("HIST")
            hists["QCD"].Draw("HIST SAME")
            for hist in hists.values():
                hist.Draw("E1 SAME")
            legend = ROOT.TLegend(0.60, 0.72, 0.92, 0.87)
            legend.SetBorderSize(0)
            legend.SetFillStyle(0)
            for label, hist in hists.items():
                legend.AddEntry(hist, f"{label} ({data[label]['selected']:,} selected)", "lep")
            legend.Draw()
            keep.extend([legend, *hists.values()])

fig.Print(str(output / "ew_vs_qcd_fiducial.pdf"))
fig.Print(str(output / "ew_vs_qcd_fiducial.png"))
with (output / "comparison.json").open("x") as f:
    json.dump({label: {k: v for k, v in item.items() if k != "observables"} for label, item in data.items()}, f, indent=2)
