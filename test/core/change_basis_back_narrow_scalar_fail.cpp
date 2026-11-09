// The inverse of e1 = e'1 / (2^24 + 1) in R(0, 0, 2) sends e'1 to
// (2^24 + 1) e1, a coefficient that float cannot hold.
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
          {1, 0},
          {0, 1},
      },
      {16777217, 1});
};
using X = core::mv<r002, core::blades<r002, "e1">, float>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis_back<wide>(x);
}

} // namespace
