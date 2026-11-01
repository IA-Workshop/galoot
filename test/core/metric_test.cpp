// Tests of the signature as data: the factories R, rigid, conformal, and
// from_gram, the accessors of metric, and the concept metric_like.
//
// Expected matrices come from the golden values of test/reference/golden.hpp,
// computed apart from the library, or are written out entry by entry from the
// definition where the golden values do not reach. Generators are counted
// from e1, and index i of a matrix belongs to e_(i+1).
#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

#include <galoot/core/metric.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>

namespace {

namespace core = galoot::core;

template <std::size_t N>
using matrix = std::array<std::array<int, N>, N>;

// A matrix written out row by row.
template <std::size_t N>
// NOLINTNEXTLINE(*-avoid-c-arrays): a C array lets N be deduced from braces.
constexpr matrix<N> written(const int (&rows)[N][N]) noexcept {
  matrix<N> m{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      m[i][j] = rows[i][j];
    }
  }
  return m;
}

// The number of entries of m that differ from the diagonal form with the
// given squares. Every entry off the diagonal is expected to be 0, because
// distinct generators of a diagonal signature are orthogonal.
template <std::size_t N>
constexpr int diagonal_mismatches(const core::metric<N>& m,
                                  const std::array<int, N>& squares) {
  int mismatches = 0;
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      const int expected = i == j ? squares[i] : 0;
      mismatches += m.gram[i][j] == expected ? 0 : 1;
    }
  }
  return mismatches;
}

template <auto Table, std::size_t... I>
consteval int r_table_mismatches(std::index_sequence<I...> /*rows*/) {
  return (diagonal_mismatches(core::R<Table[I].p, Table[I].q, Table[I].r>(),
                              Table[I].squares) +
          ... + 0);
}

// The mismatches of R(p, q, r) against every signature of Table, a list of
// (p, q, r) with the squares of its generators.
template <auto Table>
consteval int r_mismatches() {
  return r_table_mismatches<Table>(std::make_index_sequence<Table.size()>());
}

// A coordinate of a change of basis, as a fraction.
struct fraction {
  long long numerator;
  long long denominator;
};

// The number of entries where the form of `diagonal`, carried to the basis
// whose row i holds generator i of `g` in the generators of `diagonal`,
// differs from the Gram matrix of `g`. Each coordinate is doubled, so that
// the halves of the origin become integers, and the doubled form is compared
// with 4 G; a coordinate whose double is not an integer is a mismatch.
template <std::size_t N>
constexpr int change_of_basis_mismatches(const auto& in_diagonal,
                                         const core::metric<N>& diagonal,
                                         const core::metric<N>& g) {
  int mismatches = 0;
  std::array<std::array<long long, N>, N> doubled{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t k = 0; k < N; ++k) {
      const long long twice = 2 * in_diagonal[i][k].numerator;
      const long long denominator = in_diagonal[i][k].denominator;
      mismatches += twice % denominator == 0 ? 0 : 1;
      doubled[i][k] = twice / denominator;
    }
  }
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      long long product = 0;
      for (std::size_t k = 0; k < N; ++k) {
        product += doubled[i][k] * diagonal.gram[k][k] * doubled[j][k];
      }
      mismatches += product == 4LL * g.gram[i][j] ? 0 : 1;
    }
  }
  return mismatches;
}

// The most generators a blade mask can hold.
constexpr std::size_t mask_width = std::numeric_limits<core::mask_t>::digits;

// Tags for the concept. Each names an algebra and carries its metric.
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
struct above_default {
  static constexpr auto value =
      core::R<core::max_generators + 1, 0, 0, core::max_generators + 1>();
};
struct widest {
  static constexpr auto value = core::R<mask_width, 0, 0, mask_width>();
};
struct no_value {};
struct int_value {
  static constexpr int value = 4;
};
struct asymmetric {
  static constexpr core::metric<2> value{written({{0, 1}, {2, 0}})};
};
struct entry_too_large {
  static constexpr core::metric<1> value{written({{core::max_form_entry + 1}})};
};
struct no_generators {
  static constexpr core::metric<0> value{};
};
// A static value that is not constexpr cannot be read at compile time.
struct runtime_value {
  static const core::metric<2> value;
};
const core::metric<2> runtime_value::value{written({{1, 0}, {0, 1}})};
struct member_value {
  core::metric<2> value{written({{1, 0}, {0, 1}})};
};
// Has the members a metric has, but is not a metric. metric_like rejects it
// before reading generator_count.
struct lookalike_metric {
  std::array<std::array<int, 2>, 2> gram;
  [[maybe_unused]] static constexpr std::size_t generator_count = 2;
  [[nodiscard]] constexpr bool is_symmetric() const {
    return gram[0][1] == gram[1][0];
  }
  [[nodiscard]] constexpr bool entries_in_range() const {
    return gram[0][0] <= core::max_form_entry;
  }
};
struct lookalike {
  static constexpr lookalike_metric value{written({{1, 0}, {0, 1}})};
};

