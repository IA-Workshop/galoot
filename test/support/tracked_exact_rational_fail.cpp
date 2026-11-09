// Doubles that exact_rational must refuse, at run time, because the
// rational cannot hold them. The argument names the case:
//
//   magnitude_two_62    2^62, the smallest power of two not below 2^62
//   magnitude_above     (2^53 - 1) 2^10, an odd significand whose value is
//                       above 2^62
//   denominator_two_63  2^-63, whose denominator is above 2^62
//
// Each must abort with the message of exact_rational, which the tests
// support.tracked_rejects_* check. The double is scaled by the argument
// count, so no conversion is evaluated at compile time. A usage error exits
// 2 with a message, which fails every test that runs this program.
#include <cstddef>
#include <cstdio>
#include <span>
#include <string_view>

#include <support/rational.hpp>
#include <support/tracked.hpp>

namespace {

// Runs the case `name` with `one` equal to 1. Returns false if no case has
// that name.
bool run(std::string_view name, double one) {
  double d = 0.0;
  if (name == "magnitude_two_62") {
    d = 0x1p62 * one;
  } else if (name == "magnitude_above") {
    d = (0x1p53 - 1.0) * 0x1p10 * one;
  } else if (name == "denominator_two_63") {
    d = 0x1p-63 * one;
  } else {
    return false;
  }
  const test_support::rational r = test_support::exact_rational(d);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,modernize-use-std-print)
  std::printf("%lld/%lld\n", static_cast<long long>(r.numerator),
              static_cast<long long>(r.denominator));
  return true;
}

} // namespace

int main(int argc, char** argv) {
  const std::span<char*> args(argv, static_cast<std::size_t>(argc));
  if (args.size() != 2 || !run(args[1], static_cast<double>(argc - 1))) {
    std::fputs("usage: tracked_exact_rational_fail <case>\n", stderr);
    return 2;
  }
  return 0;
}
