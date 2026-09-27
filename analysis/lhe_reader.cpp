#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <TH1D.h>
#include <TCanvas.h>
#include <vector>
#include <algorithm>
#include <TH2D.h>


struct FourMomentum {
    double E;
    double px;
    double py;
    double pz;
};

int main() {

    std::string line;
    int eventCount = 0;
    int leptonCount = 0;
    int neutrinoCount = 0;

    TH1D hPt("w_pt",
         "W^{+} Transverse Momentum;p_{T} [GeV];Events",
         50, 0.0, 500.0);

    TH1D hRapidity("w_rapidity",
         "W^{+} Rapidity;y;Events",
         60, -6.0, 6.0);

    TH1D hQuarkRapidity(
        "quark_rapidity",
        "Outgoing Quark Rapidity;y;Entries",
        60, -6.0, 6.0
        );

    TH1D hDeltaY(
        "delta_y_qq",
        "Outgoing Quark Rapidity Separation;|#Delta y_{qq}|;Events",
        50, 0.0, 10.0
        );   
        
    TH1D hMqq(
        "m_qq",
        "Outgoing Quark Pair Invariant Mass;m_{qq} [GeV];Events",
        60, 0.0, 3000.0
        );

    TH2D hMqqVsDeltaY(
    "mqq_vs_delta_y",
    "Quark Pair Kinematics;|#Delta y_{qq}|;m_{qq} [GeV]",
    50, 0.0, 10.0,
    60, 0.0, 3000.0
        );

    TH1D hLeptonPt(
    "lepton_pt",
    "Positive Lepton Transverse Momentum;p_{T}^{l+} [GeV];Entries",
    60, 0.0, 300.0
        );


    TH1D hMissingPt(
    "missing_pt",
    "Missing Transverse Momentum;p_{T}^{miss} [GeV];Events",
    60, 0.0, 300.0
            );

    TH1D hMll(
    "m_ll",
    "Dilepton Invariant Mass;m_{ll} [GeV];Events",
    60, 0.0, 500.0
            );


    TH1D hDeltaRll(
    "delta_r_ll",
    "Dilepton Angular Separation;#Delta R_{ll};Events",
    50, 0.0, 6.0
            );
    

    while (std::getline(std::cin, line)) {

        if (line.find("<event>") != std::string::npos) {

                eventCount++;

                std::getline(std::cin, line);

                std::istringstream stream(line);

                int nParticles;
                stream >> nParticles;

               // std::cout << "Event " << eventCount
               //         << " has " << nParticles
               //         << " particle entries." << std::endl;

                std::vector<double> quarkRapidities; 
                std::vector<FourMomentum> quarkMomenta;
                std::vector<FourMomentum> neutrinoMomenta;
                std::vector<FourMomentum> leptonMomenta;

                for (int i = 0; i < nParticles; i++) {

                        std::getline(std::cin, line);

                        std::istringstream particleStream(line);

                        int pdgId, status;
                        int mother1, mother2;
                        int color1, color2;

                        double px, py, pz, energy, mass;
                        double lifetime, spin;

                        particleStream >> pdgId >> status
                                        >> mother1 >> mother2
                                        >> color1 >> color2
                                        >> px >> py >> pz >> energy >> mass
                                        >> lifetime >> spin;

                                if (pdgId == 24 && status == 1) {

                                        double pT = std::sqrt(px * px + py * py);
                                        hPt.Fill(pT);

                                        double rapidity = std::atanh(pz / energy);
                                        hRapidity.Fill(rapidity);
                                       // std::cout << "W+ pT = " << pT << " GeV"
                                        //        << std::endl;
                                    }

                                        //std::cout   << "PDG = " << pdgId
                                                // << ", status = " << status
                                                // << ", E = " << energy << " GeV"
                                                    //<< std::endl;     
                                                    
                                if (std::abs(pdgId) >= 1 &&
                                    std::abs(pdgId) <= 6 &&
                                    status == 1)
                                {
                                    double rapidity = std::atanh(pz / energy);
                                    quarkRapidities.push_back(rapidity);
                                    hQuarkRapidity.Fill(rapidity);
                                    quarkMomenta.push_back({energy, px, py, pz});
                                }

                                if ((pdgId == -11 || pdgId == -13) && status == 1) {
                                        leptonCount++;
                                        double leptonPt = std::sqrt(px*px + py*py);
                                        hLeptonPt.Fill(leptonPt);
                                        leptonMomenta.push_back({energy, px, py, pz});
                                    }

                                if ((pdgId == 12 || pdgId == 14) && status == 1) {
                                        neutrinoCount++;
                                        neutrinoMomenta.push_back({energy, px, py, pz});
                                    }
                            }
        if (quarkRapidities.size() == 2 && quarkMomenta.size() == 2) {

                    // Rapidity separation
                    double deltaY = std::abs(
                        quarkRapidities[0] - quarkRapidities[1]
                    );

                    // Four-momenta of the two outgoing quarks
                    const auto& q1 = quarkMomenta[0];
                    const auto& q2 = quarkMomenta[1];

                    // Total four-momentum of the qq system
                    double E  = q1.E  + q2.E;
                    double px = q1.px + q2.px;
                    double py = q1.py + q2.py;
                    double pz = q1.pz + q2.pz;

                    // Invariant mass
                    double m2 = E*E - px*px - py*py - pz*pz;
                    double mqq = std::sqrt(std::max(0.0, m2));

                    // 1D histograms
                    hDeltaY.Fill(deltaY);
                    hMqq.Fill(mqq);

                    // Correlation
                    hMqqVsDeltaY.Fill(deltaY, mqq);
                } 


                if (neutrinoMomenta.size() == 2) {

                    const auto& nu1 = neutrinoMomenta[0];
                    const auto& nu2 = neutrinoMomenta[1];

                    double missingPx = nu1.px + nu2.px;
                    double missingPy = nu1.py + nu2.py;

                    double missingPt = std::sqrt(
                        missingPx*missingPx + missingPy*missingPy
                    );
                    hMissingPt.Fill(missingPt);
                }  
                
                if (leptonMomenta.size() == 2) {

                    const auto& l1 = leptonMomenta[0];
                    const auto& l2 = leptonMomenta[1];

                    double E  = l1.E  + l2.E;
                    double px = l1.px + l2.px;
                    double py = l1.py + l2.py;
                    double pz = l1.pz + l2.pz;

                    double m2 = E*E - px*px - py*py - pz*pz;
                    double mll = std::sqrt(std::max(0.0, m2));
                    hMll.Fill(mll);

                    double pt1 = std::sqrt(l1.px*l1.px + l1.py*l1.py);
                    double pt2 = std::sqrt(l2.px*l2.px + l2.py*l2.py);

                    double eta1 = std::asinh(l1.pz / pt1);
                    double eta2 = std::asinh(l2.pz / pt2);

                    double phi1 = std::atan2(l1.py, l1.px);
                    double phi2 = std::atan2(l2.py, l2.px);
                    double deltaEta = eta1 - eta2;
                    double deltaPhi = phi1 - phi2;

                    if (deltaPhi > M_PI) {
                        deltaPhi -= 2.0 * M_PI;
                    }

                    if (deltaPhi < -M_PI) {
                        deltaPhi += 2.0 * M_PI;
                    }

                    double deltaR = std::sqrt(
                        deltaEta*deltaEta + deltaPhi*deltaPhi
                    );

                    hDeltaRll.Fill(deltaR);
                }
            }

    }

    std::cout << "Total events: " << eventCount << std::endl;
    std::cout << "W+ entries: " << hPt.GetEntries() << std::endl;
    std::cout << "W+ rapidity entries: " << hRapidity.GetEntries() << std::endl;
    std::cout << "Quark rapidity entries: " << hQuarkRapidity.GetEntries() << std::endl;
    std::cout << "Delta Y entries: " << hDeltaY.GetEntries() << std::endl;
    std::cout << "Mqq entries: " << hMqq.GetEntries() << std::endl;
    std::cout << "Mqq overflow (> 3000 GeV): " << hMqq.GetBinContent(hMqq.GetNbinsX() + 1) << std::endl;
    std::cout << "Positive lepton entries: " << leptonCount << std::endl;
    std::cout << "Neutrino entries: " << neutrinoCount << std::endl;
    std::cout << "Lepton pT entries: "<< hLeptonPt.GetEntries() << std::endl;
    std::cout << "Missing pT entries: "<< hMissingPt.GetEntries() << std::endl;
    std::cout << "Dilepton invariant mass entries: "<< hMll.GetEntries() << std::endl;
    std::cout << "Dilepton angular separation entries: "<< hDeltaRll.GetEntries() << std::endl;

// ============================================================
// 1. W+ transverse momentum
// ============================================================

TCanvas ptCanvas(
    "ptCanvas",
    "W+ pT Distribution",
    800, 600
);

hPt.Draw("HIST");
ptCanvas.SaveAs("w_pt.png");


// ============================================================
// 2. W+ rapidity
// ============================================================

TCanvas rapidityCanvas(
    "rapidityCanvas",
    "W+ Rapidity",
    800, 600
);

hRapidity.Draw("HIST");
rapidityCanvas.SaveAs("w_rapidity.png");


// ============================================================
// 3. W+ vs outgoing-quark rapidity
// ============================================================

TCanvas comparisonCanvas(
    "comparisonCanvas",
    "W vs Quark Rapidity",
    900, 600
);

hRapidity.SetLineColor(kBlue);
hQuarkRapidity.SetLineColor(kRed);

hRapidity.Draw("HIST");
hQuarkRapidity.Draw("HIST SAME");

comparisonCanvas.BuildLegend();
comparisonCanvas.SaveAs("rapidity_comparison.png");


// ============================================================
// 4. Quark-pair rapidity separation
// ============================================================

TCanvas deltaYCanvas(
    "deltaYCanvas",
    "Quark Rapidity Separation",
    900, 600
);

hDeltaY.SetLineColor(kBlue);
hDeltaY.Draw("HIST");

deltaYCanvas.SaveAs("delta_y_qq.png");


// ============================================================
// 5. Quark-pair invariant mass
// ============================================================

TCanvas mqqCanvas(
    "mqqCanvas",
    "Quark Pair Invariant Mass",
    900, 600
);

hMqq.SetLineColor(kBlue);
hMqq.Draw("HIST");

mqqCanvas.SaveAs("m_qq.png");


// ============================================================
// 6. Quark-pair invariant mass vs rapidity separation
// ============================================================

TCanvas correlationCanvas(
    "correlationCanvas",
    "mqq vs DeltaY",
    900, 700
);

hMqqVsDeltaY.Draw("COLZ");

correlationCanvas.SaveAs("mqq_vs_delta_y.png");


// 7. Positive lepton transverse momentum
TCanvas leptonPtCanvas(
    "leptonPtCanvas",
    "Positive Lepton pT",
    900, 600
);

hLeptonPt.Draw("HIST");
leptonPtCanvas.SaveAs("lepton_pt.png");

// 8. Missing transverse momentum
TCanvas missingPtCanvas(
    "missingPtCanvas",
    "Missing Transverse Momentum",
    900, 600
);

hMissingPt.Draw("HIST");
missingPtCanvas.SaveAs("missing_pt.png");

// 9. Dilepton invariant mass
TCanvas mllCanvas(
    "mllCanvas",
    "Dilepton Invariant Mass",
    900, 600
);

hMll.Draw("HIST");
mllCanvas.SaveAs("m_ll.png");

// 10. Dilepton angular separation
TCanvas deltaRllCanvas(
    "deltaRllCanvas",
    "Dilepton Delta R",
    900, 600
);

hDeltaRll.Draw("HIST");
deltaRllCanvas.SaveAs("delta_r_ll.png");


    return 0;
}