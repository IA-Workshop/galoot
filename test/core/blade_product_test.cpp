// Tests of galoot/core/blade_product.hpp against the mathematics it
// implements.
//
// The reference in this file shares no code with the header. The checks of
// the defining relations, associativity, and the wedge-monomial basis use
// only the products under test and the test's own multiplication of sums of
// blades, held densely by mask. The single-term rule of a diagonal form, the
// exterior product, determinants, and orderings are computed here from index
// lists read bit by bit, with signs counted pair by pair. Every check runs
// over every blade, pair, or triple of each signature in its range, so each
// is a proof over that range. A second route compares the header with the
// golden product tables of reference/golden.hpp, which the reference scripts
// in test/reference/ compute in Python by diagonalizing the form over
// fractions.
#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/metric.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>

namespace {

namespace core = galoot::core;
using core::coefficient_type;
using core::mask_t;
using core::detail::term;

constexpr std::size_t mask_width = std::numeric_limits<mask_t>::digits;

// The exhaustive checks cover every algebra of up to this many generators.
constexpr std::size_t exhaustive_n = 6;

constexpr std::size_t blade_count(std::size_t n) { return std::size_t{1} << n; }

// A product of two blades of exhaustive_n generators has at most
// 2^(exhaustive_n - 1) terms: half the subsets of the generators.
constexpr std::size_t capacity = blade_count(exhaustive_n - 1);
using list = core::detail::term_list<capacity>;

template <std::size_t N>
constexpr list product(const core::metric<N>& g, mask_t a, mask_t b) {
  return core::detail::blade_product<capacity>(g, a, b);
}

constexpr mask_t generator(int index) {
  return mask_t{1} << static_cast<unsigned>(index - 1);
}

// The terms of a list, as the list states them.
constexpr std::span<const term> terms_of(const list& l) {
  return std::span(l.terms).first(l.size());
}

// A blade and its coefficient, as the terms of the tests are written.
using written_term = std::pair<mask_t, coefficient_type>;

// A list with the given terms, written in the order given and not through
// add, so that it can be the expected value of a test of add.
constexpr list written(std::initializer_list<written_term> terms) {
  list l{};
  for (const auto& [blade, coefficient] : terms) {
    l.terms.at(l.length) = {.blade = blade, .coefficient = coefficient};
    ++l.length;
  }
  return l;
}

// The generator indices of a blade, counting from 1, in increasing order.
struct index_list {
  std::array<int, mask_width> at{};
  std::size_t size = 0;

