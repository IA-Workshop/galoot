// transform at run time, where the check of what it drops can fire. The
// build fixes the checking mode, and the argument names the case:
//
//   int64     in R(4,0,0), x = 1 on the scalar and V = 1 + e1234, both of
//             even antigrade: V ⟇ x ⟇ antireverse(V) = 2 + 2 e1234, by hand
//             from 1 ⟇ 1 = e1234 and 1 ⟇ e1234 = 1, so 2 e1234 is dropped;
//             it must abort with the message when checking is on, and
//             otherwise keep 2 and exit 0
//   rational  the same over the test rational
//   recorded  in R(4,0,0), x = e1 and V = 1 + e12 + 2 e13 + 3 e14, over a
//             scalar whose == records its operands and answers true. By
//             hand, a ⟇ b = b e1234 a in R(4,0,0) and e1234 e1 e1234 = -e1,
//             so the transform is -rev(V) e1 V = 13 e1 - 2 e2 - 4 e3 - 6 e4;
//             the two antiproducts reach e1, e2, e3, e4, e123, e124, and
//             e134, the last three with terms that cancel. With checking on
//             it must compare the six blades other than e1, each once with
//             zero, so the values -2, -4, -6, 0, 0, 0, and never the kept 13;
//             with checking off it must compare nothing. Either way it keeps
//             13 and exits 0
//   rigid_int64
//             in rigid(3), the point, line, plane, motor, and flector, each
//             with the coefficients 1, 2, 3, ... in order, transformed by the
//             motor and the flector with the coefficients 1 to 8, which are
//             not versors; what each drops is zero, so it must not abort in
//             either mode and must give the values of the same transforms in
//             a constant evaluation, where the check always runs
//   rigid_rational
//             the same over the test rational
//
// The inputs are read at run time, so no call is evaluated at compile time.
// A usage error, or a kept coefficient that is wrong, exits 2 or 1 with a
// message, which fails every test that runs this program.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <span>
#include <string_view>
#include <vector>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/transform.hpp>

#include <support/rational.hpp>

namespace {

// An exact integer scalar whose == records its two operands and answers
// true, so that the check never fires and the program can read what it
// compared.
struct recorded {
  std::int64_t value = 0;

  recorded() = default;
  explicit recorded(std::int64_t v) : value(v) {}

  [[maybe_unused]] friend recorded operator+(const recorded& a,
                                             const recorded& b) {
    return recorded{a.value + b.value};
  }
  [[maybe_unused]] friend recorded operator-(const recorded& a,
                                             const recorded& b) {
    return recorded{a.value - b.value};
  }
  [[maybe_unused]] friend recorded operator*(const recorded& a,
                                             const recorded& b) {
    return recorded{a.value * b.value};
  }
  [[maybe_unused]] friend recorded operator-(const recorded& a) {
    return recorded{-a.value};
  }
  friend bool operator==(const recorded& a, const recorded& b);
};

// The operands of each call of recorded's ==: test state, read in main.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::vector<std::array<std::int64_t, 2>> comparisons;

[[maybe_unused]] bool operator==(const recorded& a, const recorded& b) {
  comparisons.push_back({a.value, b.value});
  return true;
}

} // namespace

/// recorded is exact, as an integer is.
template <>
struct std::numeric_limits<recorded> {
  [[maybe_unused]] static constexpr bool is_specialized = true;
  [[maybe_unused]] static constexpr bool is_exact = true;
};

