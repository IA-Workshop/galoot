// What generic code and a curious user are told about WU8a's types: the
// answers of the standard concepts and traits a generic function might
// probe, and the values that the silent cases of first_user.cpp produce.
// Prints one line per question; built and run, not only compiled.
#include <array>
#include <concepts>
#include <cstdio>
#include <ranges>
#include <string>
#include <tuple>
#include <type_traits>

#include <galoot/core.hpp>

namespace gc = galoot::core;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};
struct cga3 {
  static constexpr auto value = gc::conformal<3>();
};

inline constexpr auto point_layout = gc::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Point = gc::mv<rga3, point_layout, double>;
using Line = gc::mv<rga3, line_layout, double>;
using FPoint = gc::mv<rga3, point_layout, float>;
using Scalar = gc::mv<rga3, gc::blades<rga3, "1">, double>;

namespace {

void say(const char* question, bool answer) {
  std::printf("%-58s %s\n", question, answer ? "yes" : "no");
}

template <class X>
concept tuple_like = requires { std::tuple_size<X>::value; };

} // namespace

int main(int argc, char** /*argv*/) {
  say("std::regular<Line>", std::regular<Line>);
  say("std::is_aggregate_v<Line>", std::is_aggregate_v<Line>);
  say("std::is_trivially_copyable_v<Line>", std::is_trivially_copyable_v<Line>);
  say("std::is_default_constructible_v<Line>",
      std::is_default_constructible_v<Line>);
  say("std::is_constructible_v<Line, double, double, double>",
      std::is_constructible_v<Line, double, double, double>);
  say("std::is_constructible_v<Line, std::array<double, 6>>",
      std::is_constructible_v<Line, std::array<double, 6>>);
  say("std::is_convertible_v<double, Scalar>",
      std::is_convertible_v<double, Scalar>);
  say("std::is_constructible_v<Scalar, double>",
      std::is_constructible_v<Scalar, double>);
  say("std::ranges::range<Line>", std::ranges::range<Line>);
  say("std::ranges::range<decltype(Line::coefficients)>",
      std::ranges::range<decltype(Line::coefficients)>);
  say("tuple-like Line (std::tuple_size)", tuple_like<Line>);
  say(
      "requires { get<\"e1\">(line) } (the line holds no e1)",
      requires(Line& line) { get<"e1">(line); });
  say("gc::has_blade<Line, \"e1\">", gc::has_blade<Line, "e1">);
  say("gc::has_blade<Line, \"e41\">", gc::has_blade<Line, "e41">);
  say("gc::ring_scalar<bool>", gc::ring_scalar<bool>);
  say("gc::ring_scalar<char>", gc::ring_scalar<char>);
  say("gc::ring_scalar<int*>", gc::ring_scalar<int*>);
  say("gc::ring_scalar<std::string>", gc::ring_scalar<std::string>);
  say("std::is_same_v<mv<cga3, line_layout>, mv<cga3, blades<cga3,...>>>",
      std::is_same_v<
          gc::mv<cga3, line_layout, double>,
          gc::mv<cga3, gc::blades<cga3, "e41 e42 e43 e23 e31 e12">, double>>);
  say("std::is_same_v<Line, mv<rga3, blades<rga3, \"...\">, double>>",
      std::is_same_v<
          Line,
          gc::mv<rga3, gc::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>>);
  say("std::is_same_v<Line, mv<rga3, blades<rga3, \"e41  e42 ...\">>> (2 sp)",
      std::is_same_v<
          Line,
          gc::mv<rga3, gc::blades<rga3, "e41  e42 e43 e23 e31 e12">, double>>);

  // Values of the cases that compile.
  volatile double tenth = 0.1;
  const FPoint narrowed(tenth, 0, 0, 1);
  std::printf("FPoint(0.1 double, ...) by parentheses holds %.17g\n",
              static_cast<double>(narrowed.coefficients[0]));
  volatile long long big = (1LL << 53) + 1;
  const Point rounded(big, 0, 0, 1);
  std::printf("Point(2^53 + 1 long long, ...) by parentheses holds %.17g\n",
              rounded.coefficients[0]);
  const Point p{1, 2, 3, 1};
  const auto [all] = p;
  std::printf("auto [all] = point binds a %zu-element array\n", all.size());
  const gc::mv<rga3, line_layout, bool> flags{true,  true,  false,
                                              false, false, false};
  std::printf("a bool line constructs: %d %d\n", flags.coefficients[0] ? 1 : 0,
              flags.coefficients[1] ? 1 : 0);
  return argc > 5 ? 1 : 0;
}
