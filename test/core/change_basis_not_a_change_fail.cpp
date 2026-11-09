// change_basis given an algebra tag where a change tag belongs.
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

using X = core::mv<cga3, core::blades<cga3, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<cga3>(x);
}

} // namespace