// metric_like accepts a tag that carries a valid metric of any n up to the
// width of a blade mask, whatever bound its factory used, and is false for
// every other type: no value, a value that is not a metric or only looks like
// one, a value not usable at compile time, an asymmetric form, an entry beyond
// max_form_entry, and n = 0.
static_assert(core::metric_like<rga3>);
static_assert(core::metric_like<cga3>);
static_assert(core::metric_like<above_default>);
static_assert(core::metric_like<widest>);
static_assert(!core::metric_like<no_value>);
static_assert(!core::metric_like<int_value>);
static_assert(!core::metric_like<int>);
static_assert(!core::metric_like<core::metric<4>>);
static_assert(!core::metric_like<lookalike>);
static_assert(!core::metric_like<runtime_value>);
static_assert(!core::metric_like<member_value>);
static_assert(!core::metric_like<asymmetric>);
static_assert(!core::metric_like<entry_too_large>);
static_assert(!core::metric_like<no_generators>);

// A metric is a structural, trivially copyable aggregate, so it can be a
// template argument and copied as plain data.
template <core::metric<4> M>
constexpr int first_square = M.gram[0][0];
static_assert(first_square<core::rigid<3>()> == 1);
static_assert(std::is_trivially_copyable_v<core::metric<5>>);
static_assert(std::is_aggregate_v<core::metric<5>>);

} // namespace

// Reference: independent value: R(p, q, r) squares p generators to +1, q to
// -1, and r to 0, for every R(p, q, r) with n from 1 to 6; proof: distinct
// generators of a diagonal signature are orthogonal
TEST_CASE("R(p, q, z) squares the first p generators to +1, the next q to -1, "
          "and the last z to 0, with no entry off the diagonal, for every "
          "split with n from 1 to 6") {
  CHECK(r_mismatches<golden::diagonal_signatures::n1>() == 0);
  CHECK(r_mismatches<golden::diagonal_signatures::n2>() == 0);
  CHECK(r_mismatches<golden::diagonal_signatures::n3>() == 0);
  CHECK(r_mismatches<golden::diagonal_signatures::n4>() == 0);
  CHECK(r_mismatches<golden::diagonal_signatures::n5>() == 0);
  CHECK(r_mismatches<golden::diagonal_signatures::n6>() == 0);
}

// Reference: independent value: R(p, q, r) squares p generators to +1, q to
// -1, and r to 0, written out for R(1, 1, 1), R(2, 2, 2), and R(4, 1, 0)
TEST_CASE("R(1, 1, 1), R(2, 2, 2), and R(4, 1, 0) equal their matrices "
          "written out from the definition of R(p, q, z)") {
  constexpr auto r111 = written({{1, 0, 0}, {0, -1, 0}, {0, 0, 0}});
  constexpr auto r222 = written({
      {1, 0, 0, 0, 0, 0},
      {0, 1, 0, 0, 0, 0},
      {0, 0, -1, 0, 0, 0},
      {0, 0, 0, -1, 0, 0},
      {0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0},
  });
  constexpr auto r410 = written({
      {1, 0, 0, 0, 0},
      {0, 1, 0, 0, 0},
      {0, 0, 1, 0, 0},
      {0, 0, 0, 1, 0},
      {0, 0, 0, 0, -1},
  });
  CHECK(core::R<1, 1, 1>().gram == r111);
  CHECK(core::R<2, 2, 2>().gram == r222);
  CHECK(core::R<4, 1, 0>().gram == r410);
}

