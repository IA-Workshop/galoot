// Tests of the rga3 model against incidences of points, lines, and planes:
// the planes that wedge (^) joins, the points and lines that antiwedge (&)
// meets, and the scalars and planes that are zero exactly when two objects
// are incident, each compared exactly over the test rational with values
// that the reference computes by cross products, dot products, and
// determinants.
//
// A point (x, y, z, w) is stored on e1, e2, e3, e4; a line with its
// direction on e41, e42, e43 and its moment on e23, e31, e12; and a plane
// with its normal (gx, gy, gz) on e423, e431, e412 and its offset gw on
// e321, the orders of Lengyel's rigid geometric algebra
// (rigidgeometricalgebra.org). A join or meet whose layout is not one of the
// model's converts to the named type explicitly.
#include <cstddef>

#include <galoot/rga3.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::rga3;
using test_support::rational;
using Point = rga3::Point<rational>;
using Line = rga3::Line<rational>;
using Plane = rga3::Plane<rational>;

rational scalar(const golden::rational& v) {
  return rational{v.numerator, v.denominator};
}
Point point_of(const golden::point& p) {
  return {scalar(p[0]), scalar(p[1]), scalar(p[2]), scalar(p[3])};
}
Line line_of(const golden::line& l) {
  return {scalar(l[0]), scalar(l[1]), scalar(l[2]),
          scalar(l[3]), scalar(l[4]), scalar(l[5])};
}
Plane plane_of(const golden::plane& g) {
  return {scalar(g[0]), scalar(g[1]), scalar(g[2]), scalar(g[3])};
}

} // namespace

// Reference: independent value: the plane p ^ q ^ r takes the value
//   det[x, p, q, r] at a point x, so its coefficients on e423, e431, e412,
//   e321 are det[e_j, p, q, r], which the reference computes for seeded
//   points of several weights by the wedge and by these determinants, which
//   agree, and the rga3 Plane (p ^ q) ^ r of the same points over the test
//   rational holds the same coefficients
TEST_CASE("the rga3 Plane p ^ q ^ r of seeded points holds the reference's "
          "plane") {
  for (std::size_t i = 0; i < golden::rigid_planes::joins.size(); ++i) {
    CAPTURE(i);
    const golden::plane_join& c = golden::rigid_planes::joins.at(i);
    const Point p = point_of(c.p);
    const Point q = point_of(c.q);
    const Point r = point_of(c.r);
    CHECK(Plane{(p ^ q) ^ r} == plane_of(c.pqr));
    CHECK(Plane{core::wedge(core::wedge(p, q), r)} == plane_of(c.pqr));
  }
}

// Reference: independent value: the meets of lines and planes are the
//   points and lines common to them, and the line through p and q meets the
//   plane g in (p ^ q) v g = (g . p) q - (g . q) p, which the reference
//   computes for seeded cases by the antiwedge and by this formula, which
//   agree, and which lies on both, so the rga3 Point (p ^ q) & g over the
//   test rational is the reference's point, coefficient for coefficient,
//   and lies on g and on the line
TEST_CASE("the rga3 Point (p ^ q) & g where a line meets a plane is the "
          "reference's point") {
  for (std::size_t i = 0; i < golden::rigid_meets::line_plane.size(); ++i) {
    CAPTURE(i);
    const golden::line_plane_meet& c = golden::rigid_meets::line_plane.at(i);
    const Point p = point_of(c.p);
    const Point q = point_of(c.q);
    const Plane g = plane_of(c.g);
    const Line l{p ^ q};
    const Point meet{l & g};
    CHECK(meet == point_of(c.meet));
    CHECK(Point{core::antiwedge(l, g)} == point_of(c.meet));
    CHECK(core::get<"1">(meet & g) == rational{0});
    CHECK(Plane{meet ^ l} == Plane{});
  }
}

// Reference: independent value: the meets of lines and planes are the
//   points and lines common to them, and the planes g and h meet in the line
//   g v h with direction h_n x g_n and moment h_w g_n - g_w h_n for normals
//   g_n, h_n and offsets g_w, h_w, which the reference computes for seeded
//   cases by the antiwedge and by these cross products, which agree, so the
//   rga3 Line g & h over the test rational is the reference's line,
//   coefficient for coefficient; independent value: by hand, swapping g and h
//   negates h_n x g_n and h_w g_n - g_w h_n, so h & g is the opposite line
TEST_CASE("the rga3 Line g & h where two planes meet is the reference's "
          "line") {
  for (std::size_t i = 0; i < golden::rigid_meets::plane_plane.size(); ++i) {
    CAPTURE(i);
    const golden::plane_plane_meet& c = golden::rigid_meets::plane_plane.at(i);
    const Plane g = plane_of(c.g);
    const Plane h = plane_of(c.h);
    CHECK(Line{g & h} == line_of(c.meet));
    CHECK(Line{core::antiwedge(g, h)} == line_of(c.meet));
    CHECK(Line{h & g} == -line_of(c.meet));
  }
}

