// The identity change of R(3, 0, 0) written with rows m e'_i over m, for
// m = 2^31 - 1: its isometry check fits, but its elimination's pivots grow
// as m, m^2, m^3, beyond std::int64_t.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
namespace core = galoot::core;
struct r3 { static constexpr auto value = core::R<3, 0, 0>(); };
struct bad { using source = r3; using target = r3;
  static constexpr auto value = core::from_rows({{2147483647, 0, 0}, {0, 2147483647, 0}, {0, 0, 2147483647}}, {2147483647, 2147483647, 2147483647}); };
using X = core::mv<r3, core::blades<r3, "e1">, double>;
[[maybe_unused]] auto f(const X& x) { return core::change_basis<bad>(x); }
