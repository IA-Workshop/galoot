// Tests of the rga3 preset against the 32 crystallographic point groups: each
// group is generated at run time from its rational generators by compose,
// over the test rational, and its elements are counted and matched one to
// one with the reference's 3 by 3 matrices by how they move points.
//
// A motor of rigid(3) is stored on e41, e42, e43, e1234, e23, e31, e12, 1
// and a flector on e1, e2, e3, e4, e423, e431, e412, e321, the orders of
// Lengyel's Motor and Flector pages (rigidgeometricalgebra.org). compose of
// two motors or two flectors is a motor, and of a motor and a flector, in
// either order, a flector. Versors that differ by a nonzero factor perform
// one motion, so each element found is divided by its first nonzero
// coefficient, taking the blades in order of mask, before it is compared
// with those found already. The three- and sixfold axes of the trigonal and
// hexagonal groups lie along (1, 1, 1), where every generator has rational
// coefficients.
#include <array>
#include <cstddef>
#include <format>
#include <vector>

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
using Flector = rga3::Flector<rational>;

// A versor of rigid(3): a Motor or, when odd is true, a Flector, by its
// eight coefficients in the order of its layout.
struct element {
  bool odd = false;
  std::array<rational, 8> coefficients{};

  bool operator==(const element&) const = default;
};

rational scalar(const golden::rational& v) {
  return rational{v.numerator, v.denominator};
}

element element_of(const golden::versor& v) {
  element e{.odd = v.odd, .coefficients = {}};
  for (std::size_t i = 0; i < e.coefficients.size(); ++i) {
    e.coefficients.at(i) = scalar(v.coefficients.at(i));
  }
  return e;
}

Motor motor_of(const element& e) {
  const auto& c = e.coefficients;
  return {c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]};
}

Flector flector_of(const element& e) {
  const auto& c = e.coefficients;
  return {c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]};
}

// compose(x, g), the versor that applies g first and then x, through one of
// the four compositions of a motor and a flector.
element composed(const element& x, const element& g) {
  if (!x.odd && !g.odd) {
    const Motor y{core::compose(motor_of(x), motor_of(g))};
    return {.odd = false, .coefficients = y.coefficients};
  }
  if (!x.odd && g.odd) {
    const Flector y{core::compose(motor_of(x), flector_of(g))};
    return {.odd = true, .coefficients = y.coefficients};
  }
  if (x.odd && !g.odd) {
    const Flector y{core::compose(flector_of(x), motor_of(g))};
    return {.odd = true, .coefficients = y.coefficients};
  }
  const Motor y{core::compose(flector_of(x), flector_of(g))};
  return {.odd = false, .coefficients = y.coefficients};
}

// e divided by its first nonzero coefficient on the canonical blades, taken
// in order of mask, so that the versors of one motion, which differ by a
// nonzero factor, become one. A stored coefficient on a blade of sign -1,
// such as e41 = -e14, is minus the coefficient on the canonical blade.
element normalized(const element& e) {
  const auto& blades =
      e.odd ? rga3::flector_layout.blades : rga3::motor_layout.blades;
  for (core::mask_t mask = 0; mask < 16; ++mask) {
    for (std::size_t i = 0; i < blades.size(); ++i) {
      const rational& c = e.coefficients.at(i);
      if (blades.at(i).mask == mask && !(c == rational{0})) {
        const rational divisor = c * rational{blades.at(i).sign};
        element out{.odd = e.odd, .coefficients = {}};
        for (std::size_t j = 0; j < out.coefficients.size(); ++j) {
          out.coefficients.at(j) = e.coefficients.at(j) / divisor;
        }
        return out;
      }
    }
  }
  return e;
}

// Every product of the generators under compose, each normalized, from the
// identity motor, the antiscalar. A closure that grows past twice the
// largest crystallographic order, 48, stops there, so a wrong product
// cannot run on.
std::vector<element> closure(const std::vector<element>& generators) {
  element identity{};
  identity.coefficients.at(3) = rational{1};
  std::vector<element> found{normalized(identity)};
  for (std::size_t next = 0; next < found.size() && found.size() <= 96;
       ++next) {
    for (const element& g : generators) {
      const element y = normalized(composed(found.at(next), g));
      bool known = false;
      for (const element& f : found) {
        known = known || f == y;
      }
      if (!known) {
        found.push_back(y);
      }
    }
  }
  return found;
}