// Reference: independent value: rigid(d) is R(d, 0, 1), with the squares of
// its generators for d from 1 to 5; proof: rigid(0) is the single null
// generator of R(0, 0, 1), and rigid(3) is written out from the definition
TEST_CASE("rigid(d) is R(d, 0, 1): e1 to ed Euclidean and the projective "
          "generator e(d+1) null, for d from 0 to 5") {
  CHECK(diagonal_mismatches(core::rigid<0>(), std::array<int, 1>{0}) == 0);
  CHECK(diagonal_mismatches(core::rigid<1>(), golden::rigid::d1_squares) == 0);
  CHECK(diagonal_mismatches(core::rigid<2>(), golden::rigid::d2_squares) == 0);
  CHECK(diagonal_mismatches(core::rigid<3>(), golden::rigid::d3_squares) == 0);
  CHECK(diagonal_mismatches(core::rigid<4>(), golden::rigid::d4_squares) == 0);
  CHECK(diagonal_mismatches(core::rigid<5>(), golden::rigid::d5_squares) == 0);

  constexpr auto rigid3 = written({
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 0},
  });
  CHECK(core::rigid<3>().gram == rigid3);
  CHECK(core::rigid<3>() == core::R<3, 0, 1>());
}

// Reference: independent value: conformal(d) has null e(d+1) and e(d+2) with
// e(d+1) . e(d+2) = -1, as Gram matrices for d from 1 to 4; proof:
// conformal(0) is written out from the definition
TEST_CASE("conformal(d) has e1 to ed Euclidean and e(d+1), e(d+2) null with "
          "e(d+1) . e(d+2) = -1, for d from 0 to 4") {
  constexpr auto conformal0 = written({{0, -1}, {-1, 0}});
  CHECK(core::conformal<0>().gram == conformal0);
  CHECK(core::conformal<1>().gram == golden::conformal::d1_gram);
  CHECK(core::conformal<2>().gram == golden::conformal::d2_gram);
  CHECK(core::conformal<3>().gram == golden::conformal::d3_gram);
  CHECK(core::conformal<4>().gram == golden::conformal::d4_gram);
}

// Reference: proof: rigid(d) is R(d, 0, 1), so rigid(3) is diagonal and only
// e4 squares to 0; proof: conformal(d) has null e(d+1) and e(d+2) with
// e(d+1) . e(d+2) = -1, so conformal(3) is not diagonal and e4, e5 are null
TEST_CASE("is_diagonal and is_null at run time: rigid(3) is diagonal with e4 "
          "its only null generator; conformal(3) is not diagonal and has e4 "
          "and e5 null") {
  // Copies made outside a constant expression, so the accessors run at run
  // time as well as at compile time.
  const auto rigid3 = core::rigid<3>();
  const auto conformal3 = core::conformal<3>();

  CHECK(rigid3.is_diagonal());
  CHECK_FALSE(rigid3.is_null(0));
  CHECK_FALSE(rigid3.is_null(1));
  CHECK_FALSE(rigid3.is_null(2));
  CHECK(rigid3.is_null(3));

  CHECK_FALSE(conformal3.is_diagonal());
  CHECK_FALSE(conformal3.is_null(0));
  CHECK_FALSE(conformal3.is_null(1));
  CHECK_FALSE(conformal3.is_null(2));
  CHECK(conformal3.is_null(3));
  CHECK(conformal3.is_null(4));

  static_assert(core::rigid<3>().is_diagonal());
  static_assert(!core::conformal<3>().is_diagonal());
  static_assert(core::conformal<3>().is_null(4));
}

