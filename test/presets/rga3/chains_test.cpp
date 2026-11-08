// Tests of the rga3 preset against chains of joint motions: each joint is a
// turn about z, a move along z, a move along x, and a turn about x, built
// as rga3 Motors and composed, and a chain of three to six joints moves
// points as the product of the joints' 4 by 4 matrices, exactly over the
// test rational.
//
// A motor of rigid(3) is stored on e41, e42, e43, e1234, e23, e31, e12, 1,
// the order of Lengyel's Motor page (rigidgeometricalgebra.org), and moves x
// to Q ⟇ x ⟇ antireverse(Q). The rotor s e43 + c e1234 with s = sin(phi)
// and c = cos(phi) turns by 2 phi about z, and s e41 + c e1234 about x;
// translation(dx, dy, dz) moves by (dx, dy, dz). compose(Q2, Q1) applies Q1
// first and then Q2, as the matrix product M2 M1 does on columns.
#include <array>
#include <cstddef>

#include <galoot/presets/rga3.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using test_support::rational;
using Point = rga3::Point<rational>;
using Motor = rga3::Motor<rational>;

rational scalar(const golden::rational& v) {
  return rational{v.numerator, v.denominator};
}

Point point_of(const golden::point& p) {
  return {scalar(p[0]), scalar(p[1]), scalar(p[2]), scalar(p[3])};
}

// The point x moved by the rigid motion [R | t]: R (x, y, z) + w t, with
// its weight w unchanged, computed from the matrix's rows.
Point moved_by(const golden::affine& m, const Point& x) {
  const auto& c = x.coefficients;
  std::array<rational, 3> out{};
  for (std::size_t i = 0; i < 3; ++i) {
    const auto& row = m.at(i);
    out.at(i) = scalar(row[0]) * c[0] + scalar(row[1]) * c[1] +
                scalar(row[2]) * c[2] + scalar(row[3]) * c[3];
  }
  return {out[0], out[1], out[2], c[3]};
}

// The motor of the joint Rz(theta) Tz(d) Tx(a) Rx(alpha), which applies the
// turn about x first and the turn about z last, each angle given by the
// sine and cosine of its half.
Motor joint_motor(const golden::joint& j) {
  const rational o{0};
  const Motor rz{o, o, scalar(j.theta_sin), scalar(j.theta_cos), o, o, o, o};
  const Motor tz = rga3::translation(o, o, scalar(j.d));
  const Motor tx = rga3::translation(scalar(j.a), o, o);
  const Motor rx{scalar(j.alpha_sin), o, o, scalar(j.alpha_cos), o, o, o, o};
  return Motor{core::compose(
      rz, Motor{core::compose(tz, Motor{core::compose(tx, rx)})})};
}

} // namespace

// Reference: independent value: a chain of joint motors moves points as its
//   product of 4x4 matrices, which the reference computes for seeded chains
//   of three, four, five, and six joints, each Rz(theta) Tz(d) Tx(a)
//   Rx(alpha) with rational half-angle sines and cosines, the first joint
//   leftmost, and applies to ten points, one of weight 2 and one a
//   direction of weight 0; independent value: by hand, rotors and translators
//   of unit norm compose to a motor of unit norm, so the rga3 Motor composed
//   from the joints, the rotors written from their half-angle sines and cosines
//   and the moves built by translation(dx, dy, dz), moves each point to the
//   reference's image and as the reference's [R | t] does, R (x, y, z) + w t
//   with the weight w kept, and Q ⟇ antireverse(Q) is the antiscalar
TEST_CASE("an rga3 Motor composed from a chain of joints moves points as its "
          "product of 4x4 matrices") {
  std::size_t smallest = golden::joint_chains::chains.at(0).size;
  std::size_t largest = smallest;
  for (std::size_t i = 0; i < golden::joint_chains::chains.size(); ++i) {
    CAPTURE(i);
    const golden::joint_chain& chain = golden::joint_chains::chains.at(i);
    CAPTURE(chain.size);
    REQUIRE(chain.size >= 1);
    REQUIRE(chain.size <= chain.joints.size());
    smallest = chain.size < smallest ? chain.size : smallest;
    largest = chain.size > largest ? chain.size : largest;

    Motor motor = joint_motor(chain.joints.at(0));
    for (std::size_t k = 1; k < chain.size; ++k) {
      motor = Motor{core::compose(motor, joint_motor(chain.joints.at(k)))};
    }

    const auto unit =
        core::geometric_antiproduct(motor, core::antireverse(motor));
    CHECK(core::get<"e1234">(unit) == rational{1});
    std::size_t nonzero = 0;
    for (const rational& c : unit.coefficients) {
      if (!(c == rational{0})) {
        ++nonzero;
      }
    }
    CHECK(nonzero == 1);

    for (std::size_t k = 0; k < golden::joint_chains::points.size(); ++k) {
      CAPTURE(k);
      const Point x = point_of(golden::joint_chains::points.at(k));
      const Point image = core::transform(x, motor);
      CHECK(image == point_of(chain.images.at(k)));
      CHECK(image == moved_by(chain.motion, x));
    }
  }
  CHECK(golden::joint_chains::chains.size() >= 3);
  CHECK(smallest == 3);
  CHECK(largest == 6);
}
