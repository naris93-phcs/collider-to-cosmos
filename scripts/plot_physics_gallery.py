#!/usr/bin/env python3
"""Render the thesis gallery from the four preserved lifetime-mode ROOT outputs."""
import argparse
from array import array
from bisect import bisect_right
from collections import Counter
import csv
import json
import math
from pathlib import Path

import ROOT


REPO = Path(__file__).resolve().parents[1]
SAMPLES = {
    "EW +": ("01_EW_WpWp", "EW W^{+}W^{+}jj", "#00659A", 20),
    "EW -": ("02_EW_WmWm", "EW W^{-}W^{-}jj", "#00876A", 24),
    "QCD +": ("03_QCD_WpWp", "QCD-induced W^{+}W^{+}jj", "#A4451F", 21),
    "QCD -": ("04_QCD_WmWm", "QCD-induced W^{-}W^{-}jj", "#8453A2", 25),
}
# branch: (title, ROOT axis label, unit, lower edge, minimum upper edge, step)
OBSERVABLES = {
    "mjj": ("Dijet invariant mass", "m_{jj}", "GeV", 500, 6500, 250),
    "dy_jj": ("Dijet rapidity separation", "|#Delta y_{jj}|", "", 2, 9, 0.5),
    "mll": ("Dilepton invariant mass", "m_{ll}", "GeV", 20, 500, 20),
    "lepton1_pt": ("Leading lepton transverse momentum", "p_{T}(#ell_{1})", "GeV", 25, 425, 20),
    "lepton2_pt": ("Subleading lepton transverse momentum", "p_{T}(#ell_{2})", "GeV", 25, 225, 10),
    "jet1_pt": ("Leading jet transverse momentum", "p_{T}(j_{1})", "GeV", 60, 660, 25),
    "jet2_pt": ("Subleading jet transverse momentum", "p_{T}(j_{2})", "GeV", 30, 430, 20),
    "met": ("Missing transverse momentum", "p_{T}^{miss}", "GeV", 30, 430, 20),
}
COMPARISONS = (
    ("ew_vs_qcd_WpWp", ("EW +", "QCD +")),
    ("ew_vs_qcd_WmWm", ("EW -", "QCD -")),
    ("ew_charge_comparison", ("EW +", "EW -")),
    ("qcd_charge_comparison", ("QCD +", "QCD -")),
    ("four_sample_comparison", tuple(SAMPLES)),
)
PRELIMINARY = "Preliminary particle-level prediction  |  pp, #sqrt{s} = 13 TeV"


def load_samples():
    manifest = json.loads((REPO / "results/samples.json").read_text())["samples"]
    with (REPO / "results/cross_sections.csv").open(newline="") as stream:
        rates = {row["sample"]: row for row in csv.DictReader(stream)}
    samples = {}
    for key in SAMPLES:
        entry = manifest[key]
        assert entry["analysis_mode"] == "lifetime", key
        root_path = REPO / entry["artifacts"]["root"]
        root = ROOT.TFile.Open(str(root_path), "READ")
        if not root or root.IsZombie():
            raise OSError(f"Cannot read {root_path}")
        tree = root.Get("fiducial_events")
        flow = root.Get("h_cutflow")
        if not tree or not tree.InheritsFrom("TTree") or not flow or not flow.InheritsFrom("TH1"):
            raise ValueError(f"Missing tree or cutflow: {root_path}")
        values = {name: [] for name in OBSERVABLES}
        for event in tree:
            for name in values:
                values[name].append(float(getattr(event, name)))
        labels = tuple(flow.GetXaxis().GetBinLabel(i) for i in range(1, flow.GetNbinsX() + 1))
        counts = tuple(int(flow.GetBinContent(i)) for i in range(1, flow.GetNbinsX() + 1))
        assert len(labels) == 12 and all(labels) and len(set(labels)) == 12, key
        assert all(flow.GetBinContent(i) == count for i, count in enumerate(counts, 1)), key
        assert all(a >= b >= 0 for a, b in zip(counts, counts[1:])), key
        assert counts[0] == int(rates[key]["generated"]), key
        assert counts[-1] == tree.GetEntries() == int(rates[key]["selected"]), key
        sigma = float(rates[key]["generator_fb"])
        assert math.isfinite(sigma) and sigma > 0, key
        assert math.isclose(sigma * counts[-1] / counts[0], float(rates[key]["fiducial_fb"])), key
        samples[key] = (values, labels, counts, sigma)
        root.Close()
    reference_labels = samples["EW +"][1]
    if any(sample[1] != reference_labels for sample in samples.values()):
        raise ValueError("Selection-stage labels differ; combined cutflow cannot be built.")
    return samples


