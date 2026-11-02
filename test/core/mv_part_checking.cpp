// part at run time in a checking build, with coefficients it drops that are
// not zero, which truncate would reject. The argument names the case:
//
//   int64     a motor over std::int64_t with the scalar 7, whose part on a
//             line must be the line
//   int64_antiscalar
//             the same with the scalar 0 and the antiscalar 7
//   rational  the same as int64 over the test rational
//   counted   the same over an exact scalar that counts its comparisons,
//             which part must not make
//
// Each must exit 0. The input is read at run time, so no call is evaluated
// at compile time. A usage error, or a kept coefficient that is wrong,
// exits 2 or 1 with a message, which fails the test that runs this program.
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

// Takes the part on a line of the motor (scalar, 1 to 6 on the line,
// antiscalar), by the line's layout and by its type, and returns whether
// both hold 1 to 6.
template <class T>
bool keeps_the_line(std::int64_t scalar, std::int64_t antiscalar) {
  using Motor = galoot::core::mv<rga3, motor_layout, T>;
  using Line = galoot::core::mv<rga3, line_layout, T>;
  const std::array<std::int64_t, 8> v{scalar, 1, 2, 3, 4, 5, 6, antiscalar};
  Motor motor{};
  for (std::size_t i = 0; i < v.size(); ++i) {
    motor.coefficients.at(i) = T(v.at(i));
  }
  const auto by_layout = galoot::core::part<line_layout>(motor);
  const Line by_type = galoot::core::part<Line>(motor);
  for (std::size_t i = 0; i < Line::component_count; ++i) {
    const T expected(static_cast<std::int64_t>(i) + 1);
    if (!same(by_layout.coefficients.at(i), expected) ||
        !same(by_type.coefficients.at(i), expected)) {
      return false;
    }
  }
  return true;
}

} // namespace

int main(int argc, char** argv) {
  const std::span<char*> args(argv, static_cast<std::size_t>(argc));
  if (args.size() != 2) {
    std::fputs("usage: mv_part_checking int64|int64_antiscalar|rational|"
               "counted\n",
               stderr);
    return 2;
  }
  // The dropped coefficient is read from the number of arguments, so that
  // the compiler cannot evaluate the call at compile time.
  const std::int64_t dropped = 5 + static_cast<std::int64_t>(args.size());
  const std::string_view which = args[1];
  bool kept = false;
  if (which == "int64") {
    kept = keeps_the_line<std::int64_t>(dropped, 0);
  } else if (which == "int64_antiscalar") {
    kept = keeps_the_line<std::int64_t>(0, dropped);
  } else if (which == "rational") {
    kept = keeps_the_line<test_support::rational>(dropped, 0);
  } else if (which == "counted") {
    kept = keeps_the_line<counted>(dropped, 0);
    if (comparisons != 0) {
      std::fputs("mv_part_checking: part compared values\n", stderr);
      return 1;
    }
  } else {
    std::fputs("mv_part_checking: unknown case\n", stderr);
    return 2;
  }
  if (!kept) {
    std::fputs("mv_part_checking: a kept coefficient is wrong\n", stderr);
    return 1;
  }
  return 0;
}
