// A change of R(0, 0, 2), where every inner product is zero, whose second
// row is zero: P G' P^T = G holds, since both sides are zero, but P has no
// inverse, and that is the fault to report.
// This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct r002 {
  static constexpr auto value = core::R<0, 0, 2>();
};
struct bad {
  using source = r002;
  using target = r002;
  static constexpr auto value = core::from_rows(
      {
          {1, 0},
          {0, 0},
      },
      {1, 1});
};
using X = core::mv<r002, core::blades<r002, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
