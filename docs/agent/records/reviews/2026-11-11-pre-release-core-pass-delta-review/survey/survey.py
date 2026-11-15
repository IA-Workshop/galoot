"""Compile each public operation alone and check it with kernels_inline.py.

Usage: python3 -I survey.py <include-dir> <checker> <out.tsv> [opt...]
"""
import concurrent.futures
import re
import pathlib
import subprocess
import sys
import tempfile

include, checker, out = sys.argv[1:4]
opts = sys.argv[4:] or ["-O2"]

PRELUDE = r"""
#include <galoot/core/basis_change.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/unary.hpp>
#include <galoot/core/transform.hpp>
#include <galoot/presets/rga3.hpp>
namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Line = rga3::Line<double>;
using Plane = rga3::Plane<double>;
using Motor = rga3::Motor<double>;
using Flector = rga3::Flector<double>;
using Full = core::mv<rga3::algebra, core::full<4>, double>;
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct vga3 { static constexpr auto value = core::R<3, 0, 0>(); };
using CFull = core::mv<cga3, core::full<5>, double>;
using CVec = core::mv<cga3, core::of_grade<5, 1>, double>;
using CBiv = core::mv<cga3, core::of_grade<5, 2>, double>;
using CEven = core::mv<cga3, core::merge<core::merge<core::of_grade<5, 0>, core::of_grade<5, 2>>, core::of_grade<5, 4>>, double>;
using CMotor = core::mv<cga3, core::merge<core::merge<core::of_grade<5, 0>, core::of_grade<5, 2>>, core::of_grade<5, 4>>, double>;
using VFull = core::mv<vga3, core::full<3>, double>;
using VVec = core::mv<vga3, core::of_grade<3, 1>, double>;
using VRotor = core::mv<vga3, core::merge<core::of_grade<3, 0>, core::of_grade<3, 2>>, double>;
namespace {
struct doubled_e4 {
  using source = rga3::algebra;
  using target = rga3::algebra;
  [[maybe_unused]] static constexpr auto value = core::from_rows(
      {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 2}}, {1, 1, 1, 1});
};
}
"""

# name, parameters, expression
UNARY_OPS = {
    "reverse": "core::reverse(a)", "tilde": "~a",
    "antireverse": "core::antireverse(a)",
    "grade_involution": "core::grade_involution(a)",
    "right_complement": "core::right_complement(a)", "bang": "!a",
    "left_complement": "core::left_complement(a)",
    "bulk": "core::bulk(a)", "weight": "core::weight(a)",
    "bulk_dual": "core::bulk_dual(a)", "weight_dual": "core::weight_dual(a)",
    "left_bulk_dual": "core::left_bulk_dual(a)",
    "left_weight_dual": "core::left_weight_dual(a)",
    "negate": "-a", "scale": "2.0 * a", "scale_right": "a * 2.0",
    "divide": "a / 2.0",
}
BINARY_OPS = {
    "geometric_product": "core::geometric_product(a, b)", "times": "a * b",
    "wedge": "core::wedge(a, b)", "caret": "a ^ b",
    "antiproduct": "core::geometric_antiproduct(a, b)", "percent": "a % b",
    "antiwedge": "core::antiwedge(a, b)", "ampersand": "a & b",
    "plus": "a + b", "minus": "a - b",
    "dot": "core::dot(a, b)", "antidot": "core::antidot(a, b)",
    "project": "core::project(a, b)", "antiproject": "core::antiproject(a, b)",
    "compose": "core::compose(a, b)",
}
cases = []
for ty in ["Full", "Motor", "Line", "CFull", "CEven", "CVec", "VFull", "VRotor"]:
    for name, expr in UNARY_OPS.items():
        cases.append((f"{name}_{ty}", f"const {ty}& a", expr))
