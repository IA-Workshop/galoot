// rotation(l, theta) at run time about axes whose direction has a computed
// length of zero or infinity. The build fixes the checking mode, and the
// argument names the case:
//
//   zero        the zero Line over double, as from the join p ^ p: with
//               checking on it must abort with the precondition's message;
//               with checking off the six line coefficients are 0 times an
//               infinite or NaN quotient, so NaN, and it exits 0
//   zero_float  the same over float
//   underflow   the direction (1e-200, 0, 0) over double, whose square
//               underflows to 0, so its computed length is 0: with checking
//               on it must abort with the message; with checking off the
//               coefficient on e41 is 1e-200 times an infinite quotient, so
//               infinite, and it exits 0
//   overflow    the direction (1e200, 0, 0) over double, whose square
//               overflows, so its computed length is infinite: with checking
//               on it must abort with the message; with checking off the
//               quotient is 0, so every line coefficient is 0, and it exits 0
//   length_2    the z axis given with the direction (0, 0, 2) and theta = 0,
//               which is not rejected in either mode and gives the identity
//               motor, 1 on e1234 and 0 elsewhere, and exits 0
//
// theta is read from the number of arguments, so no call is evaluated at
// compile time. A usage error, or a coefficient that is not as stated,
// exits 2 or 1 with a message, which fails every test that runs this
// program.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <span>
#include <string_view>

#include <galoot/rga3/motions.hpp>
#include <galoot/rga3/objects.hpp>

namespace {

namespace rga3 = galoot::rga3;

// Whether every line coefficient of q, on e41, e42, e43, e23, e31, e12,
// meets the predicate.
template <class T, class P>
bool line_part_all(const rga3::Motor<T>& q, P predicate) {
  constexpr std::array<std::size_t, 6> line_places{0, 1, 2, 4, 5, 6};
  return std::ranges::all_of(line_places, [&](std::size_t i) {
    return predicate(q.coefficients.at(i));
  });
}

} // namespace

int main(int argc, char** argv) {
  const std::span<char*> args(argv, static_cast<std::size_t>(argc));
  if (args.size() != 2) {
    std::fputs("usage: rotation_assert_fail zero|zero_float|underflow|"
               "overflow|length_2\n",
               stderr);
    return 2;
  }
  // 0 from the two arguments a valid call has, and 1/2 from it plus 1/2.
  const double zero = static_cast<double>(args.size()) - 2.0;
  const double theta = zero + 0.5;
  const std::string_view which = args[1];
  bool right = false;
  if (which == "zero") {
    const rga3::Motor<double> q = rotation(rga3::Line<double>{}, theta);
    right = line_part_all(q, [](double c) { return std::isnan(c); });
  } else if (which == "zero_float") {
    const rga3::Motor<float> q =
        rotation(rga3::Line<float>{}, static_cast<float>(theta));
    right = line_part_all(q, [](float c) { return std::isnan(c); });
  } else if (which == "underflow") {
    const rga3::Line<double> l{1e-200, 0, 0, 0, 0, 0};
    const rga3::Motor<double> q = rotation(l, theta);
    right = std::isinf(q.coefficients[0]);
  } else if (which == "overflow") {
    const rga3::Line<double> l{1e200, 0, 0, 0, 0, 0};
    const rga3::Motor<double> q = rotation(l, theta);
    right = line_part_all(q, [](double c) { return c == 0.0; });
  } else if (which == "length_2") {
    const rga3::Line<double> l{0, 0, 2, 0, 0, 0};
    right = rotation(l, zero) == rga3::Motor<double>{0, 0, 0, 1, 0, 0, 0, 0};
  } else {
    std::fputs("rotation_assert_fail: unknown case\n", stderr);
    return 2;
  }
  if (!right) {
    std::fputs("rotation_assert_fail: a coefficient is not as stated\n",
               stderr);
    return 1;
  }
  return 0;
}
