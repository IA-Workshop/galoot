// transform over double and float at run time, where the rounding check of
// what it drops can fire. The build fixes the checking mode, and the
// argument names the case (the inputs are in transform_rounding_cases.hpp,
// and transform_rounding_test.cpp shows that they are as said here). A
// case without a suffix is over double; the suffix _float is over float:
//
//   not_a_versor_double
//   not_a_versor_float
//             in R(4,0,0), x = 1 on the scalar and V = 1 + e1234, which is
//             not a versor: V ⟇ x ⟇ antireverse(V) = 2 + 2 e1234, so 2 e1234
//             is dropped, far above its rounding error, which is zero; it
//             must abort with the message when checking is on, and otherwise
//             keep 2 and exit 0
//   negative_double
//   negative_float
//             the same with V = 1 - e1234, which drops -2 e1234 and keeps 2,
//             so that the check must compare the size of what it drops
//   ten_times
//   ten_times_float
//             a versor of R(4,0,0) perturbed so that the dropped part is at
//             least ten times the largest tolerance the check may have; it
//             must abort when checking is on
//   tenth     the same versor perturbed so that the dropped part is at most
//             a tenth of the smallest tolerance the check may have; it must
//             not abort
//   nan
//   nan_float x = 1 and V = 1 + NaN e1234, whose dropped coefficient is NaN;
//             it must abort when checking is on
//   infinite
//   infinite_float
//             the same with an infinite coefficient; it must abort when
//             checking is on
//   overflow
//   overflow_float
//             a versor near the overflow threshold whose dropped
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

// The transform of 1 on the scalar by v; returns whether it keeps `kept`.
template <class T>
bool keeps(const cases::Pair<T>& v, T kept) {
  const cases::Scalar<T> result = core::transform(cases::Scalar<T>{T{1}}, v);
  return result.coefficients[0] == kept;
}

// The transform of s on the scalar by v; returns true, since only whether
// it aborts matters.
template <class T, class V>
bool runs(const V& v, T s) {
  const cases::Scalar<T> kept = core::transform(cases::Scalar<T>{s}, v);
  static_cast<void>(kept);
  return true;
}

// The perturbed versor with the perturbation m, on the scalar s.
template <class T>
bool runs_perturbed(T m, T s) {
  return runs(cases::perturbed_versor(m * s), s);
}

// The versor near overflow, its coefficients times s.
template <class T>
bool runs_near_overflow(T s) {
  cases::Even<T> v = cases::near_overflow_versor<T>();
  for (T& c : v.coefficients) {
    c *= s;
  }
  return runs(v, static_cast<T>(cases::near_overflow_scalar) * s);
}

// 1 + bad e1234 on the scalar s.
template <class T>
bool runs_with(T bad, T s) {
  return runs(cases::Pair<T>{s, bad}, s);
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

// The case named `which` over T, with s = 1 read at run time; returns
// whether it kept what it should, and sets `known` when the name is a case.
template <class T>
bool run_case(std::string_view which, T s, bool& known) {
  known = true;
  if (which == "not_a_versor") {
    return keeps(cases::not_a_versor(s), T{2} * s * s);
  }
  if (which == "negative") {
    return keeps(cases::negative_not_a_versor(s), T{2} * s * s);
  }
  if (which == "ten_times") {
    return runs_perturbed(cases::ten_times_perturbation<T>, s);
  }
  if (which == "tenth") {
    return runs_perturbed(static_cast<T>(cases::tenth_perturbation), s);
  }
  if (which == "nan") {
    return runs_with(std::numeric_limits<T>::quiet_NaN(), s);
  }
  if (which == "infinite") {
    return runs_with(std::numeric_limits<T>::infinity(), s);
  }
  if (which == "overflow") {
    return runs_near_overflow(s);
  }
  if (which == "underflow") {
    return runs_underflow(s);
  }
  known = false;
  return false;
}

} // namespace

int main(int argc, char** argv) {
  const std::span<char*> args(argv, static_cast<std::size_t>(argc));
  if (args.size() != 2) {
    std::fputs("usage: transform_rounding_assert_fail <case>[_double|_float]"
               ", <case> one of not_a_versor, negative, ten_times, tenth, "
               "nan, infinite, overflow, underflow\n",
               stderr);
    return 2;
  }
  // s is 1, read from the number of arguments, so that the compiler cannot
  // evaluate the calls at compile time.
  const auto s = static_cast<double>(args.size() - 1);
  const auto sf = static_cast<float>(args.size() - 1);
  std::string_view which = args[1];
  bool over_float = false;
  if (which.ends_with("_float")) {
    over_float = true;
    which.remove_suffix(std::string_view{"_float"}.size());
  } else if (which.ends_with("_double")) {
    which.remove_suffix(std::string_view{"_double"}.size());
  }
  bool known = false;
  const bool kept =
      over_float ? run_case(which, sf, known) : run_case(which, s, known);
  if (!known) {
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
