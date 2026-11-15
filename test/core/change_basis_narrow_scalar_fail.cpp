// A change with the coefficient 2^24 + 1, which float cannot hold. Every
// invertible P keeps the inner products of R(0, 0, 2), which are all zero,
// so e1 = (2^24 + 1) e'1 is a valid change, and its image of e1 has that
// coefficient.
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
struct wide {
  using source = r002;
  using target = r002;
  static constexpr auto value = core::from_rows(
      {
          {16777217, 0},
          {0, 1},
      },
      {1, 1});
};
using X = core::mv<r002, core::blades<r002, "e1">, float>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<wide>(x);
}

} // namespace
