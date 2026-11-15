// A first-time user's program for the conversions, truncate, and scalar
// choices, with one mistake chosen by CASE. Case 0 is correct.
#include <array>
#include <complex>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#if CASE == 30
#include <unistd.h>
#endif

#include <galoot/core.hpp>

#ifndef CASE
#define CASE 0
#endif

namespace gc = galoot::core;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};
struct r3 {
  static constexpr auto value = gc::R<3, 0, 0>();
};

inline constexpr auto point_layout = gc::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Point = gc::mv<rga3, point_layout, double>;
using Line = gc::mv<rga3, line_layout, double>;
using Motor = gc::mv<rga3, motor_layout, double>;
using FLine = gc::mv<rga3, line_layout, float>;
using ILine = gc::mv<rga3, line_layout, std::int64_t>;
using IMotor = gc::mv<rga3, motor_layout, std::int64_t>;
using Bivector = gc::mv<rga3, gc::of_grade<4, 2>, double>;
using R3Vector = gc::mv<r3, gc::blades<r3, "e1 e2 e3">, double>;

// Scalars a user might try.
struct fixed {  // no default constructor
  explicit fixed(std::int64_t v) : sixteenths(v * 16) {}
  std::int64_t sixteenths;
};
struct addonly {  // + and - only
  double v{};
  friend addonly operator+(addonly a, addonly b) { return {a.v + b.v}; }
  friend addonly operator-(addonly a, addonly b) { return {a.v - b.v}; }
  friend addonly operator-(addonly a) { return {-a.v}; }
};
struct noint {  // a ring, but not made from an integer
  double v{};
  friend noint operator+(noint a, noint b) { return {a.v + b.v}; }
  friend noint operator-(noint a, noint b) { return {a.v - b.v}; }
  friend noint operator*(noint a, noint b) { return {a.v * b.v}; }
  friend noint operator-(noint a) { return {-a.v}; }
};
struct constmember {  // default constructible and copyable, not assignable
  const double v{};
  friend constmember operator+(constmember a, constmember b) { return {a.v + b.v}; }
  friend constmember operator-(constmember a, constmember b) { return {a.v - b.v}; }
  friend constmember operator*(constmember a, constmember b) { return {a.v * b.v}; }
  friend constmember operator-(constmember a) { return {-a.v}; }
};

void take_motor(const Motor&) {}
void take_point(const Point&) {}

Motor as_motor(const Line& l) {
#if CASE == 4
  return l;  // an implicit lossless conversion in a return
#else
  return Motor{l};
#endif
}

