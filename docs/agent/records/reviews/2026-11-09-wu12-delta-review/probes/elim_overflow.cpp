// An overflow in the elimination of a 2 x 2 change whose isometry check fits.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
namespace core = galoot::core;
struct two { static constexpr auto value = core::from_gram({{2, 0}, {0, 2}}); };
struct r2 { static constexpr auto value = core::R<2, 0, 0>(); };
struct bad { using source = two; using target = r2;
  static constexpr auto value = core::from_rows({{2147483647, 2147483647}, {-2147483647, 2147483647}}, {2147483647, 2147483647}); };
using X = core::mv<two, core::blades<two, "e1">, double>;
[[maybe_unused]] auto f(const X& x) { return core::change_basis<bad>(x); }
