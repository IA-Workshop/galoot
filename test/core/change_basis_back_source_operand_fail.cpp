// change_basis_back given a multivector of the change's source,
// conformal(3), which change_basis maps.
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
struct cga3_to_r410 {
  using source = cga3;
  using target = r410;
  static constexpr auto value = core::conformal_change<3>();
};

using X = core::mv<cga3, core::blades<cga3, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis_back<cga3_to_r410>(x);
}

} // namespace
