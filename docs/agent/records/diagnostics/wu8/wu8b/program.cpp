// The program a first-time user writes, with one of the mistakes that the
// reviews of the multivector type's first part left for its second, chosen
// by WU8B_CASE. It is compiled against the headers before and after that
// part. Case 0 is the correct program and must compile with both.
#include <array>
#include <complex>
#include <cstdint>
#include <memory>

#include <galoot/core.hpp>

#ifndef WU8B_CASE
#define WU8B_CASE 0
#endif

namespace gc = galoot::core;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};

inline constexpr auto point_layout = gc::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Point = gc::mv<rga3, point_layout, double>;
using Line = gc::mv<rga3, line_layout, double>;
using Motor = gc::mv<rga3, motor_layout, double>;
// A layout that full, of_grade, or a product computes lists no names.
using Bivector = gc::mv<rga3, gc::of_grade<4, 2>, double>;

// A scalar that must be given a value when made.
struct fixed {
  explicit fixed(std::int64_t value) : sixteenths(value * 16) {}
  std::int64_t sixteenths;
};

double use(int w) {
  const Point p{1, 2, 3, 1};
  const Line l{0, 0, 1, 0, 0, 0};
  const Motor q{1, 0, 0, 0, 0, 0, 0, 0};
  const Bivector b{};
  const std::array<double, 4> values{1, 2, 3, 1};
  double r =
      q.coefficients[0] + b.coefficients[0] + values[0] + l.coefficients[0];

#if WU8B_CASE == 8
  // An implicit lossy conversion: the motor's other components are dropped.
  const Point s = q;
  r += s.coefficients[0];
#elif WU8B_CASE == 9
  // The same, explicit.
  const Point s{q};
  r += s.coefficients[0];
#elif WU8B_CASE == 10
  // A blade that a computed layout lacks.
  r += get<"e1">(b);
#elif WU8B_CASE == 11
  const gc::mv<rga3, point_layout, const double> s{};
  r += s.coefficients[0];
#elif WU8B_CASE == 12
  gc::mv<rga3, point_layout, double&> s{};
  r += s.coefficients[0];
#elif WU8B_CASE == 13
  const gc::mv<rga3, point_layout, bool> s{};
  r += s.coefficients[0] ? 1 : 0;
#elif WU8B_CASE == 14
  const gc::mv<rga3, point_layout, fixed> s{};
  r += static_cast<double>(s.coefficients[0].sixteenths);
#elif WU8B_CASE == 15
  // The habit of std::array and of an aggregate: a second pair of braces.
  const Point s{{1, 2, 3, 1}};
  r += s.coefficients[0];
#elif WU8B_CASE == 16
  // The coefficients as one array.
  const Point s{values};
  r += s.coefficients[0];
#elif WU8B_CASE == 17
  // An implicit conversion that drops nothing.
  const Motor s = l;
  r += s.coefficients[0];
#elif WU8B_CASE == 18
  // The habit of std::array after an equals sign.
  const Point s = {{1, 2, 3, 1}};
  r += s.coefficients[0];
#elif WU8B_CASE == 19
  // One value of another type.
  const Point s{std::complex<double>{1, 0}};
  r += s.coefficients[0];
#elif WU8B_CASE == 20
  // The scalar part as a number, with the result used.
  const double s = gc::truncate<double>(q);
  r += s;
#elif WU8B_CASE == 21
  const gc::mv<rga3, point_layout, std::unique_ptr<double>> s{};
  r += s.coefficients.empty() ? 0 : 1;
#elif WU8B_CASE == 22
  // The other orientation of a blade of a computed layout.
  r += get<"e21">(b);
#endif
  return r + p.coefficients[0] + w;
}

int main() { return static_cast<int>(use(1)); }
