// Tests of the signature as data: the factories R, rigid, conformal, and
// from_gram, the accessors of metric, and the concept metric_like.
//
// Each expected matrix is either written out entry by entry or computed by
// an expected_* function below, which restates the definition of the
// signature and shares no code with the header. Generators are counted from
// e1, and index i of a matrix belongs to e_(i+1).
#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

#include <galoot/core/metric.hpp>

#include <doctest/doctest.h>

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

// The square of generator e_(i+1) in R(p, q, z): p squares of +1, then q of
// -1, then z of 0, so that null generators come last.
constexpr int expected_r_square(std::size_t i, std::size_t p, std::size_t q) {
  if (i < p) {
    return 1;
  }
  if (i < p + q) {
    return -1;
  }
  return 0;
}

// The number of entries in which R<P, Q, Z>() differs from its definition.
// Distinct generators of R(p, q, z) are orthogonal, so every entry off the
// diagonal is expected to be 0.
template <std::size_t P, std::size_t Q, std::size_t Z>
consteval int r_mismatches_of() {
  constexpr std::size_t n = P + Q + Z;
  const auto m = core::R<P, Q, Z>();
  int mismatches = 0;
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < n; ++j) {
      const int expected = i == j ? expected_r_square(i, P, Q) : 0;
      mismatches += m.gram[i][j] == expected ? 0 : 1;
    }
  }
  return mismatches;
}

template <std::size_t N, std::size_t P, std::size_t... Q>
consteval int r_mismatches_with_p(std::index_sequence<Q...> /*counts*/) {
  return (r_mismatches_of<P, Q, N - P - Q>() + ... + 0);
}

template <std::size_t N, std::size_t... P>
consteval int r_mismatches_over(std::index_sequence<P...> /*counts*/) {
  return (r_mismatches_with_p<N, P>(std::make_index_sequence<N - P + 1>()) +
          ... + 0);
}

// The mismatches summed over every split p + q + z = N.
template <std::size_t N>
consteval int r_mismatches() {
  return r_mismatches_over<N>(std::make_index_sequence<N + 1>());
}

// The entry (i, j) of rigid(d) = R(d, 0, 1): e1 to ed square to 1, the
// projective generator e(d+1) to 0, and distinct generators are orthogonal.
constexpr int expected_rigid(std::size_t d, std::size_t i, std::size_t j) {
  return i == j && i < d ? 1 : 0;
}

// The entry (i, j) of conformal(d): e1 to ed square to 1, the origin e(d+1)
// and infinity e(d+2) to 0, e(d+1) . e(d+2) = -1, and every other pair is
// orthogonal.
constexpr int expected_conformal(std::size_t d, std::size_t i, std::size_t j) {
  if (i == j) {
    return i < d ? 1 : 0;
  }
  const bool origin_infinity_pair =
      (i == d && j == d + 1) || (i == d + 1 && j == d);
  return origin_infinity_pair ? -1 : 0;
}

template <std::size_t D>
consteval int rigid_mismatches() {
  const auto m = core::rigid<D>();
  int mismatches = 0;
  for (std::size_t i = 0; i < D + 1; ++i) {
    for (std::size_t j = 0; j < D + 1; ++j) {
      mismatches += m.gram[i][j] == expected_rigid(D, i, j) ? 0 : 1;
    }
  }
  return mismatches;
}

template <std::size_t D>
consteval int conformal_mismatches() {
  const auto m = core::conformal<D>();
  int mismatches = 0;
  for (std::size_t i = 0; i < D + 2; ++i) {
    for (std::size_t j = 0; j < D + 2; ++j) {
      mismatches += m.gram[i][j] == expected_conformal(D, i, j) ? 0 : 1;
    }
  }
  return mismatches;
}

// The exact change of basis from R(d+1, 1, 0) to conformal(d), as data. In
// R(d+1, 1, 0), e(d+1) squares to +1 and e(d+2) to -1. Column k holds twice
// the coordinates of conformal generator k in the basis of R(d+1, 1, 0), so
// that every entry is an integer: ek is ek for k <= d, the origin e(d+1) is
// (1/2)(e(d+2) - e(d+1)), and infinity e(d+2) is e(d+2) + e(d+1). The
// diagonal basis is indexed from 0, so its e(d+1) is index d.
template <std::size_t D>
constexpr matrix<D + 2> doubled_conformal_basis() {
  matrix<D + 2> b{};
  for (std::size_t k = 0; k < D; ++k) {
    b[k][k] = 2;
  }
  b[D][D] = -1;
  b[D + 1][D] = 1;
  b[D][D + 1] = 2;
  b[D + 1][D + 1] = 2;
  return b;
}

