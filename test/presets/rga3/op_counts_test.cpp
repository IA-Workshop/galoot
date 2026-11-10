// Tests of the arithmetic that the rga3 operations perform: the join of two
// points and of three, the meet of a line and a plane, the transforms of a
// point, a line, and a plane by a motor, and the composition of two motors,
// each run once over a scalar that counts its operations, against the counts
// that the reference derives from the terms of each operation.
//
// The counts are a check of structure, not of speed: each generated term of
// an operation is one product of an input coefficient of each operand with a
// sign of +1 or -1 and an output place. The first term on a place assigns
// its product, negated when its sign is -1, and each later one adds or
// subtracts it, so an extra term, a term kept with a zero coefficient, or a
// sign multiplied in rather than folded changes a count. The terms come in
// the order of the first operand's place and then the second's, which fixes
// which term is first on each place and so the negations.
//
// The program is built twice, once with checking on and once with it off.
// With checking on, a transform over an exact scalar with == computes every
// term of its first stage and the terms that land on blades its result
// drops, and compares each dropped coefficient with zero; with it off it
// computes only the terms its result needs. Each build checks the counts of
// its own mode.
//
// A point (x, y, z, w) is stored on e1, e2, e3, e4, a line on e41, e42,
// e43, e23, e31, e12, a plane on e423, e431, e412, e321, and a motor on e41,
// e42, e43, e1234, e23, e31, e12, 1, the orders of Lengyel's rigid geometric
// algebra (rigidgeometricalgebra.org). The joins, the meet, and the
// composition are on the canonical blades of their results, with orientation
// +1, as the operators return them.
#include <cstdint>

#include <galoot/presets/rga3.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>
#include <support/counting.hpp>

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using test_support::counting;
using Point = rga3::Point<counting>;
using Line = rga3::Line<counting>;
using Plane = rga3::Plane<counting>;
using Motor = rga3::Motor<counting>;

constexpr bool checking = GALOOT_CHECKING == 1;

counting c(std::int64_t v) { return counting{v}; }

// Inputs with no zero coefficient, so that no count could depend on one;
// a count does not depend on the values at all, since every operation is
// straight-line code.
Point point_a() { return {c(1), c(2), c(3), c(1)}; }
Point point_b() { return {c(-2), c(5), c(1), c(2)}; }
Point point_c() { return {c(3), c(-1), c(4), c(1)}; }
Line line_a() { return {c(1), c(-2), c(3), c(4), c(5), c(-6)}; }
Plane plane_a() { return {c(2), c(-3), c(1), c(5)}; }
Motor motor_a() { return {c(1), c(2), c(-1), c(3), c(2), c(1), c(-2), c(4)}; }
Motor motor_b() { return {c(-3), c(1), c(2), c(1), c(1), c(-1), c(3), c(2)}; }

// The counts seen against the reference's counts for one operation: its
// multiplications; its additions and subtractions together; its negations;
// its comparisons, made only by the check of a transform with checking on;
// and its constructions of a scalar from an integer, one for each term
// whose coefficient is other than +1 or -1, of which the rga3 operations
// have none. The default constructions, a zero for each place of each
// result, are shown on failure and not checked.
void check_counts(const test_support::operation_counts& seen,
                  const golden::operation_count& expected) {
  CAPTURE(seen.default_constructions);
  CHECK(seen.multiplications == expected.multiplications);
  CHECK(seen.additions + seen.subtractions == expected.additions);
  CHECK(seen.negations == expected.negations);
  CHECK(seen.comparisons == expected.comparisons);
  CHECK(seen.integer_constructions == expected.integer_constants);
}

} // namespace

// Reference: independent value: each term is one product and each later
//   term on a place one addition, or a subtraction for a term of sign -1,
//   and a first term of sign -1 is one negation, and the reference lists the terms of
//   p ^ q from the wedge of the blades of two points, 12 terms on 6 places
TEST_CASE("p ^ q of two rga3 points makes the reference's operations") {
  const Point p = point_a();
  const Point q = point_b();
  test_support::reset_counts();
  [[maybe_unused]] const auto l = p ^ q;
  const test_support::operation_counts seen = test_support::counts();
  check_counts(seen, checking ? golden::operation_counts::point_join.checking
                              : golden::operation_counts::point_join.unchecked);
}

