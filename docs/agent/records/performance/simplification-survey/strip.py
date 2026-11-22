import sys,re,os
# keep line numbers; drop full-line comments and blank lines; collapse comment runs into a marker with count
for f in sys.argv[1:]:
    out=[];run=0
    for i,l in enumerate(open(f).read().split('\n'),1):
        s=l.strip()
        if s.startswith('//'):
            run+=1; continue
        if run:
            out.append(f"      [c{run}]"); run=0
        if not s: continue
        out.append(f"{i:5d} {l}")
    open(os.path.join(os.environ['S'],'strip',os.path.basename(f)),'w').write('\n'.join(out)+'\n')
