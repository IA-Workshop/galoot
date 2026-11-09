// An overflow in the check that the inverse inverts, between two null forms.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
namespace core = galoot::core;
struct z3 { static constexpr auto value = core::R<0, 0, 3>(); };
struct bad { using source = z3; using target = z3;
  static constexpr auto value = core::from_rows({{65536, 0, 0}, {1, -2, 0}, {0, 1073741824, 1048576}}, {2124569019, 2, 2}); };
using X = core::mv<z3, core::blades<z3, "e1">, double>;
[[maybe_unused]] auto f(const X& x) { return core::change_basis<bad>(x); }
