#include <galoot/core.hpp>
namespace core = galoot::core;
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct r410 { static constexpr auto value = core::R<4, 1, 0>(); };
struct cga3_to_r410 { using source = cga3; using target = r410; static constexpr auto value = core::conformal_change<3>(); };
using Point = core::mv<cga3, core::blades<cga3, "e1 e2 e3 e4 e5">, double>;
#if CASE == 1
auto f(const Point& p) { return core::basis_change<cga3_to_r410>(p); }
#elif CASE == 2
// the value instead of the tag
auto f(const Point& p) { return core::change_basis<core::conformal_change<3>()>(p); }
#elif CASE == 3
// forgot the tag
auto f(const Point& p) { return core::change_basis(p); }
#endif
