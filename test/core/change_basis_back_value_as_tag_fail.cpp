// change_basis_back given the change's value, conformal_change<3>(), where
// a change tag belongs.
// This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct r410 {
  static constexpr auto value = core::R<4, 1, 0>();
};

using X = core::mv<r410, core::blades<r410, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r =
      core::change_basis_back<core::conformal_change<3>()>(x);
}

} // namespace
