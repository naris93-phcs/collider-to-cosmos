import csv, math
targets={4619,9905,2673,3298,8879,628,4403}
for mode in ('baseline','lifetime'):
 with open(mode+'.csv') as f: observed={int(x['index']):x for x in csv.DictReader(f)}
 with open('../'+mode+'.csv') as f: expected={int(x['index']):x for x in csv.DictReader(f) if int(x['index']) in targets}
 assert set(observed)==set(expected)==targets
 for i in targets:
  a,b=observed[i],expected[i]
  for k in ('index','event','weight','inputs','njets','cuts','ids'):assert a[k]==b[k],(mode,i,k)
  for k in ('pt1','pt2','mjj','dy','met','metx','mety'):assert abs(float(a[k])-float(b[k]))<1e-9,(mode,i,k)
 with open(mode+'.jets.tsv') as f:
  seen={}
  for row in csv.reader(f,delimiter='\t'):
   i,stage,rank,*v,cs=row
   assert int(i) in targets
   parts=[x.split(':') for x in cs.rstrip(';').split(';')]
   assert abs(float(v[3])-math.fsum(float(x[2]) for x in parts))<1e-9
   key=(i,stage)
   ids={(x[0],x[3]) for x in parts}
   assert len(ids)==len(parts) and not seen.setdefault(key,set()).intersection(ids)
   seen[key].update(ids)
 print(mode,'PASS: original event rows and jet constituent energies/identities')