  [[nodiscard]] constexpr std::span<const int> view() const {
    return std::span(at).first(size);
  }
};

// Bit i - 1 of a mask stands for e_i.
constexpr index_list indices_of(mask_t mask) {
  index_list found;
  for (std::size_t bit = 0; bit < mask_width; ++bit) {
    if (((mask >> bit) & 1U) == 1U) {
      found.at.at(found.size) = static_cast<int>(bit) + 1;
      ++found.size;
    }
  }
  return found;
}

// The number of pairs of a generator e_i of A and e_j of B with i > j: the
// transpositions that sort e_A e_B, apart from the pairs with i = j.
constexpr int inversions(mask_t a, mask_t b) {
  int count = 0;
  for (const int i : indices_of(a).view()) {
    for (const int j : indices_of(b).view()) {
      count += i > j ? 1 : 0;
    }
  }
  return count;
}

// The number of pairs p < q with order[p] > order[q].
constexpr int inversions(std::span<const int> order) {
  int count = 0;
  for (std::size_t p = 0; p < order.size(); ++p) {
    for (std::size_t q = p + 1; q < order.size(); ++q) {
      count += order[p] > order[q] ? 1 : 0;
    }
  }
  return count;
}

constexpr coefficient_type parity_sign(int count) {
  return count % 2 == 0 ? 1 : -1;
}

// Canonical order: by grade, then by the first index at which the increasing
// index lists differ.
constexpr bool by_grade_then_indices(mask_t a, mask_t b) {
  const index_list la = indices_of(a);
  const index_list lb = indices_of(b);
  if (la.size != lb.size) {
    return la.size < lb.size;
  }
  return std::ranges::lexicographical_compare(la.view(), lb.view());
}

// A sum of blades of up to exhaustive_n generators, by mask.
using dense = std::array<coefficient_type, blade_count(exhaustive_n)>;

constexpr void accumulate(dense& sum, const list& l, coefficient_type scale) {
  for (const term& t : terms_of(l)) {
    sum.at(t.blade) += scale * t.coefficient;
  }
}

// x e_i, multiplied out term by term.
template <std::size_t N>
constexpr dense times_generator(const core::metric<N>& g, const dense& x,
                                int index) {
  dense out{};
  for (mask_t m = 0; m < blade_count(N); ++m) {
    if (x.at(m) != 0) {
      accumulate(out, product(g, m, generator(index)), x.at(m));
    }
  }
  return out;
}

// The determinant by the Leibniz formula: the sum over permutations s of
// sgn(s) times the product of g[i][s(i)].
template <std::size_t N>
constexpr coefficient_type determinant(const core::metric<N>& g) {
  std::array<int, N> column{};
  for (std::size_t i = 0; i < N; ++i) {
    column.at(i) = static_cast<int>(i);
  }
  coefficient_type sum = 0;
  for (bool more = true; more;
       more = std::ranges::next_permutation(column).found) {
    coefficient_type term_value = parity_sign(inversions(column));
    for (std::size_t i = 0; i < N; ++i) {
      term_value *= g.gram.at(i).at(static_cast<std::size_t>(column.at(i)));
    }
    sum += term_value;
  }
  return sum;
}

// The largest number of terms of e_A e_B, counted here: half the subsets of
// the generators of A and B together, or 1 when both are the scalar.
constexpr std::size_t half_the_subsets(mask_t a, mask_t b) {
  std::size_t subsets = 1;
  for (std::size_t i = 0; i < indices_of(a | b).size; ++i) {
    subsets *= 2;
  }
  return subsets == 1 ? 1 : subsets / 2;
}

// Every signature of the exhaustive checks: the diagonal R(p, q, r) with
// null generators last, conformal(d), the Witt forms of one and two modes,
// scaled so that a a† + a† a = 2, a degenerate form with a zero first pivot,
// rigid(3) with its null generator first, and ten seeded random symmetric
// forms with entries from -3 to 3.
template <class Visit>
void for_each_signature(const Visit& visit) {
  visit("R(1,0,0)", core::R<1, 0, 0>());
  visit("R(0,1,0)", core::R<0, 1, 0>());
  visit("R(0,0,1)", core::R<0, 0, 1>());
  visit("R(2,0,0)", core::R<2, 0, 0>());
  visit("R(0,2,0)", core::R<0, 2, 0>());
  visit("R(1,1,0)", core::R<1, 1, 0>());
  visit("R(1,0,1)", core::R<1, 0, 1>());
  visit("R(0,0,2)", core::R<0, 0, 2>());
  visit("R(3,0,0)", core::R<3, 0, 0>());
  visit("R(2,1,0)", core::R<2, 1, 0>());
  visit("R(2,0,1)", core::R<2, 0, 1>());
  visit("R(1,1,1)", core::R<1, 1, 1>());
  visit("R(0,0,3)", core::R<0, 0, 3>());
  visit("R(4,0,0)", core::R<4, 0, 0>());
  visit("rigid(3)", core::rigid<3>());
  visit("R(1,3,0)", core::R<1, 3, 0>());
  visit("R(3,1,0)", core::R<3, 1, 0>());
  visit("R(2,2,0)", core::R<2, 2, 0>());
  visit("R(2,0,2)", core::R<2, 0, 2>());
  visit("R(0,0,4)", core::R<0, 0, 4>());
  visit("R(4,1,0)", core::R<4, 1, 0>());
  visit("R(4,0,1)", core::R<4, 0, 1>());
  visit("R(4,2,0)", core::R<4, 2, 0>());
  visit("R(3,3,0)", core::R<3, 3, 0>());
  visit("conformal(1)", core::conformal<1>());
  visit("conformal(2)", core::conformal<2>());
  visit("conformal(3)", core::conformal<3>());
  visit("conformal(4)", core::conformal<4>());
  visit("Witt, one mode", core::from_gram({
                              {0, 1},
                              {1, 0},
                          }));
  visit("Witt, two modes", core::from_gram({
                               {0, 1, 0, 0},
                               {1, 0, 0, 0},
                               {0, 0, 0, 1},
                               {0, 0, 1, 0},
                           }));
  visit("degenerate", core::from_gram({
                          {0, 1, 0},
                          {1, 0, 0},
                          {0, 0, 0},
                      }));
  visit("rigid(3), null first", core::from_gram({
                                    {0, 0, 0, 0},
                                    {0, 1, 0, 0},
                                    {0, 0, 1, 0},
                                    {0, 0, 0, 1},
                                }));
  visit("random 0", core::from_gram({
                        {-1, -2},
                        {-2, 3},
                    }));
  visit("random 1", core::from_gram({
                        {2, 0, -3},
                        {0, 1, -3},
                        {-3, -3, -2},
                    }));
  visit("random 2", core::from_gram({
                        {-2, 0, -2, 2},
                        {0, -2, -3, 3},
                        {-2, -3, 1, 3},
                        {2, 3, 3, 3},
                    }));
  visit("random 3", core::from_gram({
                        {3, 1, -3, -1, -2},
                        {1, -3, -2, 1, 0},
                        {-3, -2, -2, 3, 0},
                        {-1, 1, 3, 3, 2},
                        {-2, 0, 0, 2, -1},
                    }));
  visit("random 4", core::from_gram({
                        {3, 1},
                        {1, -2},
                    }));
  visit("random 5", core::from_gram({
                        {-3, -3, 0},
                        {-3, -2, 2},
                        {0, 2, 0},
                    }));
  visit("random 6", core::from_gram({
                        {0, 1, -1, 0},
                        {1, -1, 0, -1},
                        {-1, 0, 2, 0},
                        {0, -1, 0, -1},
                    }));
  visit("random 7", core::from_gram({
                        {-3, 1, 2, -1, -2},
                        {1, -3, 0, -3, -1},
                        {2, 0, 0, 3, -3},
                        {-1, -3, 3, 1, -2},
                        {-2, -1, -3, -2, 2},
                    }));
  visit("random 8", core::from_gram({
                        {3, -2},
                        {-2, -2},
                    }));
  visit("random 9", core::from_gram({
                        {-3, 1, -2},
                        {1, -3, -2},
                        {-2, -2, 1},
                    }));
}

// The number of pairs i, j for which e_i e_j + e_j e_i is not 2 g_ij.
template <std::size_t N>
int relation_mismatches(const core::metric<N>& g) {
  int mismatches = 0;
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      const mask_t ei = mask_t{1} << i;
      const mask_t ej = mask_t{1} << j;
      dense sum{};
      accumulate(sum, product(g, ei, ej), 1);
      accumulate(sum, product(g, ej, ei), 1);
      dense expected{};
      expected.at(0) = 2 * coefficient_type{g.gram.at(i).at(j)};
      mismatches += sum == expected ? 0 : 1;
    }
  }
  return mismatches;
}

