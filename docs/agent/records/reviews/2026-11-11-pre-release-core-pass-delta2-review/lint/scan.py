import concurrent.futures, pathlib, subprocess, sys, string, re
inc, out = sys.argv[1], sys.argv[2]
names = list(string.ascii_letters) + ["rn", "ls", "Is", "Js", "lS", "ll", "II", "lI", "Il", "O0", "OO", "I1", "l1", "i1",
  "pl", "pI", "PI", "Pl", "al", "aI", "el", "eI", "cl", "cI", "rl", "rI", "ln", "In", "vv", "w", "nn", "rm", "m1", "rn1",
  "KX", "KY", "NX", "NY", "Ks", "lx", "Ix", "ly", "Iy", "li", "Ii", "lj", "Ij", "Ln", "LI", "Ll", "ml", "mI", "cO", "c0", "Oi", "Ok"]
PRE = """#include <galoot/core/basis_change.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/unary.hpp>
#include <galoot/core/transform.hpp>
#include <galoot/presets/rga3.hpp>
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Line = rga3::Line<int>;
constexpr Point p{1, 0, 0, 1};
constexpr Point q{1, 2, 0, 1};
"""
def one(n):
    u = pathlib.Path(sys.argv[3]) / f"u_{n}.cpp"
    u.write_text(PRE + f"constexpr Line {n}{{p ^ q}};\nstatic_assert(Line{{q ^ p}} == -{n});\nstatic_assert(({n} + {n}) == -(-{n} - {n}));\n")
    r = subprocess.run(["clang-tidy", "--checks=-*,misc-confusable-identifiers", "--header-filter=.*", str(u), "--",
        "-std=c++23", f"-I{inc}", "-DGALOOT_CHECKING=1", "-O0"], capture_output=True, text=True)
    hits = sorted(set(re.sub(r"^.*/include/", "", l.split(": ")[0]) + " " + re.search(r"'[^']*' is confusable with '[^']*'", l).group(0)
              for l in (r.stdout + r.stderr).splitlines() if "is confusable" in l))
    return n, hits
with concurrent.futures.ThreadPoolExecutor(6) as ex:
    res = list(ex.map(one, names))
with open(out, "w") as f:
    for n, h in res:
        f.write(f"{n}\t{'; '.join(h) if h else '-'}\n")
print(sum(bool(h) for _, h in res), "of", len(res), "names fire")
