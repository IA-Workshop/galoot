// truncate at run time, with a coefficient it drops that is not zero. The
// build fixes the checking mode, and the argument names the case:
//
//   int64     a motor over std::int64_t with the scalar 7, truncated to a
//             line, which must abort with the message when checking is on
//   int64_antiscalar
//             the same with the scalar 0 and the antiscalar 7, the last
//             place truncate drops
//   rational  the same as int64 over the test rational
//   rational_antiscalar
//             the same as int64_antiscalar over the test rational
//   counted   the same over an exact scalar that counts its comparisons;
//             with checking off it must keep the line, compare nothing, and
//             exit 0
//
// The input is read at run time, so no call is evaluated at compile time.
// A usage error, or a kept coefficient that is wrong, exits 2 or 1 with a
// message, which fails every test that runs this program.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <span>
#include <string_view>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

#include <support/rational.hpp>

namespace {

// An exact integer scalar whose == counts how often it is called. The
// arithmetic is what a scalar must have, and is not called here;
// [[maybe_unused]] says so.
struct counted {
  std::int64_t value = 0;

  counted() = default;
  explicit counted(std::int64_t v) : value(v) {}

  [[maybe_unused]] friend counted operator+(const counted& a,
                                            const counted& b) {
    return counted{a.value + b.value};
  }
  [[maybe_unused]] friend counted operator-(const counted& a,
                                            const counted& b) {
    return counted{a.value - b.value};
  }
  [[maybe_unused]] friend counted operator*(const counted& a,
                                            const counted& b) {
    return counted{a.value * b.value};
  }
  [[maybe_unused]] friend counted operator-(const counted& a) {
    return counted{-a.value};
  }
  friend bool operator==(const counted& a, const counted& b);
};

// The number of calls of counted's ==: test state, read once in main.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
int comparisons = 0;

[[maybe_unused]] bool operator==(const counted& a, const counted& b) {
  ++comparisons;
  return a.value == b.value;
}

} // namespace

/// counted is exact, as an integer is.
template <>
struct std::numeric_limits<counted> {
  [[maybe_unused]] static constexpr bool is_specialized = true;
  [[maybe_unused]] static constexpr bool is_exact = true;
};

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    galoot::core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;

// Whether a equals b, without counting a comparison of counted.
template <class T>
bool same(const T& a, const T& b) {
  return a == b;
}
bool same(const counted& a, const counted& b) { return a.value == b.value; }

// Truncates the motor (scalar, 1 to 6 on the line, antiscalar) to a line,
// and returns whether the line holds 1 to 6.
template <class T>
bool keeps_the_line(std::int64_t scalar, std::int64_t antiscalar) {
  using Motor = galoot::core::mv<rga3, motor_layout, T>;
  const std::array<std::int64_t, 8> v{scalar, 1, 2, 3, 4, 5, 6, antiscalar};
  Motor motor{};
  for (std::size_t i = 0; i < v.size(); ++i) {
    motor.coefficients.at(i) = T(v.at(i));
  }
  const auto line = galoot::core::truncate<line_layout>(motor);
  for (std::size_t i = 0; i < line.coefficients.size(); ++i) {
    const T expected(static_cast<std::int64_t>(i) + 1);
    if (!same(line.coefficients[i], expected)) {
      return false;
    }
  }
  return true;
}

} // namespace

int main(int argc, char** argv) {
  const std::span<char*> args(argv, static_cast<std::size_t>(argc));
  if (args.size() != 2) {
    std::fputs("usage: mv_truncate_assert_fail int64|int64_antiscalar|"
               "rational|rational_antiscalar|counted\n",
               stderr);
    return 2;
  }
  // The scalar is read from the number of arguments, so that the compiler
  // cannot evaluate the call at compile time.
  const std::int64_t scalar = 5 + static_cast<std::int64_t>(args.size());
  const std::string_view which = args[1];
  bool kept = false;
  if (which == "int64") {
    kept = keeps_the_line<std::int64_t>(scalar, 0);
  } else if (which == "int64_antiscalar") {
    kept = keeps_the_line<std::int64_t>(0, scalar);
  } else if (which == "rational") {
    kept = keeps_the_line<test_support::rational>(scalar, 0);
  } else if (which == "rational_antiscalar") {
    kept = keeps_the_line<test_support::rational>(0, scalar);
  } else if (which == "counted") {
    kept = keeps_the_line<counted>(scalar, 0);
    if (comparisons != 0) {
      std::fputs("mv_truncate_assert_fail: the check compared values\n",
                 stderr);
      return 1;
    }
  } else {
    std::fputs("mv_truncate_assert_fail: unknown case\n", stderr);
    return 2;
  }
  if (!kept) {
    std::fputs("mv_truncate_assert_fail: a kept coefficient is wrong\n",
               stderr);
    return 1;
  }
  return 0;
}