double use() {
  const Point p{1, 2, 3, 1};
  const Line l{1, 2, 3, 4, 5, 6};
  const Motor q{1, 0, 0, 0, 0, 0, 0, 0};
  const Bivector b{};
  const std::array<double, 4> values{1, 2, 3, 1};
  double r = p.coefficients[0] + l.coefficients[0] + q.coefficients[0] +
             b.coefficients[0] + values[0] + as_motor(l).coefficients[0];
  take_motor(Motor{l});
  r += gc::truncate<Line>(q).coefficients[0];

#if CASE == 1
  const Point s = q;  // implicit, lossy
  r += s.coefficients[0];
#elif CASE == 2
  const Point s{q};  // explicit, lossy
  r += s.coefficients[0];
#elif CASE == 3
  const Motor s = l;  // implicit, lossless
  r += s.coefficients[0];
#elif CASE == 5
  take_motor(l);  // implicit, lossless, as an argument
#elif CASE == 6
  take_point(q);  // implicit, lossy, as an argument
#elif CASE == 7
  std::vector<Motor> motors;
  motors.push_back(l);  // implicit, lossless, into a container
  r += motors[0].coefficients[0];
#elif CASE == 8
  std::vector<Motor> motors;
  motors.emplace_back(l);  // explicit, lossless: works
  std::vector<Point> points;
  points.emplace_back(q);  // explicit, lossy, inside the library
  r += points[0].coefficients[0];
#elif CASE == 9
  const auto s = static_cast<Point>(q);  // a cast, lossy
  r += s.coefficients[0];
#elif CASE == 10
  const FLine f{l};  // another scalar type
  r += f.coefficients[0];
#elif CASE == 11
  const R3Vector v{p};  // another algebra
  r += v.coefficients[0];
#elif CASE == 12
  const auto s = gc::truncate<FLine>(q);  // truncate and change the scalar
  r += s.coefficients[0];
#elif CASE == 13
  const auto s = gc::truncate<R3Vector>(p);  // truncate across algebras
  r += s.coefficients[0];
#elif CASE == 14
  const auto s = gc::truncate<double>(q);  // the scalar part, as a number
  r += s;
#elif CASE == 15
  const auto s = gc::truncate<"e41 e42 e43 e23 e31 e12">(q);  // names
  r += s.coefficients[0];
#elif CASE == 16
  const auto s = gc::truncate(q);  // no target
  r += s.coefficients[0];
#elif CASE == 17
  // An exact scalar dropping a nonzero part in a constant evaluation.
  constexpr IMotor im{7, 1, 2, 3, 4, 5, 6, 0};
  constexpr auto s = gc::truncate<ILine>(im);
  r += static_cast<double>(s.coefficients[0]);
#elif CASE == 18
  const auto s = gc::truncate<Line>(l, 1e-12);  // a tolerance, as some libraries take
  r += s.coefficients[0];
#elif CASE == 19
  const Point s{{1, 2, 3, 1}};  // double braces
  r += s.coefficients[0];
#elif CASE == 20
  const Point s{values};  // a std::array
  r += s.coefficients[0];
#elif CASE == 21
  const Point s(values);  // a std::array, parentheses
  r += s.coefficients[0];
#elif CASE == 22
  const Point s = {{1, 2, 3, 1}};  // copy-list-initialization, double braces
  r += s.coefficients[0];
#elif CASE == 23
  const std::vector<double> v{1, 2, 3, 1};
  const Point s{v};  // a vector
  r += s.coefficients[0];
#elif CASE == 24
  const Point s{{1, 2, 3}};  // double braces, one short
  r += s.coefficients[0];
#elif CASE == 25
  const Point s{values.data()};  // a pointer to the coefficients
  r += s.coefficients[0];
#elif CASE == 26
  const Point s{1};  // one value for four places
  r += s.coefficients[0];
#elif CASE == 27
  const std::array<float, 4> fv{1, 2, 3, 1};
  const Point s{fv};  // an array of another element type
  r += s.coefficients[0];
#elif CASE == 28
  const Point s{std::complex<double>{1, 0}};  // a value that does not convert
  r += s.coefficients[0];
#elif CASE == 29
  const std::optional<Point> s = q;  // generic wrapper, lossy
  r += s->coefficients[0];
#elif CASE == 30
  const auto s = truncate<Line>(q);  // unqualified, with POSIX truncate in scope
  r += s.coefficients[0];
#elif CASE == 31
  const auto s = truncate<Line>(q);  // unqualified, found by ADL
  r += s.coefficients[0];
#elif CASE == 32
  const FLine s = gc::truncate<FLine>(q);  // the result used as the target
  r += s.coefficients[0];
#elif CASE == 33
  using Scalar = gc::mv<rga3, gc::blades<rga3, "1">, double>;
  const Scalar s{std::complex<double>{1, 0}};  // one value that does not convert
  r += s.coefficients[0];
#elif CASE == 34
  using Scalar = gc::mv<rga3, gc::blades<rga3, "1">, double>;
  const Scalar s = 2.0;  // a number into a one-blade type, implicitly
  r += s.coefficients[0];
#elif CASE == 35
  const Line s{gc::truncate<Line>(q)};  // the common idiom, compiles
  const Line t = gc::truncate<Line>(Motor{s});  // round trip
  r += t.coefficients[0];
#elif CASE == 40
  const gc::mv<rga3, point_layout, const double> s{};
  r += s.coefficients[0];
#elif CASE == 41
  gc::mv<rga3, point_layout, double&> s{};
  r += s.coefficients[0];
#elif CASE == 42
  const gc::mv<rga3, point_layout, bool> s{};
  r += s.coefficients[0] ? 1 : 0;
#elif CASE == 43
  const gc::mv<rga3, point_layout, fixed> s{};
  r += static_cast<double>(s.coefficients[0].sixteenths);
#elif CASE == 44
  const gc::mv<rga3, point_layout, std::string> s{};
  r += static_cast<double>(s.coefficients[0].size());
#elif CASE == 45
  const gc::mv<rga3, point_layout, addonly> s{};
  r += s.coefficients[0].v;
#elif CASE == 46
  const gc::mv<rga3, point_layout, noint> s{};
  r += s.coefficients[0].v;
#elif CASE == 47
  const gc::mv<rga3, point_layout, std::unique_ptr<double>> s{};
  r += s.coefficients[0] ? 1 : 0;
#elif CASE == 48
  const gc::mv<rga3, point_layout, constmember> s{};
  r += s.coefficients[0].v;
#elif CASE == 49
  const gc::mv<rga3, point_layout, volatile double> s{};
  r += s.coefficients[0];
#elif CASE == 50
  const gc::mv<rga3, point_layout, std::complex<double>> s{};  // should work
  r += s.coefficients[0].real();
#elif CASE == 51
  const gc::mv<rga3, point_layout, unsigned> s{};  // modular ring: accepted?
  r += s.coefficients[0];
#elif CASE == 52
  const gc::mv<rga3, point_layout, char> s{};  // accepted?
  r += s.coefficients[0];
#elif CASE == 53
  // A scalar declared only, as a forward declaration.
  struct later;
  r += sizeof(gc::mv<rga3, point_layout, later>*);
#elif CASE == 60
  r += get<"e1">(b);  // computed layout, blade absent
#elif CASE == 61
  r += get<"e21">(b);  // computed layout, other orientation
#elif CASE == 62
  r += get<"e12">(p);  // hand-written layout in canonical order, absent
#elif CASE == 63
  r += get<"e14">(l);  // hand-written, not canonical, other orientation
#elif CASE == 64
  r += get<"e1">(l);  // hand-written, not canonical, absent
#elif CASE == 65
  using Product = gc::mv<rga3, gc::merge<point_layout, gc::of_grade<4, 3>>, double>;
  const Product x{};
  r += get<"e12">(x);  // a merged layout, absent
#endif
  return r;
}

int main() { return static_cast<int>(use()); }
