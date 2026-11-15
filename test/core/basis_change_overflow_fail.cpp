// A change whose check of the inner products does not fit coefficient_type:
// with k the largest entry of a form and m = 2^31 - 1, the largest row entry,
// the form diag(k, 0) against itself with e1 = e'1 + e'2 / m and e2 = e'2 is
// an isometry, since e'2 is null, and no row has a common factor to divide
// out, but the check of the inner products in integers forms m k m, about
// 2^77, beyond std::int64_t.
// This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct big {
  static constexpr auto value = core::from_gram({
      {core::max_form_entry, 0},
      {0, 0},
  });
};
struct bad {
  using source = big;
  using target = big;
  static constexpr auto value =
      core::from_rows({{2147483647, 1}, {0, 1}}, {2147483647, 1});
};
using X = core::mv<big, core::blades<big, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
