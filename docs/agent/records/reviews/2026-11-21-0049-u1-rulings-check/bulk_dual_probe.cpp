// Theorem B with the bulk dual in conformal(3), for one even and two odd
// versors, and a counterexample in the nondegenerate diagonal form
// diag(2, 1, 1), which from_gram builds. Compiles with -fsyntax-only on both
// compilers when every assertion holds.
#include <galoot/core.hpp>
namespace core = galoot::core;
struct c3 { static constexpr auto value = core::conformal<3>(); };
struct d2 { static constexpr auto value = core::from_gram({{2, 0, 0}, {0, 1, 0}, {0, 0, 1}}); };
using V = core::mv<c3, core::blades<c3, "e1 e2 e3 e4 e5">, int>;
constexpr V a{1, 1, 0, 1, 2}, b{1, 0, 2, 0, -1}, c{0, 3, -1, 2, 1}, x{2, -3, 5, 1, 4};
// odd versor: one vector, and three vectors
constexpr auto odd1 = a;
constexpr auto odd3 = a * b * c;
constexpr auto ev = a * b;
static_assert(core::transform(x, core::bulk_dual(ev)) == core::truncate<V>(ev * x * ~ev));
static_assert(core::transform(x, core::bulk_dual(odd1)) == core::truncate<V>(core::grade_involution(odd1) * x * ~odd1));
static_assert(core::transform(x, core::bulk_dual(odd3)) == core::truncate<V>(core::grade_involution(odd3) * x * ~odd3));
// diag(2, 1, 1): a single mirror
using W = core::mv<d2, core::blades<d2, "e1 e2 e3">, int>;
constexpr W m{1, 1, 0}, y{2, -3, 5};
static_assert(core::transform(y, core::bulk_dual(m)) != core::truncate<W>(core::grade_involution(m) * y * ~m));
int main() {}
