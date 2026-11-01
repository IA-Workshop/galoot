import sys
sys.path.insert(0, sys.argv[1])
from cases import CASES
def oriented(indices):
    inv = sum(1 for p in range(len(indices)) for q in range(p+1, len(indices)) if indices[p] > indices[q])
    return sum(1 << (i-1) for i in indices), (-1 if inv % 2 else 1)
out = ["#include <cstdio>", "#include <galoot/core.hpp>", "namespace core = galoot::core;",
       "using core::detail::gp_plan;"]
mains = []
for k, (name, c) in enumerate(CASES.items()):
    n = len(c["gram"])
    rows = ", ".join("{" + ", ".join(map(str, r)) + "}" for r in c["gram"])
    out.append(f"struct alg{k} {{ static constexpr auto value = core::from_gram({{{rows}}}); }};")
    for sn in ("A", "B"):
        bl = [oriented(ix) for ix, s in c[sn]]
        bl = [(m, sg * s) for (m, sg), (ix, s) in zip(bl, c[sn])]
        if name.startswith("rigid3"):
            names = " ".join(("e" + "".join(map(str, ix))) if ix else "1" for ix, s in c[sn])
            assert all(s == 1 for ix, s in c[sn])
            out.append(f'constexpr auto {sn}{k} = core::blades<4, "{names}">;')
        else:
            inner = ", ".join(f"{{{m}U, {sg}}}" for m, sg in bl)
            out.append(f"constexpr core::blade_set<{len(bl)}> {sn}{k}{{{{{{{inner}}}}}}};")
    mains.append(f'  for (const auto& t : gp_plan<alg{k}, A{k}, B{k}>) std::printf("{name} %zu %zu %u %lld\\n", t.a, t.b, t.out, static_cast<long long>(t.coefficient));')
out.append("int main() {")
out += mains
out.append("}")
print("\n".join(out))
