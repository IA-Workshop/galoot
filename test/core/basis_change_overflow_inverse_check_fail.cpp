// A change between two copies of R(0, 0, 3), whose inner products are all
// zero, so every invertible change keeps them, with an overflow in the check
// that the inverse inverts the rows, which must be reported as an overflow,
// not as the inverse's self-check.
// This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct r003 {
  static constexpr auto value = core::R<0, 0, 3>();
};
struct bad {
  using source = r003;
  using target = r003;
  static constexpr auto value =
      core::from_rows({{65536, 0, 0}, {1, -2, 0}, {0, 1073741824, 1048576}},
                      {2124569019, 2, 2});
};
using X = core::mv<r003, core::blades<r003, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