// Reference: independent value: the meets of lines and planes are the
//   points and lines common to them, and three planes meet in the point
//   (g v h) v k with coordinates det[g, h, k, e_j], which the reference
//   computes for seeded cases by the antiwedge and by these determinants,
//   which agree, so the rga3 Point (g & h) & k over the test rational is the
//   reference's point, coefficient for coefficient, and lies on all three
//   planes
TEST_CASE("the rga3 Point (g & h) & k where three planes meet is the "
          "reference's point") {
  for (std::size_t i = 0; i < golden::rigid_meets::three_planes.size(); ++i) {
    CAPTURE(i);
    const golden::three_plane_meet& c = golden::rigid_meets::three_planes.at(i);
    const Plane g = plane_of(c.g);
    const Plane h = plane_of(c.h);
    const Plane k = plane_of(c.k);
    const Point meet{Line{g & h} & k};
    CHECK(meet == point_of(c.meet));
    CHECK(Point{(g & h) & k} == point_of(c.meet));
    CHECK(core::get<"1">(meet & g) == rational{0});
    CHECK(core::get<"1">(meet & h) == rational{0});
    CHECK(core::get<"1">(meet & k) == rational{0});
  }
}

// Reference: independent value: p v g, p ^ l, and l v k are zero exactly
//   when the objects are incident, and p v g = g . p, which the reference
//   computes for seeded cases, every other one a point on the plane by
//   construction, by the antiwedge and by this dot product, which agree, so
//   the scalar of the rga3 antiwedge p & g over the test rational is the
//   reference's value, an exact zero for each point on its plane and not
//   zero for the others
TEST_CASE("the rga3 scalar p & g is zero exactly when the point lies on the "
          "plane") {
  std::size_t incident = 0;
  std::size_t apart = 0;
  for (std::size_t i = 0; i < golden::rigid_incidence::point_plane.size();
       ++i) {
    CAPTURE(i);
    const golden::point_on_plane& c =
        golden::rigid_incidence::point_plane.at(i);
    const rational value = core::get<"1">(point_of(c.p) & plane_of(c.g));
    CHECK(value == scalar(c.value));
    CHECK(core::get<"1">(core::antiwedge(point_of(c.p), plane_of(c.g))) ==
          scalar(c.value));
    if (scalar(c.value) == rational{0}) {
      ++incident;
      CHECK(value == rational{0});
    } else {
      ++apart;
      CHECK(!(value == rational{0}));
    }
  }
  CHECK(incident > 0);
  CHECK(apart > 0);
}

// Reference: independent value: p v g, p ^ l, and l v k are zero exactly
//   when the objects are incident, and p ^ (a ^ b) is the plane through p
//   and the line through a and b, which is zero when the three points lie on
//   one line, which the reference computes for seeded cases, every other one
//   a point on the line by construction, by the wedge and by the rank of the
//   three points, which agree, so the rga3 Plane p ^ (a ^ b) over the test
//   rational is the reference's plane, the zero plane exactly for each point
//   on its line and not zero for the others
TEST_CASE("the rga3 Plane p ^ (a ^ b) is zero exactly when the point lies on "
          "the line") {
  std::size_t incident = 0;
  std::size_t apart = 0;
  for (std::size_t i = 0; i < golden::rigid_incidence::point_line.size(); ++i) {
    CAPTURE(i);
    const golden::point_on_line& c = golden::rigid_incidence::point_line.at(i);
    const Line l{point_of(c.a) ^ point_of(c.b)};
    const Plane join{point_of(c.p) ^ l};
    CHECK(join == plane_of(c.join));
    if (plane_of(c.join) == Plane{}) {
      ++incident;
      CHECK(join == Plane{});
    } else {
      ++apart;
      CHECK(!(join == Plane{}));
    }
  }
  CHECK(incident > 0);
  CHECK(apart > 0);
}

// Reference: independent value: p v g, p ^ l, and l v k are zero exactly
//   when the objects are incident, and (a ^ b) v (c ^ d) = det[a, b, c, d],
//   which is zero when the two lines lie in one plane, which the reference
//   computes for seeded cases, every other one a coplanar pair by
//   construction, by the antiwedge and by this determinant, which agree, so
//   the scalar of the rga3 antiwedge (a ^ b) & (c ^ d) over the test
//   rational is the reference's value, an exact zero for each coplanar pair
//   and not zero for the others
TEST_CASE("the rga3 scalar (a ^ b) & (c ^ d) is zero exactly when the lines "
          "are coplanar") {
  std::size_t incident = 0;
  std::size_t apart = 0;
  for (std::size_t i = 0; i < golden::rigid_incidence::line_line.size(); ++i) {
    CAPTURE(i);
    const golden::line_on_line& c = golden::rigid_incidence::line_line.at(i);
    const Line l{point_of(c.a) ^ point_of(c.b)};
    const Line k{point_of(c.c) ^ point_of(c.d)};
    const rational value = core::get<"1">(l & k);
    CHECK(value == scalar(c.value));
    if (scalar(c.value) == rational{0}) {
      ++incident;
      CHECK(value == rational{0});
    } else {
      ++apart;
      CHECK(!(value == rational{0}));
    }
  }
  CHECK(incident > 0);
  CHECK(apart > 0);
}