// Reference: independent value: each term is one product and each later
//   term on a place one addition, or a subtraction for a term of sign -1,
//   and a first term of sign -1 is one negation, and the reference lists the terms of
//   p ^ q and of its wedge with r from the wedge of the blades, 12 and 12
TEST_CASE("(p ^ q) ^ r of three rga3 points makes the reference's "
          "operations") {
  const Point p = point_a();
  const Point q = point_b();
  const Point r = point_c();
  test_support::reset_counts();
  [[maybe_unused]] const auto g = (p ^ q) ^ r;
  const test_support::operation_counts seen = test_support::counts();
  check_counts(seen, checking ? golden::operation_counts::plane_join.checking
                              : golden::operation_counts::plane_join.unchecked);
}

// Reference: independent value: each term is one product and each later
//   term on a place one addition, or a subtraction for a term of sign -1,
//   and a first term of sign -1 is one negation, and the reference lists the terms of
//   l & g from the antiwedge of the blades of a line and a plane, 12 terms on
//   4 places
TEST_CASE("l & g of an rga3 line and plane makes the reference's "
          "operations") {
  const Line l = line_a();
  const Plane g = plane_a();
  test_support::reset_counts();
  [[maybe_unused]] const auto p = l & g;
  const test_support::operation_counts seen = test_support::counts();
  check_counts(seen,
               checking ? golden::operation_counts::line_plane_meet.checking
                        : golden::operation_counts::line_plane_meet.unchecked);
}

// Reference: independent value: each term is one product and each later
//   term on a place one addition, or a subtraction for a term of sign -1,
//   and a first term of sign -1 is one negation, and the reference lists the terms of
//   Q ⟇ x for a point x restricted to the places that the second stage reads
//   (all of them with checking on), the terms of that times antireverse(Q)
//   on the point's blades, and with checking on those on the other blades,
//   each of which is compared with zero once
TEST_CASE("transform(p, Q) of an rga3 point by a motor makes the "
          "reference's operations") {
  const Point p = point_a();
  const Motor q = motor_a();
  test_support::reset_counts();
  [[maybe_unused]] const Point moved = core::transform(p, q);
  const test_support::operation_counts seen = test_support::counts();
  check_counts(seen,
               checking ? golden::operation_counts::point_transform.checking
                        : golden::operation_counts::point_transform.unchecked);
}

// Reference: independent value: each term is one product and each later
//   term on a place one addition, or a subtraction for a term of sign -1,
//   and a first term of sign -1 is one negation, and the reference lists the terms of
//   Q ⟇ x for a line x restricted to the places that the second stage reads
//   (all of them with checking on), the terms of that times antireverse(Q)
//   on the line's blades, and with checking on those on the other blades,
//   each of which is compared with zero once
TEST_CASE("transform(l, Q) of an rga3 line by a motor makes the "
          "reference's operations") {
  const Line l = line_a();
  const Motor q = motor_a();
  test_support::reset_counts();
  [[maybe_unused]] const Line moved = core::transform(l, q);
  const test_support::operation_counts seen = test_support::counts();
  check_counts(seen,
               checking ? golden::operation_counts::line_transform.checking
                        : golden::operation_counts::line_transform.unchecked);
}

// Reference: independent value: each term is one product and each later
//   term on a place one addition, or a subtraction for a term of sign -1,
//   and a first term of sign -1 is one negation, and the reference lists the terms of
//   Q ⟇ x for a plane x restricted to the places that the second stage reads
//   (all of them with checking on), the terms of that times antireverse(Q)
//   on the plane's blades, and with checking on those on the other blades,
//   each of which is compared with zero once
TEST_CASE("transform(g, Q) of an rga3 plane by a motor makes the "
          "reference's operations") {
  const Plane g = plane_a();
  const Motor q = motor_a();
  test_support::reset_counts();
  [[maybe_unused]] const Plane moved = core::transform(g, q);
  const test_support::operation_counts seen = test_support::counts();
  check_counts(seen,
               checking ? golden::operation_counts::plane_transform.checking
                        : golden::operation_counts::plane_transform.unchecked);
}

// Reference: independent value: each term is one product and each later
//   term on a place one addition, or a subtraction for a term of sign -1,
//   and a first term of sign -1 is one negation, and the reference lists the terms of
//   Q2 ⟇ Q1 from the geometric antiproduct of the blades of two motors, 48
//   terms on 8 places
TEST_CASE("compose(Q2, Q1) of two rga3 motors makes the reference's "
          "operations") {
  const Motor q2 = motor_a();
  const Motor q1 = motor_b();
  test_support::reset_counts();
  [[maybe_unused]] const auto q = core::compose(q2, q1);
  const test_support::operation_counts seen = test_support::counts();
  check_counts(seen,
               checking ? golden::operation_counts::motor_compose.checking
                        : golden::operation_counts::motor_compose.unchecked);
}