// The point x moved by e.
Point moved(const Point& x, const element& e) {
  return e.odd ? core::transform(x, flector_of(e))
               : core::transform(x, motor_of(e));
}

// Whether e moves each of the points (1, 0, 0), (0, 1, 0), (0, 0, 1), and
// (1, 2, 3), of weight 1, to the point m (x, y, z): its image (X, Y, Z, W),
// whose weight W is the factor by which e differs from a unitized versor,
// has (X, Y, Z) = W m (x, y, z), with W positive.
bool acts_as(const element& e, const golden::matrix3& m) {
  const rational o{0};
  const rational one{1};
  const std::array<Point, 4> points{
      {
          {one, o, o, one},
          {o, one, o, one},
          {o, o, one, one},
          {one, rational{2}, rational{3}, one},
      },
  };
  for (const Point& p : points) {
    const Point image = moved(p, e);
    const auto& x = p.coefficients;
    const auto& y = image.coefficients;
    if (!(y[3].numerator > 0)) {
      return false;
    }
    for (std::size_t i = 0; i < 3; ++i) {
      const auto& row = m.at(i);
      const rational expected =
          scalar(row[0]) * x[0] + scalar(row[1]) * x[1] + scalar(row[2]) * x[2];
      if (!(y[i] == y[3] * expected)) {
        return false;
      }
    }
  }
  return true;
}

} // namespace

// Reference: independent value: the 32 crystal classes close under compose
//   with their published orders, which the reference transcribes from a
//   published table of the crystallographic point groups: 1 for 1, 2 for
//   -1, 2, and m, 4 for 2/m, 222, mm2, 4, and -4, 8 for mmm, 4/m, 422, 4mm,
//   and -42m, 16 for 4/mmm, 3 for 3, 6 for -3, 32, 3m, 6, and -6, 12 for
//   -3m, 6/m, 622, 6mm, -6m2, and 23, 24 for 6/mmm, m-3, 432, and -43m, and
//   48 for m-3m, and whose elements it computes as rational 3 by 3 matrices
//   closed under multiplication, from Rodrigues' formula for the turns,
//   I - 2 n n^T / |n|^2 for the mirrors, and -I for the inversion, with the
//   fourfold axes along z and the three- and sixfold axes along (1, 1, 1),
//   so that the closure of each group's rga3 generators under compose, over
//   the test rational, each element divided by its first nonzero
//   coefficient in order of mask, has that order, and its elements move the
//   points (1, 0, 0), (0, 1, 0), (0, 0, 1), and (1, 2, 3) as the reference's
//   matrices do, up to the positive weight a versor that is not unitized
//   gives, one element to each matrix
TEST_CASE("the closure of each crystal class's rga3 generators under compose "
          "has its published order and acts as its matrices") {
  REQUIRE(golden::point_groups::groups.size() == 32);
  std::size_t total = 0;
  for (std::size_t g = 0; g < golden::point_groups::groups.size(); ++g) {
    const golden::point_group& group = golden::point_groups::groups.at(g);
    INFO(std::format("group {}, {}", g, group.name));
    total += group.order;
    REQUIRE(group.first + group.order <= golden::point_groups::elements.size());

    std::vector<element> generators;
    generators.reserve(group.generator_count);
    for (std::size_t k = 0; k < group.generator_count; ++k) {
      generators.push_back(element_of(group.generators.at(k)));
    }
    const std::vector<element> found = closure(generators);
    CHECK(found.size() == group.order);

    std::vector<std::size_t> hits(group.order, 0);
    for (std::size_t e = 0; e < found.size(); ++e) {
      CAPTURE(e);
      std::size_t matches = 0;
      for (std::size_t j = 0; j < group.order; ++j) {
        if (acts_as(found.at(e),
                    golden::point_groups::elements.at(group.first + j))) {
          ++matches;
          ++hits.at(j);
        }
      }
      CHECK(matches == 1);
    }
    for (std::size_t j = 0; j < hits.size(); ++j) {
      CAPTURE(j);
      CHECK(hits.at(j) == 1);
    }
  }
  CHECK(total == golden::point_groups::elements.size());
}
