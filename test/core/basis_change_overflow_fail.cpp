// A change whose check of the inner products does not fit coefficient_type:
// with k the largest entry of a form and m = 2^31 - 1, the largest row entry,
// the form k of one generator against itself with e1 = m e'1 / m is an
// isometry, but m k m is about 2^77, beyond std::int64_t.
// This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct big {
  static constexpr auto value = core::from_gram({{core::max_form_entry}});
};
struct bad {
  using source = big;
  using target = big;
  static constexpr auto value = core::from_rows({{2147483647}}, {2147483647});
};
using X = core::mv<big, core::blades<big, "e1">, double>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<bad>(x);
}

} // namespace
