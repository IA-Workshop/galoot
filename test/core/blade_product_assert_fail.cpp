// Misuse of galoot/core/blade_product.hpp at run time in a checking build.
// The argument names the input:
//
//   beyond_n       blade_product of e4 in R(3,0,0), e4 the first operand
//   beyond_n_second  the same, with e4 the second operand
//   capacity       e1 e2 in [[0, 1], [1, 0]], two terms, with capacity 1
//   overflow       e12345 e12345 in a form whose det g is about 2^75
//   sum_above      a sum just above the largest coefficient
//   sum_below      a sum just below its negative
//   product_above  a product just above the largest coefficient
//   product_below  a product equal to the smallest value of coefficient_type,
//                  which lies outside the range the guards keep, with the
//                  negative factor first
//   product_below_second  the same product with the negative factor second
//
// Each must abort with its message, which the tests core.blade_product_*
// check. The input is chosen at run time, so no call is evaluated at compile
// time. A usage error exits 2 with a message, which fails every test that
// runs this program.
#include <cstdio>
#include <limits>
#include <span>
#include <string_view>

#include <galoot/core/blade_product.hpp>
#include <galoot/core/metric.hpp>

namespace {

namespace core = galoot::core;
using core::coefficient_type;

constexpr coefficient_type max = std::numeric_limits<coefficient_type>::max();

// Returns false if no input has that name.
bool run(std::string_view input) {
  if (input == "beyond_n") {
    static_cast<void>(
        core::detail::blade_product<4>(core::R<3, 0, 0>(), 0b1000U, 0U));
  } else if (input == "beyond_n_second") {
    static_cast<void>(
        core::detail::blade_product<4>(core::R<3, 0, 0>(), 0U, 0b1000U));
  } else if (input == "capacity") {
    static_cast<void>(core::detail::blade_product<1>(
        core::from_gram({{0, 1}, {1, 0}}), 0b01U, 0b10U));
  } else if (input == "overflow") {
    constexpr int d = core::max_form_entry;
    static_cast<void>(core::detail::blade_product<16>(core::from_gram({
                                                          {d, 1, 1, 1, 1},
                                                          {1, d, 1, 1, 1},
                                                          {1, 1, d, 1, 1},
                                                          {1, 1, 1, d, 1},
                                                          {1, 1, 1, 1, d},
                                                      }),
                                                      0b11111U, 0b11111U));
  } else if (input == "sum_above") {
    core::detail::term_list<1> l{};
    l.add(0b1U, max);
    l.add(0b1U, 1);
  } else if (input == "sum_below") {
    core::detail::term_list<1> l{};
    l.add(0b1U, -max);
    l.add(0b1U, -1);
  } else if (input == "product_above") {
    static_cast<void>(core::detail::multiply_checked((max / 2) + 1, 2));
  } else if (input == "product_below") {
    static_cast<void>(core::detail::multiply_checked(-((max / 2) + 1), 2));
  } else if (input == "product_below_second") {
    static_cast<void>(core::detail::multiply_checked(2, -((max / 2) + 1)));
  } else {
    return false;
  }
  return true;
}

} // namespace

int main(int argc, char** argv) {
  if (argc != 2 ||
      !run(std::span<char*>(argv, static_cast<std::size_t>(argc)).back())) {
    std::fputs("usage: blade_product_assert_fail beyond_n|beyond_n_second|"
               "capacity|overflow|sum_above|sum_below|product_above|"
               "product_below|"
               "product_below_second\n",
               stderr);
    return 2;
  }
  // Reached only when the call returned, so the input was accepted.
  return 0;
}