// Every product of two blades, indexed by a 2^n + b.
template <std::size_t N>
std::vector<list> product_table(const core::metric<N>& g) {
  std::vector<list> table(blade_count(N) * blade_count(N));
  for (mask_t a = 0; a < blade_count(N); ++a) {
    for (mask_t b = 0; b < blade_count(N); ++b) {
      table.at((a * blade_count(N)) + b) = product(g, a, b);
    }
  }
  return table;
}

// The number of triples of blades with (e_A e_B) e_C != e_A (e_B e_C), each
// side multiplied out term by term over the table of products.
template <std::size_t N>
int associativity_mismatches(const core::metric<N>& g) {
  const std::vector<list> table = product_table(g);
  const auto times = [&table](mask_t a, mask_t b) -> const list& {
    return table.at((a * blade_count(N)) + b);
  };
  int mismatches = 0;
  for (mask_t a = 0; a < blade_count(N); ++a) {
    for (mask_t b = 0; b < blade_count(N); ++b) {
      for (mask_t c = 0; c < blade_count(N); ++c) {
        dense difference{};
        for (const term& t : terms_of(times(a, b))) {
          accumulate(difference, times(t.blade, c), t.coefficient);
        }
        for (const term& t : terms_of(times(b, c))) {
          accumulate(difference, times(a, t.blade), -t.coefficient);
        }
        mismatches += difference == dense{} ? 0 : 1;
      }
    }
  }
  return mismatches;
}

