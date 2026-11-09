// conformal_change<3>() with the point at infinity negated, e5 = -(e'4 + e'5):
// every generator keeps its square, but the null pair's inner product
// e4 . e5 maps to (1 + 1) / 2 = +1, not -1, so only the inner products off
// the diagonal tell that the change does not keep the form.
// This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
struct r410 {
  static constexpr auto value = core::R<4, 1, 0>();
};
struct bad {
  using source = cga3;
  using target = r410;
  static constexpr auto value = core::from_rows(
      {
          {1, 0, 0, 0, 0},
          {0, 1, 0, 0, 0},
          {0, 0, 1, 0, 0},
          {0, 0, 0, -1, 1},
          {0, 0, 0, -1, -1},
      },
      {1, 1, 1, 2, 1});
};
using X = core::mv<cga3, core::blades<cga3, "e4">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
