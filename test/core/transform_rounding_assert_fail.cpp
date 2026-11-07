// transform over double and float at run time, where the rounding check of
// what it drops can fire. The build fixes the checking mode, and the
// argument names the case (the inputs are in transform_rounding_cases.hpp,
// and transform_rounding_test.cpp shows that they are as said here):
//
//   not_a_versor_double
//   not_a_versor_float
//             in R(4,0,0), x = 1 on the scalar and V = 1 + e1234, which is
//             not a versor: V ⟇ x ⟇ antireverse(V) = 2 + 2 e1234, so 2 e1234
//             is dropped, far above its rounding error, which is zero; it
//             must abort with the message when checking is on, and otherwise
//             keep 2 and exit 0
//   ten_times a versor of R(4,0,0) perturbed so that the dropped part is at
//             least ten times the largest tolerance the check may have; it
//             must abort when checking is on
//   tenth     the same versor perturbed so that the dropped part is at most
//             a tenth of the smallest tolerance the check may have; it must
//             not abort
//   nan       x = 1 and V = 1 + NaN e1234, whose dropped coefficient is NaN;
//             it must abort when checking is on
//   infinite  the same with an infinite coefficient; it must abort when
//             checking is on
//   overflow  a versor near the overflow threshold whose dropped
//             coefficients cancel to finite values, while the magnitude
//             that bounds their rounding error overflows, so the check
//             cannot verify them; it must abort when checking is on
//   underflow_double
//   underflow_float
//             a rigid(3) motor and point scaled so that the second stage's
//             products fall below the smallest normal number; what a motor
//             drops from a point is zero for every motor, and the check
//             allows for underflow, so it must not abort
//
// The inputs are multiplied by a value read at run time, so no call is
// evaluated at compile time. A usage error, or a kept coefficient that is
// wrong, exits 2 or 1 with a message, which fails every test that runs this
// program.
#include <cstddef>
#include <cstdio>
#include <limits>
#include <span>
#include <string_view>

#include <galoot/core/transform.hpp>

#include "transform_rounding_cases.hpp"

namespace {

namespace core = galoot::core;
namespace cases = transform_rounding_cases;

// The transform of 1 on the scalar by s (1 + e1234); returns whether it
// keeps 2 s^2.
template <class T>
bool keeps_twice(T s) {
  const cases::Scalar<T> kept =
      core::transform(cases::Scalar<T>{T{1}}, cases::not_a_versor(s));
  return kept.coefficients[0] == T{2} * s * s;
}

// The transform of s on the scalar by v; returns true, since only whether
// it aborts matters.
template <class V>
bool runs(const V& v, double s) {
  const cases::Scalar<double> kept =
      core::transform(cases::Scalar<double>{s}, v);
  static_cast<void>(kept);
  return true;
}

// The perturbed versor with the perturbation m, on the scalar s.
bool runs_perturbed(double m, double s) {
  return runs(cases::perturbed_versor(m * s), s);
}

// The versor near overflow, its coefficients times s.
bool runs_near_overflow(double s) {
  cases::Even<double> v = cases::near_overflow_versor<double>();
  for (double& c : v.coefficients) {
    c *= s;
  }
  return runs(v, cases::near_overflow_scalar * s);
}

// 1 + bad e1234 on the scalar s.
bool runs_with(double bad, double s) {
  return runs(cases::Pair<double>{s, bad}, s);
}

// The underflowing motor and point, times s.
template <class T>
bool runs_underflow(T s) {
  cases::Motor<T> v = cases::tiny_motor<T>();
  cases::Point<T> x = cases::tiny_point<T>();
  for (T& c : v.coefficients) {
    c *= s;
  }
  for (T& c : x.coefficients) {
    c *= s;
  }
  const cases::Point<T> kept = core::transform(x, v);
  static_cast<void>(kept);
  return true;
}

} // namespace

int main(int argc, char** argv) {
  const std::span<char*> args(argv, static_cast<std::size_t>(argc));
  if (args.size() != 2) {
    std::fputs("usage: transform_rounding_assert_fail not_a_versor_double|"
               "not_a_versor_float|ten_times|tenth|nan|infinite|overflow|"
               "underflow_double|underflow_float\n",
               stderr);
    return 2;
  }
  // s is 1, read from the number of arguments, so that the compiler cannot
  // evaluate the calls at compile time.
  const auto s = static_cast<double>(args.size() - 1);
  const auto sf = static_cast<float>(args.size() - 1);
  const std::string_view which = args[1];
  bool kept = false;
  if (which == "not_a_versor_double") {
    kept = keeps_twice(s);
  } else if (which == "not_a_versor_float") {
    kept = keeps_twice(sf);
  } else if (which == "ten_times") {
    kept = runs_perturbed(cases::ten_times_perturbation, s);
  } else if (which == "tenth") {
    kept = runs_perturbed(cases::tenth_perturbation, s);
  } else if (which == "nan") {
    kept = runs_with(std::numeric_limits<double>::quiet_NaN(), s);
  } else if (which == "infinite") {
    kept = runs_with(std::numeric_limits<double>::infinity(), s);
  } else if (which == "overflow") {
    kept = runs_near_overflow(s);
  } else if (which == "underflow_double") {
    kept = runs_underflow(s);
  } else if (which == "underflow_float") {
    kept = runs_underflow(sf);
  } else {
    std::fputs("transform_rounding_assert_fail: unknown case\n", stderr);
    return 2;
  }
  if (!kept) {
    std::fputs("transform_rounding_assert_fail: a kept coefficient is "
               "wrong\n",
               stderr);
    return 1;
  }
  return 0;
}
