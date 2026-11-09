// Divisions and conversions that the tracking scalar must refuse, at run
// time. The argument names the case:
//
//   rounded_divisor   a value with one rounding as a divisor, whose
//                     quotient's error the count does not bound
//   zero_divisor      division by zero
//   not_a_double      correctly_rounded of (2^53 + 1) / 5, where
//                     (2^53 + 1) is not a double
//
// Each must abort with its message, which the tests
// support.tracked_rejects_* check. The values are formed from the argument
// count, so nothing is evaluated at compile time. A usage error exits 2 with
// a message, which fails every test that runs this program.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <span>
#include <string_view>

#include <support/rational.hpp>
#include <support/tracked.hpp>

namespace {

using test_support::rational;
using test_support::tracked;

// Runs the case `name` with `one` equal to 1. Returns false if no case has
// that name.
bool run(std::string_view name, std::int64_t one) {
  const tracked a = tracked::input(rational{3 * one, 4});
  double d = 0.0;
  if (name == "rounded_divisor") {
    d = test_support::nearest_double((a / (a * a)).value);
  } else if (name == "zero_divisor") {
    d = test_support::nearest_double((a / tracked{one - 1}).value);
  } else if (name == "not_a_double") {
    d = test_support::correctly_rounded(
        rational{test_support::power_of_two(53) + one, 5});
  } else {
    return false;
  }
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,modernize-use-std-print)
  std::printf("%g\n", d);
  return true;
}

} // namespace

int main(int argc, char** argv) {
  const std::span<char*> args(argv, static_cast<std::size_t>(argc));
  if (args.size() != 2 || !run(args[1], argc - 1)) {
    std::fputs("usage: tracked_division_fail <case>\n", stderr);
    return 2;
  }
  return 0;
}
