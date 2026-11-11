"""The first survey of public operations, each compiled alone: 61 cases on
the rga3 preset's types and on full<4>, run on 2026-11-11 at 721afe7 and at
d47e8f9.

Usage: python3 -I first_survey.py <repository> <include dir> <out dir>

Writes one unit per case to the out directory, runs the repository's
test/tools/kernels_inline.py on it with g++ and clang++ (-O2, checking off,
no contraction, the warnings of the builds), and prints one row per case
and compiler: the case, the compiler, and the calls found or "inline".
"""
import subprocess, sys, pathlib, re, concurrent.futures as cf
repo, inc, out = map(pathlib.Path, sys.argv[1:4])
out.mkdir(parents=True, exist_ok=True)
HEAD = """#include <galoot/core.hpp>
#include <galoot/presets/rga3.hpp>
namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>; using Line = rga3::Line<double>;
using Plane = rga3::Plane<double>; using Motor = rga3::Motor<double>;
using Flector = rga3::Flector<double>;
using Full = core::mv<rga3::algebra, core::full<4>, double>;
struct doubled_e4 { using source = rga3::algebra; using target = rga3::algebra;
  static constexpr auto value = core::from_rows({{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,2}}, {1,1,1,1}); };
"""
cases = {
 "gp_motor_motor": "auto f(const Motor& a, const Motor& b) { return core::geometric_product(a, b); }",
 "times_motor_motor": "auto f(const Motor& a, const Motor& b) { return a * b; }",
 "gp_full": "auto f(const Full& a, const Full& b) { return core::geometric_product(a, b); }",
 "times_full": "auto f(const Full& a, const Full& b) { return a * b; }",
 "wedge_line_point": "auto f(const Line& a, const Point& b) { return core::wedge(a, b); }",
 "caret_line_point": "auto f(const Line& a, const Point& b) { return a ^ b; }",
 "wedge_full": "auto f(const Full& a, const Full& b) { return core::wedge(a, b); }",
 "caret_full": "auto f(const Full& a, const Full& b) { return a ^ b; }",
 "antigp_motor_motor": "auto f(const Motor& a, const Motor& b) { return core::geometric_antiproduct(a, b); }",
 "antigp_motor_flector": "auto f(const Motor& a, const Flector& b) { return core::geometric_antiproduct(a, b); }",
 "percent_motor_flector": "auto f(const Motor& a, const Flector& b) { return a % b; }",
 "percent_motor_motor": "auto f(const Motor& a, const Motor& b) { return a % b; }",
 "antigp_full": "auto f(const Full& a, const Full& b) { return core::geometric_antiproduct(a, b); }",
 "antiwedge_line_plane": "auto f(const Line& a, const Plane& b) { return core::antiwedge(a, b); }",
 "amp_line_plane": "auto f(const Line& a, const Plane& b) { return a & b; }",
 "antiwedge_full": "auto f(const Full& a, const Full& b) { return core::antiwedge(a, b); }",
 "amp_full": "auto f(const Full& a, const Full& b) { return a & b; }",
 "compose_motor_flector": "auto f(const Motor& a, const Flector& b) { return core::compose(a, b); }",
 "compose_motor_motor": "auto f(const Motor& a, const Motor& b) { return core::compose(a, b); }",
 "transform_point_flector": "auto f(const Point& x, const Flector& v) { return core::transform(x, v); }",
 "transform_line_motor": "auto f(const Line& x, const Motor& v) { return core::transform(x, v); }",
 "reverse_motor": "auto f(const Motor& a) { return core::reverse(a); }",
 "tilde_motor": "auto f(const Motor& a) { return ~a; }",
 "reverse_full": "auto f(const Full& a) { return core::reverse(a); }",
 "antireverse_motor": "auto f(const Motor& a) { return core::antireverse(a); }",
 "antireverse_full": "auto f(const Full& a) { return core::antireverse(a); }",
 "grade_involution_full": "auto f(const Full& a) { return core::grade_involution(a); }",
 "right_complement_flector": "auto f(const Flector& a) { return core::right_complement(a); }",
 "bang_flector": "auto f(const Flector& a) { return !a; }",
 "right_complement_full": "auto f(const Full& a) { return core::right_complement(a); }",
 "left_complement_full": "auto f(const Full& a) { return core::left_complement(a); }",
 "plus_motor": "auto f(const Motor& a, const Motor& b) { return a + b; }",
 "minus_motor": "auto f(const Motor& a, const Motor& b) { return a - b; }",
 "plus_full": "auto f(const Full& a, const Full& b) { return a + b; }",
 "minus_full": "auto f(const Full& a, const Full& b) { return a - b; }",
 "plus_line_point": "auto f(const Line& a, const Point& b) { return a + b; }",
 "scale_left_full": "auto f(double s, const Full& a) { return s * a; }",
 "scale_right_full": "auto f(const Full& a, double s) { return a * s; }",
 "divide_full": "auto f(const Full& a, double s) { return a / s; }",
 "bulk_full": "auto f(const Full& a) { return core::bulk(a); }",
 "weight_full": "auto f(const Full& a) { return core::weight(a); }",
 "bulk_dual_full": "auto f(const Full& a) { return core::bulk_dual(a); }",
 "weight_dual_full": "auto f(const Full& a) { return core::weight_dual(a); }",
 "left_bulk_dual_full": "auto f(const Full& a) { return core::left_bulk_dual(a); }",
 "left_weight_dual_full": "auto f(const Full& a) { return core::left_weight_dual(a); }",
 "bulk_dual_flector": "auto f(const Flector& a) { return core::bulk_dual(a); }",
 "dot_full": "auto f(const Full& a, const Full& b) { return core::dot(a, b); }",
 "antidot_full": "auto f(const Full& a, const Full& b) { return core::antidot(a, b); }",
 "dot_line": "auto f(const Line& a, const Line& b) { return core::dot(a, b); }",
 "project_point_plane": "auto f(const Point& a, const Plane& b) { return core::project(a, b); }",
 "antiproject_plane_point": "auto f(const Plane& a, const Point& b) { return core::antiproject(a, b); }",
 "project_line_plane": "auto f(const Line& a, const Plane& b) { return core::project(a, b); }",
 "change_basis_motor": "auto f(const Motor& a) { return core::change_basis<doubled_e4>(a); }",
 "change_basis_back_motor": "auto f(const Motor& a) { return core::change_basis_back<doubled_e4>(a); }",
 "change_basis_full": "auto f(const Full& a) { return core::change_basis<doubled_e4>(a); }",
 "change_basis_back_full": "auto f(const Full& a) { return core::change_basis_back<doubled_e4>(a); }",
 "convert_line_to_motor": "Motor f(const Line& a) { return Motor{a}; }",
 "convert_motor_to_full": "Full f(const Motor& a) { return Full{a}; }",
 "truncate_full_to_motor": "auto f(const Full& a) { return core::truncate<Motor>(a); }",
 "part_full_to_motor": "auto f(const Full& a) { return core::part<Motor>(a); }",
 "part_motor_to_line": "auto f(const Motor& a) { return core::part<Line>(a); }",
}
flags = ["-std=c++23", "-DGALOOT_CHECKING=0", "-O2", "-ffp-contract=off", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion", "-Wshadow", "-Werror", "-S", "-o", "-"]
def one(item):
    (name, body), cxx = item
    src = out / f"{name}.cpp"
    src.write_text(HEAD + "// NOLINTNEXTLINE\n" + body + "\n")
    r = subprocess.run(["python3", "-I", str(repo / "test/tools/kernels_inline.py"), "f", "--", cxx, f"-I{inc}", *flags, str(src)], capture_output=True, text=True)
    calls = []
    for line in r.stdout.splitlines():
        m = re.search(r": (call\w*|jmp\w*|j\w+)\s+(\S+)", line)
        if m and "call into" in line:
            sym = m.group(2); mm = re.search(r"6galoot4core(?:6detail)?(\d+)", sym)
            nm = sym[mm.end():mm.end()+int(mm.group(1))] if mm else sym[:40]
            if not mm:
                mo = re.search(r"6galoot4core(?:6detail)?(o\w)", sym); nm = "operator" + (mo.group(1) if mo else "?")
            calls.append(f"{m.group(1)} {nm}")
    if r.returncode and not calls:
        calls = ["FAILED: " + (r.stdout + r.stderr).strip().splitlines()[0][:150]]
    return name, cxx, calls
items = [((n, b), c) for n, b in cases.items() for c in ("g++", "clang++")]
with cf.ThreadPoolExecutor(4) as ex:
    res = list(ex.map(one, items))
for name, cxx, calls in res:
    print(f"{name}\t{cxx}\t{'; '.join(calls) or 'inline'}")