// The number of blades e_A of grade k for which the antisymmetrized product
// of its generators, the sum over orders s of sgn(s) e_s1 ... e_sk, is not
// k! e_A.
template <std::size_t N>
int wedge_basis_mismatches(const core::metric<N>& g) {
  int mismatches = 0;
  for (mask_t a = 0; a < blade_count(N); ++a) {
    index_list order = indices_of(a);
    const std::span<int> generators = std::span(order.at).first(order.size);
    dense sum{};
    for (bool more = true; more;
         more = std::ranges::next_permutation(generators).found) {
      dense ordered{};
      ordered.at(0) = 1;
      for (const int index : generators) {
        ordered = times_generator(g, ordered, index);
      }
      const coefficient_type sign = parity_sign(inversions(generators));
      for (std::size_t m = 0; m < sum.size(); ++m) {
        sum.at(m) += sign * ordered.at(m);
      }
    }

    coefficient_type factorial = 1;
    for (std::size_t k = 2; k <= order.size; ++k) {
      factorial *= static_cast<coefficient_type>(k);
    }
    dense expected{};
    expected.at(a) = factorial;
    mismatches += sum == expected ? 0 : 1;
  }
  return mismatches;
}

// The number of pairs whose product is not the single-term rule of a
// diagonal form: the sign of the inversions times the squares of the common
// generators, on e_(A xor B), or no term when a common square is zero.
template <std::size_t N>
int diagonal_rule_mismatches(const core::metric<N>& g) {
  int mismatches = 0;
  for (mask_t a = 0; a < blade_count(N); ++a) {
    for (mask_t b = 0; b < blade_count(N); ++b) {
      coefficient_type c = parity_sign(inversions(a, b));
      for (const int index : indices_of(a & b).view()) {
        const auto i = static_cast<std::size_t>(index - 1);
        c *= g.gram.at(i).at(i);
      }
      const list expected = c == 0 ? list{} : written({{a ^ b, c}});
      mismatches += product(g, a, b) == expected ? 0 : 1;
    }
  }
  return mismatches;
}

// The number of pairs whose product is not the exterior product: zero when
// A and B share a generator, and otherwise the sign of the inversions on
// e_(A with B).
template <std::size_t N>
int exterior_mismatches(const core::metric<N>& g) {
  int mismatches = 0;
  for (mask_t a = 0; a < blade_count(N); ++a) {
    for (mask_t b = 0; b < blade_count(N); ++b) {
      const list expected =
          (a & b) != 0 ? list{}
                       : written({{a | b, parity_sign(inversions(a, b))}});
      mismatches += product(g, a, b) == expected ? 0 : 1;
    }
  }
  return mismatches;
}

// The number of pairs with more terms than half the subsets of A with B, or
// whose product_capacity is not that count.
template <std::size_t N>
int capacity_mismatches(const core::metric<N>& g) {
  int mismatches = 0;
  for (mask_t a = 0; a < blade_count(N); ++a) {
    for (mask_t b = 0; b < blade_count(N); ++b) {
      const std::size_t bound = half_the_subsets(a, b);
      const bool within = product(g, a, b).size() <= bound &&
                          core::detail::product_capacity(a, b) == bound;
      mismatches += within ? 0 : 1;
    }
  }
  return mismatches;
}

// The pseudoscalar square of an algebra of n generators:
// (-1)^(n(n-1)/2) det g on the scalar, or no term when det g = 0.
template <std::size_t N>
list pseudoscalar_square(coefficient_type det) {
  const coefficient_type sign = parity_sign(static_cast<int>(N * (N - 1) / 2));
  return det == 0 ? list{} : written({{0, sign * det}});
}

template <std::size_t N>
core::metric<N> metric_of(const std::array<std::array<int, N>, N>& gram) {
  core::metric<N> g{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      g.gram.at(i).at(j) = gram.at(i).at(j);
    }
  }
  return g;
}

constexpr list listed(const golden::product& p) {
  list l{};
  for (std::size_t k = 0; k < p.size; ++k) {
    l.terms.at(k) = {
        .blade = p.terms.at(k).blade,
        .coefficient = p.terms.at(k).coefficient,
    };
  }
  l.length = p.size;
  return l;
}

// The number of rows of a golden table whose product differs, by ==.
template <std::size_t N, std::size_t K>
constexpr int table_mismatches(const core::metric<N>& g,
                               const std::array<golden::product, K>& table) {
  int mismatches = 0;
  for (const golden::product& p : table) {
    mismatches +=
        core::detail::blade_product<capacity>(g, p.a, p.b) == listed(p) ? 0 : 1;
  }
  return mismatches;
}

// The number of rows of a golden table whose blades are not in increasing
// canonical order, which the test reads from the index lists.
template <std::size_t K>
int order_mismatches(const std::array<golden::product, K>& table) {
  int mismatches = 0;
  for (const golden::product& p : table) {
    for (std::size_t k = 1; k < p.size; ++k) {
      mismatches +=
          by_grade_then_indices(p.terms.at(k - 1).blade, p.terms.at(k).blade)
              ? 0
              : 1;
    }
  }
  return mismatches;
}

