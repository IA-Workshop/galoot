// Tests of how the types of the rga3 preset print with
// galoot/core/format.hpp: std::format("{}", x) writes every component of x
// in the order of its layout, each as the name of its blade, a colon and a
// space, and its coefficient. The rga3 algebra tag names its blades as
// Lengyel's rigid geometric algebra (rigidgeometricalgebra.org) writes them,
// by their generators in the order of their product, so each component of a
// point, line, plane, motor, and flector prints under the name its layout is
// written with: e41, not -e14. A blade the layouts do not hold with that
// orientation, such as the e14 of a join p ^ q, prints by its canonical name,
// e and its indices in increasing order.
//
// The expected texts are written out by hand from those names, never read
// from the preset: a point on e1, e2, e3, e4; a line on e41, e42, e43, e23,
// e31, e12; a plane on e423, e431, e412, e321; a motor on e41, e42, e43,
// e1234, e23, e31, e12, 1; and a flector on e1, e2, e3, e4, e423, e431,
// e412, e321, where e41 = -e14, e42 = -e24, e43 = -e34, e31 = -e13,
// e423 = +e234, e431 = -e134, e412 = +e124, and e321 = -e123.
#include <format>

#include <galoot/core/format.hpp> // IWYU pragma: keep
#include <galoot/presets/rga3.hpp>

#include <doctest/doctest.h>

namespace {

namespace rga3 = galoot::presets::rga3;

} // namespace

// Reference: independent value: a point x e1 + y e2 + z e3 + w e4 is
//   written on e1, e2, e3, e4, the generators themselves
TEST_CASE("an rga3 Point prints on e1, e2, e3, e4") {
  CHECK(std::format("{}", rga3::Point<int>{1, 2, 3, 4}) ==
        "{e1: 1, e2: 2, e3: 3, e4: 4}");
}

// Reference: independent value: in Lengyel's rigid geometric algebra a line
//   has its direction on e41, e42, e43 and its moment on e23, e31, e12, with
//   e41 = -e14, e42 = -e24, e43 = -e34, and e31 = -e13
TEST_CASE("an rga3 Line prints on e41, e42, e43, e23, e31, e12") {
  CHECK(std::format("{}", rga3::Line<int>{1, 2, 3, 4, 5, 6}) ==
        "{e41: 1, e42: 2, e43: 3, e23: 4, e31: 5, e12: 6}");
  CHECK(std::format("{}", rga3::Line<int>{-1, 0, 0, 0, -5, 0}) ==
        "{e41: -1, e42: 0, e43: 0, e23: 0, e31: -5, e12: 0}");
}

// Reference: independent value: in Lengyel's rigid geometric algebra a plane
//   has its normal on e423, e431, e412 and its offset on e321, with
//   e423 = +e234, e431 = -e134, e412 = +e124, and e321 = -e123
TEST_CASE("an rga3 Plane prints on e423, e431, e412, e321") {
  CHECK(std::format("{}", rga3::Plane<int>{1, 2, 3, 4}) ==
        "{e423: 1, e431: 2, e412: 3, e321: 4}");
}

// Reference: independent value: Lengyel's Motor page stores Q_vx, Q_vy,
//   Q_vz, Q_vw on e41, e42, e43, e1234 and Q_mx, Q_my, Q_mz, Q_mw on e23,
//   e31, e12, 1
TEST_CASE("an rga3 Motor prints on e41, e42, e43, e1234, e23, e31, e12, 1") {
  CHECK(std::format("{}", rga3::Motor<int>{1, 2, 3, 4, 5, 6, 7, 8}) ==
        "{e41: 1, e42: 2, e43: 3, e1234: 4, e23: 5, e31: 6, e12: 7, 1: 8}");
}

// Reference: independent value: the translator by d = (1, 2, 3) holds half
//   of d on e23, e31, e12 and 1 on e1234, and 0.5, 1, 1.5 are exact in
//   double
TEST_CASE("an rga3 translation prints its half distances on e23, e31, e12") {
  CHECK(std::format("{}", rga3::translation(1.0, 2.0, 3.0)) ==
        "{e41: 0, e42: 0, e43: 0, e1234: 1, e23: 0.5, e31: 1, e12: 1.5, 1: 0}");
  CHECK(std::format("{:.2f}", rga3::translation(1.0, 2.0, 3.0)) ==
        "{e41: 0.00, e42: 0.00, e43: 0.00, e1234: 1.00, e23: 0.50, "
        "e31: 1.00, e12: 1.50, 1: 0.00}");
}

// Reference: independent value: Lengyel's Flector page stores F_px, F_py,
//   F_pz, F_pw on e1, e2, e3, e4 and F_gx, F_gy, F_gz, F_gw on e423, e431,
//   e412, e321
TEST_CASE("an rga3 Flector prints on e1, e2, e3, e4, e423, e431, e412, "
          "e321") {
  CHECK(std::format("{}", rga3::Flector<int>{1, 2, 3, 4, 5, 6, 7, 8}) ==
        "{e1: 1, e2: 2, e3: 3, e4: 4, e423: 5, e431: 6, e412: 7, e321: 8}");
}

// Reference: independent value: for p = e1 + e4 and q = e1 + 2 e2 + e4,
//   p ^ q = 2 e12 - 2 e24, on the blades e12, e13, e14, e23, e24, e34 in
//   increasing order, each with orientation +1, and as a line it has
//   direction q - p = (0, 2, 0) and moment p x q = (0, 0, 2)
TEST_CASE("an rga3 join prints by canonical names until it is converted to "
          "a Line") {
  constexpr rga3::Point<int> p{1, 0, 0, 1};
  constexpr rga3::Point<int> q{1, 2, 0, 1};
  CHECK(std::format("{}", p ^ q) ==
        "{e12: 2, e13: 0, e14: 0, e23: 0, e24: -2, e34: 0}");
  CHECK(std::format("{}", rga3::Line<int>{p ^ q}) ==
        "{e41: 0, e42: 2, e43: 0, e23: 0, e31: 0, e12: 2}");
}