namespace {

namespace core = galoot::core;

struct r400 {
  static constexpr auto value = core::R<4, 0, 0>();
};
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
inline constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
inline constexpr auto flector_layout =
    core::blades<rga3, "e1 e2 e3 e4 e423 e431 e412 e321">;

// The transform of s on the scalar by 1 + e1234 in R(4,0,0); returns
// whether it keeps 2 s.
template <class T>
bool keeps_twice(std::int64_t s) {
  using X = core::mv<r400, core::blades<r400, "1">, T>;
  using V = core::mv<r400, core::blades<r400, "1 e1234">, T>;
  const X kept = core::transform(X{T(s)}, V{T(1), T(1)});
  return kept.coefficients[0] == T(2 * s);
}

// The transform of s e1 by 1 + e12 + 2 e13 + 3 e14 in R(4,0,0) over
// recorded; returns whether it keeps 13 s on e1 and compared what the
// checking mode says.
bool compares_the_dropped(std::int64_t s) {
  using X = core::mv<r400, core::blades<r400, "e1">, recorded>;
  using V = core::mv<r400, core::blades<r400, "1 e12 e13 e14">, recorded>;
  const X kept = core::transform(
      X{recorded{s}}, V{recorded{1}, recorded{1}, recorded{2}, recorded{3}});
  if (kept.coefficients[0].value != 13 * s) {
    return false;
  }
  if (GALOOT_CHECKING == 0) {
    return comparisons.empty();
  }
  // The compared values, each with zero, against -2 s, -4 s, -6 s, and three
  // zeros in any order.
  std::array<std::int64_t, 6> expected{-2 * s, -4 * s, -6 * s, 0, 0, 0};
  std::array<bool, 6> matched{};
  if (comparisons.size() != expected.size()) {
    return false;
  }
  for (const auto& [left, right] : comparisons) {
    if (right != 0) {
      return false;
    }
    bool found = false;
    for (std::size_t k = 0; k < expected.size() && !found; ++k) {
      if (!matched.at(k) && expected.at(k) == left) {
        matched.at(k) = true;
        found = true;
      }
    }
    if (!found) {
      return false;
    }
  }
  return true;
}

// x of the layout of X with the coefficients s, 2 s, 3 s, ... in order.
template <class X>
constexpr X filled(std::int64_t s) {
  using T = typename X::scalar_type;
  X x{};
  for (std::size_t k = 0; k < X::component_count; ++k) {
    x.coefficients[k] = T(s * (static_cast<std::int64_t>(k) + 1));
  }
  return x;
}

// Whether transform(x, v) at run time, with x and v read from s, equals the
// same transform in a constant evaluation with s = 1.
template <class X, class V>
bool as_at_compile_time(std::int64_t s) {
  constexpr X at_compile_time = core::transform(filled<X>(1), filled<V>(1));
  return core::transform(filled<X>(s), filled<V>(s)) == at_compile_time;
}

template <class T>
bool keeps_rigid(std::int64_t s) {
  using Point = core::mv<rga3, point_layout, T>;
  using Line = core::mv<rga3, line_layout, T>;
  using Plane = core::mv<rga3, plane_layout, T>;
  using Motor = core::mv<rga3, motor_layout, T>;
  using Flector = core::mv<rga3, flector_layout, T>;
  return as_at_compile_time<Point, Motor>(s) &&
         as_at_compile_time<Line, Motor>(s) &&
         as_at_compile_time<Plane, Motor>(s) &&
         as_at_compile_time<Motor, Motor>(s) &&
         as_at_compile_time<Flector, Motor>(s) &&
         as_at_compile_time<Point, Flector>(s) &&
         as_at_compile_time<Line, Flector>(s) &&
         as_at_compile_time<Plane, Flector>(s) &&
         as_at_compile_time<Motor, Flector>(s) &&
         as_at_compile_time<Flector, Flector>(s);
}

} // namespace

int main(int argc, char** argv) {
  const std::span<char*> args(argv, static_cast<std::size_t>(argc));
  if (args.size() != 2) {
    std::fputs("usage: transform_assert_fail int64|rational|recorded|"
               "rigid_int64|rigid_rational\n",
               stderr);
    return 2;
  }
  // s is read from the number of arguments, so that the compiler cannot
  // evaluate the calls at compile time.
  const std::int64_t s = static_cast<std::int64_t>(args.size()) - 1;
  const std::string_view which = args[1];
  bool kept = false;
  if (which == "int64") {
    kept = keeps_twice<std::int64_t>(s);
  } else if (which == "rational") {
    kept = keeps_twice<test_support::rational>(s);
  } else if (which == "recorded") {
    kept = compares_the_dropped(s);
  } else if (which == "rigid_int64") {
    kept = keeps_rigid<std::int64_t>(s);
  } else if (which == "rigid_rational") {
    kept = keeps_rigid<test_support::rational>(s);
  } else {
    std::fputs("transform_assert_fail: unknown case\n", stderr);
    return 2;
  }
  if (!kept) {
    std::fputs("transform_assert_fail: a kept coefficient or a comparison "
               "is wrong\n",
               stderr);
    return 1;
  }
  return 0;
}