// Sums built through add, in two orders of the same terms.
constexpr list added(std::initializer_list<written_term> terms) {
  list l{};
  for (const auto& [blade, coefficient] : terms) {
    l.add(blade, coefficient);
  }
  return l;
}

constexpr mask_t e1 = 0b00001;
constexpr mask_t e2 = 0b00010;
constexpr mask_t e12 = 0b00011;
constexpr mask_t e3 = 0b00100;
constexpr mask_t e13 = 0b00101;
constexpr mask_t e23 = 0b00110;
constexpr mask_t e123 = 0b00111;
constexpr mask_t e14 = 0b01001;
constexpr mask_t e4 = 0b01000;
constexpr mask_t e1234 = 0b01111;
constexpr mask_t e5 = 0b10000;
constexpr mask_t e45 = 0b11000;

// Reference: contract: a term list is in canonical order whatever the order
//   of add, merges a blade added twice, and drops a zero sum, and canonical
//   order puts e14 before e23, which numeric order of masks would not
static_assert(added({{e23, 2}, {e14, 5}, {e1, 1}, {e14, -5}, {e12, 3}}) ==
              written({{e1, 1}, {e12, 3}, {e23, 2}}));
static_assert(added({{e23, 2}, {e14, 5}}) == added({{e14, 5}, {e23, 2}}));
static_assert(added({{e14, 5}, {e23, 2}}) == written({{e14, 5}, {e23, 2}}));

// Reference: contract: a term and a term list are trivially copyable
//   aggregates, as a structural value must be
static_assert(std::is_aggregate_v<term>);
static_assert(std::is_trivially_copyable_v<term>);
static_assert(std::is_aggregate_v<core::detail::term_list<4>>);
static_assert(std::is_trivially_copyable_v<core::detail::term_list<4>>);

// A term list is structural, so it can be a template argument.
template <core::detail::term_list<2> L>
constexpr std::size_t size_of = L.size();
static_assert(size_of<core::detail::term_list<2>{
                  .terms = {{{.blade = e1, .coefficient = 1}, {}}},
                  .length = 1,
              }> == 1);

// Reference: independent value: e45 = e4 ^ e5 in conformal(3), so
//   e4 e5 = -1 + e45, by the nine golden products of e4, e5, and e45, in a
//   constant evaluation
static_assert(table_mismatches(core::conformal<3>(),
                               golden::wedge_basis::conformal_3_products) == 0);

} // namespace

// Reference: identity: e_i e_j + e_j e_i = 2 g_ij, the defining relation of
//   the Clifford algebra of g, for every pair of generators of every
//   signature of up to six generators listed here, the Witt forms giving
//   a a† + a† a = 2 and every other anticommutator of the modes zero
TEST_CASE("e_i e_j + e_j e_i = 2 g_ij for every pair of generators") {
  for_each_signature([](const char* name, const auto& g) {
    INFO(name);
    CHECK(relation_mismatches(g) == 0);
  });
}

// Reference: identity: the geometric product is associative, for every
//   triple of blades of every signature of up to six generators listed here
TEST_CASE("The product of blades is associative on every triple") {
  for_each_signature([](const char* name, const auto& g) {
    INFO(name);
    CHECK(associativity_mismatches(g) == 0);
  });
}

// Reference: proof: the blade of a mask is the wedge of its generators, so
//   the sum over orders s of sgn(s) e_s1 ... e_sk is k! times it, for every
//   blade of every signature of up to five generators listed here. The
//   defining relations and associativity hold in any basis, and this is the
//   check that tells the wedge-monomial basis from products of generators
TEST_CASE("Each blade is the antisymmetrized product of its generators, up to "
          "k!") {
  for_each_signature([](const char* name, const auto& g) {
    if constexpr (std::remove_cvref_t<decltype(g)>::generator_count <= 5) {
      INFO(name);
      CHECK(wedge_basis_mismatches(g) == 0);
    }
  });
}

// Reference: independent value: in a diagonal form e_A e_B is the sign of
//   reordering, counted here pair by pair, times the squares of the common
//   generators, on e_(A xor B), for every pair of blades of every diagonal
//   signature listed here
TEST_CASE("In a diagonal form a product of blades is one signed term") {
  for_each_signature([](const char* name, const auto& g) {
    if (g.is_diagonal()) {
      INFO(name);
      CHECK(diagonal_rule_mismatches(g) == 0);
    }
  });
}

