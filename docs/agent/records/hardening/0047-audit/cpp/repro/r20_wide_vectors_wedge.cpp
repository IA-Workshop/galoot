// The wedge of two vectors of R(GEN, 0, 0), MaxN 32: GEN^2 - GEN terms on
// C(GEN, 2) blades. Clang 23 builds it at GEN = 18 and stops at GEN = 20 with
// "constexpr evaluation hit maximum step limit" in collect_result (19 also
// builds), the placement of terms the limits page describes; GCC builds it up
// to 30 and at 31 and 32 exceeds its operation limit, with 39 errors. Nothing
// above 16 generators is tested, and the limits page gives no figure for
// sparse layouts there.
#include <galoot/core.hpp>
#ifndef GEN
#define GEN 20
#endif
namespace core = galoot::core;
struct alg { static constexpr auto value = core::R<GEN, 0, 0, 32>(); };
using Vector = core::mv<alg, core::of_grade<GEN, 1>, double>;
auto join(const Vector& a, const Vector& b) { return core::wedge(a, b); }
