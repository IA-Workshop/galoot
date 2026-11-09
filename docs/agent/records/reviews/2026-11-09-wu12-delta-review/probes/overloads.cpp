// Valid calls and near misses beside the value catch-alls; PROBE selects one.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
namespace core = galoot::core;
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct r410 { static constexpr auto value = core::R<4, 1, 0>(); };
struct cga3_to_r410 { using source = cga3; using target = r410;
  static constexpr auto value = core::conformal_change<3>(); };
using P = core::mv<cga3, core::blades<cga3, "e1 e2 e3 e4 e5">, double>;
template <class C, class T> auto generic(const T& t) { return core::change_basis<C>(t); }
[[maybe_unused]] void use(const P& p) {
#if PROBE == 0
  auto q = core::change_basis<cga3_to_r410>(p);
  [[maybe_unused]] P p2 = core::change_basis_back<cga3_to_r410>(q);
  [[maybe_unused]] auto g = generic<cga3_to_r410>(p);
  // The overload's own parameters, given explicitly.
  [[maybe_unused]] auto e = core::change_basis<cga3_to_r410, P::layout, double>(p);
#elif PROBE == 1
  [[maybe_unused]] auto q = core::change_basis<cga3_to_r410{}>(p);
#elif PROBE == 2
  [[maybe_unused]] auto q = core::change_basis<3>(p);
#elif PROBE == 3
  [[maybe_unused]] auto q = core::change_basis<cga3::value>(p);
#elif PROBE == 4
  [[maybe_unused]] auto q = core::change_basis<core::conformal_change<3>()>(5);
#elif PROBE == 5
  [[maybe_unused]] auto q = core::change_basis<int>(p);
#elif PROBE == 6
  [[maybe_unused]] auto q = generic<core::basis_change<5>>(p);
#endif
}
