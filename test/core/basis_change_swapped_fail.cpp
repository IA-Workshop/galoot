// conformal_change<3>() in a tag with source and target swapped: from
// R(4, 1, 0) to conformal(3), where its rows do not keep the inner products.
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
  using source = r410;
  using target = cga3;
  static constexpr auto value = core::conformal_change<3>();
};
using X = core::mv<r410, core::blades<r410, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
