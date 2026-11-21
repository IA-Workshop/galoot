import re,glob,collections,os
tot=0;norm=collections.Counter();per={}
allm=[]
for f in sorted(glob.glob('include/galoot/**/*.hpp',recursive=True)):
    t=open(f).read()
    t2=re.sub(r'^\s*//.*$','',t,flags=re.M)
    ms=[]
    for m in re.finditer(r'(static_assert|GALOOT_ASSERT)\s*\(',t2):
        # find end paren
        i=m.end();d=1;instr=False
        while d and i<len(t2):
            c=t2[i]
            if c=='"' and t2[i-1]!='\\': instr=not instr
            elif not instr:
                if c=='(':d+=1
                elif c==')':d-=1
            i+=1
        body=t2[m.end():i-1]
        lits=re.findall(r'"((?:[^"\\]|\\.)*)"',body)
        msg=''.join(lits)
        if msg: ms.append((m.group(1),msg))
    per[os.path.basename(f)]=ms
    allm+=[(os.path.basename(f),k,m) for k,m in ms]
print("file static_assert-msgs GALOOT_ASSERT-msgs distinct")
for f,ms in per.items():
    print(f, sum(1 for k,_ in ms if k=='static_assert'), sum(1 for k,_ in ms if k!='static_assert'), len(set(m for _,m in ms)))
print("total",len(allm),"distinct",len(set(m for _,_,m in allm)))
# normalise by stripping "prefix: " and the subject noun
def nz(m):
    m=re.sub(r'^[^:]{1,40}: ','',m)
    return m
c=collections.Counter(nz(m) for _,_,m in allm)
print("distinct after stripping prefix",len(c))
for m,n in c.most_common(14): print(n,m[:110])
# further: scalar cannot hold family
fam=[m for _,_,m in allm if 'cannot hold one of' in m]
print("cannot-hold family",len(fam))
fam=[m for _,_,m in allm if 'different algebras' in m or 'different scalar types' in m]
print("mismatch family",len(fam))
