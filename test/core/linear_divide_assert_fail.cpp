// Division of a multivector by a scalar at run time, in a checking build.
// The argument names the case:
//
//   int64_zero    a line over std::int64_t divided by zero, which must abort
//                 with the precondition's message
//   int64         the same line divided by 2, which must give the truncated
//                 quotients and exit 0
//   int64_negative
//                 the same line divided by -2, which must give the quotients
//                 truncated toward zero and exit 0
//   unsigned_zero a line over unsigned divided by zero, which must abort
//                 with the precondition's message
//   short_zero    a line over short divided by zero, which must abort with
//                 the precondition's message
//   double_zero   a line over double divided by 0.0, which IEEE 754 defines,
//                 so it must give infinities and a NaN and exit 0
//
// The divisor is read from the number of arguments, so no call is evaluated
// at compile time. A usage error, or a quotient that is wrong, exits 2 or 1
// with a message, which fails every test that runs this program.
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <span>
#include <string_view>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
template <class T>
using Line = galoot::core::mv<rga3, line_layout, T>;

} // namespace

int main(int argc, char** argv) {
  const std::span<char*> args(argv, static_cast<std::size_t>(argc));
  if (args.size() != 2) {
    std::fputs("usage: linear_divide_assert_fail int64_zero|int64|"
               "int64_negative|unsigned_zero|short_zero|double_zero\n",
               stderr);
    return 2;
  }
  // 0 from the two arguments a valid call has.
  const std::int64_t zero = static_cast<std::int64_t>(args.size()) - 2;
  const std::string_view which = args[1];
  bool right = false;
  if (which == "int64_zero") {
    const Line<std::int64_t> l{7, -7, 8, -8, 1, 0};
    const Line<std::int64_t> q = l / zero;
    right = q == Line<std::int64_t>{};
  } else if (which == "int64") {
    const Line<std::int64_t> l{7, -7, 8, -8, 1, 0};
    right = (l / (zero + 2)) == Line<std::int64_t>{3, -3, 4, -4, 0, 0};
  } else if (which == "int64_negative") {
    const Line<std::int64_t> l{7, -7, 8, -8, 1, 0};
    right = (l / (zero - 2)) == Line<std::int64_t>{-3, 3, -4, 4, 0, 0};
  } else if (which == "unsigned_zero") {
    const Line<unsigned> l{7U, 8U, 9U, 0U, 1U, 2U};
    const Line<unsigned> q = l / static_cast<unsigned>(zero);
    right = q == Line<unsigned>{};
  } else if (which == "short_zero") {
    const Line<short> l{7, -7, 8, -8, 1, 0};
    const Line<short> q = l / static_cast<short>(zero);
    right = q == Line<short>{};
  } else if (which == "double_zero") {
    const Line<double> l{1.0, -1.0, 0.0, 2.0, -2.0, 0.0};
    const Line<double> q = l / static_cast<double>(zero);
    right = std::isinf(q.coefficients[0]) && q.coefficients[0] > 0 &&
            std::isinf(q.coefficients[1]) && q.coefficients[1] < 0 &&
            std::isnan(q.coefficients[2]);
  } else {
    std::fputs("linear_divide_assert_fail: unknown case\n", stderr);
    return 2;
  }
  if (!right) {
    std::fputs("linear_divide_assert_fail: a quotient is wrong\n", stderr);
    return 1;
  }
  return 0;
}
