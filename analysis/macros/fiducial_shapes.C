// Usage from analysis/: root -l -b -q 'macros/fiducial_shapes.C()'
// Shape-only plots of selected events; NOT an ATLAS-data comparison.
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TSystem.h"
#include <iostream>

void fiducial_shapes(const char* input = "particle_level.root") {
  TFile file(input, "READ");
  if (file.IsZombie()) { std::cerr << "Cannot open " << input << '\n'; return; }
  auto* tree = file.Get<TTree>("fiducial_events");
  if (!tree) { std::cerr << "Missing fiducial_events tree\n"; return; }
  std::cout << "Selected events in tree: " << tree->GetEntries() << '\n';
  gSystem->mkdir("../plots/fiducial", true);
  struct Spec { const char* branch; const char* title; int bins; double lo, hi; };
  Spec specs[] = {
    {"mjj", "Selected events;m_{jj} [GeV];Fraction / bin", 25, 500, 3000},
    {"dy_jj", "Selected events;|#Deltay_{jj}|;Fraction / bin", 20, 2, 10},
    {"lepton1_pt", "Selected events;Leading dressed lepton p_{T} [GeV];Fraction / bin", 25, 0, 250},
    {"mll", "Selected events;m_{ll} [GeV];Fraction / bin", 30, 0, 600}
  };
  for (const auto& spec : specs) {
    TH1D hist(spec.branch, spec.title, spec.bins, spec.lo, spec.hi);
    hist.Sumw2();
    tree->Draw(Form("%s>>%s", spec.branch, spec.branch), "", "goff");
    const double n = hist.Integral(0, hist.GetNbinsX()+1);
    if (n > 0) hist.Scale(1. / n);
    TCanvas canvas("canvas", "Fiducial shape", 850, 600);
    hist.SetLineWidth(2);
    hist.Draw("HIST E");
    canvas.SaveAs(Form("../plots/fiducial/%s.png", spec.branch));
  }
}