// The number of entries where 4 G differs from (2B)^T diag (2B), with G the
// Gram matrix of conformal(d), diag that of R(d+1, 1, 0), and 2B the doubled
// basis above. Zero means the change of basis carries the diagonal form to
// the conformal one exactly.
template <std::size_t D>
consteval int change_of_basis_mismatches() {
  constexpr std::size_t n = D + 2;
  const auto g = core::conformal<D>().gram;
  const auto diag = core::R<D + 1, 1, 0>().gram;
  const auto b = doubled_conformal_basis<D>();
  int mismatches = 0;
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < n; ++j) {
      int product = 0;
      for (std::size_t k = 0; k < n; ++k) {
        product += b[k][i] * diag[k][k] * b[k][j];
      }
      mismatches += product == 4 * g[i][j] ? 0 : 1;
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

TEST_CASE("R(p, q, z) squares the first p generators to +1, the next q to -1, "
          "and the last z to 0, with no entry off the diagonal, for every "
          "split with n from 1 to 6") {
  CHECK(r_mismatches<1>() == 0);
  CHECK(r_mismatches<2>() == 0);
  CHECK(r_mismatches<3>() == 0);
  CHECK(r_mismatches<4>() == 0);
  CHECK(r_mismatches<5>() == 0);
  CHECK(r_mismatches<6>() == 0);
}

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

TEST_CASE("rigid(d) is R(d, 0, 1): e1 to ed Euclidean and the projective "
          "generator e(d+1) null, for d from 0 to 5") {
  CHECK(rigid_mismatches<0>() == 0);
  CHECK(rigid_mismatches<1>() == 0);
  CHECK(rigid_mismatches<2>() == 0);
  CHECK(rigid_mismatches<3>() == 0);
  CHECK(rigid_mismatches<4>() == 0);
  CHECK(rigid_mismatches<5>() == 0);

  constexpr auto rigid3 = written({
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 0},
  });
  CHECK(core::rigid<3>().gram == rigid3);
  CHECK(core::rigid<3>() == core::R<3, 0, 1>());
}

TEST_CASE("conformal(d) has e1 to ed Euclidean and e(d+1), e(d+2) null with "
          "e(d+1) . e(d+2) = -1, for d from 0 to 4") {
  CHECK(conformal_mismatches<0>() == 0);
  CHECK(conformal_mismatches<1>() == 0);
  CHECK(conformal_mismatches<2>() == 0);
  CHECK(conformal_mismatches<3>() == 0);
  CHECK(conformal_mismatches<4>() == 0);

  constexpr auto conformal3 = written({
      {1, 0, 0, 0, 0},
      {0, 1, 0, 0, 0},
      {0, 0, 1, 0, 0},
      {0, 0, 0, 0, -1},
      {0, 0, 0, -1, 0},
  });
  CHECK(core::conformal<3>().gram == conformal3);
}

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

TEST_CASE("conformal(3) and R(4, 1, 0) are different signatures, and the "
          "change of basis origin = (1/2)(e5 - e4), infinity = e5 + e4 "
          "carries the form of R(4, 1, 0) to that of conformal(3)") {
  CHECK(core::conformal<3>() != core::R<4, 1, 0>());
  CHECK(core::R<4, 1, 0>().is_diagonal());

  // The basis for d = 3, written out: column k is twice conformal generator
  // k in the coordinates of R(4, 1, 0).
  constexpr auto doubled_basis3 = written({
      {2, 0, 0, 0, 0},
      {0, 2, 0, 0, 0},
      {0, 0, 2, 0, 0},
      {0, 0, 0, -1, 2},
      {0, 0, 0, 1, 2},
  });
  CHECK(doubled_conformal_basis<3>() == doubled_basis3);

  CHECK(change_of_basis_mismatches<0>() == 0);
  CHECK(change_of_basis_mismatches<1>() == 0);
  CHECK(change_of_basis_mismatches<2>() == 0);
  CHECK(change_of_basis_mismatches<3>() == 0);
  CHECK(change_of_basis_mismatches<4>() == 0);
}

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
