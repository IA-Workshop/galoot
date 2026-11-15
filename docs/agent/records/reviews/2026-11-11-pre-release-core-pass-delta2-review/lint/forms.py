import sys, subprocess, re, concurrent.futures, pathlib
inc = sys.argv[1]
PRE = open(sys.argv[2]).read().split("constexpr Line")[0]
names = ["l", "I", "O", "rn", "m", "ls", "Is", "lx", "Ix", "K", "J", "Js", "N", "lS", "ll"]
forms = {
 "global": "constexpr Line {n}{{p ^ q}};\nstatic_assert(Line{{q ^ p}} == -{n});\n",
 "local": "Line f() {{ Line {n}{{p ^ q}}; return -{n} + {n}; }}\n",
 "param": "Line f(const Line& {n}) {{ return -{n} + ({n} - {n}); }}\n",
 "tparam": "template <class {n}> auto f(const {n}& x) {{ return -x + x; }}\nauto g() {{ return f(Line{{p ^ q}}); }}\n",
}
def one(job):
    n, form = job
    u = pathlib.Path(f"u_{form}_{n}.cpp"); u.write_text(PRE + forms[form].format(n=n))
    r = subprocess.run(["clang-tidy", "--checks=-*,misc-confusable-identifiers", "--header-filter=.*", str(u), "--",
        "-std=c++23", f"-I{inc}", "-DGALOOT_CHECKING=1", "-O0"], capture_output=True, text=True)
    out = r.stdout + r.stderr
    err = "COMPILE-ERROR" if re.search(r"error: (?!.*confusable)", out) else ""
    hits = sorted(set(re.sub(r"^.*/include/", "", l.split(": ")[0]) + " " + re.search(r"'[^']*' is confusable with '[^']*'", l).group(0)
              for l in out.splitlines() if "is confusable" in l))
    return n, form, err + "; ".join(hits)
with concurrent.futures.ThreadPoolExecutor(6) as ex:
    for n, form, h in ex.map(one, [(n, f) for n in names for f in forms]):
        print(f"{form}\t{n}\t{h or '-'}")
