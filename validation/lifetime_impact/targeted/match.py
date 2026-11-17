import math,csv
from collections import defaultdict
def read(mode):
 jets=defaultdict(list)
 for row in csv.reader(open(mode+'.jets.tsv'),delimiter='\t'):
  i,stage,rank,*v,cs=row
  c={}
  for item in cs.rstrip(';').split(';'):
   b,p,e,g=item.split(':')
   if g=='0':c[int(b)]=(int(p),float(e))
  jets[int(i),stage].append((int(rank),list(map(float,v)),c))
 return jets
B,L=map(read,('baseline','lifetime'))
def signature(j):return frozenset(j[2])
def dr(x,y):return math.hypot(x[1][5]-y[1][5],math.remainder(x[1][7]-y[1][7],2*math.pi))
def score(x,y):
 a,b=x[2],y[2];common=a.keys()&b.keys()
 shared=sum(a[k][1] for k in common)
 return shared,shared/sum(v[1] for v in a.values()),shared/sum(v[1] for v in b.values())
for i in (4619,9905,2673,3298,8879,628,4403):
 print(i)
 for mode,J in [('B',B),('L',L)]:
  print(mode,[(s,[(r,round(v[4],3),round(v[6],3)) for r,v,c in J[i,s]]) for s in ('pre','post','vbs')])
 for stage in ('pre','vbs'):
  for y in L[i,stage][:3]:
   pairs=sorted(((score(x,y),dr(x,y),x) for x in B[i,stage]),key=lambda t:-t[0][0])[:2]
   print(stage,'L',y[0],[(x[0],tuple(round(v,3) for v in sc),round(d,3)) for sc,d,x in pairs])
 for mode,J in [('B',B),('L',L)]:
  for a,b in [('pre','post'),('post','vbs')]:
   gone=[(r,round(v[4],3)) for r,v,c in J[i,a] if frozenset(c) not in {signature(j) for j in J[i,b]}]
   print(mode,a+'->'+b,'removed ranks/pt',gone)
 for y in L[i,'vbs'][:2]:
  x=max(B[i,'pre'],key=lambda x:score(x,y)[0])
  for name,j,k in [('removed',x,y),('added',y,x)]:
   diff=[(bc,*j[2][bc]) for bc in j[2].keys()-k[2].keys()]
   print('L VBS',y[0], 'B pre',x[0],name,'n',len(diff),'E',round(sum(t[2] for t in diff),3),'top',sorted(diff,key=lambda t:-t[2])[:3])
