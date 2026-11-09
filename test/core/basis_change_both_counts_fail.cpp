// A change tag whose change has the wrong dimension for both algebras:
// conformal_change<2>(), of four rows, between conformal(3) and R(4, 1, 0),
// of five generators each. The source's count is reported, once.
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
  using source = cga3;
  using target = r410;
  static constexpr auto value = core::conformal_change<2>();
};
using X = core::mv<cga3, core::blades<cga3, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
