import csv,json,math,statistics
from pathlib import Path
import ROOT
ROOT.gROOT.SetBatch(True);ROOT.gStyle.SetOptStat(0)
p=Path(__file__).resolve().parent
rows=[list(csv.DictReader((p/(s+'.csv')).open())) for s in ['baseline','lifetime']]
a,b=rows
assert len(a)==len(b)==10000
labels=['Input','2 leptons','Same sign','mll','3rd lepton veto','Z veto','MET','b veto','2 jets','jet pt','mjj','dy']
summary={'events':len(a),'normalization':'raw unweighted counts; input weights preserved, not applied','cutflow':[],'observables':{}}
for i,(x,y) in enumerate(zip(a,b),1):
 assert x['index']==y['index']==str(i) and x['event']==y['event'] and x['weight']==y['weight']
 for k in ['metx','mety','met','pt1','pt2','mjj','dy']: assert math.isfinite(float(x[k])) and math.isfinite(float(y[k]))
summary['jet_input_identity_changed_events']=sum(x['ids']!=y['ids'] for x,y in zip(a,b))
summary['jet_input_totals']=[sum(int(x['inputs']) for x in z) for z in rows]
for j,label in enumerate(labels):
 n=[sum(x['cuts'][j]=='1' for x in z) for z in rows]
 entering=sum(x['cuts'][j]=='0' and y['cuts'][j]=='1' for x,y in zip(a,b));leaving=sum(x['cuts'][j]=='1' and y['cuts'][j]=='0' for x,y in zip(a,b))
 summary['cutflow'].append({'stage':label,'baseline':n[0],'lifetime':n[1],'entering':entering,'leaving':leaving})
assert [v['baseline'] for v in summary['cutflow']]==[10000,3258,3258,3225,3225,3115,2729,2648,2540,2246,772,339]
for key in ['metx','mety','met','njets','pt1','pt2','mjj','dy']:
 pairs=[(float(x[key]),float(y[key])) for x,y in zip(a,b) if (key not in ['pt1','pt2','mjj','dy'] or min(int(x['njets']),int(y['njets'])) >= (1 if key=='pt1' else 2))]
 d=[y-x for x,y in pairs]
 summary['observables'][key]={'paired_events':len(d),'mean_baseline':statistics.mean(x for x,y in pairs),'mean_lifetime':statistics.mean(y for x,y in pairs),'mean_delta':statistics.mean(d),'mean_abs_delta':statistics.mean(abs(x) for x in d),'max_abs_delta':max(map(abs,d)),'changed_above_1e-8':sum(abs(x)>1e-8 for x in d)}
with (p/'transitions.csv').open('x') as f:
 w=csv.writer(f);w.writerow(['index','event','baseline_cuts','lifetime_cuts','baseline_mjj','lifetime_mjj','baseline_dy','lifetime_dy'])
 for x,y in zip(a,b):
  if x['cuts']!=y['cuts']: w.writerow([x['index'],x['event'],x['cuts'],y['cuts'],x['mjj'],y['mjj'],x['dy'],y['dy']])
# Compare selected tree against the previous current-baseline checkpoint.
ref=ROOT.TFile.Open(str(p.parent/'physics_checkpoints/mjj/checkpoint.root'))
f=ROOT.TFile.Open(str(p/'baseline.root'))
t,u=f.Get('fiducial_events'),ref.Get('fiducial_events')
assert t.GetEntries()==u.GetEntries()==339
for i in range(339):
 t.GetEntry(i);u.GetEntry(i)
 for branch in t.GetListOfBranches():
  key=branch.GetName();assert abs(getattr(t,key)-getattr(u,key))<1e-9
summary['baseline_previous_checkpoint_339_entries_8_observables']='PASS (absolute tolerance 1e-9)'
with (p/'summary.json').open('x') as f:json.dump(summary,f,indent=2)
canvas=ROOT.TCanvas('impact','',1100,850);canvas.Divide(2,2);keep=[]
for panel,(key,bins,lo,hi) in enumerate([('met',50,0,250),('njets',12,-.5,11.5),('mjj',50,0,4000),('dy',40,0,10)],1):
 canvas.cd(panel); legend=ROOT.TLegend(.52,.72,.88,.87)
 for mode,z,color in zip(['baseline','lifetime'],rows,[ROOT.kBlue+1,ROOT.kRed+1]):
  h=ROOT.TH1D(mode+key,'PRELIMINARY;'+key+';Raw events / bin',bins,lo,hi)
  for x in z:
   if key in ['mjj','dy'] and int(x['njets'])<2:continue
   h.Fill(float(x[key]))
  h.SetLineColor(color);h.SetLineWidth(2);h.Draw('HIST' if mode=='baseline' else 'HIST SAME');legend.AddEntry(h,mode,'l');keep.append(h)
 legend.Draw();keep.append(legend)
canvas.SaveAs(str(p/'impact.png'));canvas.SaveAs(str(p/'impact.pdf'))
print(json.dumps(summary,indent=2))
selected={}
for k in ['met','pt1','pt2','mjj','dy']:
 d=[float(y[k])-float(x[k]) for x,y in zip(a,b) if x['cuts'][-1]=='1']
 selected[k]={'mean_abs_delta':statistics.mean(map(abs,d)),'max_abs_delta':max(map(abs,d))}
with (p/'selected_summary.json').open('x') as f:json.dump(selected,f,indent=2)
with (p/'significant_changes.csv').open('x') as f:
 w=csv.writer(f);w.writerow(['observable','index','stored_event','baseline','lifetime','delta'])
 for k in ['met','pt1','pt2','mjj','dy']:
  pairs=[(x,y) for x,y in zip(a,b) if k=='met' or min(int(x['njets']),int(y['njets']))>=(1 if k=='pt1' else 2)]
  for x,y in sorted(pairs,key=lambda z:abs(float(z[1][k])-float(z[0][k])),reverse=True)[:20]:w.writerow([k,x['index'],x['event'],x[k],y[k],float(y[k])-float(x[k])])