// Reference: independent value: e_i e_j + e_j e_i = 2 g_ij, by the golden
//   product tables of rigid(3), conformal(3), a degenerate form, and a seeded
//   random form, which the reference scripts validate by the defining
//   relations, associativity, and the wedge-monomial basis before writing
//   them; contract: each table is compared with == on whole term lists, so
//   the order of the terms is compared too
TEST_CASE("Every product of four forms equals the golden product tables") {
  CHECK(table_mismatches(metric_of(golden::gp_tables::rigid_3_gram),
                         golden::gp_tables::rigid_3) == 0);
  CHECK(table_mismatches(metric_of(golden::gp_tables::conformal_3_gram),
                         golden::gp_tables::conformal_3) == 0);
  CHECK(table_mismatches(metric_of(golden::gp_tables::degenerate_3_gram),
                         golden::gp_tables::degenerate_3) == 0);
  CHECK(table_mismatches(metric_of(golden::gp_tables::random_1_gram),
                         golden::gp_tables::random_1) == 0);
}

// Reference: independent value: worked by hand from e_i e_j + e_j e_i =
//   2 g_ij and e_i x = e_i . x + e_i ^ x: e45 = e4 ^ e5 in conformal(3),
//   where e4 e4 = e5 e5 = 0 and e4 . e5 = -1, so e4 e5 = -1 + e45, in
//   [[0, 1], [1, 0]], e1 e2 = 1 + e12, and the classification: R(0,1,0) is
//   the complex numbers, R(0,2,0) the quaternions with i, j, k = e1, e2,
//   e12, R(1,1,0) the real 2 x 2 matrices, R(3,0,0) the Pauli algebra, and
//   R(1,3,0) the Dirac algebra
TEST_CASE("Hand-derived products in conformal(3), the Witt form, a "
          "degenerate form, and the classical algebras") {
  constexpr auto c3 = core::conformal<3>();
  CHECK(product(c3, e4, e5) == written({{0, -1}, {e45, 1}}));
  CHECK(product(c3, e5, e4) == written({{0, -1}, {e45, -1}}));
  CHECK(product(c3, e4, e45) == written({{e4, 1}}));
  CHECK(product(c3, e45, e4) == written({{e4, -1}}));
  CHECK(product(c3, e5, e45) == written({{e5, -1}}));
  CHECK(product(c3, e45, e5) == written({{e5, 1}}));
  CHECK(product(c3, e45, e45) == written({
                                     {0, 1},
                                 }));
  CHECK(product(c3, e4, e4) == list{});
  CHECK(product(c3, e5, e5) == list{});

  constexpr auto witt = core::from_gram({{0, 1}, {1, 0}});
  CHECK(product(witt, e1, e2) == written({{0, 1}, {e12, 1}}));
  CHECK(product(witt, e2, e1) == written({{0, 1}, {e12, -1}}));
  CHECK(product(witt, e12, e12) == written({
                                       {0, 1},
                                   }));
  CHECK(product(witt, e12, e1) == written({{e1, 1}}));
  CHECK(product(witt, e1, e12) == written({{e1, -1}}));
  CHECK(product(witt, e1, e1) == list{});

  constexpr auto degenerate =
      core::from_gram({{0, 1, 0}, {1, 0, 0}, {0, 0, 0}});
  CHECK(product(degenerate, e1, e2) == written({{0, 1}, {e12, 1}}));
  CHECK(product(degenerate, e3, e3) == list{});
  CHECK(product(degenerate, e12, e12) == written({
                                             {0, 1},
                                         }));
  CHECK(product(degenerate, e123, e123) == list{});

  CHECK(product(core::R<0, 1, 0>(), e1, e1) == written({
                                                   {0, -1},
                                               }));
  constexpr auto quaternions = core::R<0, 2, 0>();
  CHECK(product(quaternions, e1, e2) == written({{e12, 1}}));
  CHECK(product(quaternions, e2, e12) == written({{e1, 1}}));
  CHECK(product(quaternions, e12, e1) == written({{e2, 1}}));
  CHECK(product(quaternions, e12, e12) == written({
                                              {0, -1},
                                          }));
  CHECK(product(core::R<1, 1, 0>(), e12, e12) == written({
                                                     {0, 1},
                                                 }));
  CHECK(product(core::R<3, 0, 0>(), e12, e12) == written({
                                                     {0, -1},
                                                 }));
  CHECK(product(core::R<3, 0, 0>(), e123, e123) == written({
                                                       {0, -1},
                                                   }));
  CHECK(product(core::R<1, 3, 0>(), e1234, e1234) == written({
                                                         {0, -1},
                                                     }));
  // e13 e23 = e1 e3 e2 e3 = -e1 e2 e3 e3 = -e12.
  CHECK(product(core::R<3, 0, 0>(), e13, e23) == written({{e12, -1}}));
}

