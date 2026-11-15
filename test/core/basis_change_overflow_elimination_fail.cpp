// The change e_i = m e'_i of R(0, 0, 3) to itself, for m = 2^31 - 1: every
// invertible change keeps the inner products of a form that is all zero, and
// no row has a common factor to divide out, but the elimination forms m^2 m^2,
// beyond std::int64_t, which must be reported as an overflow, not as
// dependent rows.
// This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct r003 {
  static constexpr auto value = core::R<0, 0, 3>();
};
struct bad {
  using source = r003;
  using target = r003;
  static constexpr auto value = core::from_rows(
      {{2147483647, 0, 0}, {0, 2147483647, 0}, {0, 0, 2147483647}}, {1, 1, 1});
};
using X = core::mv<r003, core::blades<r003, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
