import ROOT, json
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
f=ROOT.TFile.Open('checkpoint.root')
hs=[f.Get(n) for n in ['checkpoint_pre','checkpoint_mass','checkpoint_full']]
c=ROOT.TCanvas('comparison','',1100,750); c.SetLogy(); c.SetBottomMargin(.14)
colors=[ROOT.kGray+2,ROOT.kBlue+1,ROOT.kRed+1]
labels=['Pre-VBS (all preceding cuts)','+ m_{jj} > 500 GeV','+ |#Deltay_{jj}| > 2 (full baseline)']
legend=ROOT.TLegend(.42,.65,.88,.82); legend.SetBorderSize(0)
for i,h in enumerate(hs):
 h.SetLineColor(colors[i]);h.SetLineWidth(3);h.SetTitle(';m_{jj} [GeV];Raw events / 100 GeV');h.SetMinimum(.3);h.SetMaximum(1000)
 assert h.Integral(41,131)==0, 'Nonempty hidden high-mass bins'
 h.GetXaxis().SetRangeUser(0,4000)
 h.Draw('HIST' if i==0 else 'HIST SAME');legend.AddEntry(h,f'{labels[i]}: N={int(h.GetEntries())}','l')
legend.Draw(); text=ROOT.TLatex();text.SetNDC();text.SetTextSize(.029)
text.DrawLatex(.13,.93,'PRELIMINARY: SM EW W^{+}W^{+}jj, 10,000 events')
text.DrawLatex(.13,.88,'status==1 baseline; unit fills; no area / luminosity scaling')
line=ROOT.TLine(500,.3,500,1000);line.SetLineStyle(2);line.Draw()
c.SaveAs('mjj_comparison.png');c.SaveAs('mjj_comparison.pdf')
report={}
for h in hs:
 report[h.GetName()]={'entries':int(h.GetEntries()),'mean_GeV':h.GetMean(),'underflow':h.GetBinContent(0),'overflow':h.GetBinContent(131),'N_500_1000':h.Integral(6,10),'N_above_1000':h.Integral(11,130)}
print(json.dumps(report,indent=2))
# counts.json from the initial full-range statistics is preserved.
