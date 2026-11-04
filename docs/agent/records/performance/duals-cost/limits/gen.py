"""Writes one translation unit per (operation, algebra, n) for the limit search."""
import pathlib, sys
out = pathlib.Path(sys.argv[1])
UNARY = ["bulk", "weight", "bulk_dual", "weight_dual", "left_bulk_dual", "left_weight_dual"]
BINARY = ["dot", "antidot", "project"]
ALGEBRAS = {
    "euclidean": lambda n: f"core::R<{n}, 0, 0>()",
}
def tu(op, alg_expr, n, name):
    call = f"core::{op}(x)" if op in UNARY else f"core::{op}(x, x)"
    text = f"""#include <galoot/core.hpp>
namespace core = galoot::core;
struct alg {{ static constexpr auto value = {alg_expr}; }};
using X = core::mv<alg, core::full<{n}>, double>;
auto f(const X& x) {{ return {call}; }}
"""
    (out / f"{name}.cpp").write_text(text)
for op in UNARY + BINARY:
    for n in range(4, 10):
        tu(op, f"core::R<{n}, 0, 0>()", n, f"{op}_r{n}")
    tu(op, "core::conformal<3>()", 5, f"{op}_cga3")
    tu(op, """core::from_gram({{2, 1, 1, 1, 1}, {1, 2, 1, 1, 1}, {1, 1, 2, 1, 1},
                                {1, 1, 1, 2, 1}, {1, 1, 1, 1, 2}})""", 5, f"{op}_dense5")