// Reference: proof: the pseudoscalar of the wedge basis squares to
//   (-1)^(n(n-1)/2) det g for every symmetric g, with det g by the Leibniz
//   formula computed here, for every signature listed here; independent
//   value: det g of the golden product tables, in which
//   e_i e_j + e_j e_i = 2 g_ij, with the values written out: conformal(3) -1,
//   rigid(3) 0, the degenerate form 0, R(3,0,0) -1, R(1,3,0) -1, and the seeded
//   random form 31
TEST_CASE("The pseudoscalar squares to (-1)^(n(n-1)/2) det g") {
  for_each_signature([](const char* name, const auto& g) {
    constexpr std::size_t n = std::remove_cvref_t<decltype(g)>::generator_count;
    constexpr mask_t pseudoscalar = (mask_t{1} << n) - 1;
    INFO(name);
    CHECK(product(g, pseudoscalar, pseudoscalar) ==
          pseudoscalar_square<n>(determinant(g)));
  });

  CHECK(determinant(metric_of(golden::gp_tables::rigid_3_gram)) ==
        golden::gp_tables::rigid_3_det);
  CHECK(determinant(metric_of(golden::gp_tables::conformal_3_gram)) ==
        golden::gp_tables::conformal_3_det);
  CHECK(determinant(metric_of(golden::gp_tables::degenerate_3_gram)) ==
        golden::gp_tables::degenerate_3_det);
  CHECK(determinant(metric_of(golden::gp_tables::random_1_gram)) ==
        golden::gp_tables::random_1_det);

  const auto random_1 = metric_of(golden::gp_tables::random_1_gram);
  CHECK(product(core::conformal<3>(), 0b11111, 0b11111) == written({
                                                               {0, -1},
                                                           }));
  CHECK(product(core::rigid<3>(), e1234, e1234) == list{});
  CHECK(product(core::from_gram({{0, 1, 0}, {1, 0, 0}, {0, 0, 0}}), e123,
                e123) == list{});
  CHECK(product(core::R<3, 0, 0>(), e123, e123) == written({
                                                       {0, -1},
                                                   }));
  CHECK(product(core::R<1, 3, 0>(), e1234, e1234) == written({
                                                         {0, -1},
                                                     }));
  CHECK(product(random_1, e123, e123) == written({
                                             {0, 31},
                                         }));
}

// Reference: independent value: the exterior product, zero for blades that
//   share a generator and otherwise the sign of reordering on their union,
//   counted here pair by pair, since in R(0,0,n) every generator squares to
//   zero and the geometric product is the exterior product, for n from 1 to 6
TEST_CASE("In R(0,0,n) the product is the exterior product") {
  CHECK(exterior_mismatches(core::R<0, 0, 1>()) == 0);
  CHECK(exterior_mismatches(core::R<0, 0, 2>()) == 0);
  CHECK(exterior_mismatches(core::R<0, 0, 3>()) == 0);
  CHECK(exterior_mismatches(core::R<0, 0, 4>()) == 0);
  CHECK(exterior_mismatches(core::R<0, 0, 5>()) == 0);
  CHECK(exterior_mismatches(core::R<0, 0, 6>()) == 0);
}

// Reference: proof: every term of e_A e_B lies on a subset of A with B whose
//   grade has the parity of |A| + |B|, so at most half the subsets appear,
//   a count made here and compared with product_capacity and with the size
//   of every product of every signature listed here; independent value:
//   in [[0, 1], [1, 0]], e1 e2 = 1 + e12 attains the bound of two terms
TEST_CASE("No product has more terms than product_capacity, and the bound is "
          "attained") {
  for_each_signature([](const char* name, const auto& g) {
    INFO(name);
    CHECK(capacity_mismatches(g) == 0);
  });
  const auto witt = core::from_gram({{0, 1}, {1, 0}});
  CHECK(core::detail::product_capacity(e1, e2) == 2);
  CHECK(core::detail::blade_product<2>(witt, e1, e2).size() == 2);
  CHECK(core::detail::product_capacity(0, 0) == 1);
}

