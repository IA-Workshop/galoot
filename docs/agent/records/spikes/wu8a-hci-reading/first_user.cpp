// The program a first-time user writes with WU8a's mv.hpp, knowing geometric
// algebra but not the library's internals, with one likely mistake or
// confusion chosen by CASE. Case 0 is the correct program and must compile.
// Cases marked "compiles" are traps: they compile, and the question is
// whether they should. The numbering is this spike's own.
#include <array>
#include <cstddef>
#include <format>
#include <iostream>
#include <string>
#include <vector>

#include <galoot/core.hpp>

#ifndef CASE
#define CASE 0
#endif

namespace gc = galoot::core;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};
struct cga3 {
  static constexpr auto value = gc::conformal<3>();
};
struct r3 {
  static constexpr auto value = gc::R<3, 0, 0>();
};

inline constexpr auto point_layout = gc::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
inline constexpr auto scalar_layout = gc::blades<rga3, "1">;

using Point = gc::mv<rga3, point_layout, double>;
using Line = gc::mv<rga3, line_layout, double>;
using Motor = gc::mv<rga3, motor_layout, double>;
using Scalar = gc::mv<rga3, scalar_layout, double>;
using FPoint = gc::mv<rga3, point_layout, float>;
using FLine = gc::mv<rga3, line_layout, float>;
using ILine = gc::mv<rga3, line_layout, int>;

// An algebra tag whose metric member is not named value (case 15).
struct bad_tag {
  static constexpr auto metric = gc::rigid<3>();
};

Line make_line() { return Line{0, 0, 1, 0, 0, 0}; }

