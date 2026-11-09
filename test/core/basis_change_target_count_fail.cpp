// A change tag whose target, rigid(3), has four generators while the change,
// conformal_change<3>(), has five rows.
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
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct bad {
  using source = cga3;
  using target = rga3;
  static constexpr auto value = core::conformal_change<3>();
};
using X = core::mv<cga3, core::blades<cga3, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
