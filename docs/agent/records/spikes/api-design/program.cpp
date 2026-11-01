// The program a first-time user writes, with one mistake chosen by APX_CASE.
// APX_SPELL selects how the types are declared: 'M' (masks and signs, through
// blades<n, "...">) or 'N' (the names as written). Case 0 is the correct
// program and must compile in every form and spelling.
#include "fixture.hpp"

#include <string>

#ifndef APX_SPELL
#define APX_SPELL 'M'
#endif
#ifndef APX_CASE
#define APX_CASE 0
#endif

namespace gc = galoot::core;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};
struct r400 {
  static constexpr auto value = gc::R<4, 0, 0>();
};

#if APX_SPELL == 'M'
using Point = apx::mv<rga3, gc::blades<4, "e1 e2 e3 e4">>;
using Line = apx::mv<rga3, gc::blades<4, "e41 e42 e43 e23 e31 e12">>;
using Motor =
    apx::mv<rga3, gc::blades<4, "1 e41 e42 e43 e23 e31 e12 e1234">>;
using Vector4 = apx::mv<r400, gc::blades<4, "e1 e2 e3 e4">>;
#else
using Point = apx::mvn<rga3, "e1 e2 e3 e4">;
using Line = apx::mvn<rga3, "e41 e42 e43 e23 e31 e12">;
using Motor = apx::mvn<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Vector4 = apx::mvn<r400, "e1 e2 e3 e4">;
#endif

double use(int w) {
  Point p{1, 2, 3, 1};
  Line l{0, 0, 1, 0, 0, 0};
  const Motor q{1, 0, 0, 0, 0, 0, 0, 0};
  double r = q.c[0];

#if APX_CASE == 0
  get<"e41">(l) = 2;
  const Point m = apx::make<Point>(1, 2, 3, w);
  r += get<"e41">(l) + get<"e3">(p) + (p == m ? 1 : 0);
#elif APX_CASE == 1
  // Too few coefficients: the plane z = 0 meant as a point, or a forgotten w.
  const Point s{1, 2, 3};
  r += s.c[3];
#elif APX_CASE == 2
  // Too few coefficients through a factory.
  const Point s = apx::make<Point>(1, 2, 3);
  r += s.c[3];
#elif APX_CASE == 3
  // Too many coefficients.
  const Point s{1, 2, 3, 1, 5};
  r += s.c[3];
#elif APX_CASE == 4
  // An int variable in a list of doubles.
  const Point s{1, 2, 3, w};
  r += s.c[3];
#elif APX_CASE == 5
  // The opposite orientation: the line stores e41, the user asks for e14.
  r += get<"e14">(l);
#elif APX_CASE == 6
  // A blade the algebra has, but the type does not hold.
  r += get<"e1">(l);
#elif APX_CASE == 7
  // A name that is not a blade of the algebra.
  r += get<"e5">(l);
#elif APX_CASE == 8
  // An implicit lossy conversion: the motor's other components are dropped.
  const Point s = q;
  r += s.c[0];
#elif APX_CASE == 9
  // Comparing elements of two algebras with the same number of generators.
  const Vector4 v{1, 2, 3, 1};
  r += (p == v) ? 1 : 0;
#elif APX_CASE == 10
  // A scalar type without subtraction or multiplication.
#if APX_SPELL == 'M'
  using Bad = apx::mv<rga3, gc::blades<4, "e1 e2 e3 e4">, std::string>;
#else
  using Bad = apx::mvn<rga3, "e1 e2 e3 e4", std::string>;
#endif
  const Bad b{};
  r += static_cast<double>(b.c.size());
#elif APX_CASE == 11
  // A blade set written for the wrong number of generators.
#if APX_SPELL == 'M'
  using Wrong = apx::mv<rga3, gc::blades<5, "e1 e5">>;
#else
  using Wrong = apx::mvn<rga3, "e1 e5">;
#endif
  const Wrong x{};
  r += x.c[0];
#endif
  return r + p.c[0] + l.c[0] + w;
}

int main() { return static_cast<int>(use(1)); }
