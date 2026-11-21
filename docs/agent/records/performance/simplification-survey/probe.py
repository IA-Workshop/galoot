#!/usr/bin/env python3
import sys,subprocess,time,resource,os,tempfile
S=os.path.dirname(os.path.abspath(__file__)); G=os.environ['GALOOT_ROOT']
v,cxx,src=sys.argv[1:4]; extra=sys.argv[4:]
inc = v if v.startswith('/') else f'{S}/{v}/include'
cmd=[cxx,'-ffp-contract=off','-D_GLIBCXX_ASSERTIONS','-std=c++23','-DDOCTEST_CONFIG_SUPER_FAST_ASSERTS',*extra,f'-I{inc}',f'-I{G}/test','-isystem',f'{G}/include/ext','-c',src if src.startswith('/') else f'{G}/{src}','-o','/dev/null']
pf=tempfile.mktemp()
t=time.time()
r=subprocess.run(['perf','stat','-x,','-e','instructions:u','-o',pf,'--',*cmd],capture_output=True,text=True)
w=time.time()-t
rss=resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
inst=sum(int(l.split(',')[0]) for l in open(pf) if 'instructions' in l and l.split(',')[0].isdigit())
print(f"{os.path.basename(v) if not v.startswith('/') else v.split('/')[-2]}\t{cxx}\t{os.path.basename(src)}\t{' '.join(extra)}\twall={w:.1f}s\trss={rss/1048576:.2f}GiB\tinst={inst/1e9:.2f}G\trc={r.returncode}",flush=True)
if r.returncode: sys.stderr.write(r.stderr[-1500:])