for ta, tb in [("Full", "Full"), ("Motor", "Motor"), ("Motor", "Flector"),
               ("Point", "Plane"), ("Line", "Point"), ("Plane", "Point"),
               ("Line", "Line"),
               ("CFull", "CFull"), ("CEven", "CEven"), ("CVec", "CBiv"),
               ("VFull", "VFull"), ("VRotor", "VVec")]:
    for name, expr in BINARY_OPS.items():
        if name in ("plus", "minus") and ta != tb:
            continue
        cases.append((f"{name}_{ta}_{tb}", f"const {ta}& a, const {tb}& b",
                      expr))
for ta, tb in [("Point", "Motor"), ("Line", "Motor"), ("Plane", "Motor"),
               ("Full", "Motor"), ("CVec", "CEven"), ("CFull", "CEven"),
               ("VVec", "VRotor"), ("VFull", "VRotor")]:
    cases.append((f"transform_{ta}_{tb}", f"const {ta}& a, const {tb}& b",
                   "core::transform(a, b)"))
cases += [
    ("change_Full", "const Full& a", "core::change_basis<doubled_e4>(a)"),
    ("change_back_Full", "const Full& a",
     "core::change_basis_back<doubled_e4>(a)"),
    ("change_Point", "const Point& a", "core::change_basis<doubled_e4>(a)"),
    ("convert_Motor", "const Motor& a", "Full{a}"),
    ("truncate_Full", "const Full& a", "core::truncate<Motor>(a)"),
    ("part_Full", "const Full& a", "core::part<Motor>(a)"),
    ("part_CFull", "const CFull& a", "core::part<CEven>(a)"),
    ("truncate_CFull", "const CFull& a", "core::truncate<CEven>(a)"),
]

COMPILERS = {"gcc": "g++", "clang": "clang++"}


def one(compiler, opt, case):
    name, params, expr = case
    with tempfile.TemporaryDirectory() as d:
        unit = pathlib.Path(d) / f"{name}.cpp"
        unit.write_text(PRELUDE + f"auto {name}({params}) {{ return {expr}; }}\n")
        compile_ = [COMPILERS[compiler], f"-I{include}", "-std=c++23",
                    "-DGALOOT_CHECKING=0", opt, "-ffp-contract=off",
                    "-w", "-fconstexpr-steps=100000000" if compiler == "clang"
                    else "-fconstexpr-ops-limit=1000000000",
                    "-S", "-o", "-", str(unit)]
        r = subprocess.run([sys.executable, "-I", checker, name, "--",
                            *compile_], capture_output=True, text=True)
        status = "inline" if r.returncode == 0 else "CALL"
        targets = []
        for l in r.stdout.splitlines():
            if "a call" not in l:
                continue
            m = re.search(r"in (\S+): (\S+)\s+\*?(\S+)", l)
            def short(sym):
                n = re.match(r"_ZZ?N[KVrROH]*6galoot((?:\d+[A-Za-z_]\w*?)+?)(?:I|E|$)", sym)
                t = sym
                parts = []
                rest = sym[sym.index("6galoot"):] if "6galoot" in sym else ""
                while rest and rest[0].isdigit():
                    k = re.match(r"(\d+)", rest).group(1)
                    parts.append(rest[len(k):len(k)+int(k)])
                    rest = rest[len(k)+int(k):]
                return "::".join(parts) if parts else sym[:40]
            if m:
                targets.append(f"{short(m.group(1)) if m.group(1).startswith('_ZN') else 'user'}->{m.group(2)} {short(m.group(3))}")
        detail = "; ".join(sorted(set(targets)))
        if "not x86-64" in r.stdout:
            status = "REFUSED"
        if "failed" in r.stdout:
            status = "ERROR"
            detail = r.stdout.replace("\n", " ")[:300]
        return compiler, opt, name, status, detail[:400]


jobs = [(c, o, k) for c in COMPILERS for o in opts for k in cases]
with concurrent.futures.ThreadPoolExecutor(max_workers=int(
        __import__("os").environ.get("SURVEY_JOBS", "8"))) as ex:
    results = list(ex.map(lambda j: one(*j), jobs))
with open(out, "w") as f:
    for r in results:
        f.write("\t".join(r) + "\n")
print(len(results), "compiles;", sum(r[3] == "CALL" for r in results), "calls;",
      sum(r[3] == "ERROR" for r in results), "errors")
