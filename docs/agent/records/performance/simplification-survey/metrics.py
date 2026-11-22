import re,sys,glob,os
files=sorted(glob.glob('include/galoot/core/*.hpp'))+['include/galoot/core.hpp']+sorted(glob.glob('include/galoot/presets/**/*.hpp',recursive=True))
tot=[0]*8
print("file\tlines\tblank\tdoc///\tcmt//\tcode\tcmt/code\tstatic_assert\ttemplate<\tconcepts\tmacros")
for f in files:
    L=open(f).read().split('\n')
    if L[-1]=='':L=L[:-1]
    blank=doc=cmt=code=0
    for l in L:
        s=l.strip()
        if not s: blank+=1
        elif s.startswith('///'): doc+=1
        elif s.startswith('//'): cmt+=1
        else: code+=1
    t=open(f).read()
    sa=len(re.findall(r'static_assert',t)); tp=len(re.findall(r'^\s*template\s*<',t,re.M)); cc=len(re.findall(r'^\s*concept\s',t,re.M))+len(re.findall(r'\bconcept\s+\w+\s*=',t)); mc=len(re.findall(r'^\s*#\s*define',t,re.M))
    print(f"{os.path.basename(f)}\t{len(L)}\t{blank}\t{doc}\t{cmt}\t{code}\t{(doc+cmt)/max(code,1):.2f}\t{sa}\t{tp}\t{cc}\t{mc}")
    for i,v in enumerate([len(L),blank,doc,cmt,code]): tot[i]+=v
print("TOTAL\t%d\t%d\t%d\t%d\t%d\t%.2f"%(tot[0],tot[1],tot[2],tot[3],tot[4],(tot[2]+tot[3])/tot[4]))
