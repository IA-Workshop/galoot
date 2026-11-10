// Tests of the arithmetic of transform in a form where its first stage is
// restricted: the degenerate form of three generators with e1 . e2 = 1 and
// every other product of generators 0. There, for V on e1, e2, e3, e123 and
// x on e123, the second stage reads only some of the middle blades, so with
// checking off the first stage computes only the terms on them. In rigid(3)
// the second stage of a motor on a point, a line, or a plane reads every
// middle blade, so the operation counts of the rga3 transforms cannot tell
// whether the restriction is made; this one can.
//
// The program is built twice, once with checking on and once with it off,
// and each build checks the counts of its own mode against the reference's,
// as for the rga3 operations: each term one multiplication, each later term
// on a place one addition or subtraction, a first term of sign -1 one
// negation, and with checking on the whole first stage, the terms on the
// blades x lacks, and one comparison for each of those blades.
#include <cstdint>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/transform.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>
#include <support/counting.hpp>

namespace {

namespace core = galoot::core;
using test_support::counting;

struct degenerate3 {
  static constexpr auto value = core::from_gram({
      {0, 1, 0},
      {1, 0, 0},
      {0, 0, 0},
  });
};

using Versor =
    core::mv<degenerate3, core::blades<degenerate3, "e1 e2 e3 e123">, counting>;
using Antiscalar =
    core::mv<degenerate3, core::blades<degenerate3, "e123">, counting>;

constexpr bool checking = GALOOT_CHECKING == 1;

counting c(std::int64_t v) { return counting{v}; }

// The counts seen against the reference's, as in the rga3 counting tests.
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
//   and a first term of sign -1 is one negation, and the reference lists the
//   terms of V ⟇ x in the degenerate form with e1 . e2 = 1 for V on e1, e2,
//   e3, e123 and x on e123, restricted with checking off to the middle blades
//   that the terms of the second stage on e123 read, and the terms of that
//   times antireverse(V), on e123 and with checking on on the other blades
TEST_CASE("transform(x, V) in a degenerate form makes the reference's "
          "operations") {
  // V is the antiscalar, the identity of the antiproduct, so what the
  // transform drops is zero and the check with checking on passes. The
  // values do not change what is computed: the code is straight-line.
  const Versor v{c(0), c(0), c(0), c(1)};
  const Antiscalar x{c(1)};
  test_support::reset_counts();
  [[maybe_unused]] const Antiscalar moved = core::transform(x, v);
  const test_support::operation_counts seen = test_support::counts();
  check_counts(seen,
               checking
                   ? golden::operation_counts::degenerate_transform.checking
                   : golden::operation_counts::degenerate_transform.unchecked);
}
