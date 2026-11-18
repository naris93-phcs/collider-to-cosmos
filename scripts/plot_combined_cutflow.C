// Run from the project root. Pass false to display without exporting.
#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TLatex.h>

#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
std::vector<std::string> fields(const std::string &line) {
    std::istringstream stream(line);
    std::vector<std::string> row;
    std::string value;
    while (std::getline(stream, value, ',')) row.push_back(value);
    return row;
}
}

void plot_combined_cutflow(bool save = true) {
    // Canonical ROOT paths from docs/RESULTS_INDEX.md.
    const std::array<const char *, 4> paths = {
        "production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/analysis/ew_wpwpjj_10000_lifetime.root",
        "production_runs/ew_wmwmjj_manual_analysis_seed161805/analysis/ew_minus_lifetime.root",
        "production_runs/ew_wpwpjj_50000_seed314159/supervisor_10k/analysis/qcd_wpwpjj_archived_run04_lifetime.root",
        "production_runs/ew_wmwmjj_10000_seed161803/analysis/qcd_minus_lifetime.root"
    };
    const std::array<std::string, 4> keys = {"EW +", "EW -", "QCD +", "QCD -"};
    const std::array<const char *, 5> titles = {
        "EW W^{+}W^{+}", "EW W^{-}W^{-}", "QCD W^{+}W^{+}", "QCD W^{-}W^{-}",
        "EW + QCD without interference"
    };
    const std::array<int, 5> colors = {kBlue+1, kCyan+2, kRed+1, kOrange+7, kBlack};

    std::ifstream csv("results/cross_sections.csv");
    std::string line;
    if (!std::getline(csv, line)) throw std::runtime_error("Cannot read results/cross_sections.csv");
    const auto header = fields(line);
    std::map<std::string, std::size_t> column;
    for (std::size_t i = 0; i < header.size(); ++i) column[header[i]] = i;
    const auto sampleCol = column.at("sample");
    const auto countCol = column.at("generated"); // Analyzed count in this CSV.
    const auto sigmaCol = column.at("generator_fb");
    std::map<std::string, std::vector<std::string>> rows;
    while (std::getline(csv, line)) {
        auto row = fields(line);
        if (row.size() == header.size()) rows.emplace(row.at(sampleCol), row);
    }

    std::array<std::unique_ptr<TH1D>, 5> curves;
    for (std::size_t i = 0; i < paths.size(); ++i) {
        std::unique_ptr<TFile> file(TFile::Open(paths[i], "READ"));
        if (!file || file->IsZombie()) throw std::runtime_error(paths[i]);
        const auto *counts = file->Get<TH1D>("h_cutflow");
        const auto &row = rows.at(keys[i]);
        const double analyzed = std::stod(row.at(countCol));
        const double sigma = std::stod(row.at(sigmaCol));
        if (!counts || counts->GetNbinsX() != 12 || !std::isfinite(sigma) ||
            sigma <= 0 || analyzed <= 0 || counts->GetBinContent(1) != analyzed)
            throw std::runtime_error("Invalid cutflow or normalization for " + keys[i]);

        const auto *axis = counts->GetXaxis();
        for (int bin = 1; bin <= counts->GetNbinsX(); ++bin) {
            const std::string label = axis->GetBinLabel(bin);
            const double count = counts->GetBinContent(bin);
            if (label.empty() || !std::isfinite(count) || count < 0 ||
                (bin > 1 && count > counts->GetBinContent(bin - 1)))
                throw std::runtime_error("Invalid cumulative bin for " + keys[i]);
            if (i > 0) {
                const auto *reference = curves[0]->GetXaxis();
                if (label != reference->GetBinLabel(bin) ||
                    axis->GetBinLowEdge(bin) != reference->GetBinLowEdge(bin) ||
                    axis->GetBinUpEdge(bin) != reference->GetBinUpEdge(bin))
                    throw std::runtime_error("Selection-stage mismatch for " + keys[i] +
                                             " at bin " + std::to_string(bin));
            }
        }
        curves[i].reset(static_cast<TH1D *>(counts->Clone(("cutflow_" + std::to_string(i)).c_str())));
        curves[i]->SetDirectory(nullptr);
        curves[i]->Scale(sigma / analyzed);
    }
    curves[4].reset(static_cast<TH1D *>(curves[0]->Clone("cutflow_sum")));
    curves[4]->SetDirectory(nullptr);
    for (std::size_t i = 1; i < paths.size(); ++i) curves[4]->Add(curves[i].get());

    std::ostringstream table;
    table << "Surviving cross section [fb] — EW + QCD without interference\n"
          << std::left << std::setw(25) << "Selection stage" << std::right;
    for (const auto *name : {"EW W+W+", "EW W-W-", "QCD W+W+", "QCD W-W-", "Sum"})
        table << std::setw(13) << name;
    table << '\n' << std::fixed << std::setprecision(6);
    for (int bin = 1; bin <= curves[4]->GetNbinsX(); ++bin) {
        table << std::left << std::setw(25) << curves[4]->GetXaxis()->GetBinLabel(bin) << std::right;
        for (const auto &curve : curves) table << std::setw(13) << curve->GetBinContent(bin);
        table << '\n';
    }
    std::cout << table.str();

    auto *canvas = new TCanvas("wwjj_combined_cutflow", "Combined WWjj cutflow", 1350, 750);
    canvas->SetBottomMargin(0.23);
    canvas->SetLeftMargin(0.11);
    canvas->SetTicks(1, 1);
    canvas->SetLogy();
    auto &sum = curves[4];
    sum->SetTitle(";;Surviving cross section [fb]");
    sum->LabelsOption("v", "X");
    sum->GetXaxis()->SetLabelSize(0.03);
    sum->SetMinimum(0.06);
    sum->SetMaximum(65.0);
    for (std::size_t i = 0; i < curves.size(); ++i) {
        curves[i]->SetStats(false);
        curves[i]->SetLineColor(colors[i]);
        curves[i]->SetLineStyle(i == 1 || i == 3 ? 2 : 1);
        curves[i]->SetLineWidth(i == 4 ? 3 : 2);
    }
    std::array<TH1 *, 5> drawn{};
    drawn[4] = sum->DrawCopy("HIST");
    for (std::size_t i = 0; i < paths.size(); ++i)
        drawn[i] = curves[i]->DrawCopy("HIST SAME");
    TLegend legend(0.15, 0.28, 0.49, 0.50);
    legend.SetBorderSize(0);
    legend.SetFillStyle(0);
    for (std::size_t i = 0; i < drawn.size(); ++i) legend.AddEntry(drawn[i], titles[i], "l");
    legend.DrawClone();
    TLatex heading;
    heading.SetNDC();
    heading.SetTextSize(0.029);
    heading.DrawLatex(0.12, 0.94, "Same-sign WWjj at 13 TeV | Preliminary particle-level baseline");
    canvas->Modified();
    canvas->Update();
    if (save) {
        std::ofstream output("results/combined_cutflow.txt");
        output << table.str();
        if (!output) throw std::runtime_error("Could not write results/combined_cutflow.txt");
        canvas->SaveAs("results/combined_cutflow.pdf");
        canvas->SaveAs("results/combined_cutflow.png");
    }
}