// Reference: independent value: results are ordered by grade, then by index
//   list, read here from the index lists of the golden product tables, in
//   which e_i e_j + e_j e_i = 2 g_ij; contract: add merges a blade added
//   twice, removes a zero sum, and leaves the unused places zero, so that ==
//   compares sums
TEST_CASE("Term lists are in canonical order, merge repeated blades, and drop "
          "zero sums") {
  CHECK(order_mismatches(golden::gp_tables::rigid_3) == 0);
  CHECK(order_mismatches(golden::gp_tables::conformal_3) == 0);
  CHECK(order_mismatches(golden::gp_tables::degenerate_3) == 0);
  CHECK(order_mismatches(golden::gp_tables::random_1) == 0);

  list l{};
  l.add(e12, 3);
  l.add(e1, 1);
  l.add(e12, -3);
  CHECK(l == written({{e1, 1}}));
  CHECK(l.terms.at(1) == term{});
  l.add(e3, 0);
  CHECK(l == written({{e1, 1}}));
  l.add(e1, -1);
  CHECK(l == list{});
}

// Reference: contract: one constexpr function serves both times, so the
//   table of rigid(3) built in a constant evaluation equals the same
//   function run at run time on a metric that the compiler cannot know
TEST_CASE("The product table of rigid(3) is the same at compile time and at "
          "run time") {
  constexpr std::size_t n = 4;
  using rigid_list = core::detail::term_list<8>;
  static constexpr auto at_compile_time = [] {
    std::array<rigid_list, blade_count(n) * blade_count(n)> table{};
    for (mask_t a = 0; a < blade_count(n); ++a) {
      for (mask_t b = 0; b < blade_count(n); ++b) {
        table.at((a * blade_count(n)) + b) =
            core::detail::blade_product<8>(core::rigid<3>(), a, b);
      }
    }
    return table;
  }();

  // Each entry is read through a volatile, so the products below cannot be
  // folded into constants.
  core::metric<n> g{};
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < n; ++j) {
      const volatile int entry = core::rigid<3>().gram.at(i).at(j);
      g.gram.at(i).at(j) = entry;
    }
  }
  int mismatches = 0;
  for (mask_t a = 0; a < blade_count(n); ++a) {
    for (mask_t b = 0; b < blade_count(n); ++b) {
      mismatches += core::detail::blade_product<8>(g, a, b) ==
                            at_compile_time.at((a * blade_count(n)) + b)
                        ? 0
                        : 1;
    }
  }
  CHECK(mismatches == 0);
}

// Reference: independent value: in R(32,0,0), the widest algebra a mask
//   holds, e32 e32 = 1 and e1 e32 = e1 ^ e32, so a blade on the top bit of
//   the mask is within the algebra
TEST_CASE("blade_product accepts the generators of the widest algebra") {
  constexpr auto widest = core::R<mask_width, 0, 0, mask_width>();
  constexpr mask_t top = mask_t{1} << (mask_width - 1);
  static_assert(core::detail::blade_product<1>(widest, top, top).terms[0] ==
                term{.blade = 0, .coefficient = 1});
  CHECK(product(widest, top, top) == written({{0, 1}}));
  CHECK(product(widest, e1, top) == written({{e1 | top, 1}}));
}

// Reference: contract: the guards accept every sum and product within -max
//   to max, for max the largest coefficient_type, including the bounds
//   themselves
TEST_CASE("The coefficient guards accept results up to the bounds") {
  constexpr coefficient_type max = std::numeric_limits<coefficient_type>::max();
  CHECK(core::detail::add_checked(max - 1, 1) == max);
  CHECK(core::detail::add_checked(1, max - 1) == max);
  CHECK(core::detail::add_checked(-max + 1, -1) == -max);
  CHECK(core::detail::add_checked(-1, -max + 1) == -max);
  CHECK(core::detail::add_checked(max, -max) == 0);
  CHECK(core::detail::multiply_checked(max, 1) == max);
  CHECK(core::detail::multiply_checked(-1, max) == -max);
  CHECK(core::detail::multiply_checked(-max, -1) == max);
  CHECK(core::detail::multiply_checked(max / 2, 2) == max - 1);
  CHECK(core::detail::multiply_checked(-2, max / 2) == -(max - 1));
  CHECK(core::detail::multiply_checked(0, max) == 0);
  CHECK(core::detail::multiply_checked(max, 0) == 0);
}
