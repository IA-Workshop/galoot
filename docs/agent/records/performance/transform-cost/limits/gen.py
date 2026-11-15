"""Writes one translation unit per (operation, algebra, n) for the limit
search of transform, compose, and antiproject: x = full<n>, and a versor on
the blades of even antigrade (n - k even), the largest layout of one parity.

Usage: gen.py <out-dir>
"""
import itertools
import pathlib
import sys

out = pathlib.Path(sys.argv[1])
out.mkdir(parents=True, exist_ok=True)
DENSE5 = ("core::from_gram({{2, 1, 1, 1, 1}, {1, 2, 1, 1, 1}, "
          "{1, 1, 2, 1, 1}, {1, 1, 1, 2, 1}, {1, 1, 1, 1, 2}})")


def even_names(n):
    names = []
    for k in range(n + 1):
        if (n - k) % 2 == 0:
            for c in itertools.combinations(range(1, n + 1), k):
                names.append("e" + "".join(map(str, c)) if c else "1")
    return " ".join(names)


CALLS = {
    "transform": "core::transform(x, v)",
    "compose": "core::compose(v, v)",
    "antiproject": "core::antiproject(x, x)",
}


def tu(op, alg_expr, n, name):
    text = f"""#include <galoot/core.hpp>
namespace core = galoot::core;
struct alg {{ static constexpr auto value = {alg_expr}; }};
using X = core::mv<alg, core::full<{n}>, double>;
using V = core::mv<alg, core::blades<alg, "{even_names(n)}">, double>;
auto f([[maybe_unused]] const X& x, [[maybe_unused]] const V& v) {{
  return {CALLS[op]};
}}
"""
    (out / f"{name}.cpp").write_text(text)


for op in CALLS:
    for n in range(3, 8):
        tu(op, f"core::R<{n}, 0, 0>()", n, f"{op}_r{n}")
    tu(op, "core::conformal<3>()", 5, f"{op}_cga3")
    tu(op, DENSE5, 5, f"{op}_dense5")
