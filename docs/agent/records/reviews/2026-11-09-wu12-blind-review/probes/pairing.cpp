// A change that keeps every square of conformal(3) but not the pairing of
// the null pair: the point at infinity negated, so e4 . e5 maps to +1.
#include <galoot/core.hpp>
namespace core = galoot::core;
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct r410 { static constexpr auto value = core::R<4, 1, 0>(); };
struct bad {
  using source = cga3; using target = r410;
  static constexpr auto value = core::from_rows(
      {{1, 0, 0, 0, 0}, {0, 1, 0, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, -1, 1}, {0, 0, 0, -1, -1}},
      {1, 1, 1, 2, 1});
};
using X = core::mv<cga3, core::blades<cga3, "e4">, double>;
auto f(const X& x) { return core::change_basis<bad>(x); }
