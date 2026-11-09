// A change tag whose source, rigid(3), has four generators while the change,
// conformal_change<3>(), has five rows.
// This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct r410 {
  static constexpr auto value = core::R<4, 1, 0>();
};
struct bad {
  using source = rga3;
  using target = r410;
  static constexpr auto value = core::conformal_change<3>();
};
using X = core::mv<rga3, core::blades<rga3, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
