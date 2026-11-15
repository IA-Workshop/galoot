// The one-mode Witt change e1 = (e'1 + e'2) / 2, e2 = e'1 - e'2 written with
// its matrix transposed, rows (1/2, 1) and (1/2, -1): the image of e1 then
// squares to 1/4 - 1, not to 0.
// This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct witt1 {
  static constexpr auto value = core::from_gram({
      {0, 1},
      {1, 0},
  });
};
struct r110 {
  static constexpr auto value = core::R<1, 1, 0>();
};
struct bad {
  using source = witt1;
  using target = r110;
  static constexpr auto value = core::from_rows(
      {
          {1, 2},
          {1, -2},
      },
      {2, 2});
};
using X = core::mv<witt1, core::blades<witt1, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
