// A change whose second row has the denominator -2, so that e2 = e'2 / -2,
// although it would keep the inner products of R(2, 0, 0) were the sign in
// the row.
// This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct r200 {
  static constexpr auto value = core::R<2, 0, 0>();
};
struct bad {
  using source = r200;
  using target = r200;
  static constexpr auto value = core::from_rows(
      {
          {1, 0},
          {0, 1},
      },
      {1, -2});
};
using X = core::mv<r200, core::blades<r200, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