// Reference: independent value: conformal(d) has null e(d+1) and e(d+2) with
// e(d+1) . e(d+2) = -1, with its diagonal counterpart R(d+1, 1, 0) and the
// change of basis between them for d from 1 to 4; identity: the change of
// basis origin = (e(d+2) - e(d+1)) / 2, infinity = e(d+2) + e(d+1) carries
// the form of R(d+1, 1, 0) to that of conformal(d)
TEST_CASE("conformal(d) and R(d+1, 1, 0) are different signatures, and the "
          "change of basis origin = (1/2)(e(d+2) - e(d+1)), infinity = "
          "e(d+2) + e(d+1) carries the form of R(d+1, 1, 0) to that of "
          "conformal(d), for d from 0 to 4") {
  CHECK(core::conformal<3>() != core::R<4, 1, 0>());

  CHECK(diagonal_mismatches(core::R<2, 1, 0>(),
                            golden::conformal::d1_diagonal_squares) == 0);
  CHECK(diagonal_mismatches(core::R<3, 1, 0>(),
                            golden::conformal::d2_diagonal_squares) == 0);
  CHECK(diagonal_mismatches(core::R<4, 1, 0>(),
                            golden::conformal::d3_diagonal_squares) == 0);
  CHECK(diagonal_mismatches(core::R<5, 1, 0>(),
                            golden::conformal::d4_diagonal_squares) == 0);

  // conformal(0) in the generators of R(1, 1, 0), written out: the origin is
  // (e2 - e1) / 2 and infinity e2 + e1.
  constexpr fraction half{.numerator = 1, .denominator = 2};
  constexpr fraction minus_half{.numerator = -1, .denominator = 2};
  constexpr fraction one{.numerator = 1, .denominator = 1};
  using row = std::array<fraction, 2>;
  constexpr row origin{minus_half, half};
  constexpr row infinity{one, one};
  constexpr std::array<row, 2> conformal0_in_diagonal{origin, infinity};
  CHECK(change_of_basis_mismatches(conformal0_in_diagonal, core::R<1, 1, 0>(),
                                   core::conformal<0>()) == 0);
  CHECK(change_of_basis_mismatches(golden::conformal::d1_in_diagonal,
                                   core::R<2, 1, 0>(),
                                   core::conformal<1>()) == 0);
  CHECK(change_of_basis_mismatches(golden::conformal::d2_in_diagonal,
                                   core::R<3, 1, 0>(),
                                   core::conformal<2>()) == 0);
  CHECK(change_of_basis_mismatches(golden::conformal::d3_in_diagonal,
                                   core::R<4, 1, 0>(),
                                   core::conformal<3>()) == 0);
  CHECK(change_of_basis_mismatches(golden::conformal::d4_in_diagonal,
                                   core::R<5, 1, 0>(),
                                   core::conformal<4>()) == 0);
}

// Reference: proof: an entry off the diagonal makes a form non-diagonal, and
// a zero diagonal entry is a generator that squares to zero
TEST_CASE("from_gram keeps a non-diagonal degenerate form entry by entry, "
          "and its accessors read it: [[0, 1, 0], [1, 0, 0], [0, 0, 0]] is "
          "not diagonal and every generator is null") {
  constexpr auto m = core::from_gram({{0, 1, 0}, {1, 0, 0}, {0, 0, 0}});
  constexpr auto expected = written({{0, 1, 0}, {1, 0, 0}, {0, 0, 0}});
  CHECK(m.gram == expected);
  CHECK(decltype(m)::generator_count == 3);
  CHECK_FALSE(m.is_diagonal());
  CHECK(m.is_null(0));
  CHECK(m.is_null(1));
  CHECK(m.is_null(2));
}

// Reference: proof: max_form_entry is the largest k whose 2 x 2 Gram minor
// 2 k^2 is at most the largest int
TEST_CASE("max_form_entry is the largest k whose 2 x 2 Gram minor 2 k^2 fits "
          "in an int, and is_symmetric and entries_in_range judge "
          "hand-written metrics against it at either sign") {
  constexpr long long k = core::max_form_entry;
  constexpr long long int_max = std::numeric_limits<int>::max();
  CHECK(2 * k * k <= int_max);
  CHECK(2 * (k + 1) * (k + 1) > int_max);

  constexpr core::metric<2> asymmetric_form{written({{0, 1}, {2, 0}})};
  CHECK_FALSE(asymmetric_form.is_symmetric());
  CHECK(asymmetric_form.entries_in_range());

  constexpr int top = core::max_form_entry;
  constexpr auto at_bound_entries = written({{top, -top}, {-top, top}});
  constexpr core::metric<2> at_bound{at_bound_entries};
  CHECK(at_bound.is_symmetric());
  CHECK(at_bound.entries_in_range());

  constexpr core::metric<1> above{written({{top + 1}})};
  constexpr core::metric<1> below{written({{-top - 1}})};
  CHECK_FALSE(above.entries_in_range());
  CHECK_FALSE(below.entries_in_range());
}

// Reference: contract: the factories accept n up to max_generators unless
// MaxN is raised, and never more generators than a blade mask has bits
TEST_CASE("The factories accept n up to max_generators by default, and up to "
          "the width of a blade mask when MaxN is raised") {
  CHECK(core::R<core::max_generators, 0, 0>().generator_count ==
        core::max_generators);
  CHECK(core::R<mask_width, 0, 0, mask_width>().generator_count == mask_width);
  CHECK(core::rigid<mask_width - 1, mask_width>().is_null(mask_width - 1));
  CHECK(core::conformal<mask_width - 2, mask_width>().generator_count ==
        mask_width);
  CHECK(core::from_gram<1, mask_width>({{1}}).generator_count == 1);
}