double use(int w, double d) {
  Point p{1, 2, 3, 1};
  Line l{0, 0, 1, 0, 0, 0};
  const Motor q{1, 0, 0, 0, 0, 0, 0, 0};
  double r = p.coefficients[0] + l.coefficients[0] + q.coefficients[0] + w + d;

#if CASE == 0
  // The three tasks: declare a type, make and compare values, read and write
  // a coefficient by its oriented name.
  get<"e23">(l) = 2.0;
  r += get<"e43">(l) + gc::get<"e4">(p);
  Line none;
  constexpr Line zero{};
  r += get<"e41">(none) + get<"e41">(zero);
  p = {1, 2, 3, 1};
  r += (l != Line{0, 0, 1, 2, 0, 0}) ? 1.0 : 0.0;
  for (double c : l.coefficients) {
    r += c;
  }
  const auto [x, y, z, weight] = p.coefficients;
  r += x + y + z + weight;
  std::cout << std::format("{}\n", l.coefficients);
#elif CASE == 1
  // One value short: a point without its weight.
  const Point s{1, 2, 3};
  r += s.coefficients[0];
#elif CASE == 2
  // One value too many.
  const Line s{0, 0, 1, 0, 0, 0, 1};
  r += s.coefficients[0];
#elif CASE == 3
  // An int variable for a double coefficient.
  const Point s{1, 2, 3, w};
  r += s.coefficients[0];
#elif CASE == 4
  // A double variable for a float coefficient.
  const FPoint s{d, 0, 0, 1};
  r += s.coefficients[0];
#elif CASE == 5
  // The same with parentheses, which do not check narrowing.
  const FPoint s(d, 0, 0, 1);
  r += s.coefficients[0];
#elif CASE == 6
  // Double braces, the habit of std::array and of aggregates.
  const Point s{{1, 2, 3, 1}};
  r += s.coefficients[0];
#elif CASE == 7
  // From an array of four values.
  const std::array<double, 4> values{1, 2, 3, 1};
  const Point s{values};
  r += s.coefficients[0];
#elif CASE == 8
  // emplace_back with one value short.
  std::vector<Point> points;
  points.emplace_back(1, 2, 3);
#elif CASE == 9
  // A misspelled name: the letter l for the digit 1.
  r += get<"e4l">(l);
#elif CASE == 10
  // Commas between the names of a layout.
  constexpr auto commas = gc::blades<rga3, "e41, e42, e43, e23, e31, e12">;
  r += static_cast<double>(commas.size());
#elif CASE == 11
  // The other orientation: the line stores e41, the user asks for e14.
  r += get<"e14">(l);
#elif CASE == 12
  // A blade the algebra has and the type does not hold.
  r += get<"e1">(l);
#elif CASE == 13
  // The names written straight into the type.
  using Named = gc::mv<rga3, "e41 e42 e43 e23 e31 e12", double>;
  const Named s{};
  r += s.coefficients[0];
#elif CASE == 14
  // The count of generators, as blades took it before.
  constexpr auto counted = gc::blades<4, "e1 e2 e3 e4">;
  r += static_cast<double>(counted.size());
#elif CASE == 15
  // An algebra tag whose metric member has another name.
  constexpr auto set = gc::blades<bad_tag, "e1 e2">;
  r += static_cast<double>(set.size());
#elif CASE == 16
  // The metric's type used as the algebra.
  using metric_type = decltype(gc::rigid<3>());
  constexpr auto set = gc::blades<metric_type, "e1 e2">;
  r += static_cast<double>(set.size());
#elif CASE == 17
  // compiles: a layout written for rigid(3) used in conformal(3).
  using Mixed = gc::mv<cga3, line_layout, double>;
  const Mixed s{0, 0, 1, 0, 0, 0};
  r += s.coefficients[0];
#elif CASE == 18
  // A layout of rigid(3), which names e4, used in R(3,0,0).
  using Mixed = gc::mv<r3, line_layout, double>;
  const Mixed s{};
  r += s.coefficients[0];
#elif CASE == 19
  // Comparing a line with a motor.
  r += (l == q) ? 1.0 : 0.0;
#elif CASE == 20
  // Comparing a line over double with a line over float.
  const FLine f{0, 0, 1, 0, 0, 0};
  r += (l == f) ? 1.0 : 0.0;
#elif CASE == 21
  // Comparing a one-blade scalar type with a number.
  const Scalar s{2.0};
  r += (s == 2.0) ? 1.0 : 0.0;
#elif CASE == 22
  // A line assigned to a motor, which holds every blade of the line.
  const Motor m = l;
  r += m.coefficients[0];
#elif CASE == 23
  // A motor assigned to a point, which drops blades.
  const Point s = q;
  r += s.coefficients[0];
#elif CASE == 24
  // The explicit lossless conversion, which arrives with WU8b.
  const Motor m{l};
  r += m.coefficients[0];
#elif CASE == 25
  // A fractional literal for an integer scalar.
  const ILine s{0.5, 0, 0, 0, 0, 0};
  r += s.coefficients[0];
#elif CASE == 26
  // compiles: a constexpr default value read by name.
  constexpr Line zero;
  static_assert(get<"e41">(zero) == 0.0);
  r += get<"e41">(zero);
#elif CASE == 27
  // Printing a value.
  std::cout << l << '\n';
#elif CASE == 28
  // Formatting a value.
  r += static_cast<double>(std::format("{}", l).size());
#elif CASE == 29
  // compiles: a reference to a coefficient of a temporary.
  const double& dangling = get<"e43">(make_line());
  r += dangling;
#elif CASE == 30
  // A non-const reference to a coefficient of a temporary.
  double& dangling = get<"e43">(make_line());
  r += dangling;
#elif CASE == 31
  // Structured bindings, one name per coefficient.
  const auto [x, y, z, weight] = p;
  r += x + y + z + weight;
#elif CASE == 32
  // compiles: structured bindings with one name bind the whole array.
  const auto [all] = p;
  r += all[0];
#elif CASE == 33
  // Generic code that asks whether a name can be read, told yes for e1.
  const auto first = [](const auto& x) {
    if constexpr (requires { get<"e1">(x); }) {
      return get<"e1">(x);
    } else {
      return 0.0;
    }
  };
  r += first(l);
#elif CASE == 34
  // A range loop over the value itself.
  for (double c : l) {
    r += c;
  }
#elif CASE == 35
  // A scalar with no ==, such as a pack of lanes.
  struct lanes {
    double a = 0;
    lanes() = default;
    explicit lanes(long v) : a(static_cast<double>(v)) {}
    lanes operator+(const lanes& o) const { return lanes(*this) += o; }
    lanes& operator+=(const lanes& o) {
      a += o.a;
      return *this;
    }
    lanes operator-(const lanes& o) const {
      lanes x = *this;
      x.a -= o.a;
      return x;
    }
    lanes operator*(const lanes& o) const {
      lanes x = *this;
      x.a *= o.a;
      return x;
    }
    lanes operator-() const {
      lanes x = *this;
      x.a = -x.a;
      return x;
    }
  };
  using LaneLine = gc::mv<rga3, line_layout, lanes>;
  const LaneLine a{};
  const LaneLine b{};
  r += (a == b) ? 1.0 : 0.0;
#elif CASE == 36
  // A container as the scalar.
  using VectorLine = gc::mv<rga3, line_layout, std::vector<double>>;
  const VectorLine s{};
  r += s.coefficients[0][0];
#elif CASE == 37
  // A reference as the scalar.
  using RefLine = gc::mv<rga3, line_layout, double&>;
  r += sizeof(RefLine);
#elif CASE == 38
  // A const scalar.
  using ConstLine = gc::mv<rga3, line_layout, const double>;
  const ConstLine s{};
  r += s.coefficients[0];
#elif CASE == 39
  // compiles: bool as the scalar.
  using BoolLine = gc::mv<rga3, line_layout, bool>;
  const BoolLine s{true, false, true, false, true, false};
  r += s.coefficients[0] ? 1.0 : 0.0;
#elif CASE == 40
  // No scalar type given.
  using NoScalar = gc::mv<rga3, line_layout>;
  const NoScalar s{};
  r += s.coefficients[0];
#elif CASE == 41
  // The scalar type of a multivector type, by name.
  const Line::scalar_type v = 1.0;
  r += v;
#elif CASE == 42
  // A coefficient by subscript.
  r += l[0];
#elif CASE == 43
  // A coefficient by position, as for a tuple.
  r += std::get<0>(l);
#elif CASE == 44
  // Assigning a list one value short.
  p = {1, 2, 3};
#elif CASE == 45
  // The template arguments in another order.
  using Swapped = gc::mv<line_layout, rga3, double>;
  const Swapped s{};
  r += s.coefficients[0];
#elif CASE == 46
  // A designated initializer, as for an aggregate.
  const Point s{.coefficients = {1, 2, 3, 1}};
  r += s.coefficients[0];
#elif CASE == 47
  // The other orientation, under using namespace std.
  using namespace std;
  r += get<"e14">(l);
#elif CASE == 48
  // get with a position instead of a name.
  r += get<0>(l);
#endif
  return r;
}

int main() { return static_cast<int>(use(1, 0.5)); }
