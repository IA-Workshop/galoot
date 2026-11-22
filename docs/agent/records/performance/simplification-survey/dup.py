import re,glob,os,collections
files=sorted(glob.glob('include/galoot/**/*.hpp',recursive=True))
# --- code duplication: normalized token windows
def toks(t):
    t=re.sub(r'//.*','',t)
    t=re.sub(r'"(?:[^"\\]|\\.)*"','"S"',t)
    return re.findall(r'[A-Za-z_]\w*|\d+|\S',t)
W=40
win=collections.defaultdict(list)
alltoks={}
for f in files:
    tk=toks(open(f).read()); alltoks[f]=tk
    # normalise identifiers? keep keywords, replace other ids by 'I' for type-2 clones
    for i in range(0,len(tk)-W):
        win[hash(tuple(tk[i:i+W]))].append((f,i))
dup=set()
for h,v in win.items():
    if len(v)>1:
        for f,i in v:
            for k in range(i,i+W): dup.add((f,k))
tot=sum(len(v) for v in alltoks.values())
print("type-1 (exact, strings masked) duplicated tokens: %d of %d = %.1f%%"%(len(dup),tot,100*len(dup)/tot))
per=collections.Counter(f for f,_ in dup)
for f in files:
    if per[f]: print("  %-18s %5d / %5d = %4.1f%%"%(os.path.basename(f),per[f],len(alltoks[f]),100*per[f]/len(alltoks[f])))
# type-2: identifiers normalised
kw=set("template class struct constexpr consteval const auto static inline if else for while return requires concept using namespace typename noexcept true false static_assert bool int".split())
win=collections.defaultdict(list);dup=set()
for f in files:
    tk=[t if (t in kw or not re.match(r'[A-Za-z_]',t)) else 'I' for t in alltoks[f]]
    for i in range(0,len(tk)-60):
        win[hash(tuple(tk[i:i+60]))].append((f,i))
for h,v in win.items():
    if len(v)>1:
        for f,i in v:
            for k in range(i,i+60): dup.add((f,k))
print("type-2 (identifiers normalised, window 60): %d of %d = %.1f%%"%(len(dup),tot,100*len(dup)/tot))
per=collections.Counter(f for f,_ in dup)
for f in files:
    if per[f]: print("  %-18s %5d / %5d = %4.1f%%"%(os.path.basename(f),per[f],len(alltoks[f]),100*per[f]/len(alltoks[f])))
# --- comment duplication: sentences repeated
sent=collections.Counter();where=collections.defaultdict(set)
for f in files:
    L=open(f).read().split('\n'); buf=[]
    def flush():
        txt=' '.join(buf); buf.clear()
        for s in re.split(r'(?<=[.:;])\s+',txt):
            s=s.strip()
            if len(s)>50: sent[s]+=1; where[s].add(os.path.basename(f))
    for l in L:
        s=l.strip()
        if s.startswith('//'): buf.append(s.lstrip('/').strip())
        else: flush()
    flush()
rep=[(n,s) for s,n in sent.items() if n>1]
print("comment sentences (>50 chars) repeated: %d distinct, %d occurrences, ~%d chars"%(len(rep),sum(n for n,_ in rep),sum(n*len(s) for n,s in rep)))
for n,s in sorted(rep,reverse=True)[:12]: print(n,s[:150])
