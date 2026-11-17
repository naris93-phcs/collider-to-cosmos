"""Remove only blank lines between the LHE event header and first particle."""
import gzip, re
from pathlib import Path
with gzip.open('../unweighted_events.lhe.gz','rb') as f:
    original=f.read()
pattern=rb'(?m)(^<event>[ \t]*\r?\n[^\r\n]+\r?\n)(?:[ \t]*\r?\n)+(?=[ \t]*[+-]?\d)'
prepared,n=re.subn(pattern,rb'\1',original)
assert n == 9642 and not re.search(pattern, prepared)
assert original.count(b'<event>') == prepared.count(b'<event>') == 10000
assert original.splitlines() and [x for x in original.splitlines() if x.strip()]==[x for x in prepared.splitlines() if x.strip()]
with Path('prepared.lhe.gz').open('xb') as f:
    with gzip.GzipFile(fileobj=f,mode='wb',mtime=0) as out:
        out.write(prepared)
print(f'Removed post-header whitespace in {n} events; all nonblank lines unchanged')
