#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TError.h>


void plot_cutflow()
{
    TFile *file = TFile::Open("particle_level.root", "READ");

    if (!file || file->IsZombie()) {
        Error("plot_cutflow", "Cannot open particle_level.root");
        return;
    }

    TH1D *h = nullptr;
    file->GetObject("h_cutflow", h);

    if (!h) {
        Error("plot_cutflow", "Cannot find h_cutflow");
        file->Close();
        return;
    }

    TCanvas *c = new TCanvas("c_cutflow", "Cutflow", 1000, 650);

    c->SetBottomMargin(0.22);

    h->SetStats(0);
    h->SetFillStyle(0);
    h->GetXaxis()->LabelsOption("d");
    h->GetXaxis()->SetTitleOffset(2.0);

    h->Draw("BAR0");

    c->Modified();
    c->Update();
    c->SaveAs("cutflow.png");
}