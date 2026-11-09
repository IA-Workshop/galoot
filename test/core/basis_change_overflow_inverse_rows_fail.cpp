// A change between two copies of R(0, 0, 3), whose inner products are all
// zero, with an overflow in the inverse's rows, which must be reported, not
// passed on as rows that hold 0 where a product overflowed.
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
      core::from_rows({{0, 1048576, 0}, {0, -16777216, 46341}, {2, -378, 0}},
                      {1, 1073741824, 2147483647});
};
using X = core::mv<r003, core::blades<r003, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