def common_bins(samples, name, lower, upper, step):
    values = [x for sample in samples.values() for x in sample[0][name]]
    assert all(math.isfinite(x) and x >= lower for x in values), name
    upper = max(upper, lower + (math.floor((max(values) - lower) / step) + 1) * step)
    return tuple(lower + i * step for i in range(round((upper - lower) / step) + 1))


def histogram(values, edges):
    indices = Counter(bisect_right(edges, x) - 1 for x in values)
    bins = [indices[i] for i in range(len(edges) - 1)]
    # Zero flow and total count closure are required, so normalization includes every event.
    assert not indices[-1] and not indices[len(edges) - 1] and sum(bins) == len(values)
    return bins


def color(key):
    return ROOT.TColor.GetColor(SAMPLES[key][2]) if key in SAMPLES else ROOT.kBlack


def make_series(key, points, errors, name=None):
    line_color = color(key)
    marker = SAMPLES[key][3] if key in SAMPLES else 33
    graph = ROOT.TGraphErrors(len(points))
    graph.SetName(name or key)
    graph.SetLineColor(line_color)
    graph.SetMarkerColor(line_color)
    graph.SetLineWidth(3 if key in SAMPLES else 5)
    graph.SetMarkerStyle(marker)
    graph.SetMarkerSize(1.2 if key in SAMPLES else 1.5)
    if key.endswith("-"):
        graph.SetLineStyle(2)
    for i, ((x, y), error) in enumerate(zip(points, errors)):
        graph.SetPoint(i, x, y)
        graph.SetPointError(i, 0, error)
    return graph


def render(path, title, xlabel, ylabel, edges, series, stages=None, log=False, ymax=None):
    canvas = ROOT.TCanvas(path.stem, "", 1800, 1120)
    canvas.SetCanvasSize(1800, 1120)
    canvas.SetMargin(0.15, 0.045, 0.24 if stages else 0.13, 0.25)
    canvas.SetTicks(1, 1)
    canvas.SetLogy(log)
    frame = ROOT.TH1D(f"axis_{path.stem}", "", len(edges) - 1, array("d", edges))
    frame.SetDirectory(0)
    frame.GetXaxis().SetTitle(xlabel)
    frame.GetYaxis().SetTitle(ylabel)
    frame.GetYaxis().SetTitleOffset(1.55)
    frame.GetYaxis().CenterTitle()
    frame.GetYaxis().SetLabelSize(0.029)
    frame.GetXaxis().SetTitleOffset(1.10)
    if stages:
        for i, label in enumerate(stages, 1):
            frame.GetXaxis().SetBinLabel(i, label)
        frame.LabelsOption("v", "X")
        frame.GetXaxis().SetLabelSize(0.026)
    highs, positives = [], []
    for graph, _ in series:
        for i in range(graph.GetN()):
            y = graph.GetPointY(i)
            highs.append(y + graph.GetErrorY(i))
            if y > 0:
                positives.append(y)
    frame.SetMaximum(ymax if ymax is not None else max(highs) * (1.6 if log else 1.18))
    frame.SetMinimum(min(positives) * 0.55 if log else 0)
    frame.Draw("AXIS")
    for graph, _ in series:
        graph.Draw("LPZ SAME" if stages else "PZ SAME")
    legend = ROOT.TLegend(0.15, 0.755, 0.95, 0.855)
    legend.SetNColumns(2 if len(series) > 2 else len(series))
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextSize(0.025)
    for graph, label in series:
        legend.AddEntry(graph, label, "lp" if stages else "pe")
    legend.Draw()
    text = ROOT.TLatex()
    text.SetNDC()
    text.SetTextFont(62)
    text.SetTextSize(0.043)
    text.DrawLatex(0.15, 0.933, title)
    text.SetTextFont(42)
    text.SetTextSize(0.028)
    text.DrawLatex(0.15, 0.88, PRELIMINARY)
    canvas.Modified()
    canvas.Update()
    canvas.SaveAs(str(path))
    canvas.Close()


