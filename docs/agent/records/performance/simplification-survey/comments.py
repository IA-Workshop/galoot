import re,glob,os,collections
T=collections.Counter()
print("file\tprologue\tdoc_pub\tdoc_detail\tinline_pub\tinline_detail\tNOLINT\texample_lines\tlongest_block")
cite=[];proc=[]
pat_cite=re.compile(r'\b(decision|plan \d|WU ?\d|research note|lesson|spike|audit|workbench|blind review|governance|CONSTITUTION|STYLE|VVUQ|\b[CSVWP]\d{1,2}\b|DoD|gate \d|milestone|agent)\b',re.I)
pat_proc=re.compile(r'\b(measured|measurement|the structure test|is checked for|was found|test found|benchmark|is tested|tests? (pin|check|verif)|records?)\b',re.I)
for f in sorted(glob.glob('include/galoot/**/*.hpp',recursive=True)):
    L=open(f).read().split('\n')
    depth_detail=False; seen_include=False
    c=collections.Counter(); run=0; longest=0
    for i,l in enumerate(L,1):
        s=l.strip()
        if s.startswith('#include'): seen_include=True
        if re.match(r'namespace (galoot::core::)?detail\b',s) or s.startswith('namespace galoot::core::detail'): depth_detail=True
        if s.startswith('} // namespace detail') or s.startswith('} // namespace galoot::core::detail'): depth_detail=False
        if s.startswith('//'):
            run+=1; longest=max(longest,run)
            if 'NOLINT' in s: c['nolint']+=1
            if not seen_include: c['prologue']+=1
            elif s.startswith('///'): c['doc_detail' if depth_detail else 'doc_pub']+=1
            else: c['inline_detail' if depth_detail else 'inline_pub']+=1
            if re.match(r'//+\s{3,}\S',s): c['example']+=1
            if pat_cite.search(s): cite.append((os.path.basename(f),i,s[:120]))
            if pat_proc.search(s): proc.append((os.path.basename(f),i,s[:120]))
        else: run=0
    print(f"{os.path.basename(f)}\t{c['prologue']}\t{c['doc_pub']}\t{c['doc_detail']}\t{c['inline_pub']}\t{c['inline_detail']}\t{c['nolint']}\t{c['example']}\t{longest}")
    T.update(c)
print("TOTAL",dict(T))
print("cite-pattern hits",len(cite))
for x in cite[:40]: print(x)
print("process-pattern hits",len(proc))
for x in proc[:45]: print(x)
