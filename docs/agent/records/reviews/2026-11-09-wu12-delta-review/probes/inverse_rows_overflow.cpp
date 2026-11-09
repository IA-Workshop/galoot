// An overflow in the inverse's rows of a change between two null forms,
// which every invertible change keeps.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
namespace core = galoot::core;
struct z3 { static constexpr auto value = core::R<0, 0, 3>(); };
struct bad { using source = z3; using target = z3;
  static constexpr auto value = core::from_rows({{0, 1048576, 0}, {0, -16777216, 46341}, {2, -378, 0}}, {1, 1073741824, 2147483647}); };
using X = core::mv<z3, core::blades<z3, "e1">, double>;
[[maybe_unused]] auto f(const X& x) { return core::change_basis<bad>(x); }