def distribution_series(key, counts, edges, normalize):
    total = sum(counts)
    widths = [b - a for a, b in zip(edges, edges[1:])]
    centers = [(a + b) / 2 for a, b in zip(edges, edges[1:])]
    fractions = [n / total for n in counts]
    if normalize:
        heights = [p / w for p, w in zip(fractions, widths)]
        errors = [math.sqrt(p * (1 - p) / total) / w for p, w in zip(fractions, widths)]
        assert math.isclose(sum(y * w for y, w in zip(heights, widths)), 1)
    else:
        heights = counts
        errors = [math.sqrt(n) for n in counts]
    return make_series(key, list(zip(centers, heights)), errors)


def cutflow_series(key, sample, normalized):
    _, _, counts, sigma = sample
    n = counts[0]
    weight = sigma / n if normalized else 1
    points = [(i + 0.5, count * weight) for i, count in enumerate(counts)]
    errors = [weight * math.sqrt(count * (1 - count / n)) for count in counts]
    return make_series(key, points, errors)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=REPO / "results/physics_gallery",
                        help="New gallery directory; an existing directory is refused.")
    out = parser.parse_args().output.resolve()
    if out.exists():
        parser.error(f"Output already exists: {out}")
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)
    ROOT.gStyle.SetEndErrorSize(4)
    samples = load_samples()
    out.mkdir(parents=True)
    for folder, *_ in SAMPLES.values():
        (out / folder).mkdir()
    comparisons = out / "05_Comparisons"
    comparisons.mkdir()

    for name, (title, variable, unit, lower, upper, step) in OBSERVABLES.items():
        edges = common_bins(samples, name, lower, upper, step)
        xlabel = f"{variable} [{unit}]" if unit else variable
        counts = {key: histogram(sample[0][name], edges) for key, sample in samples.items()}
        for key, (folder, label, _, _) in SAMPLES.items():
            graph = distribution_series(key, counts[key], edges, False)
            render(out / folder / f"{name}_fiducial.png", title, xlabel, "Events / bin", edges,
                   [(graph, f"{label}  |  {sum(counts[key]):,} selected")])
        if name in ("mjj", "dy_jj"):
            graphs = {key: distribution_series(key, counts[key], edges, True) for key in SAMPLES}
            ymax = 1.18 * max(graph.GetPointY(i) + graph.GetErrorY(i)
                              for graph in graphs.values() for i in range(graph.GetN()))
            for basename, keys in COMPARISONS:
                series = [(graphs[key], SAMPLES[key][1]) for key in keys]
                ylabel = "Unit-area density [GeV^{-1}]" if unit else "Unit-area density"
                render(comparisons / f"{basename}_{name}_shape.png", title, xlabel, ylabel, edges, series, ymax=ymax)

    labels = samples["EW +"][1]
    stage_edges = tuple(range(len(labels) + 1))
    for key, (folder, label, _, _) in SAMPLES.items():
        graph = cutflow_series(key, samples[key], False)
        render(out / folder / "cumulative_cutflow_counts.png", "Cumulative selection cutflow", "",
               "Surviving events", stage_edges, [(graph, label)], stages=labels)

    components = [cutflow_series(key, samples[key], True) for key in SAMPLES]
    points, errors = [], []
    for i in range(len(labels)):
        points.append((i + 0.5, sum(g.GetPointY(i) for g in components)))
        errors.append(math.hypot(*(g.GetErrorY(i) for g in components)))
    combined = make_series("sum", points, errors)
    series = list(zip(components, (sample[1] for sample in SAMPLES.values())))
    series.append((combined, "EW + QCD sum, interference excluded"))
    render(comparisons / "combined_cross_section_cutflow_fb.png", "Cross-section-weighted cutflow", "",
           "Surviving cross section [fb]", stage_edges, series, stages=labels, log=True)

    files = sorted(out.rglob("*.png"))
    assert len(files) == 47
    for filename in files:
        image = ROOT.TASImage(str(filename))
        assert image.IsValid() and image.GetWidth() == 1800 and image.GetHeight() == 1120, filename
    print(f"Decoded and verified {len(files)} PNGs at 1800 × 1120 in {out}")


if __name__ == "__main__":
    main()
