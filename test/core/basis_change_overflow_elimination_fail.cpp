// The identity change of R(3, 0, 0) written with rows m e'_i over m, for
// m = 2^31 - 1: its inner products fit, but the pivots of its elimination
// grow as m, m^2, m^3, beyond std::int64_t, which must be reported as an
// overflow, not as dependent rows.
// This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};
struct bad {
  using source = r300;
  using target = r300;
  static constexpr auto value = core::from_rows(
      {{2147483647, 0, 0}, {0, 2147483647, 0}, {0, 0, 2147483647}},
      {2147483647, 2147483647, 2147483647});
};
using X = core::mv<r300, core::blades<r300, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
