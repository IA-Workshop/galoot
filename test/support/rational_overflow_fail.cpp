// Operations of the test rational whose results do not fit, at run time. The
// argument names the case:
//
//   add_above         max + 1
//   subtract_below    -max - 1
//   multiply_above    3037000500^2, the smallest square above max
//   numerator_min     the minimum of std::int64_t, which is below -max
//   zero_denominator  1/0
//   divide_by_zero    1 / 0
//
// where max is the maximum of std::int64_t. Each must abort with its message,
// which the tests support.rational_rejects_* check. The operands are formed
// from the argument count, so no operation is evaluated at compile time. A
// usage error exits 2 with a message, which fails every test that runs this
// program.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <span>
#include <string_view>

#include <support/rational.hpp>

namespace {

using test_support::rational;

// Runs the case `name` with `one` equal to 1. Returns false if no case has
// that name.
bool run(std::string_view name, std::int64_t one) {
  constexpr std::int64_t max = std::numeric_limits<std::int64_t>::max();
  constexpr std::int64_t min = std::numeric_limits<std::int64_t>::min();
  rational result;
  if (name == "add_above") {
    result = rational{max * one} + rational{one};
  } else if (name == "subtract_below") {
    result = rational{-max * one} - rational{one};
  } else if (name == "multiply_above") {
    result = rational{3037000500 * one} * rational{3037000500 * one};
  } else if (name == "numerator_min") {
    result = rational{min * one};
  } else if (name == "zero_denominator") {
    result = rational{one, one - 1};
  } else if (name == "divide_by_zero") {
    result = rational{one} / rational{one - 1};
  } else {
    return false;
  }
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,modernize-use-std-print)
  std::printf("%lld/%lld\n", static_cast<long long>(result.numerator),
              static_cast<long long>(result.denominator));
  return true;
}

} // namespace

int main(int argc, char** argv) {
  const std::span<char*> args(argv, static_cast<std::size_t>(argc));
  if (args.size() != 2 || !run(args[1], argc - 1)) {
    std::fputs("usage: rational_overflow_fail <case>\n", stderr);
    return 2;
  }
  return 0;
}
