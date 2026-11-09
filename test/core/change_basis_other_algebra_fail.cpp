// change_basis given a multivector of rigid(3), an algebra that is neither
// the change's source nor its target, used as a statement.
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
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3_to_r410 {
  using source = cga3;
  using target = r410;
  static constexpr auto value = core::conformal_change<3>();
};

using X = core::mv<rga3, core::blades<rga3, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  static_cast<void>(core::change_basis<cga3_to_r410>(x));
}

} // namespace
