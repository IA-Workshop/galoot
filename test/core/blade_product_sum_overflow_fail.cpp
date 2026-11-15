// A sum of two coefficients that does not fit coefficient_type, in a constant
// evaluation of term_list::add. This file must not compile, and the
// diagnostic must show the overflow's message
// (core.blade_product_sum_overflow).
#include <cstdint>
#include <limits>

#include <galoot/core/blade_product.hpp>

namespace {

namespace core = galoot::core;

constexpr core::coefficient_type max_plus_one() {
  core::detail::term_list<1> l{};
  l.add(0b1U, std::numeric_limits<core::coefficient_type>::max());
  l.add(0b1U, 1);
  return l.terms[0].coefficient;
}

static_assert(max_plus_one() != 0);

} // namespace
