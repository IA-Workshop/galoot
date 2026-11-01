// Tests of galoot/core/plan.hpp against the products it is built from and
// against values derived here.
//
// A plan lists, for every blade s_a e_A of the first set and s_b e_B of the
// second, in that order, the terms of e_A e_B with the signs folded in:
// s_a s_b c on e_C for each term c e_C of the product. The exhaustive checks
// compare every plan term with blade_product, whose own tests check it
// against the defining relations and the golden tables. The plans of the
// typed and sparse sets are compared with values written out by hand, or
// computed here by the single-term rule of a diagonal form, with signs
// counted pair by pair.
#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <span>
#include <tuple>
#include <type_traits>
#include <vector>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

#include <doctest/doctest.h>

#include "signatures.hpp"
#include <reference/golden.hpp>

namespace {

namespace core = galoot::core;
using core::coefficient_type;
using core::mask_t;
using core::oriented_blade;
using core::detail::BinaryOperation;
using core::detail::generate_plan;
using core::detail::plan_capacity;
using core::detail::plan_term;
using core::detail::product_plan;
using galoot_test::for_each_signature;

constexpr std::size_t mask_width = std::numeric_limits<mask_t>::digits;

constexpr std::size_t blade_count(std::size_t n) { return std::size_t{1} << n; }

// Every product of two blades of up to six generators fits a list of 32
// terms, half the subsets of six generators.
constexpr std::size_t test_capacity = 32;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct r400 {
  static constexpr auto value = core::R<4, 0, 0>();
};
struct r130 {
  static constexpr auto value = core::R<1, 3, 0>();
};
struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};
// Two forms that are not diagonal, in which a product of blades can have
// more than one term: conformal(2), with e3 . e4 = -1 for the null e3 and e4,
// and a degenerate form with e1 . e2 = 1 and e3 null.
struct conformal2 {
  static constexpr auto value = core::conformal<2>();
};
struct degenerate {
  static constexpr auto value = core::from_gram({
      {0, 1, 0},
      {1, 0, 0},
      {0, 0, 0},
  });
};
// Seeded random form 1 of the reference scripts, dense, with up to four
// terms in a product of blades.
struct random1 {
  static constexpr auto value = core::from_gram({
      {2, 0, -3},
      {0, 1, -3},
      {-3, -3, -2},
  });
};
struct widest {
  static constexpr auto value = core::R<mask_width, 0, 0, mask_width>();
};
// R(n - 1, 0, 1): rigid(n - 1), with its null generator e_n last.
template <std::size_t N>
struct rigid_n {
  static constexpr auto value = core::R<N - 1, 0, 1>();
};

// The objects of rigid(3): the oriented names of the point, the line, the
// plane, and the motor.
constexpr auto point = core::blades<4, "e1 e2 e3 e4">;
constexpr auto line = core::blades<4, "e41 e42 e43 e23 e31 e12">;
constexpr auto plane = core::blades<4, "e423 e431 e412 e321">;
constexpr auto motor = core::blades<4, "1 e41 e42 e43 e23 e31 e12 e1234">;

// The terms of a typed plan with each output position read back as the mask
// of the result blade at that position, so that a plan can be compared with
// the masks the products give.
template <std::size_t R, std::size_t T>
constexpr std::array<plan_term, T>
read_back(const core::detail::typed_plan<R, T>& plan) {
  std::array<plan_term, T> terms{};
  for (std::size_t k = 0; k < T; ++k) {
    const core::detail::typed_term& t = plan.terms.at(k);
    terms.at(k) = {
        .a = t.a,
        .b = t.b,
        .out = plan.result.blades.at(t.out).mask,
        .coefficient = t.coefficient,
    };
  }
  return terms;
}

// The geometric-product plan of A and B in Alg, read back.
template <core::metric_like Alg, core::blade_set A, core::blade_set B>
constexpr auto gp_terms =
    read_back(product_plan<BinaryOperation::gp, Alg, A, B>);

constexpr mask_t e1 = 0b0001;
constexpr mask_t e2 = 0b0010;
constexpr mask_t e12 = 0b0011;
constexpr mask_t e13 = 0b0101;
constexpr mask_t e23 = 0b0110;
constexpr mask_t e123 = 0b0111;
constexpr mask_t e4 = 0b1000;
constexpr mask_t e14 = 0b1001;
constexpr mask_t e24 = 0b1010;
constexpr mask_t e34 = 0b1100;

// The number of pairs of a generator e_i of A and e_j of B with i > j: the
// transpositions that sort e_A e_B, apart from the pairs with i = j.
constexpr int inversions(mask_t a, mask_t b) {
  int count = 0;
  for (std::size_t i = 0; i < mask_width; ++i) {
    for (std::size_t j = 0; j < i; ++j) {
      if (((a >> i) & 1U) == 1U && ((b >> j) & 1U) == 1U) {
        ++count;
      }
    }
  }
  return count;
}

constexpr coefficient_type parity_sign(int count) {
  return count % 2 == 0 ? 1 : -1;
}

constexpr std::size_t generators_of(mask_t mask) {
  std::size_t count = 0;
  for (std::size_t bit = 0; bit < mask_width; ++bit) {
    count += (mask >> bit) & 1U;
  }
  return count;
}

// Whether `plan` is, in order, the single-term rule of a diagonal form for
// every pair of `as` and `bs`: s_a s_b times the sign of the inversions times
// the squares of the common generators, on e_(A xor B), and no term when a
// common square is zero.
template <std::size_t N, std::size_t KA, std::size_t KB, std::size_t T>
constexpr bool follows_diagonal_rule(const core::metric<N>& g,
                                     const core::blade_set<KA>& as,
                                     const core::blade_set<KB>& bs,
                                     const std::array<plan_term, T>& plan) {
  std::size_t k = 0;
  for (std::size_t a = 0; a < KA; ++a) {
    for (std::size_t b = 0; b < KB; ++b) {
      const mask_t ma = as.blades.at(a).mask;
      const mask_t mb = bs.blades.at(b).mask;
      coefficient_type c = coefficient_type{as.blades.at(a).sign} *
                           bs.blades.at(b).sign *
                           parity_sign(inversions(ma, mb));
      for (std::size_t i = 0; i < N; ++i) {
        if ((((ma & mb) >> i) & 1U) == 1U) {
          c *= g.gram.at(i).at(i);
        }
      }
      if (c == 0) {
        continue;
      }
      const plan_term expected{
          .a = a,
          .b = b,
          .out = ma ^ mb,
          .coefficient = c,
      };
      if (k == T || plan.at(k) != expected) {
        return false;
      }
      ++k;
    }
  }
  return k == T;
}

// The largest number of terms of e_A e_B over the pairs, counted here: half
// the subsets of the generators of A and B together, or 1 for two scalars.
template <std::size_t KA, std::size_t KB>
constexpr std::size_t largest_pair_bound(const core::blade_set<KA>& as,
                                         const core::blade_set<KB>& bs) {
  std::size_t largest = 1;
  for (const oriented_blade& x : as.blades) {
    for (const oriented_blade& y : bs.blades) {
      const std::size_t together = generators_of(x.mask | y.mask);
      const std::size_t bound = together == 0 ? 1 : blade_count(together) / 2;
      largest = std::max(largest, bound);
    }
  }
  return largest;
}

template <std::size_t K>
constexpr core::blade_set<K> with_plus_signs(core::blade_set<K> set) {
  for (oriented_blade& blade : set.blades) {
    blade.sign = 1;
  }
  return set;
}

// The plan of `as` and `bs` with every coefficient multiplied by the two
// signs, from the plan of the same masks with +1 signs.
template <std::size_t KA, std::size_t KB, std::size_t T>
constexpr bool folds_signs(const std::array<plan_term, T>& signed_plan,
                           const std::array<plan_term, T>& plus_plan,
                           const core::blade_set<KA>& as,
                           const core::blade_set<KB>& bs) {
  for (std::size_t k = 0; k < T; ++k) {
    const plan_term& p = plus_plan.at(k);
    const plan_term expected{
        .a = p.a,
        .b = p.b,
        .out = p.out,
        .coefficient = coefficient_type{as.blades.at(p.a).sign} *
                       bs.blades.at(p.b).sign * p.coefficient,
    };
    if (signed_plan.at(k) != expected) {
      return false;
    }
  }
  return true;
}

// Whether generate_plan, called directly, writes the same terms as the plan
// read back.
template <std::size_t Capacity, std::size_t N, std::size_t KA, std::size_t KB,
          std::size_t T>
constexpr bool equals_generation(const core::metric<N>& g,
                                 const core::blade_set<KA>& as,
                                 const core::blade_set<KB>& bs,
                                 const std::array<plan_term, T>& plan) {
  std::array<plan_term, T> again{};
  const std::size_t count = generate_plan<BinaryOperation::gp, Capacity>(
      g, as.blades, bs.blades, again);
  return count == T && again == plan;
}

// Whether the terms of the pair (a, b) in `plan` are `expected`, in order.
template <std::size_t T>
constexpr bool pair_terms_are(const std::array<plan_term, T>& plan,
                              std::size_t a, std::size_t b,
                              std::initializer_list<plan_term> expected) {
  const std::vector<plan_term> wanted(expected);
  std::size_t k = 0;
  for (const plan_term& t : plan) {
    if (t.a != a || t.b != b) {
      continue;
    }
    if (k == wanted.size() || t != wanted.at(k)) {
      return false;
    }
    ++k;
  }
  return k == wanted.size();
}

template <std::size_t K>
constexpr core::blade_set<K> with_reverse_signs(core::blade_set<K> set) {
  for (oriented_blade& blade : set.blades) {
    blade.sign = generators_of(blade.mask) % 4 >= 2 ? -1 : 1;
  }
  return set;
}

// Whether `plan` is, in order, the products of the golden `table` for every
// pair of `as` and `bs`, each term times s_a s_b.
template <std::size_t T, std::size_t KA, std::size_t KB, std::size_t R>
constexpr bool matches_table(const std::array<plan_term, T>& plan,
                             const core::blade_set<KA>& as,
                             const core::blade_set<KB>& bs,
                             const std::array<golden::product, R>& table) {
  std::size_t k = 0;
  for (std::size_t a = 0; a < KA; ++a) {
    for (std::size_t b = 0; b < KB; ++b) {
      const oriented_blade& x = as.blades.at(a);
      const oriented_blade& y = bs.blades.at(b);
      std::size_t row = 0;
      while (row < R &&
             (table.at(row).a != x.mask || table.at(row).b != y.mask)) {
        ++row;
      }
      if (row == R) {
        return false;
      }
      const golden::product& p = table.at(row);
      for (std::size_t t = 0; t < p.size; ++t) {
        const plan_term expected{
            .a = a,
            .b = b,
            .out = p.terms.at(t).blade,
            .coefficient =
                coefficient_type{x.sign} * y.sign * p.terms.at(t).coefficient,
        };
        if (k == T || plan.at(k) != expected) {
          return false;
        }
        ++k;
      }
    }
  }
  return k == T;
}

// The ten terms of the sparse sets {1, e1, e_n, e1 ^ e_n} and {e1, e2, e_n}
// in R(n - 1, 0, 1), worked by hand from e_i e_i = 1 for i < n, e_n e_n = 0,
// and e_j e_i = -e_i e_j for i < j: e1 e1 = 1, e_n e1 = -e1n,
// e1n e1 = e1 e_n e1 = -e_n, e1n e2 = e1 e_n e2 = -e12n, and e_n e_n and
// e1n e_n vanish.
template <std::size_t N>
constexpr std::array<plan_term, 10> sparse_terms() {
  constexpr mask_t en = mask_t{1} << (N - 1);
  return {
      {
          {.a = 0, .b = 0, .out = e1, .coefficient = 1},
          {.a = 0, .b = 1, .out = e2, .coefficient = 1},
          {.a = 0, .b = 2, .out = en, .coefficient = 1},
          {.a = 1, .b = 0, .out = 0, .coefficient = 1},
          {.a = 1, .b = 1, .out = e12, .coefficient = 1},
          {.a = 1, .b = 2, .out = e1 | en, .coefficient = 1},
          {.a = 2, .b = 0, .out = e1 | en, .coefficient = -1},
          {.a = 2, .b = 1, .out = e2 | en, .coefficient = -1},
          {.a = 3, .b = 0, .out = en, .coefficient = -1},
          {.a = 3, .b = 1, .out = e12 | en, .coefficient = -1},
      },
  };
}

template <std::size_t N>
constexpr core::blade_set<4> sparse_first{
    {
        {
            {.mask = 0, .sign = 1},
            {.mask = e1, .sign = 1},
            {.mask = mask_t{1} << (N - 1), .sign = 1},
            {.mask = e1 | (mask_t{1} << (N - 1)), .sign = 1},
        },
    },
};
template <std::size_t N>
constexpr core::blade_set<3> sparse_second{
    {
        {
            {.mask = e1, .sign = 1},
            {.mask = e2, .sign = 1},
            {.mask = mask_t{1} << (N - 1), .sign = 1},
        },
    },
};

// Reference: independent value: e_i e_i = 1 for i < 4 and e4 e4 = 0 in
// rigid(3), and e_j e_i = -e_i e_j for i < j, worked by hand into the 15
// terms of a point times a point, 12 of them bivectors
static_assert(gp_terms<rga3, point, point> ==
              std::array<plan_term, 15>{
                  {
                      {.a = 0, .b = 0, .out = 0, .coefficient = 1},
                      {.a = 0, .b = 1, .out = e12, .coefficient = 1},
                      {.a = 0, .b = 2, .out = e13, .coefficient = 1},
                      {.a = 0, .b = 3, .out = e14, .coefficient = 1},
                      {.a = 1, .b = 0, .out = e12, .coefficient = -1},
                      {.a = 1, .b = 1, .out = 0, .coefficient = 1},
                      {.a = 1, .b = 2, .out = e23, .coefficient = 1},
                      {.a = 1, .b = 3, .out = e24, .coefficient = 1},
                      {.a = 2, .b = 0, .out = e13, .coefficient = -1},
                      {.a = 2, .b = 1, .out = e23, .coefficient = -1},
                      {.a = 2, .b = 2, .out = 0, .coefficient = 1},
                      {.a = 2, .b = 3, .out = e34, .coefficient = 1},
                      {.a = 3, .b = 0, .out = e14, .coefficient = -1},
                      {.a = 3, .b = 1, .out = e24, .coefficient = -1},
                      {.a = 3, .b = 2, .out = e34, .coefficient = -1},
                  },
              });

// Reference: independent value: in rigid(3) a product of blades is one term
// unless both hold e4, whose square is 0, so a line times a plane has
// 6 x 4 - 3 x 3 = 15 terms and a motor times a point, either way round,
// 8 x 4 - 4 = 28, each term s_a s_b times the sign of reordering times the
// common squares, on e_(A xor B), counted here pair by pair
static_assert(gp_terms<rga3, line, plane>.size() == 15);
static_assert(follows_diagonal_rule(rga3::value, line, plane,
                                    gp_terms<rga3, line, plane>));
static_assert(gp_terms<rga3, motor, point>.size() == 28);
static_assert(follows_diagonal_rule(rga3::value, motor, point,
                                    gp_terms<rga3, motor, point>));
static_assert(gp_terms<rga3, point, motor>.size() == 28);
static_assert(follows_diagonal_rule(rga3::value, point, motor,
                                    gp_terms<rga3, point, motor>));

// Reference: contract: a coefficient stored on s e_A multiplies s e_A, so
// the plan of oriented blades is the plan of their masks with each
// coefficient times s_a s_b, and an oriented name lists generators in the
// order of their product, so e41 = -e14 and e31 = -e13. A line times a line
// has 6 x 6 - 3 x 3 = 27 terms, as e4 e4 = 0
static_assert(gp_terms<rga3, line, line>.size() == 27);
static_assert(folds_signs(
    gp_terms<rga3, line, line>,
    gp_terms<rga3, with_plus_signs(line), with_plus_signs(line)>, line, line));
static_assert(
    folds_signs(gp_terms<rga3, line, plane>,
                gp_terms<rga3, with_plus_signs(line), with_plus_signs(plane)>,
                line, plane));

// Reference: independent value: the sparse sets {1, e1, e_n, e1 ^ e_n} and
// {e1, e2, e_n} in R(n - 1, 0, 1), where e_n e_n = 0, give ten terms worked
// by hand, at n = 8, 12, and 16, so that a plan in a large algebra costs
// what its sets use
static_assert(gp_terms<rigid_n<8>, sparse_first<8>, sparse_second<8>> ==
              sparse_terms<8>());
static_assert(gp_terms<rigid_n<12>, sparse_first<12>, sparse_second<12>> ==
              sparse_terms<12>());
static_assert(gp_terms<rigid_n<16>, sparse_first<16>, sparse_second<16>> ==
              sparse_terms<16>());

// Reference: contract: the plan is generate_plan's output, so the memoized
// plans of every blade of rigid(3), R(4,0,0), and R(1,3,0) times every blade
// equal a direct call of generate_plan in a constant evaluation
static_assert(
    equals_generation<8>(rga3::value, core::full<4>, core::full<4>,
                         gp_terms<rga3, core::full<4>, core::full<4>>));
static_assert(
    equals_generation<8>(r400::value, core::full<4>, core::full<4>,
                         gp_terms<r400, core::full<4>, core::full<4>>));
static_assert(
    equals_generation<8>(r130::value, core::full<4>, core::full<4>,
                         gp_terms<r130, core::full<4>, core::full<4>>));

// Reference: contract: the plans of conformal(2) and of a degenerate form,
// whose products of blades can have two terms, so that the term lists of the
// plan need more than one place, equal a direct call of generate_plan;
// independent value: worked by hand from e_i e_j + e_j e_i = 2 g_ij and
// e_i x = e_i . x + e_i ^ x, e3 e4 = -1 + e34 and e4 e3 = -1 - e34 in
// conformal(2), and e1 e2 = 1 + e12, e12 e12 = 1, and e123 e123 = 0 in the
// degenerate form, at their places in full<4> and full<3>
static_assert(
    equals_generation<8>(conformal2::value, core::full<4>, core::full<4>,
                         gp_terms<conformal2, core::full<4>, core::full<4>>));
static_assert(
    equals_generation<4>(degenerate::value, core::full<3>, core::full<3>,
                         gp_terms<degenerate, core::full<3>, core::full<3>>));
static_assert(pair_terms_are(gp_terms<conformal2, core::full<4>, core::full<4>>,
                             3, 4,
                             {
                                 {.a = 3, .b = 4, .out = 0, .coefficient = -1},
                                 {.a = 3, .b = 4, .out = e34, .coefficient = 1},
                             }));
static_assert(
    pair_terms_are(gp_terms<conformal2, core::full<4>, core::full<4>>, 4, 3,
                   {
                       {.a = 4, .b = 3, .out = 0, .coefficient = -1},
                       {.a = 4, .b = 3, .out = e34, .coefficient = -1},
                   }));
static_assert(pair_terms_are(gp_terms<degenerate, core::full<3>, core::full<3>>,
                             1, 2,
                             {
                                 {.a = 1, .b = 2, .out = 0, .coefficient = 1},
                                 {.a = 1, .b = 2, .out = e12, .coefficient = 1},
                             }));
static_assert(pair_terms_are(gp_terms<degenerate, core::full<3>, core::full<3>>,
                             4, 4,
                             {
                                 {.a = 4, .b = 4, .out = 0, .coefficient = 1},
                             }));
static_assert(pair_terms_are(gp_terms<degenerate, core::full<3>, core::full<3>>,
                             7, 7, {}));

// Reference: independent value: the golden product table of seeded random
// form 1, in which e_i e_j + e_j e_i = 2 g_ij, a dense form with up to four
// terms in a product, so that the term lists of the plan need four places,
// for every blade times every blade with the reverse's signs on the first set
static_assert(random1::value.gram == golden::gp_tables::random_1_gram &&
              matches_table(gp_terms<random1, with_reverse_signs(core::full<3>),
                                     core::full<3>>,
                            with_reverse_signs(core::full<3>), core::full<3>,
                            golden::gp_tables::random_1));

// Reference: contract: a plan term is a trivially copyable aggregate, and
// the terms of a plan are a std::array of the counted size
static_assert(std::is_aggregate_v<plan_term>);
static_assert(std::is_trivially_copyable_v<plan_term>);
static_assert(
    std::is_same_v<
        decltype(product_plan<BinaryOperation::gp, rga3, point, point>.terms),
        std::array<core::detail::typed_term, 15>>);

// The terms of a plan, generated at run time into storage of the counted
// size, and the count of the counting pass.
struct generated_plan {
  std::size_t counted = 0;
  std::size_t written = 0;
  std::vector<plan_term> terms;
};

template <std::size_t Capacity, std::size_t N>
generated_plan generate(const core::metric<N>& g,
                        std::span<const oriented_blade> as,
                        std::span<const oriented_blade> bs) {
  generated_plan plan;
  plan.counted = generate_plan<BinaryOperation::gp, Capacity>(g, as, bs, {});
  plan.terms.resize(plan.counted);
  plan.written =
      generate_plan<BinaryOperation::gp, Capacity>(g, as, bs, plan.terms);
  return plan;
}

// The number of terms of `plan` that differ from blade_product's terms of
// their pair times the two signs, walking the pairs in order of a, then b,
// and each product in its own order; a term missing or left over counts as
// one.
template <std::size_t N>
int pair_mismatches(const core::metric<N>& g,
                    std::span<const oriented_blade> as,
                    std::span<const oriented_blade> bs,
                    std::span<const plan_term> plan) {
  int mismatches = 0;
  std::size_t k = 0;
  for (std::size_t a = 0; a < as.size(); ++a) {
    for (std::size_t b = 0; b < bs.size(); ++b) {
      const auto product =
          core::detail::blade_product<test_capacity>(g, as[a].mask, bs[b].mask);
      const coefficient_type sign = coefficient_type{as[a].sign} * bs[b].sign;
      for (std::size_t t = 0; t < product.size(); ++t) {
        const plan_term expected{
            .a = a,
            .b = b,
            .out = product.terms.at(t).blade,
            .coefficient = sign * product.terms.at(t).coefficient,
        };
        mismatches += k < plan.size() && plan[k] == expected ? 0 : 1;
        ++k;
      }
    }
  }
  if (k < plan.size()) {
    mismatches += static_cast<int>(plan.size() - k);
  }
  return mismatches;
}

// The properties a plan must have whatever its metric: no zero coefficient,
// positions within the two sets, output blades within n generators, and no
// (a, b, out) listed twice.
struct plan_properties {
  int zero_coefficients = 0;
  int beyond_sets = 0;
  int beyond_n = 0;
  int repeats = 0;
};

plan_properties properties_of(std::span<const plan_term> plan,
                              std::size_t first, std::size_t second,
                              std::size_t n) {
  plan_properties found;
  std::vector<std::tuple<std::size_t, std::size_t, mask_t>> keys;
  for (const plan_term& t : plan) {
    found.zero_coefficients += t.coefficient == 0 ? 1 : 0;
    found.beyond_sets += t.a < first && t.b < second ? 0 : 1;
    found.beyond_n += n < mask_width && (t.out >> n) != 0 ? 1 : 0;
    keys.emplace_back(t.a, t.b, t.out);
  }
  std::ranges::sort(keys);
  for (std::size_t k = 1; k < keys.size(); ++k) {
    found.repeats += keys.at(k - 1) == keys.at(k) ? 1 : 0;
  }
  return found;
}

void check_properties(std::span<const plan_term> plan, std::size_t first,
                      std::size_t second, std::size_t n) {
  const plan_properties found = properties_of(plan, first, second, n);
  CHECK(found.zero_coefficients == 0);
  CHECK(found.beyond_sets == 0);
  CHECK(found.beyond_n == 0);
  CHECK(found.repeats == 0);
}

// A sign pattern for a set: -1 on the blades of grade 2 and 3 mod 4, the
// blades the reverse negates.
std::vector<oriented_blade> reverse_signs(std::span<const oriented_blade> set) {
  std::vector<oriented_blade> signed_set(set.begin(), set.end());
  for (oriented_blade& blade : signed_set) {
    blade.sign = generators_of(blade.mask) % 4 >= 2 ? -1 : 1;
  }
  return signed_set;
}

// Another: -1 on the blades that hold e1.
std::vector<oriented_blade> e1_signs(std::span<const oriented_blade> set) {
  std::vector<oriented_blade> signed_set(set.begin(), set.end());
  for (oriented_blade& blade : signed_set) {
    blade.sign = (blade.mask & e1) != 0 ? -1 : 1;
  }
  return signed_set;
}

// Two uses of one plan, in two functions. That they share one evaluation is
// not something a test of values can see; the values are what is checked.
std::size_t point_terms() {
  return product_plan<BinaryOperation::gp, rga3, point, point>.terms.size();
}
coefficient_type point_scalar() {
  const auto& plan = product_plan<BinaryOperation::gp, rga3, point, point>;
  return plan.terms.front().coefficient;
}

} // namespace

// Reference: proof: a plan is the products of its pairs, so every term of
// every plan of every blade times every blade, of every signature of up to
// six generators listed here, is compared with blade_product of its pair,
// whose tests check it against e_i e_j + e_j e_i = 2 g_ij and associativity,
// with its coefficient times s_a s_b, for the sets with every sign +1 and
// with two different sign patterns, and the counting pass is compared with
// the number of terms written
TEST_CASE("Every plan of every blade times every blade is the products of "
          "its pairs, signs folded") {
  for_each_signature([](const char* name, const auto& g) {
    constexpr std::size_t n = std::remove_cvref_t<decltype(g)>::generator_count;
    constexpr auto& all = core::full<n>;
    constexpr std::size_t capacity =
        plan_capacity(BinaryOperation::gp, all.blades, all.blades, n);
    INFO(name);
    CHECK(capacity == blade_count(n) / 2);

    const generated_plan plain = generate<capacity>(g, all.blades, all.blades);
    CHECK(plain.counted == plain.written);
    CHECK(pair_mismatches(g, all.blades, all.blades, plain.terms) == 0);

    const std::vector<oriented_blade> first = reverse_signs(all.blades);
    const std::vector<oriented_blade> second = e1_signs(all.blades);
    const generated_plan oriented = generate<capacity>(g, first, second);
    CHECK(oriented.counted == oriented.written);
    CHECK(pair_mismatches(g, first, second, oriented.terms) == 0);
  });
}

// Reference: proof: no plan term has a zero coefficient, a position outside
// its set, an output blade beyond the algebra, or a repeated (a, b, out),
// checked here over every plan of every blade times every blade of every
// signature of up to six generators listed here and over the typed and
// sparse plans
TEST_CASE("No plan has a zero coefficient, a position or blade out of range, "
          "or a repeated term") {
  for_each_signature([](const char* name, const auto& g) {
    constexpr std::size_t n = std::remove_cvref_t<decltype(g)>::generator_count;
    constexpr auto& all = core::full<n>;
    const generated_plan plan =
        generate<test_capacity>(g, all.blades, all.blades);
    INFO(name);
    check_properties(plan.terms, all.size(), all.size(), n);
  });

  check_properties(gp_terms<rga3, point, point>, 4, 4, 4);
  check_properties(gp_terms<rga3, line, line>, 6, 6, 4);
  check_properties(gp_terms<rga3, line, plane>, 6, 4, 4);
  check_properties(gp_terms<rga3, motor, point>, 8, 4, 4);
  check_properties(gp_terms<rga3, point, motor>, 4, 8, 4);
  check_properties(gp_terms<rigid_n<16>, sparse_first<16>, sparse_second<16>>,
                   4, 3, 16);
}

// Reference: contract: product_plan and generate_plan are one constexpr
// function, so the plans of every blade times every blade of rigid(3),
// R(4,0,0), R(1,3,0), conformal(2), and a degenerate form of three
// generators, built in a constant evaluation, equal the same function run at
// run time on a metric that the compiler cannot know
TEST_CASE("product_plan equals generate_plan run at run time") {
  const auto read_at_run_time = [](const auto& fixed) {
    std::remove_cvref_t<decltype(fixed)> g{};
    for (std::size_t i = 0; i < g.gram.size(); ++i) {
      for (std::size_t j = 0; j < g.gram.size(); ++j) {
        const volatile int entry = fixed.gram.at(i).at(j);
        g.gram.at(i).at(j) = entry;
      }
    }
    return g;
  };
  constexpr const auto& all = core::full<4>;
  const auto same = [&](const auto& g, std::span<const plan_term> plan) {
    const generated_plan again = generate<8>(g, all.blades, all.blades);
    return again.written == plan.size() &&
           std::ranges::equal(again.terms, plan);
  };
  CHECK(same(read_at_run_time(rga3::value), gp_terms<rga3, all, all>));
  CHECK(same(read_at_run_time(r400::value), gp_terms<r400, all, all>));
  CHECK(same(read_at_run_time(r130::value), gp_terms<r130, all, all>));
  CHECK(same(read_at_run_time(conformal2::value),
             gp_terms<conformal2, all, all>));

  constexpr const auto& three = core::full<3>;
  const generated_plan again = generate<4>(read_at_run_time(degenerate::value),
                                           three.blades, three.blades);
  CHECK(again.written == gp_terms<degenerate, three, three>.size());
  CHECK(std::ranges::equal(again.terms, gp_terms<degenerate, three, three>));
}

// Reference: independent value: the golden product table of seeded random
// form 1, in which e_i e_j + e_j e_i = 2 g_ij, a dense form with up to four
// terms in a product, compared with product_plan and with generate_plan run
// at run time, for every blade times every blade with the reverse's signs on
// the first set
TEST_CASE("The plan of a dense form equals its golden product table") {
  constexpr auto first = with_reverse_signs(core::full<3>);
  constexpr const auto& second = core::full<3>;
  CHECK(random1::value.gram == golden::gp_tables::random_1_gram);
  CHECK(matches_table(gp_terms<random1, first, second>, first, second,
                      golden::gp_tables::random_1));

  const generated_plan again =
      generate<4>(random1::value, first.blades, second.blades);
  CHECK(again.written == gp_terms<random1, first, second>.size());
  CHECK(std::ranges::equal(again.terms, gp_terms<random1, first, second>));
}

// Reference: contract: generate_plan writes as many terms as fit in its
// output and returns the count of all, so an empty output counts and a
// short one is filled without a write past its end
TEST_CASE("generate_plan counts every term and writes only what fits") {
  const plan_term untouched{.a = 9, .b = 9, .out = 0, .coefficient = 9};
  std::array<plan_term, 5> storage{};
  storage.fill(untouched);
  const std::size_t count = generate_plan<BinaryOperation::gp, 2>(
      rga3::value, point.blades, point.blades, std::span(storage).first(3));
  CHECK(count == 15);
  CHECK(storage.at(0) == plan_term{.a = 0, .b = 0, .out = 0, .coefficient = 1});
  CHECK(storage.at(1) ==
        plan_term{.a = 0, .b = 1, .out = e12, .coefficient = 1});
  CHECK(storage.at(2) ==
        plan_term{.a = 0, .b = 2, .out = e13, .coefficient = 1});
  CHECK(storage.at(3) == untouched);
  CHECK(storage.at(4) == untouched);

  CHECK(generate_plan<BinaryOperation::gp, 2>(rga3::value, point.blades,
                                              point.blades, {}) == 15);
  std::array<plan_term, 16> roomy{};
  roomy.fill(untouched);
  CHECK(generate_plan<BinaryOperation::gp, 2>(rga3::value, point.blades,
                                              point.blades, roomy) == 15);
  CHECK(std::ranges::equal(std::span(roomy).first(15),
                           gp_terms<rga3, point, point>));
  CHECK(roomy.at(15) == untouched);
}

// Reference: independent value: a product of no blades has no terms, so a
// plan with an empty set is empty and its capacity is the smallest, 1
TEST_CASE("A plan with an empty blade set is empty") {
  constexpr core::blade_set<0> none{};
  CHECK(gp_terms<rga3, none, point>.empty());
  CHECK(gp_terms<rga3, point, none>.empty());
  CHECK(gp_terms<rga3, none, none>.empty());
  CHECK(generate_plan<BinaryOperation::gp, 1>(rga3::value, none.blades,
                                              point.blades, {}) == 0);
  CHECK(plan_capacity(BinaryOperation::gp, none.blades, point.blades, 4) == 1);
  CHECK(plan_capacity(BinaryOperation::gp, point.blades, none.blades, 4) == 1);
}

// Reference: contract: a mask listed twice in a set is not rejected, and
// each place that lists it gets its own terms with its own sign, here
// e1 e1 = 1 in R(3,0,0) for +e1 and -e1 at two places
TEST_CASE("A repeated blade gets the terms of each place that lists it") {
  constexpr core::blade_set<3> repeated{
      {
          {
              {.mask = e1, .sign = 1},
              {.mask = e2, .sign = 1},
              {.mask = e1, .sign = -1},
          },
      },
  };
  constexpr core::blade_set<1> just_e1{{{{.mask = e1, .sign = 1}}}};
  CHECK(gp_terms<r300, repeated, just_e1> ==
        std::array<plan_term, 3>{{
            {.a = 0, .b = 0, .out = 0, .coefficient = 1},
            {.a = 1, .b = 0, .out = e12, .coefficient = -1},
            {.a = 2, .b = 0, .out = 0, .coefficient = -1},
        }});
}

// Reference: independent value: e_A e_B has at most half the subsets of the
// generators of A and B together, counted here, so the capacity of a plan is
// the largest of these over its pairs, wherever that pair lies in the sets
TEST_CASE("plan_capacity is the largest bound over the pairs") {
  CHECK(plan_capacity(BinaryOperation::gp, point.blades, point.blades, 4) ==
        largest_pair_bound(point, point));
  CHECK(plan_capacity(BinaryOperation::gp, point.blades, point.blades, 4) == 2);
  CHECK(plan_capacity(BinaryOperation::gp, line.blades, plane.blades, 4) == 8);
  CHECK(plan_capacity(BinaryOperation::gp, motor.blades, point.blades, 4) == 8);
  CHECK(plan_capacity(BinaryOperation::gp, sparse_first<16>.blades,
                      sparse_second<16>.blades, 16) == 4);
  CHECK(plan_capacity(BinaryOperation::gp, sparse_first<16>.blades,
                      sparse_second<16>.blades, 16) ==
        largest_pair_bound(sparse_first<16>, sparse_second<16>));

  // The largest pair in the middle of each set: e123 with e1 needs 4.
  constexpr core::blade_set<3> middle{
      {
          {
              {.mask = e1, .sign = 1},
              {.mask = e123, .sign = 1},
              {.mask = e2, .sign = 1},
          },
      },
  };
  constexpr core::blade_set<1> just_e1{{{{.mask = e1, .sign = 1}}}};
  CHECK(plan_capacity(BinaryOperation::gp, middle.blades, just_e1.blades, 3) ==
        4);
  CHECK(plan_capacity(BinaryOperation::gp, just_e1.blades, middle.blades, 3) ==
        4);
  CHECK(plan_capacity(BinaryOperation::gp, just_e1.blades, just_e1.blades, 3) ==
        1);
  constexpr core::blade_set<1> just_e2{{{{.mask = e2, .sign = 1}}}};
  CHECK(plan_capacity(BinaryOperation::gp, just_e1.blades, just_e2.blades, 3) ==
        2);
}

// Reference: independent value: in R(32,0,0), the widest algebra a mask
// holds, e32 e32 = 1, so a blade on the top bit is within the algebra
TEST_CASE("product_plan accepts the generators of the widest algebra") {
  constexpr mask_t top = mask_t{1} << (mask_width - 1);
  constexpr core::blade_set<2> tops{
      {
          {
              {.mask = e1, .sign = 1},
              {.mask = top, .sign = -1},
          },
      },
  };
  CHECK(gp_terms<widest, tops, tops> ==
        std::array<plan_term, 4>{{
            {.a = 0, .b = 0, .out = 0, .coefficient = 1},
            {.a = 0, .b = 1, .out = e1 | top, .coefficient = -1},
            {.a = 1, .b = 0, .out = e1 | top, .coefficient = 1},
            {.a = 1, .b = 1, .out = 0, .coefficient = 1},
        }});
}

// Reference: independent value: two functions that read the point times
// point plan of rigid(3) each read its 15 terms, worked by hand, with
// e1 e1 = 1 first
TEST_CASE("Two functions that use one plan read its values") {
  CHECK(point_terms() == 15);
  CHECK(point_scalar() == 1);
}

// Reference: contract: the check of a generated plan reports a zero
// coefficient, a position at or past the size of its set, a blade beyond
// e_n, and terms out of the order a, then b, then strictly increasing
// canonical order of out, and accepts each value just within its bound
TEST_CASE("The checks of a generated plan find each defect at its bound") {
  using core::detail::defects_of;
  constexpr std::size_t n = 4;
  const auto one = [](plan_term t) { return std::array<plan_term, 1>{t}; };

  const auto in_bounds = defects_of(
      one({.a = 2, .b = 1, .out = 0b1111, .coefficient = -1}), 3, 2, n);
  CHECK_FALSE(in_bounds.zero_coefficient);
  CHECK_FALSE(in_bounds.beyond_first);
  CHECK_FALSE(in_bounds.beyond_second);
  CHECK_FALSE(in_bounds.beyond_n);
  CHECK_FALSE(in_bounds.out_of_order);

  CHECK(defects_of(one({.a = 0, .b = 0, .out = 0, .coefficient = 0}), 1, 1, n)
            .zero_coefficient);
  CHECK(defects_of(one({.a = 3, .b = 0, .out = 0, .coefficient = 1}), 3, 2, n)
            .beyond_first);
  CHECK(defects_of(one({.a = 0, .b = 2, .out = 0, .coefficient = 1}), 3, 2, n)
            .beyond_second);
  CHECK(defects_of(one({.a = 0, .b = 0, .out = 0b10000, .coefficient = 1}), 1,
                   1, n)
            .beyond_n);
  CHECK_FALSE(defects_of(one({.a = 0,
                              .b = 0,
                              .out = mask_t{1} << (mask_width - 1),
                              .coefficient = 1}),
                         1, 1, mask_width)
                  .beyond_n);

  const auto two = [](plan_term first, plan_term second) {
    return std::array<plan_term, 2>{first, second};
  };
  const auto ordered = [&](plan_term first, plan_term second) {
    return !defects_of(two(first, second), 3, 3, n).out_of_order;
  };
  // Within a pair, canonical order: e4 before e12, and e14 before e23.
  CHECK(ordered({.a = 0, .b = 0, .out = e4, .coefficient = 1},
                {.a = 0, .b = 0, .out = e12, .coefficient = 1}));
  CHECK(ordered({.a = 0, .b = 0, .out = e14, .coefficient = 1},
                {.a = 0, .b = 0, .out = e23, .coefficient = 1}));
  CHECK_FALSE(ordered({.a = 0, .b = 0, .out = e23, .coefficient = 1},
                      {.a = 0, .b = 0, .out = e14, .coefficient = 1}));
  CHECK_FALSE(ordered({.a = 0, .b = 0, .out = e12, .coefficient = 1},
                      {.a = 0, .b = 0, .out = e12, .coefficient = 2}));
  // A later pair may start at any blade.
  CHECK(ordered({.a = 0, .b = 0, .out = e23, .coefficient = 1},
                {.a = 0, .b = 1, .out = 0, .coefficient = 1}));
  CHECK(ordered({.a = 0, .b = 2, .out = e23, .coefficient = 1},
                {.a = 1, .b = 0, .out = 0, .coefficient = 1}));
  CHECK_FALSE(ordered({.a = 0, .b = 1, .out = 0, .coefficient = 1},
                      {.a = 0, .b = 0, .out = e1, .coefficient = 1}));
  CHECK_FALSE(ordered({.a = 1, .b = 0, .out = 0, .coefficient = 1},
                      {.a = 0, .b = 2, .out = e1, .coefficient = 1}));
}

// The typed plans, and the wedge.
//
// A typed plan holds a result set, every blade a term lands on, once, with
// sign +1, in canonical order, and its terms name a position in that set in
// place of a mask. The checks below read each typed plan back through its
// result set and compare it with the mask-level plan of the same generator,
// order the result set by a rule written here, and compare the wedge with
// its definition: zero when the blades share a generator, and otherwise the
// sign of the inversions of e_A e_B on e_(A | B), counted here.
namespace {

using core::detail::collect_result;
using core::detail::type_terms;
using core::detail::typed_plan;
using core::detail::typed_term;

struct conformal3 {
  static constexpr auto value = core::conformal<3>();
};

constexpr mask_t e3 = 0b0100;
constexpr mask_t e124 = 0b1011;
constexpr mask_t e134 = 0b1101;
constexpr mask_t e234 = 0b1110;
constexpr mask_t e1234 = 0b1111;
constexpr mask_t e5 = 0b10000;

constexpr auto gp = BinaryOperation::gp;
constexpr auto wedge = BinaryOperation::wedge;

// Whether `result` holds the blades of `masks`, in that order, each with sign
// +1.
template <std::size_t R>
constexpr bool result_is(const core::blade_set<R>& result,
                         std::initializer_list<mask_t> masks) {
  if (masks.size() != R) {
    return false;
  }
  std::size_t k = 0;
  for (const mask_t mask : masks) {
    if (result.blades.at(k) != oriented_blade{.mask = mask, .sign = 1}) {
      return false;
    }
    ++k;
  }
  return true;
}

// Whether `plan` is, in order, the wedge of every pair of `as` and `bs`:
// nothing when the two blades share a generator, and otherwise s_a s_b times
// the sign of the inversions on e_(A | B).
template <std::size_t KA, std::size_t KB, std::size_t T>
constexpr bool follows_wedge_rule(const core::blade_set<KA>& as,
                                  const core::blade_set<KB>& bs,
                                  const std::array<plan_term, T>& plan) {
  std::size_t k = 0;
  for (std::size_t a = 0; a < KA; ++a) {
    for (std::size_t b = 0; b < KB; ++b) {
      const mask_t ma = as.blades.at(a).mask;
      const mask_t mb = bs.blades.at(b).mask;
      if ((ma & mb) != 0) {
        continue;
      }
      const plan_term expected{
          .a = a,
          .b = b,
          .out = ma | mb,
          .coefficient = coefficient_type{as.blades.at(a).sign} *
                         bs.blades.at(b).sign * parity_sign(inversions(ma, mb)),
      };
      if (k == T || plan.at(k) != expected) {
        return false;
      }
      ++k;
    }
  }
  return k == T;
}

// The wedge plan of A and B in Alg, read back.
template <core::metric_like Alg, core::blade_set A, core::blade_set B>
constexpr auto wedge_terms =
    read_back(product_plan<BinaryOperation::wedge, Alg, A, B>);

// Whether a typed plan is what generate_plan, collect_result, and type_terms
// give when they are called directly on the same sets in the metric `g`.
template <BinaryOperation Op, std::size_t Capacity, std::size_t N,
          std::size_t KA, std::size_t KB, std::size_t R, std::size_t T>
constexpr bool
equals_typing(const core::metric<N>& g, const core::blade_set<KA>& as,
              const core::blade_set<KB>& bs, const typed_plan<R, T>& plan) {
  std::array<plan_term, T> terms{};
  if (generate_plan<Op, Capacity>(g, as.blades, bs.blades, terms) != T) {
    return false;
  }
  std::array<oriented_blade, T> result{};
  if (collect_result(terms, result) != R) {
    return false;
  }
  for (std::size_t r = 0; r < R; ++r) {
    if (result.at(r) != plan.result.blades.at(r)) {
      return false;
    }
  }
  std::array<typed_term, T> typed{};
  type_terms(terms, std::span(result).first(R), typed);
  return typed == plan.terms;
}

// Reference: independent value: e_i e_i = 1 for i < 4 and e4 e4 = 0 in
// rigid(3), e_j e_i = -e_i e_j for i < j, and e_i ^ e_i = 0, worked by hand:
// a point times a point has the scalar and the six bivectors, in that order,
// and 15 terms; a point wedge a point drops the three squares and the scalar
// and has 12; each term names the position of its blade
static_assert(product_plan<gp, rga3, point, point> ==
              typed_plan<7, 15>{
                  .result =
                      {
                          {
                              {
                                  {.mask = 0, .sign = 1},
                                  {.mask = e12, .sign = 1},
                                  {.mask = e13, .sign = 1},
                                  {.mask = e14, .sign = 1},
                                  {.mask = e23, .sign = 1},
                                  {.mask = e24, .sign = 1},
                                  {.mask = e34, .sign = 1},
                              },
                          },
                      },
                  .terms =
                      {
                          {
                              {.a = 0, .b = 0, .out = 0, .coefficient = 1},
                              {.a = 0, .b = 1, .out = 1, .coefficient = 1},
                              {.a = 0, .b = 2, .out = 2, .coefficient = 1},
                              {.a = 0, .b = 3, .out = 3, .coefficient = 1},
                              {.a = 1, .b = 0, .out = 1, .coefficient = -1},
                              {.a = 1, .b = 1, .out = 0, .coefficient = 1},
                              {.a = 1, .b = 2, .out = 4, .coefficient = 1},
                              {.a = 1, .b = 3, .out = 5, .coefficient = 1},
                              {.a = 2, .b = 0, .out = 2, .coefficient = -1},
                              {.a = 2, .b = 1, .out = 4, .coefficient = -1},
                              {.a = 2, .b = 2, .out = 0, .coefficient = 1},
                              {.a = 2, .b = 3, .out = 6, .coefficient = 1},
                              {.a = 3, .b = 0, .out = 3, .coefficient = -1},
                              {.a = 3, .b = 1, .out = 5, .coefficient = -1},
                              {.a = 3, .b = 2, .out = 6, .coefficient = -1},
                          },
                      },
              });
static_assert(product_plan<wedge, rga3, point, point> ==
              typed_plan<6, 12>{
                  .result =
                      {
                          {
                              {
                                  {.mask = e12, .sign = 1},
                                  {.mask = e13, .sign = 1},
                                  {.mask = e14, .sign = 1},
                                  {.mask = e23, .sign = 1},
                                  {.mask = e24, .sign = 1},
                                  {.mask = e34, .sign = 1},
                              },
                          },
                      },
                  .terms =
                      {
                          {
                              {.a = 0, .b = 1, .out = 0, .coefficient = 1},
                              {.a = 0, .b = 2, .out = 1, .coefficient = 1},
                              {.a = 0, .b = 3, .out = 2, .coefficient = 1},
                              {.a = 1, .b = 0, .out = 0, .coefficient = -1},
                              {.a = 1, .b = 2, .out = 3, .coefficient = 1},
                              {.a = 1, .b = 3, .out = 4, .coefficient = 1},
                              {.a = 2, .b = 0, .out = 1, .coefficient = -1},
                              {.a = 2, .b = 1, .out = 3, .coefficient = -1},
                              {.a = 2, .b = 3, .out = 5, .coefficient = 1},
                              {.a = 3, .b = 0, .out = 2, .coefficient = -1},
                              {.a = 3, .b = 1, .out = 4, .coefficient = -1},
                              {.a = 3, .b = 2, .out = 5, .coefficient = -1},
                          },
                      },
              });

// Reference: independent value: in rigid(3) a product of blades is one term
// on e_(A xor B) unless both hold e4, and a wedge is one term on e_(A | B)
// unless they share a generator, so, counted by hand: a line times a plane
// has 6 x 4 - 3 x 3 = 15 terms on the four vectors and the three trivectors
// with e4, and a line wedge a plane none, since grades 2 and 3 exceed 4; a
// point wedge a line has 4 x 3 = 12 terms on the four trivectors; a motor
// times a point has 8 x 4 - 4 = 28 terms and a motor wedge a point
// 4 + 6 x 2 = 16, both on the vectors and the trivectors; a line times a
// line has 6 x 6 - 3 x 3 = 27 terms on the scalar, the bivectors, and e1234,
// and a line wedge a line 6, on e1234. Each term is the rule of the
// operation, s_a s_b times the sign of reordering (and for the product the
// common squares), counted here pair by pair
static_assert(product_plan<gp, rga3, line, plane>.terms.size() == 15);
static_assert(result_is(product_plan<gp, rga3, line, plane>.result,
                        {e1, e2, e3, e4, e124, e134, e234}));
static_assert(follows_diagonal_rule(rga3::value, line, plane,
                                    gp_terms<rga3, line, plane>));
static_assert(product_plan<wedge, rga3, line, plane>.terms.empty());
static_assert(result_is(product_plan<wedge, rga3, line, plane>.result, {}));
static_assert(product_plan<wedge, rga3, point, line>.terms.size() == 12);
static_assert(result_is(product_plan<wedge, rga3, point, line>.result,
                        {e123, e124, e134, e234}));
static_assert(follows_wedge_rule(point, line, wedge_terms<rga3, point, line>));
static_assert(product_plan<gp, rga3, motor, point>.terms.size() == 28);
static_assert(result_is(product_plan<gp, rga3, motor, point>.result,
                        {e1, e2, e3, e4, e123, e124, e134, e234}));
static_assert(product_plan<wedge, rga3, motor, point>.terms.size() == 16);
static_assert(result_is(product_plan<wedge, rga3, motor, point>.result,
                        {e1, e2, e3, e4, e123, e124, e134, e234}));
static_assert(follows_wedge_rule(motor, point,
                                 wedge_terms<rga3, motor, point>));
static_assert(product_plan<gp, rga3, line, line>.terms.size() == 27);
static_assert(result_is(product_plan<gp, rga3, line, line>.result,
                        {0, e12, e13, e14, e23, e24, e34, e1234}));
static_assert(product_plan<wedge, rga3, line, line>.terms.size() == 6);
static_assert(result_is(product_plan<wedge, rga3, line, line>.result, {e1234}));
static_assert(follows_wedge_rule(line, line, wedge_terms<rga3, line, line>));

// Reference: independent value: worked by hand from e4 . e5 = -1, e4 e4 =
// e5 e5 = 0, and e_i e_i = 1 for i < 4 in conformal(3): e4 e5 = -1 + e45 and
// e5 e4 = -1 - e45, so the vectors times the vectors have 3 + 20 + 2 = 25
// terms on the scalar and the ten bivectors; e4 ^ e5 = e45, so the vectors
// wedge the vectors have 20 terms on the ten bivectors and no scalar
static_assert(product_plan<gp, conformal3, core::of_grade<5, 1>,
                           core::of_grade<5, 1>>.terms.size() == 25);
static_assert(result_is(
    product_plan<gp, conformal3, core::of_grade<5, 1>, core::of_grade<5, 1>>.result,
    {0, e12, e13, e14, e1 | e5, e23, e24, e2 | e5, e34, e3 | e5, e4 | e5}));
static_assert(pair_terms_are(
    gp_terms<conformal3, core::of_grade<5, 1>, core::of_grade<5, 1>>, 3, 4,
    {
        {.a = 3, .b = 4, .out = 0, .coefficient = -1},
        {.a = 3, .b = 4, .out = e4 | e5, .coefficient = 1},
    }));
static_assert(pair_terms_are(
    gp_terms<conformal3, core::of_grade<5, 1>, core::of_grade<5, 1>>, 4, 3,
    {
        {.a = 4, .b = 3, .out = 0, .coefficient = -1},
        {.a = 4, .b = 3, .out = e4 | e5, .coefficient = -1},
    }));
static_assert(pair_terms_are(
    gp_terms<conformal3, core::of_grade<5, 1>, core::of_grade<5, 1>>, 3, 3,
    {}));
static_assert(product_plan<wedge, conformal3, core::of_grade<5, 1>,
                           core::of_grade<5, 1>>.terms.size() == 20);
static_assert(result_is(
    product_plan<wedge, conformal3, core::of_grade<5, 1>, core::of_grade<5, 1>>.result,
    {e12, e13, e14, e1 | e5, e23, e24, e2 | e5, e34, e3 | e5, e4 | e5}));
static_assert(follows_wedge_rule(
    core::of_grade<5, 1>, core::of_grade<5, 1>,
    wedge_terms<conformal3, core::of_grade<5, 1>, core::of_grade<5, 1>>));

// The eight wedge terms of the sparse sets {1, e1, e_n, e1 ^ e_n} and
// {e1, e2, e_n}, worked by hand: the ten terms of their product less
// e1 ^ e1 = 0 and e1n ^ e1 = 0, the two pairs that share a generator whose
// square is not zero.
template <std::size_t N>
constexpr std::array<plan_term, 8> sparse_wedge_terms() {
  constexpr mask_t en = mask_t{1} << (N - 1);
  return {
      {
          {.a = 0, .b = 0, .out = e1, .coefficient = 1},
          {.a = 0, .b = 1, .out = e2, .coefficient = 1},
          {.a = 0, .b = 2, .out = en, .coefficient = 1},
          {.a = 1, .b = 1, .out = e12, .coefficient = 1},
          {.a = 1, .b = 2, .out = e1 | en, .coefficient = 1},
          {.a = 2, .b = 0, .out = e1 | en, .coefficient = -1},
          {.a = 2, .b = 1, .out = e2 | en, .coefficient = -1},
          {.a = 3, .b = 1, .out = e12 | en, .coefficient = -1},
      },
  };
}

// Whether the sparse plans of both operations at n = N have the terms worked
// by hand and the result sets (1, e1, e2, e_n, e12, e1n, e2n, e12n) and the
// same without the scalar, in canonical order.
template <std::size_t N>
constexpr bool sparse_plans_hold() {
  constexpr mask_t en = mask_t{1} << (N - 1);
  constexpr auto& product =
      product_plan<gp, rigid_n<N>, sparse_first<N>, sparse_second<N>>;
  constexpr auto& exterior =
      product_plan<wedge, rigid_n<N>, sparse_first<N>, sparse_second<N>>;
  return read_back(product) == sparse_terms<N>() &&
         result_is(product.result,
                   {0, e1, e2, en, e12, e1 | en, e2 | en, e12 | en}) &&
         read_back(exterior) == sparse_wedge_terms<N>() &&
         result_is(exterior.result,
                   {e1, e2, en, e12, e1 | en, e2 | en, e12 | en});
}

// Reference: independent value: the sparse sets {1, e1, e_n, e1 ^ e_n} and
// {e1, e2, e_n} in R(n - 1, 0, 1), where e_n e_n = 0, give ten product terms
// and eight wedge terms worked by hand, on eight and seven result blades, at
// n = 8, 12, and 16, so that a typed plan in a large algebra costs what its
// sets use
static_assert(sparse_plans_hold<8>());
static_assert(sparse_plans_hold<12>());
static_assert(sparse_plans_hold<16>());

// Reference: contract: results are ordered by grade, then by index list, so
// the result of a point wedge a point lists e14 before e23, although the mask
// of e23, 0b0110, is below that of e14, 0b1001
static_assert(
    product_plan<wedge, rga3, point, point>.result.blades.at(2).mask == e14);
static_assert(
    product_plan<wedge, rga3, point, point>.result.blades.at(3).mask == e23);

// Reference: contract: a typed plan is what generate_plan, collect_result,
// and type_terms give when called directly, so the typed plans of both
// operations over every blade times every blade of rigid(3), R(4,0,0),
// R(1,3,0), conformal(2), and a degenerate form of three generators equal a
// direct call in a constant evaluation
static_assert(
    equals_typing<gp, 8>(rga3::value, core::full<4>, core::full<4>,
                         product_plan<gp, rga3, core::full<4>, core::full<4>>));
static_assert(equals_typing<wedge, 1>(
    rga3::value, core::full<4>, core::full<4>,
    product_plan<wedge, rga3, core::full<4>, core::full<4>>));
static_assert(
    equals_typing<gp, 8>(r400::value, core::full<4>, core::full<4>,
                         product_plan<gp, r400, core::full<4>, core::full<4>>));
static_assert(equals_typing<wedge, 1>(
    r400::value, core::full<4>, core::full<4>,
    product_plan<wedge, r400, core::full<4>, core::full<4>>));
static_assert(
    equals_typing<gp, 8>(r130::value, core::full<4>, core::full<4>,
                         product_plan<gp, r130, core::full<4>, core::full<4>>));
static_assert(equals_typing<wedge, 1>(
    r130::value, core::full<4>, core::full<4>,
    product_plan<wedge, r130, core::full<4>, core::full<4>>));
static_assert(equals_typing<gp, 8>(
    conformal2::value, core::full<4>, core::full<4>,
    product_plan<gp, conformal2, core::full<4>, core::full<4>>));
static_assert(equals_typing<wedge, 1>(
    conformal2::value, core::full<4>, core::full<4>,
    product_plan<wedge, conformal2, core::full<4>, core::full<4>>));
static_assert(equals_typing<gp, 4>(
    degenerate::value, core::full<3>, core::full<3>,
    product_plan<gp, degenerate, core::full<3>, core::full<3>>));
static_assert(equals_typing<wedge, 1>(
    degenerate::value, core::full<3>, core::full<3>,
    product_plan<wedge, degenerate, core::full<3>, core::full<3>>));

// Reference: contract: a typed term is a trivially copyable aggregate, and a
// typed plan is a structural one, so that it can be a template argument
static_assert(std::is_aggregate_v<typed_term>);
static_assert(std::is_trivially_copyable_v<typed_term>);
static_assert(std::is_aggregate_v<typed_plan<7, 15>>);
static_assert(std::is_trivially_copyable_v<typed_plan<7, 15>>);
template <typed_plan<7, 15> Plan>
constexpr std::size_t terms_of = Plan.terms.size();
static_assert(terms_of<product_plan<gp, rga3, point, point>> == 15);

// The mask-level plan of `Op`, generated at run time.
template <BinaryOperation Op, std::size_t Capacity, std::size_t N>
std::vector<plan_term> plan_of(const core::metric<N>& g,
                               std::span<const oriented_blade> as,
                               std::span<const oriented_blade> bs) {
  std::vector<plan_term> terms(generate_plan<Op, Capacity>(g, as, bs, {}));
  generate_plan<Op, Capacity>(g, as, bs, terms);
  return terms;
}

// A plan typed at run time.
struct typing {
  std::vector<oriented_blade> result;
  std::vector<typed_term> terms;
};

typing typed_at_run_time(std::span<const plan_term> plan) {
  typing typed;
  typed.result.resize(plan.size());
  typed.result.resize(collect_result(plan, typed.result));
  typed.terms.resize(plan.size());
  type_terms(plan, typed.result, typed.terms);
  return typed;
}

// The indices of the generators of a mask, increasing.
std::vector<std::size_t> indices_of(mask_t mask) {
  std::vector<std::size_t> indices;
  for (std::size_t bit = 0; bit < mask_width; ++bit) {
    if (((mask >> bit) & 1U) == 1U) {
      indices.push_back(bit + 1);
    }
  }
  return indices;
}

// Canonical order, written here: lower grade first, and within a grade the
// lexicographically smaller list of indices.
bool comes_before(mask_t a, mask_t b) {
  if (generators_of(a) != generators_of(b)) {
    return generators_of(a) < generators_of(b);
  }
  return std::ranges::lexicographical_compare(indices_of(a), indices_of(b));
}

// The number of ways in which `typed` differs from `plan` read through it:
// its result set is not the distinct output blades of `plan` in the order
// written here with sign +1, or a term does not read back as the term of
// `plan` at its place, or names a position outside the result set.
int typing_mismatches(std::span<const plan_term> plan, const typing& typed) {
  std::vector<mask_t> outputs;
  for (const plan_term& t : plan) {
    outputs.push_back(t.out);
  }
  std::ranges::sort(outputs);
  const auto repeats = std::ranges::unique(outputs);
  outputs.erase(repeats.begin(), repeats.end());
  std::ranges::sort(outputs, comes_before);

  int mismatches = 0;
  if (typed.result.size() != outputs.size()) {
    ++mismatches;
  } else {
    for (std::size_t r = 0; r < outputs.size(); ++r) {
      const oriented_blade expected{.mask = outputs.at(r), .sign = 1};
      mismatches += typed.result.at(r) == expected ? 0 : 1;
    }
  }
  if (typed.terms.size() != plan.size()) {
    return mismatches + 1;
  }
  for (std::size_t k = 0; k < plan.size(); ++k) {
    const typed_term& t = typed.terms.at(k);
    const bool reads_back = t.a == plan[k].a && t.b == plan[k].b &&
                            t.coefficient == plan[k].coefficient &&
                            t.out < typed.result.size() &&
                            typed.result.at(t.out).mask == plan[k].out;
    mismatches += reads_back ? 0 : 1;
  }
  return mismatches;
}

// The terms of the geometric product of e_A and e_B of the grade
// |A| + |B|, with the pair's positions.
template <std::size_t N>
std::vector<plan_term> top_grade_part(const core::metric<N>& g, std::size_t a,
                                      std::size_t b, mask_t ma, mask_t mb) {
  std::vector<plan_term> part;
  const auto product = core::detail::blade_product<test_capacity>(g, ma, mb);
  for (std::size_t t = 0; t < product.size(); ++t) {
    const core::detail::term& term = product.terms.at(t);
    if (generators_of(term.blade) == generators_of(ma) + generators_of(mb)) {
      part.push_back(
          {.a = a, .b = b, .out = term.blade, .coefficient = term.coefficient});
    }
  }
  return part;
}

// The wedge plan of every blade times every blade of R(0,0,N), where every
// generator squares to 0, against the geometric-product plan.
template <std::size_t N>
bool grassmann_wedge_is_product() {
  constexpr auto g = core::R<0, 0, N>();
  constexpr auto& all = core::full<N>;
  return plan_of<wedge, 1>(g, all.blades, all.blades) ==
         plan_of<gp, blade_count(N) / 2>(g, all.blades, all.blades);
}

// The number of triples of blades of full<n> on which (e_A ^ e_B) ^ e_C and
// e_A ^ (e_B ^ e_C), each multiplied out over the wedge plan, differ.
template <std::size_t N>
int wedge_associativity_failures(const core::metric<N>& g) {
  constexpr auto& all = core::full<N>;
  constexpr std::size_t count = blade_count(N);
  const std::vector<plan_term> plan =
      plan_of<wedge, 1>(g, all.blades, all.blades);

  // The one term of each pair, or a zero coefficient when there is none,
  // and the position of each mask in full<n>.
  std::vector<plan_term> pair(count * count);
  for (const plan_term& t : plan) {
    pair.at((t.a * count) + t.b) = t;
  }
  std::vector<std::size_t> position(count);
  for (std::size_t k = 0; k < count; ++k) {
    position.at(all.blades.at(k).mask) = k;
  }
  const auto times = [&](std::size_t x, std::size_t y) -> const plan_term& {
    return pair.at((x * count) + y);
  };
  const auto blade = [&](mask_t mask) { return position.at(mask); };

  int failures = 0;
  for (std::size_t a = 0; a < count; ++a) {
    for (std::size_t b = 0; b < count; ++b) {
      for (std::size_t c = 0; c < count; ++c) {
        coefficient_type left = 0;
        mask_t left_out = 0;
        const plan_term& ab = times(a, b);
        if (ab.coefficient != 0) {
          const plan_term& abc = times(blade(ab.out), c);
          left = ab.coefficient * abc.coefficient;
          left_out = abc.out;
        }
        coefficient_type right = 0;
        mask_t right_out = 0;
        const plan_term& bc = times(b, c);
        if (bc.coefficient != 0) {
          const plan_term& abc = times(a, blade(bc.out));
          right = bc.coefficient * abc.coefficient;
          right_out = abc.out;
        }
        const bool same = left == right && (left == 0 || left_out == right_out);
        failures += same ? 0 : 1;
      }
    }
  }
  return failures;
}

// The number of pairs of the golden table of a form of N generators on
// which the wedge plan, in that form, is not the golden product's terms of
// the grade |A| + |B|.
template <std::size_t N, std::size_t Rows>
int golden_wedge_mismatches(const std::array<std::array<int, N>, N>& gram,
                            const std::array<golden::product, Rows>& table) {
  core::metric<N> g{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      g.gram.at(i).at(j) = gram.at(i).at(j);
    }
  }
  constexpr auto& all = core::full<N>;
  const std::vector<plan_term> plan =
      plan_of<wedge, 1>(g, all.blades, all.blades);
  int mismatches = 0;
  std::size_t k = 0;
  for (std::size_t a = 0; a < all.size(); ++a) {
    for (std::size_t b = 0; b < all.size(); ++b) {
      const mask_t ma = all.blades.at(a).mask;
      const mask_t mb = all.blades.at(b).mask;
      const auto row =
          std::ranges::find_if(table, [&](const golden::product& p) {
            return p.a == ma && p.b == mb;
          });
      if (row == table.end()) {
        ++mismatches;
        continue;
      }
      for (std::size_t t = 0; t < row->size; ++t) {
        const auto& term = row->terms.at(t);
        if (generators_of(term.blade) !=
            generators_of(ma) + generators_of(mb)) {
          continue;
        }
        const plan_term expected{
            .a = a,
            .b = b,
            .out = term.blade,
            .coefficient = term.coefficient,
        };
        mismatches += k < plan.size() && plan.at(k) == expected ? 0 : 1;
        ++k;
      }
    }
  }
  return mismatches + static_cast<int>(plan.size() - std::min(k, plan.size()));
}

// A copy of the metric `fixed` that the compiler cannot know, read through
// volatile.
template <std::size_t N>
core::metric<N> at_run_time(const core::metric<N>& fixed) {
  core::metric<N> g{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      const volatile int entry = fixed.gram.at(i).at(j);
      g.gram.at(i).at(j) = entry;
    }
  }
  return g;
}

} // namespace

// Reference: proof: a typed plan is its mask-level plan read through its
// result set, so for both operations, for every blade times every blade of
// every signature of up to six generators listed here, typed at run time by
// the functions the compile-time plan uses, the result set is the distinct
// output blades, each with sign +1, ordered here by grade and then by index
// list, and every term reads back as the generated term at its place
TEST_CASE("Every typed plan reads back as its generated terms, on a result "
          "set of exactly their blades in canonical order") {
  for_each_signature([](const char* name, const auto& g) {
    constexpr std::size_t n = std::remove_cvref_t<decltype(g)>::generator_count;
    constexpr auto& all = core::full<n>;
    INFO(name);
    const std::vector<plan_term> product =
        plan_of<gp, blade_count(n) / 2>(g, all.blades, all.blades);
    CHECK(typing_mismatches(product, typed_at_run_time(product)) == 0);
    const std::vector<plan_term> exterior =
        plan_of<wedge, 1>(g, all.blades, all.blades);
    CHECK(typing_mismatches(exterior, typed_at_run_time(exterior)) == 0);
  });
}

// Reference: proof: the wedge of two blades that share a generator is zero,
// and otherwise e_A ^ e_B is the sign of reordering e_A e_B, counted here,
// times e_(A | B); identity: e_A ^ e_B is the part of e_A e_B of grade
// |A| + |B|, it does not depend on the metric, and where every generator
// squares to 0 it is the geometric product. Checked for every blade times
// every blade of every signature of up to six generators listed here, and of
// R(0,0,n) for n = 1 to 6
TEST_CASE("The wedge plan is the exterior product of each pair") {
  std::array<std::vector<plan_term>, 7> first_of_size;
  for_each_signature([&first_of_size](const char* name, const auto& g) {
    constexpr std::size_t n = std::remove_cvref_t<decltype(g)>::generator_count;
    constexpr auto& all = core::full<n>;
    INFO(name);
    const std::vector<plan_term> plan =
        plan_of<wedge, 1>(g, all.blades, all.blades);

    std::vector<plan_term> rule;
    std::vector<plan_term> top_grade;
    for (std::size_t a = 0; a < all.size(); ++a) {
      for (std::size_t b = 0; b < all.size(); ++b) {
        const mask_t ma = all.blades.at(a).mask;
        const mask_t mb = all.blades.at(b).mask;
        if ((ma & mb) == 0) {
          rule.push_back({
              .a = a,
              .b = b,
              .out = ma | mb,
              .coefficient = parity_sign(inversions(ma, mb)),
          });
        }
        for (const plan_term& t : top_grade_part(g, a, b, ma, mb)) {
          top_grade.push_back(t);
        }
      }
    }
    CHECK(plan == rule);
    CHECK(plan == top_grade);

    std::vector<plan_term>& first = first_of_size.at(n);
    if (first.empty()) {
      first = plan;
    }
    CHECK(plan == first);
  });

  CHECK(grassmann_wedge_is_product<1>());
  CHECK(grassmann_wedge_is_product<2>());
  CHECK(grassmann_wedge_is_product<3>());
  CHECK(grassmann_wedge_is_product<4>());
  CHECK(grassmann_wedge_is_product<5>());
  CHECK(grassmann_wedge_is_product<6>());
}

// Reference: identity: the exterior product is associative, checked on every
// triple of blades of every signature of up to six generators listed here,
// each side multiplied out over the wedge plan
TEST_CASE("The wedge plan is associative on every triple of blades") {
  for_each_signature([](const char* name, const auto& g) {
    INFO(name);
    CHECK(wedge_associativity_failures(g) == 0);
  });
}

// Reference: independent value: the golden product tables, in which
// e_i e_j + e_j e_i = 2 g_ij, of rigid(3), conformal(3), a degenerate form,
// and seeded random form 1: for every pair of blades the wedge plan in that
// form is the golden product's terms of grade |A| + |B|
TEST_CASE("The wedge plan is the top grade of the golden products") {
  CHECK(golden_wedge_mismatches(golden::gp_tables::rigid_3_gram,
                                golden::gp_tables::rigid_3) == 0);
  CHECK(golden_wedge_mismatches(golden::gp_tables::conformal_3_gram,
                                golden::gp_tables::conformal_3) == 0);
  CHECK(golden_wedge_mismatches(golden::gp_tables::degenerate_3_gram,
                                golden::gp_tables::degenerate_3) == 0);
  CHECK(golden_wedge_mismatches(golden::gp_tables::random_1_gram,
                                golden::gp_tables::random_1) == 0);
}

// Reference: independent value: results are ordered by grade, then by index
// list, as the golden lists of every blade of four and of five generators
// give them, and the product of every blade times every blade in R(n,0,0)
// lands on every blade, so its result set is that list
TEST_CASE("The result set of every blade times every blade is the golden "
          "canonical order") {
  constexpr const auto& four =
      product_plan<gp, r400, core::full<4>, core::full<4>>;
  REQUIRE(four.result.size() == golden::result_order::n4.size());
  for (std::size_t k = 0; k < four.result.size(); ++k) {
    CHECK(four.result.blades.at(k) ==
          oriented_blade{.mask = golden::result_order::n4.at(k), .sign = 1});
  }

  constexpr auto r500 = core::R<5, 0, 0>();
  constexpr const auto& five = core::full<5>;
  const typing typed =
      typed_at_run_time(plan_of<gp, 16>(r500, five.blades, five.blades));
  REQUIRE(typed.result.size() == golden::result_order::n5.size());
  for (std::size_t k = 0; k < typed.result.size(); ++k) {
    CHECK(typed.result.at(k) ==
          oriented_blade{.mask = golden::result_order::n5.at(k), .sign = 1});
  }
}

// Reference: contract: a typed plan built at compile time is the same
// functions run at run time, so the typed plans of both operations over
// every blade times every blade of rigid(3), R(4,0,0), R(1,3,0),
// conformal(2), and a degenerate form of three generators equal a run-time
// call on a metric that the compiler cannot know
TEST_CASE("product_plan equals its typing run at run time") {
  const core::metric<4> g = at_run_time(rga3::value);
  CHECK(equals_typing<gp, 8>(
      g, core::full<4>, core::full<4>,
      product_plan<gp, rga3, core::full<4>, core::full<4>>));
  CHECK(equals_typing<wedge, 1>(
      g, core::full<4>, core::full<4>,
      product_plan<wedge, rga3, core::full<4>, core::full<4>>));
  CHECK(equals_typing<gp, 2>(g, point, line,
                             product_plan<gp, rga3, point, line>));
  CHECK(equals_typing<wedge, 1>(g, point, line,
                                product_plan<wedge, rga3, point, line>));

  constexpr const auto& four = core::full<4>;
  CHECK(equals_typing<gp, 8>(at_run_time(r400::value), four, four,
                             product_plan<gp, r400, four, four>));
  CHECK(equals_typing<wedge, 1>(at_run_time(r400::value), four, four,
                                product_plan<wedge, r400, four, four>));
  CHECK(equals_typing<gp, 8>(at_run_time(r130::value), four, four,
                             product_plan<gp, r130, four, four>));
  CHECK(equals_typing<wedge, 1>(at_run_time(r130::value), four, four,
                                product_plan<wedge, r130, four, four>));
  CHECK(equals_typing<gp, 8>(at_run_time(conformal2::value), four, four,
                             product_plan<gp, conformal2, four, four>));
  CHECK(equals_typing<wedge, 1>(at_run_time(conformal2::value), four, four,
                                product_plan<wedge, conformal2, four, four>));
  constexpr const auto& three = core::full<3>;
  CHECK(equals_typing<gp, 4>(at_run_time(degenerate::value), three, three,
                             product_plan<gp, degenerate, three, three>));
  CHECK(equals_typing<wedge, 1>(at_run_time(degenerate::value), three, three,
                                product_plan<wedge, degenerate, three, three>));
}

// Reference: contract: a mask listed twice in a set is not rejected, and
// each place that lists it gets its own terms, which land on one position of
// the result, here e1 e1 = 1 in R(3,0,0) for +e1 and -e1, and e1 ^ e2 = e12
// for both, and a plan with an empty set has an empty result and no terms
TEST_CASE("Repeated blades share a result position, and empty sets give "
          "empty typed plans") {
  constexpr core::blade_set<3> repeated{
      {
          {
              {.mask = e1, .sign = 1},
              {.mask = e2, .sign = 1},
              {.mask = e1, .sign = -1},
          },
      },
  };
  constexpr core::blade_set<1> just_e1{{{{.mask = e1, .sign = 1}}}};
  constexpr core::blade_set<1> just_e2{{{{.mask = e2, .sign = 1}}}};
  CHECK(product_plan<gp, r300, repeated, just_e1> ==
        typed_plan<2, 3>{
            .result = {{{{.mask = 0, .sign = 1}, {.mask = e12, .sign = 1}}}},
            .terms = {{
                {.a = 0, .b = 0, .out = 0, .coefficient = 1},
                {.a = 1, .b = 0, .out = 1, .coefficient = -1},
                {.a = 2, .b = 0, .out = 0, .coefficient = -1},
            }},
        });
  CHECK(product_plan<wedge, r300, repeated, just_e2> ==
        typed_plan<1, 2>{
            .result = {{{{.mask = e12, .sign = 1}}}},
            .terms = {{
                {.a = 0, .b = 0, .out = 0, .coefficient = 1},
                {.a = 2, .b = 0, .out = 0, .coefficient = -1},
            }},
        });

  constexpr core::blade_set<0> none{};
  CHECK(product_plan<gp, rga3, none, point> == typed_plan<0, 0>{});
  CHECK(product_plan<gp, rga3, point, none> == typed_plan<0, 0>{});
  CHECK(product_plan<wedge, rga3, none, none> == typed_plan<0, 0>{});
  CHECK(product_plan<wedge, rga3, point, none> == typed_plan<0, 0>{});
  CHECK(plan_capacity(wedge, none.blades, point.blades, 4) == 1);
  CHECK(plan_capacity(wedge, motor.blades, motor.blades, 4) == 1);
}

// Reference: contract: collect_result writes the distinct blades of a
// plan's terms once each, with sign +1, in canonical order, returns their
// number, and leaves the places after them, and type_terms writes as many
// terms as fit, each with the position of its blade, or the size of the
// result for a blade the result lacks
TEST_CASE("collect_result and type_terms keep to their stated bounds") {
  const std::array<plan_term, 4> plan{
      {
          {.a = 0, .b = 0, .out = e23, .coefficient = 1},
          {.a = 0, .b = 1, .out = e14, .coefficient = -1},
          {.a = 1, .b = 0, .out = e23, .coefficient = 2},
          {.a = 1, .b = 1, .out = 0, .coefficient = 3},
      },
  };
  const oriented_blade untouched{.mask = 0b111, .sign = 7};
  std::array<oriented_blade, 5> result{};
  result.fill(untouched);
  CHECK(collect_result(plan, result) == 3);
  CHECK(result.at(0) == oriented_blade{.mask = 0, .sign = 1});
  CHECK(result.at(1) == oriented_blade{.mask = e14, .sign = 1});
  CHECK(result.at(2) == oriented_blade{.mask = e23, .sign = 1});
  CHECK(result.at(3) == untouched);
  CHECK(result.at(4) == untouched);
  CHECK(collect_result({}, result) == 0);

  const typed_term blank{.a = 9, .b = 9, .out = 9, .coefficient = 9};
  std::array<typed_term, 5> terms{};
  terms.fill(blank);
  type_terms(plan, std::span(result).first(3), terms);
  CHECK(terms.at(0) == typed_term{.a = 0, .b = 0, .out = 2, .coefficient = 1});
  CHECK(terms.at(1) == typed_term{.a = 0, .b = 1, .out = 1, .coefficient = -1});
  CHECK(terms.at(2) == typed_term{.a = 1, .b = 0, .out = 2, .coefficient = 2});
  CHECK(terms.at(3) == typed_term{.a = 1, .b = 1, .out = 0, .coefficient = 3});
  CHECK(terms.at(4) == blank);

  // A blade the result lacks, and an output shorter than the plan.
  std::array<typed_term, 2> short_terms{};
  type_terms(plan, std::span(result).first(2), short_terms);
  CHECK(short_terms.at(0) ==
        typed_term{.a = 0, .b = 0, .out = 2, .coefficient = 1});
  CHECK(short_terms.at(1) ==
        typed_term{.a = 0, .b = 1, .out = 1, .coefficient = -1});
}

// Reference: contract: the check of a typed plan reports a result set out of
// strictly increasing canonical order, a result sign other than +1, a
// position at or past the size of the result, a result blade that no term
// uses, and terms out of the order a, then b, then strictly increasing
// position, each alone, and accepts each value just within its bound
TEST_CASE("The checks of a typed plan find each defect at its bound") {
  using core::detail::typed_defects;
  using core::detail::typed_defects_of;
  const auto set = [](oriented_blade first, oriented_blade second) {
    return core::blade_set<2>{{{first, second}}};
  };
  // Whether `found` reports the defect `which` and no other, or none when
  // `which` is null.
  const auto none_but = [](const typed_defects& found,
                           bool typed_defects::* which) {
    const std::array<bool typed_defects::*, 5> all{
        &typed_defects::result_out_of_order, &typed_defects::result_sign,
        &typed_defects::beyond_result,       &typed_defects::result_unused,
        &typed_defects::out_of_order,
    };
    return std::ranges::all_of(all, [&](bool typed_defects::* flag) {
      return found.*flag == (flag == which);
    });
  };
  const oriented_blade plus_e1{.mask = e1, .sign = 1};
  const oriented_blade plus_e2{.mask = e2, .sign = 1};
  const std::array<typed_term, 2> both{
      {
          {.a = 0, .b = 0, .out = 0, .coefficient = 1},
          {.a = 0, .b = 1, .out = 1, .coefficient = 1},
      },
  };

  CHECK(none_but(typed_defects_of(set(plus_e1, plus_e2), both), nullptr));
  // Canonical order, not mask order: e4 before e12, e14 before e23.
  CHECK(none_but(
      typed_defects_of(set({.mask = e4, .sign = 1}, {.mask = e12, .sign = 1}),
                       both),
      nullptr));
  CHECK(none_but(
      typed_defects_of(set({.mask = e14, .sign = 1}, {.mask = e23, .sign = 1}),
                       both),
      nullptr));
  CHECK(none_but(
      typed_defects_of(set({.mask = e23, .sign = 1}, {.mask = e14, .sign = 1}),
                       both),
      &typed_defects::result_out_of_order));
  CHECK(none_but(typed_defects_of(set(plus_e2, plus_e1), both),
                 &typed_defects::result_out_of_order));
  CHECK(none_but(typed_defects_of(set(plus_e1, plus_e1), both),
                 &typed_defects::result_out_of_order));
  CHECK(none_but(typed_defects_of(set(plus_e1, {.mask = e2, .sign = -1}), both),
                 &typed_defects::result_sign));
  // Every sign but +1 is wrong for a result blade, on either side of it.
  CHECK(none_but(typed_defects_of(set(plus_e1, {.mask = e2, .sign = 0}), both),
                 &typed_defects::result_sign));
  CHECK(none_but(typed_defects_of(set(plus_e1, {.mask = e2, .sign = 2}), both),
                 &typed_defects::result_sign));

  const std::array<typed_term, 3> past{
      {
          {.a = 0, .b = 0, .out = 0, .coefficient = 1},
          {.a = 0, .b = 1, .out = 1, .coefficient = 1},
          {.a = 0, .b = 2, .out = 2, .coefficient = 1},
      },
  };
  CHECK(none_but(typed_defects_of(set(plus_e1, plus_e2), past),
                 &typed_defects::beyond_result));
  const typed_term on_0{.a = 0, .b = 0, .out = 0, .coefficient = 1};
  const std::array<typed_term, 1> only_first{on_0};
  CHECK(none_but(typed_defects_of(set(plus_e1, plus_e2), only_first),
                 &typed_defects::result_unused));
  const typed_term on_1{.a = 0, .b = 0, .out = 1, .coefficient = 1};
  const std::array<typed_term, 1> only_second{on_1};
  CHECK(none_but(typed_defects_of(set(plus_e1, plus_e2), only_second),
                 &typed_defects::result_unused));

  const auto ordered = [&](typed_term first, typed_term second) {
    return none_but(typed_defects_of(set(plus_e1, plus_e2),
                                     std::array<typed_term, 2>{first, second}),
                    nullptr);
  };
  const auto disordered = [&](typed_term first, typed_term second) {
    return none_but(typed_defects_of(set(plus_e1, plus_e2),
                                     std::array<typed_term, 2>{first, second}),
                    &typed_defects::out_of_order);
  };
  // Within a pair, increasing position; a later pair may start anywhere.
  CHECK(ordered({.a = 0, .b = 0, .out = 0, .coefficient = 1},
                {.a = 0, .b = 0, .out = 1, .coefficient = 1}));
  CHECK(ordered({.a = 0, .b = 0, .out = 1, .coefficient = 1},
                {.a = 0, .b = 1, .out = 0, .coefficient = 1}));
  CHECK(ordered({.a = 0, .b = 2, .out = 1, .coefficient = 1},
                {.a = 1, .b = 0, .out = 0, .coefficient = 1}));
  CHECK(disordered({.a = 0, .b = 0, .out = 1, .coefficient = 1},
                   {.a = 0, .b = 0, .out = 0, .coefficient = 1}));
  CHECK(disordered({.a = 0, .b = 1, .out = 0, .coefficient = 1},
                   {.a = 0, .b = 0, .out = 1, .coefficient = 1}));
  CHECK(disordered({.a = 1, .b = 0, .out = 0, .coefficient = 1},
                   {.a = 0, .b = 2, .out = 1, .coefficient = 1}));
  const std::array<typed_term, 3> repeated{
      {
          {.a = 0, .b = 0, .out = 0, .coefficient = 1},
          {.a = 0, .b = 0, .out = 0, .coefficient = 2},
          {.a = 0, .b = 1, .out = 1, .coefficient = 1},
      },
  };
  CHECK(none_but(typed_defects_of(set(plus_e1, plus_e2), repeated),
                 &typed_defects::out_of_order));
}

// Reference: independent value: in R(32,0,0), the widest algebra a mask
// holds, e1 ^ e1 = e32 ^ e32 = 0 and e32 ^ e1 = -(e1 ^ e32), worked by hand
// for e1 and -e32: e1 ^ (-e32) = -e1,32 and (-e32) ^ e1 = +e1,32, so the
// wedge's own range check accepts the top bit
TEST_CASE("The wedge plan accepts the generators of the widest algebra") {
  constexpr mask_t top = mask_t{1} << (mask_width - 1);
  constexpr core::blade_set<2> tops{
      {
          {
              {.mask = e1, .sign = 1},
              {.mask = top, .sign = -1},
          },
      },
  };
  CHECK(product_plan<wedge, widest, tops, tops> ==
        typed_plan<1, 2>{
            .result = {{{{.mask = e1 | top, .sign = 1}}}},
            .terms = {{
                {.a = 0, .b = 1, .out = 0, .coefficient = -1},
                {.a = 1, .b = 0, .out = 0, .coefficient = 1},
            }},
        });
  CHECK(generate_plan<wedge, 1>(widest::value, tops.blades, tops.blades, {}) ==
        2);
}

// Reference: contract: a pair of blades has at most one wedge term, whether
// or not the blades share a generator, and at most product_capacity(a, b)
// terms of the geometric product, half the subsets of the generators of A
// and B together
TEST_CASE("pair_capacity is 1 for the wedge and product_capacity for the "
          "geometric product") {
  using core::detail::pair_capacity;
  CHECK(pair_capacity(wedge, e1, e2, 4) == 1);
  CHECK(pair_capacity(wedge, e1, e1, 4) == 1);
  CHECK(pair_capacity(wedge, e123, e1234, 4) == 1);
  CHECK(pair_capacity(wedge, 0, 0, 4) == 1);
  CHECK(pair_capacity(gp, e1, e2, 4) == 2);
  CHECK(pair_capacity(gp, e123, e1234, 4) == 8);
  CHECK(pair_capacity(gp, 0, 0, 4) == 1);
}

namespace {

using core::detail::closed_form_right_sign;
using core::detail::generate_map;
using core::detail::map_term;
using core::detail::term;
using core::detail::typed_map;
using core::detail::unary_map;
using core::detail::unary_term;
using core::detail::UnaryOperation;

constexpr auto antigp = BinaryOperation::antigp;
constexpr auto antiwedge = BinaryOperation::antiwedge;
constexpr auto reverse = UnaryOperation::reverse;
constexpr auto antireverse = UnaryOperation::antireverse;
constexpr auto involution = UnaryOperation::grade_involution;
constexpr auto right_complement = UnaryOperation::right_complement;
constexpr auto left_complement = UnaryOperation::left_complement;

constexpr std::array<UnaryOperation, 5> unary_operations{
    reverse, antireverse, involution, right_complement, left_complement,
};

// The largest number of generators of the run-time checks of the maps.
constexpr std::size_t largest_map_n = 10;

// The set of the one blade of the mask M, with sign +1.
template <mask_t M>
constexpr core::blade_set<1> blade_of{{{{.mask = M, .sign = 1}}}};

// Whether `map` has the result set `result`, each blade with sign +1, and
// the terms `terms`, in that order.
template <std::size_t R, std::size_t K>
constexpr bool map_is(const typed_map<R, K>& map,
                      std::initializer_list<mask_t> result,
                      std::initializer_list<map_term> terms) {
  return result_is(map.result, result) && terms.size() == K &&
         std::ranges::equal(map.terms, terms);
}

// Whether a typed map is what generate_map, collect_result, and type_terms
// give when they are called directly on the same set in n generators.
template <std::size_t K, std::size_t R>
constexpr bool equals_map_typing(UnaryOperation op, std::size_t n,
                                 const core::blade_set<K>& set,
                                 const typed_map<R, K>& map) {
  std::array<plan_term, K> terms{};
  generate_map(op, n, set.blades, terms);
  std::array<oriented_blade, K> result{};
  if (collect_result(terms, result) != R) {
    return false;
  }
  for (std::size_t r = 0; r < R; ++r) {
    if (result.at(r) != map.result.blades.at(r)) {
      return false;
    }
  }
  std::array<typed_term, K> typed{};
  type_terms(terms, std::span(result).first(R), typed);
  for (std::size_t k = 0; k < K; ++k) {
    const map_term expected{
        .a = typed.at(k).a,
        .out = typed.at(k).out,
        .coefficient = typed.at(k).coefficient,
    };
    if (typed.at(k).b != 0 || map.terms.at(k) != expected) {
      return false;
    }
  }
  return true;
}

// Reference: independent value: worked by hand from the definition of the
// complements, u ^ right(u) = antiscalar and left(u) ^ u = antiscalar, and
// from the reverse, which multiplies a blade of grade k by (-1)^(k(k-1)/2),
// in rigid(3), for the oriented line e41 e42 e43 e23 e31 e12, that is
// -e14, -e24, -e34, +e23, -e13, +e12: the reverse of a bivector is -1 times
// it and its grade involution +1 times it, right(e41) = right(-e14) = -e23,
// because e14 ^ e23 = +e1234, and in the same way right(e42) = +e13,
// right(e43) = -e12, right(e23) = +e14, right(e31) = +e24, right(e12) = +e34,
// a bivector has grade 2 and antigrade 2, so its left complement is its
// right complement
static_assert(map_is(unary_map<reverse, rga3, line>,
                     {e12, e13, e14, e23, e24, e34},
                     {
                         {.a = 0, .out = 2, .coefficient = 1},
                         {.a = 1, .out = 4, .coefficient = 1},
                         {.a = 2, .out = 5, .coefficient = 1},
                         {.a = 3, .out = 3, .coefficient = -1},
                         {.a = 4, .out = 1, .coefficient = 1},
                         {.a = 5, .out = 0, .coefficient = -1},
                     }));
static_assert(map_is(unary_map<involution, rga3, line>,
                     {e12, e13, e14, e23, e24, e34},
                     {
                         {.a = 0, .out = 2, .coefficient = -1},
                         {.a = 1, .out = 4, .coefficient = -1},
                         {.a = 2, .out = 5, .coefficient = -1},
                         {.a = 3, .out = 3, .coefficient = 1},
                         {.a = 4, .out = 1, .coefficient = -1},
                         {.a = 5, .out = 0, .coefficient = 1},
                     }));
static_assert(map_is(unary_map<right_complement, rga3, line>,
                     {e12, e13, e14, e23, e24, e34},
                     {
                         {.a = 0, .out = 3, .coefficient = -1},
                         {.a = 1, .out = 1, .coefficient = 1},
                         {.a = 2, .out = 0, .coefficient = -1},
                         {.a = 3, .out = 2, .coefficient = 1},
                         {.a = 4, .out = 4, .coefficient = 1},
                         {.a = 5, .out = 5, .coefficient = 1},
                     }));
static_assert(unary_map<left_complement, rga3, line> ==
              unary_map<right_complement, rga3, line>);

// Reference: independent value: worked by hand from u ^ right(u) =
// antiscalar and left(u) ^ u = antiscalar in rigid(3): right(e1) = e234,
// right(e2) = -e134, right(e3) = e124, right(e4) = -e123, since moving e_i
// past the generators below it takes i - 1 transpositions, a vector has
// antigrade 3, so its left complement is -1 times its right complement and
// its antireverse is (-1)^(3 2 / 2) = -1 times it, and for the oriented plane
// e423 e431 e412 e321, that is +e234, -e134, +e124, -e123, right(e423) = -e1,
// right(e431) = -e2, right(e412) = -e3, right(e321) = -e4, and the left
// complements are their negatives
static_assert(map_is(unary_map<right_complement, rga3, point>,
                     {e123, e124, e134, e234},
                     {
                         {.a = 0, .out = 3, .coefficient = 1},
                         {.a = 1, .out = 2, .coefficient = -1},
                         {.a = 2, .out = 1, .coefficient = 1},
                         {.a = 3, .out = 0, .coefficient = -1},
                     }));
static_assert(map_is(unary_map<left_complement, rga3, point>,
                     {e123, e124, e134, e234},
                     {
                         {.a = 0, .out = 3, .coefficient = -1},
                         {.a = 1, .out = 2, .coefficient = 1},
                         {.a = 2, .out = 1, .coefficient = -1},
                         {.a = 3, .out = 0, .coefficient = 1},
                     }));
static_assert(map_is(unary_map<antireverse, rga3, point>, {e1, e2, e3, e4},
                     {
                         {.a = 0, .out = 0, .coefficient = -1},
                         {.a = 1, .out = 1, .coefficient = -1},
                         {.a = 2, .out = 2, .coefficient = -1},
                         {.a = 3, .out = 3, .coefficient = -1},
                     }));
static_assert(map_is(unary_map<right_complement, rga3, plane>, {e1, e2, e3, e4},
                     {
                         {.a = 0, .out = 0, .coefficient = -1},
                         {.a = 1, .out = 1, .coefficient = -1},
                         {.a = 2, .out = 2, .coefficient = -1},
                         {.a = 3, .out = 3, .coefficient = -1},
                     }));
static_assert(map_is(unary_map<left_complement, rga3, plane>, {e1, e2, e3, e4},
                     {
                         {.a = 0, .out = 0, .coefficient = 1},
                         {.a = 1, .out = 1, .coefficient = 1},
                         {.a = 2, .out = 2, .coefficient = 1},
                         {.a = 3, .out = 3, .coefficient = 1},
                     }));

// The typed plan of one term, coefficient times the blade of `mask`, of the
// first blades of two sets of one blade each.
constexpr typed_plan<1, 1> one_term(mask_t mask, coefficient_type coefficient) {
  return {
      .result = {{{{.mask = mask, .sign = 1}}}},
      .terms = {{{.a = 0, .b = 0, .out = 0, .coefficient = coefficient}}},
  };
}

// Reference: independent value: worked by hand from a ⟇ b =
// left(right(a) right(b)) and a v b = left(right(a) ^ right(b)) in rigid(3),
// where e1, e2, e3 square to 1 and e4 to 0: right(e234) = -e1 and
// (-e1)(-e1) = 1, whose left complement is the antiscalar, so
// e234 ⟇ e234 = e1234, right(e123) = e4 and e4 e4 = 0, so e123 ⟇ e123 = 0,
// right(e1) = e234 and e234 e234 = 0, so e1 ⟇ e1 = 0, right(e4) = -e123 and
// e123 e123 = -1, so e4 ⟇ e4 = -e1234, right(e1234) = 1, so
// e1234 ⟇ e1 = e1 ⟇ e1234 = e1, the antiscalar being the identity,
// right(e134) = e2 and (-e1) ^ e2 = -e12, whose left complement is -e34, so
// e234 v e134 = -e34, e234 ^ (-e1) = +e1234, so e1 v e234 = 1, while
// e234 v e1 = -1
static_assert(product_plan<antigp, rga3, blade_of<e234>, blade_of<e234>> ==
              one_term(e1234, 1));
static_assert(product_plan<antigp, rga3, blade_of<e123>, blade_of<e123>> ==
              typed_plan<0, 0>{});
static_assert(product_plan<antigp, rga3, blade_of<e1>, blade_of<e1>> ==
              typed_plan<0, 0>{});
static_assert(product_plan<antigp, rga3, blade_of<e4>, blade_of<e4>> ==
              one_term(e1234, -1));
static_assert(product_plan<antigp, rga3, blade_of<e1234>, blade_of<e1>> ==
              one_term(e1, 1));
static_assert(product_plan<antigp, rga3, blade_of<e1>, blade_of<e1234>> ==
              one_term(e1, 1));
static_assert(product_plan<antiwedge, rga3, blade_of<e234>, blade_of<e134>> ==
              one_term(e34, -1));
static_assert(product_plan<antiwedge, rga3, blade_of<e1>, blade_of<e234>> ==
              one_term(0, 1));
static_assert(product_plan<antiwedge, rga3, blade_of<e234>, blade_of<e1>> ==
              one_term(0, -1));

// Reference: independent value: worked by hand in conformal(3), where
// e4 e5 = -1 + e45: right(e1235) = -e4, since e1235 ^ e4 takes one
// transposition, and right(e1234) = e5, so right(e1235) right(e1234) =
// -e4 e5 = 1 - e45, the left complement of 1 is e12345 and that of e45 is
// e123, so e1235 ⟇ e1234 = -e123 + e12345, whose two terms come in canonical
// order although the product of the complements lists the scalar first
static_assert(
    product_plan<antigp, conformal3, blade_of<e123 | e5>, blade_of<e1234>> ==
    typed_plan<2, 2>{
        .result =
            {
                {
                    {
                        {.mask = e123, .sign = 1},
                        {.mask = e1234 | e5, .sign = 1},
                    },
                },
            },
        .terms =
            {
                {
                    {.a = 0, .b = 0, .out = 0, .coefficient = -1},
                    {.a = 0, .b = 0, .out = 1, .coefficient = 1},
                },
            },
    });

// Reference: independent value: counted by hand from a ⟇ b =
// left(right(a) right(b)) and a v b = left(right(a) ^ right(b)) in rigid(3),
// where a product of two blades is zero when both hold e4: the right
// complements of the point's blades are three trivectors with e4 and -e123,
// so a point antiproduct a point has the 7 pairs that involve e4, on the
// three bivectors without e4 and e1234, and a point antiwedge a point has
// none, the right complements of the plane's blades are the four vectors, so
// a plane antiproduct a plane has 4 x 4 - 1 = 15 terms, on the bivectors and
// e1234, and a plane antiwedge a plane has 12, on the bivectors: two planes
// meet in a line, a line antiwedge a plane has 12, on the vectors: a line
// meets a plane in a point, a motor antiproduct a point has 20 terms on the
// eight odd blades, and a motor antiproduct a motor 48 terms on the scalar,
// the bivectors, and e1234. In conformal(3) the right complements of the
// vectors are the quadrivectors, so the vectors antiproduct the vectors have
// 25 terms, on the trivectors and e12345, and antiwedge none
static_assert(product_plan<antigp, rga3, point, point>.terms.size() == 7);
static_assert(result_is(product_plan<antigp, rga3, point, point>.result,
                        {e12, e13, e23, e1234}));
static_assert(product_plan<antiwedge, rga3, point, point> ==
              typed_plan<0, 0>{});
static_assert(product_plan<antigp, rga3, plane, plane>.terms.size() == 15);
static_assert(result_is(product_plan<antigp, rga3, plane, plane>.result,
                        {e12, e13, e14, e23, e24, e34, e1234}));
static_assert(read_back(product_plan<antiwedge, rga3, plane, plane>) ==
              std::array<plan_term, 12>{
                  {
                      {.a = 0, .b = 1, .out = e34, .coefficient = 1},
                      {.a = 0, .b = 2, .out = e24, .coefficient = -1},
                      {.a = 0, .b = 3, .out = e23, .coefficient = 1},
                      {.a = 1, .b = 0, .out = e34, .coefficient = -1},
                      {.a = 1, .b = 2, .out = e14, .coefficient = 1},
                      {.a = 1, .b = 3, .out = e13, .coefficient = -1},
                      {.a = 2, .b = 0, .out = e24, .coefficient = 1},
                      {.a = 2, .b = 1, .out = e14, .coefficient = -1},
                      {.a = 2, .b = 3, .out = e12, .coefficient = 1},
                      {.a = 3, .b = 0, .out = e23, .coefficient = -1},
                      {.a = 3, .b = 1, .out = e13, .coefficient = 1},
                      {.a = 3, .b = 2, .out = e12, .coefficient = -1},
                  },
              });
static_assert(result_is(product_plan<antiwedge, rga3, plane, plane>.result,
                        {e12, e13, e14, e23, e24, e34}));
static_assert(product_plan<antiwedge, rga3, line, plane>.terms.size() == 12);
static_assert(result_is(product_plan<antiwedge, rga3, line, plane>.result,
                        {e1, e2, e3, e4}));
static_assert(product_plan<antigp, rga3, motor, point>.terms.size() == 20);
static_assert(result_is(product_plan<antigp, rga3, motor, point>.result,
                        {e1, e2, e3, e4, e123, e124, e134, e234}));
static_assert(product_plan<antigp, rga3, motor, motor>.terms.size() == 48);
static_assert(result_is(product_plan<antigp, rga3, motor, motor>.result,
                        {0, e12, e13, e14, e23, e24, e34, e1234}));
static_assert(product_plan<antigp, conformal3, core::of_grade<5, 1>,
                           core::of_grade<5, 1>>.terms.size() == 25);
static_assert(result_is(
    product_plan<antigp, conformal3, core::of_grade<5, 1>, core::of_grade<5, 1>>.result,
    {
        e123,
        e124,
        e12 | e5,
        e134,
        e13 | e5,
        e14 | e5,
        e234,
        e23 | e5,
        e24 | e5,
        e34 | e5,
        e1234 | e5,
    }));
static_assert(product_plan<antiwedge, conformal3, core::of_grade<5, 1>,
                           core::of_grade<5, 1>> == typed_plan<0, 0>{});

// Reference: contract: a typed map built at compile time is the same
// functions run at run time, so the maps of every operation over every blade
// of four and of five generators, and the antiproduct plans of every blade
// times every blade of rigid(3) and conformal(2), equal a direct call in a
// constant evaluation
static_assert(equals_map_typing(reverse, 4, core::full<4>,
                                unary_map<reverse, rga3, core::full<4>>));
static_assert(equals_map_typing(antireverse, 4, core::full<4>,
                                unary_map<antireverse, rga3, core::full<4>>));
static_assert(equals_map_typing(involution, 4, core::full<4>,
                                unary_map<involution, rga3, core::full<4>>));
static_assert(
    equals_map_typing(right_complement, 4, core::full<4>,
                      unary_map<right_complement, rga3, core::full<4>>));
static_assert(
    equals_map_typing(left_complement, 4, core::full<4>,
                      unary_map<left_complement, rga3, core::full<4>>));
static_assert(equals_map_typing(reverse, 5, core::full<5>,
                                unary_map<reverse, conformal3, core::full<5>>));
static_assert(
    equals_map_typing(antireverse, 5, core::full<5>,
                      unary_map<antireverse, conformal3, core::full<5>>));
static_assert(
    equals_map_typing(involution, 5, core::full<5>,
                      unary_map<involution, conformal3, core::full<5>>));
static_assert(
    equals_map_typing(right_complement, 5, core::full<5>,
                      unary_map<right_complement, conformal3, core::full<5>>));
static_assert(
    equals_map_typing(left_complement, 5, core::full<5>,
                      unary_map<left_complement, conformal3, core::full<5>>));
static_assert(equals_typing<antigp, 8>(
    rga3::value, core::full<4>, core::full<4>,
    product_plan<antigp, rga3, core::full<4>, core::full<4>>));
static_assert(equals_typing<antiwedge, 1>(
    rga3::value, core::full<4>, core::full<4>,
    product_plan<antiwedge, rga3, core::full<4>, core::full<4>>));
static_assert(equals_typing<antigp, 8>(
    conformal2::value, core::full<4>, core::full<4>,
    product_plan<antigp, conformal2, core::full<4>, core::full<4>>));
static_assert(equals_typing<antiwedge, 1>(
    conformal2::value, core::full<4>, core::full<4>,
    product_plan<antiwedge, conformal2, core::full<4>, core::full<4>>));

// Whether pair_terms accepts the operation Op.
template <BinaryOperation Op>
constexpr bool has_pair_rule = requires(const core::metric<4>& g) {
  // clang-tidy reads the operation, once substituted, as a C-style cast of
  // its value, which the source does not have.
  // NOLINTNEXTLINE(modernize-avoid-c-style-cast)
  core::detail::pair_terms<Op, 8>(g, mask_t{}, mask_t{});
};

// Reference: contract: pair_terms is the rule of one pair for the geometric
// product and the wedge only, and the antiproducts, which conjugate those
// rules, are rejected by its constraint
static_assert(has_pair_rule<gp> && has_pair_rule<wedge>);
static_assert(!has_pair_rule<antigp> && !has_pair_rule<antiwedge>);

// Reference: contract: a map term is a trivially copyable aggregate, and a
// typed map is a structural one, so that it can be a template argument
static_assert(std::is_aggregate_v<map_term>);
static_assert(std::is_trivially_copyable_v<map_term>);
static_assert(std::is_aggregate_v<typed_map<6, 6>>);
static_assert(std::is_trivially_copyable_v<typed_map<6, 6>>);
template <typed_map<6, 6> Map>
constexpr std::size_t map_terms_of = Map.terms.size();
static_assert(map_terms_of<unary_map<reverse, rga3, line>> == 6);

// Every blade of n generators, with sign +1, in the order written here: by
// grade, then by index list.
std::vector<oriented_blade> all_blades(std::size_t n) {
  std::vector<oriented_blade> all;
  all.reserve(blade_count(n));
  for (mask_t mask = 0; mask < (mask_t{1} << n); ++mask) {
    all.push_back({.mask = mask, .sign = 1});
  }
  std::ranges::sort(all, comes_before, &oriented_blade::mask);
  return all;
}

// The position of each mask in `set`, indexed by mask.
std::vector<std::size_t> positions(std::span<const oriented_blade> set) {
  std::vector<std::size_t> position(set.size());
  for (std::size_t k = 0; k < set.size(); ++k) {
    position.at(set[k].mask) = k;
  }
  return position;
}

// The number of pairs out of order in the index list of `mask` written
// backwards: the transpositions that reverse the list.
int reversal_inversions(mask_t mask) {
  std::vector<std::size_t> list = indices_of(mask);
  std::ranges::reverse(list);
  int count = 0;
  for (std::size_t p = 0; p < list.size(); ++p) {
    for (std::size_t q = p + 1; q < list.size(); ++q) {
      count += list.at(p) > list.at(q) ? 1 : 0;
    }
  }
  return count;
}

// The image of the canonical blade of `mask` under `op` in n generators, by
// the rule of each operation written here: the reverse by the transpositions
// that reverse its index list, the antireverse the same for a list as long as
// its antigrade (that of the other generators), the grade involution by
// (-1)^k, and the complements as the blade C on the other generators with
// the sign s that makes e_A ^ (s e_C) = e_12...n, or (s e_C) ^ e_A =
// e_12...n, where e_X ^ e_Y = (-1)^(inversions of X then Y) e_12...n.
term own_image(UnaryOperation op, mask_t mask, std::size_t n) {
  const mask_t other = (mask_t{1} << n) - 1U - mask;
  if (op == reverse) {
    return {
        .blade = mask,
        .coefficient = parity_sign(reversal_inversions(mask)),
    };
  }
  if (op == antireverse) {
    return {
        .blade = mask,
        .coefficient = parity_sign(reversal_inversions(other)),
    };
  }
  if (op == involution) {
    return {
        .blade = mask,
        .coefficient = parity_sign(static_cast<int>(generators_of(mask))),
    };
  }
  if (op == right_complement) {
    return {
        .blade = other,
        .coefficient = parity_sign(inversions(mask, other)),
    };
  }
  return {.blade = other, .coefficient = parity_sign(inversions(other, mask))};
}

// The mask-level map `op` of `set` in n generators, generated at run time.
std::vector<plan_term> map_of(UnaryOperation op, std::size_t n,
                              std::span<const oriented_blade> set) {
  std::vector<plan_term> terms(set.size());
  generate_map(op, n, set, terms);
  return terms;
}

// The number of terms of the map `op` of `set` in n generators that are not,
// at their own place and with second position 0, s_a times the image written
// here, plus the mismatches of its typing.
int map_mismatches(UnaryOperation op, std::size_t n,
                   std::span<const oriented_blade> set) {
  const std::vector<plan_term> map = map_of(op, n, set);
  int mismatches = 0;
  for (std::size_t k = 0; k < set.size(); ++k) {
    const term image = own_image(op, set[k].mask, n);
    const plan_term expected{
        .a = k,
        .b = 0,
        .out = image.blade,
        .coefficient = set[k].sign * image.coefficient,
    };
    mismatches += map.at(k) == expected ? 0 : 1;
  }
  return mismatches + typing_mismatches(map, typed_at_run_time(map));
}

// The number of blades of n generators whose images under the five maps
// differ from the golden grade signs and complements, both indexed by mask.
template <std::size_t Size>
int golden_map_mismatches(
    const std::array<golden::blade_signs, Size>& signs,
    const std::array<golden::complement_pair, Size>& complements,
    std::size_t n) {
  const std::vector<oriented_blade> all = all_blades(n);
  std::array<std::vector<plan_term>, 5> maps;
  for (std::size_t op = 0; op < unary_operations.size(); ++op) {
    maps.at(op) = map_of(unary_operations.at(op), n, all);
  }
  int mismatches = 0;
  for (std::size_t k = 0; k < all.size(); ++k) {
    const mask_t mask = all.at(k).mask;
    const golden::blade_signs& sign = signs.at(mask);
    const golden::complement_pair& pair = complements.at(mask);
    const std::array<term, 5> expected{
        {
            {.blade = sign.blade, .coefficient = sign.reverse},
            {.blade = sign.blade, .coefficient = sign.antireverse},
            {.blade = sign.blade, .coefficient = sign.involution},
            {.blade = pair.right.blade, .coefficient = pair.right.sign},
            {.blade = pair.left.blade, .coefficient = pair.left.sign},
        },
    };
    for (std::size_t op = 0; op < unary_operations.size(); ++op) {
      const plan_term& got = maps.at(op).at(k);
      mismatches += sign.blade == mask && pair.blade == mask &&
                            got.out == expected.at(op).blade &&
                            got.coefficient == expected.at(op).coefficient
                        ? 0
                        : 1;
    }
  }
  return mismatches;
}

// The image of each blade of n generators under `op`, indexed by mask, read
// from the map that generate_map writes over every blade.
std::vector<term> images(UnaryOperation op, std::size_t n) {
  const std::vector<oriented_blade> all = all_blades(n);
  const std::vector<plan_term> map = map_of(op, n, all);
  std::vector<term> image(all.size());
  for (std::size_t k = 0; k < all.size(); ++k) {
    image.at(all.at(k).mask) = {
        .blade = map.at(k).out,
        .coefficient = map.at(k).coefficient,
    };
  }
  return image;
}

// The images of `outer` applied after `inner`, indexed by mask.
std::vector<term> composed(const std::vector<term>& outer,
                           const std::vector<term>& inner) {
  std::vector<term> image(inner.size());
  for (std::size_t mask = 0; mask < inner.size(); ++mask) {
    const term& first = inner.at(mask);
    const term& second = outer.at(first.blade);
    image.at(mask) = {
        .blade = second.blade,
        .coefficient = first.coefficient * second.coefficient,
    };
  }
  return image;
}

// Each blade of n generators times the sign `sign` gives it, indexed by mask.
template <class Sign>
std::vector<term> scaled_identity(std::size_t n, const Sign& sign) {
  std::vector<term> image(blade_count(n));
  for (std::size_t mask = 0; mask < image.size(); ++mask) {
    const auto m = static_cast<mask_t>(mask);
    image.at(mask) = {.blade = m, .coefficient = sign(m)};
  }
  return image;
}

// A plan of every blade times every blade of a form, generated at run time,
// with the place where the run of each pair (a, b) starts, at a count + b,
// and its end after the last pair.
struct full_plan {
  std::vector<plan_term> terms;
  std::vector<std::size_t> starts;
  std::size_t count;

  [[nodiscard]] std::span<const plan_term> run(std::size_t a,
                                               std::size_t b) const {
    const std::size_t pair = (a * count) + b;
    return std::span(terms).subspan(starts.at(pair),
                                    starts.at(pair + 1) - starts.at(pair));
  }
};

template <BinaryOperation Op, std::size_t N>
full_plan full_plan_of(const core::metric<N>& g) {
  constexpr auto& all = core::full<N>;
  constexpr std::size_t capacity =
      core::detail::base_operation(Op) == gp ? blade_count(N) / 2 : 1;
  full_plan plan{
      .terms = plan_of<Op, capacity>(g, all.blades, all.blades),
      .starts = std::vector<std::size_t>((all.size() * all.size()) + 1),
      .count = all.size(),
  };
  std::size_t at = 0;
  for (std::size_t pair = 0; pair < all.size() * all.size(); ++pair) {
    plan.starts.at(pair) = at;
    while (at < plan.terms.size() &&
           (plan.terms.at(at).a * all.size()) + plan.terms.at(at).b == pair) {
      ++at;
    }
  }
  plan.starts.back() = at;
  return plan;
}

// A run of terms as (blade, coefficient), each coefficient times `scale`.
std::vector<term> scaled_run(std::span<const plan_term> run,
                             coefficient_type scale) {
  std::vector<term> terms;
  for (const plan_term& t : run) {
    terms.push_back({.blade = t.out, .coefficient = scale * t.coefficient});
  }
  return terms;
}

// The number of pairs of blades of full<N> for which c(e_A x e_B), with c
// the map `image` applied term by term and its terms sorted by grade then
// index list here, is not, as a list of terms, c(e_A) y c(e_B), the run of
// the plan `after` for the blades of c(e_A) and c(e_B) times their signs.
int de_morgan_failures(const full_plan& before, const full_plan& after,
                       const std::vector<term>& image,
                       std::span<const oriented_blade> all) {
  const std::vector<std::size_t> position = positions(all);
  int failures = 0;
  for (std::size_t a = 0; a < all.size(); ++a) {
    for (std::size_t b = 0; b < all.size(); ++b) {
      std::vector<term> mapped;
      for (const plan_term& t : before.run(a, b)) {
        const term& c = image.at(t.out);
        mapped.push_back(
            {.blade = c.blade, .coefficient = t.coefficient * c.coefficient});
      }
      std::ranges::sort(mapped, comes_before, &term::blade);
      const term& ca = image.at(all[a].mask);
      const term& cb = image.at(all[b].mask);
      const std::vector<term> expected =
          scaled_run(after.run(position.at(ca.blade), position.at(cb.blade)),
                     ca.coefficient * cb.coefficient);
      failures += mapped == expected ? 0 : 1;
    }
  }
  return failures;
}

// The number of pairs of blades of full<N> for which c(e_A x e_B) is not,
// as a list of terms, c(e_B) x c(e_A), for the map `image` applied term by
// term and its terms sorted here: the product rule of the reverse for the
// geometric product, and of the antireverse for the antiproduct.
int reversal_rule_failures(const full_plan& plan,
                           const std::vector<term>& image,
                           std::span<const oriented_blade> all) {
  int failures = 0;
  for (std::size_t a = 0; a < all.size(); ++a) {
    for (std::size_t b = 0; b < all.size(); ++b) {
      std::vector<term> mapped;
      for (const plan_term& t : plan.run(a, b)) {
        const term& c = image.at(t.out);
        mapped.push_back(
            {.blade = c.blade, .coefficient = t.coefficient * c.coefficient});
      }
      std::ranges::sort(mapped, comes_before, &term::blade);
      const coefficient_type signs =
          image.at(all[a].mask).coefficient * image.at(all[b].mask).coefficient;
      failures += mapped == scaled_run(plan.run(b, a), signs) ? 0 : 1;
    }
  }
  return failures;
}

// (e_A x e_B) x e_C, multiplied out over the plan into `out`, indexed by
// mask, for the blades at positions a, b, and c and the position of each
// mask, `position`.
void grouped_left(const full_plan& plan, std::span<const std::size_t> position,
                  std::size_t a, std::size_t b, std::size_t c,
                  std::vector<coefficient_type>& out) {
  std::ranges::fill(out, 0);
  for (const plan_term& ab : plan.run(a, b)) {
    for (const plan_term& abc : plan.run(position[ab.out], c)) {
      out.at(abc.out) += ab.coefficient * abc.coefficient;
    }
  }
}

// e_A x (e_B x e_C), in the same way.
void grouped_right(const full_plan& plan, std::span<const std::size_t> position,
                   std::size_t a, std::size_t b, std::size_t c,
                   std::vector<coefficient_type>& out) {
  std::ranges::fill(out, 0);
  for (const plan_term& bc : plan.run(b, c)) {
    for (const plan_term& abc : plan.run(a, position[bc.out])) {
      out.at(abc.out) += bc.coefficient * abc.coefficient;
    }
  }
}

// The number of triples of blades of full<N> on which (e_A x e_B) x e_C and
// e_A x (e_B x e_C), each multiplied out over the plan, differ.
int associativity_failures(const full_plan& plan,
                           std::span<const oriented_blade> all) {
  const std::vector<std::size_t> position = positions(all);
  std::vector<coefficient_type> left(all.size());
  std::vector<coefficient_type> right(all.size());
  int failures = 0;
  for (std::size_t a = 0; a < all.size(); ++a) {
    for (std::size_t b = 0; b < all.size(); ++b) {
      for (std::size_t c = 0; c < all.size(); ++c) {
        grouped_left(plan, position, a, b, c, left);
        grouped_right(plan, position, a, b, c, right);
        failures += left == right ? 0 : 1;
      }
    }
  }
  return failures;
}

// The number of blades e_A of full<N> for which u x e_A or e_A x u is not
// e_A, for the blade u of the mask `unit` and the plan of the product x.
int identity_failures(const full_plan& plan, mask_t unit,
                      std::span<const oriented_blade> all) {
  const std::size_t u = positions(all).at(unit);
  int failures = 0;
  for (std::size_t a = 0; a < all.size(); ++a) {
    const std::vector<term> itself{{.blade = all[a].mask, .coefficient = 1}};
    failures += scaled_run(plan.run(u, a), 1) == itself ? 0 : 1;
    failures += scaled_run(plan.run(a, u), 1) == itself ? 0 : 1;
  }
  return failures;
}

// The number of blades u of n generators for which u v right(u) or
// left(u) v u is not the scalar 1, over the antiwedge plan and the two
// complement maps.
int antiwedge_unit_failures(const full_plan& antiwedge_plan, std::size_t n,
                            std::span<const oriented_blade> all) {
  const std::vector<term> right = images(right_complement, n);
  const std::vector<term> left = images(left_complement, n);
  const std::vector<std::size_t> position = positions(all);
  const std::vector<term> one{{.blade = 0, .coefficient = 1}};
  int failures = 0;
  for (std::size_t a = 0; a < all.size(); ++a) {
    const term& r = right.at(all[a].mask);
    const term& l = left.at(all[a].mask);
    failures += scaled_run(antiwedge_plan.run(a, position.at(r.blade)),
                           r.coefficient) == one
                    ? 0
                    : 1;
    failures += scaled_run(antiwedge_plan.run(position.at(l.blade), a),
                           l.coefficient) == one
                    ? 0
                    : 1;
  }
  return failures;
}

// The run of e_A x e_B for the antiproduct `op`, composed from golden values
// alone: the right complements of e_A and e_B from `complements`, their
// product from `table` for the geometric antiproduct or their wedge by the
// rule written here for the antiwedge, and the left complement of each term
// from `complements`, sorted here by grade, then index list.
template <std::size_t Rows, std::size_t Size>
std::vector<term>
golden_antiproduct(BinaryOperation op,
                   const std::array<golden::product, Rows>& table,
                   const std::array<golden::complement_pair, Size>& complements,
                   mask_t ma, mask_t mb) {
  const golden::signed_blade ra = complements.at(ma).right;
  const golden::signed_blade rb = complements.at(mb).right;
  std::vector<term> product;
  if (op == antigp) {
    const auto row = std::ranges::find_if(table, [&](const golden::product& p) {
      return p.a == ra.blade && p.b == rb.blade;
    });
    for (std::size_t t = 0; row != table.end() && t < row->size; ++t) {
      product.push_back({
          .blade = row->terms.at(t).blade,
          .coefficient = row->terms.at(t).coefficient,
      });
    }
  } else if ((ra.blade & rb.blade) == 0) {
    product.push_back({
        .blade = ra.blade | rb.blade,
        .coefficient = parity_sign(inversions(ra.blade, rb.blade)),
    });
  }

  std::vector<term> run;
  for (const term& t : product) {
    const golden::signed_blade left = complements.at(t.blade).left;
    run.push_back({
        .blade = left.blade,
        .coefficient =
            coefficient_type{ra.sign} * rb.sign * t.coefficient * left.sign,
    });
  }
  std::ranges::sort(run, comes_before, &term::blade);
  return run;
}

// The mask-level plan of the antiproduct `op` over `as` and `bs`, composed
// from golden values: for each pair, in order of a and then b, s_a s_b times
// the golden run of the pair.
template <std::size_t Rows, std::size_t Size>
std::vector<plan_term> golden_antiproduct_plan(
    BinaryOperation op, const std::array<golden::product, Rows>& table,
    const std::array<golden::complement_pair, Size>& complements,
    std::span<const oriented_blade> as, std::span<const oriented_blade> bs) {
  std::vector<plan_term> plan;
  for (std::size_t a = 0; a < as.size(); ++a) {
    for (std::size_t b = 0; b < bs.size(); ++b) {
      for (const term& t :
           golden_antiproduct(op, table, complements, as[a].mask, bs[b].mask)) {
        plan.push_back({
            .a = a,
            .b = b,
            .out = t.blade,
            .coefficient =
                coefficient_type{as[a].sign} * bs[b].sign * t.coefficient,
        });
      }
    }
  }
  return plan;
}

// The number of places at which the antiproduct plans of every blade times
// every blade, generated at run time in the form of `gram`, differ from the
// golden composition, for both antiproducts.
template <std::size_t N, std::size_t Rows, std::size_t Size>
int golden_antiproduct_mismatches(
    const std::array<std::array<int, N>, N>& gram,
    const std::array<golden::product, Rows>& table,
    const std::array<golden::complement_pair, Size>& complements) {
  core::metric<N> g{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      g.gram.at(i).at(j) = gram.at(i).at(j);
    }
  }
  constexpr auto& all = core::full<N>;
  int mismatches = 0;
  mismatches +=
      plan_of<antigp, blade_count(N) / 2>(g, all.blades, all.blades) ==
              golden_antiproduct_plan(antigp, table, complements, all.blades,
                                      all.blades)
          ? 0
          : 1;
  mismatches += plan_of<antiwedge, 1>(g, all.blades, all.blades) ==
                        golden_antiproduct_plan(antiwedge, table, complements,
                                                all.blades, all.blades)
                    ? 0
                    : 1;
  return mismatches;
}

// Whether the typed plan `plan`, read back, is the golden composition of the
// antiproduct `op` over the sets `as` and `bs`.
template <std::size_t R, std::size_t T, std::size_t Rows, std::size_t Size,
          std::size_t KA, std::size_t KB>
bool is_golden_antiproduct(
    const typed_plan<R, T>& plan, BinaryOperation op,
    const std::array<golden::product, Rows>& table,
    const std::array<golden::complement_pair, Size>& complements,
    const core::blade_set<KA>& as, const core::blade_set<KB>& bs) {
  const std::array<plan_term, T> terms = read_back(plan);
  return std::ranges::equal(
      terms,
      golden_antiproduct_plan(op, table, complements, as.blades, bs.blades));
}

} // namespace

// Reference: proof: for every n from 1 to 10 and every blade of n
// generators, each map is the rule written here: the reverse of a grade-k
// blade is (-1)^(k(k-1)/2) times it, counted as the transpositions that
// reverse its index list, the antireverse the same with the antigrade, the
// grade involution (-1)^k, and u ^ right(u) = antiscalar and left(u) ^ u =
// antiscalar, by this file's own count of inversions, at its own place, with
// the input's sign folded in, typed onto the distinct blades in canonical
// order. Even and odd n are both included, because for odd n the left and
// right complements coincide
TEST_CASE("Every map of every blade is its definition, typed onto its "
          "result") {
  for (std::size_t n = 1; n <= largest_map_n; ++n) {
    INFO("n = " << n);
    const std::vector<oriented_blade> all = all_blades(n);
    const std::vector<oriented_blade> signed_blades = reverse_signs(all);
    for (const UnaryOperation op : unary_operations) {
      INFO("operation " << static_cast<int>(op));
      CHECK(map_mismatches(op, n, all) == 0);
      CHECK(map_mismatches(op, n, signed_blades) == 0);
    }
  }
}

// Reference: independent value: the golden grade signs, in which the reverse
// of a grade-k blade is (-1)^(k(k-1)/2) times it, the antireverse the same
// with the antigrade and the grade involution (-1)^k, and the golden
// complements, in which u ^ right(u) = antiscalar and left(u) ^ u =
// antiscalar, for three, four, and five generators: every map of every blade
// has the golden image
TEST_CASE("Every map of every blade is its golden image") {
  CHECK(golden_map_mismatches(golden::grade_signs::n3, golden::complements::n3,
                              3) == 0);
  CHECK(golden_map_mismatches(golden::grade_signs::n4, golden::complements::n4,
                              4) == 0);
  CHECK(golden_map_mismatches(golden::grade_signs::n5, golden::complements::n5,
                              5) == 0);
}

// Reference: proof: for every n from 1 to 10, composed over every blade from
// the maps that generate_map writes: the reverse and the grade involution are
// their own inverses, the left complement undoes the right one and the right
// the left, applying either complement twice gives (-1)^(gr ag) times the
// blade, with ag = n - gr, and the antireverse is the reverse conjugated by
// the complements, left(reverse(right(u)))
TEST_CASE("The maps compose as the involutions and complements do") {
  for (std::size_t n = 1; n <= largest_map_n; ++n) {
    INFO("n = " << n);
    const std::vector<term> identity =
        scaled_identity(n, [](mask_t) { return coefficient_type{1}; });
    const std::vector<term> gr_ag = scaled_identity(n, [n](mask_t mask) {
      const std::size_t gr = generators_of(mask);
      return parity_sign(static_cast<int>(gr * (n - gr)));
    });
    const std::vector<term> reversed = images(reverse, n);
    const std::vector<term> involuted = images(involution, n);
    const std::vector<term> right = images(right_complement, n);
    const std::vector<term> left = images(left_complement, n);
    CHECK(composed(reversed, reversed) == identity);
    CHECK(composed(involuted, involuted) == identity);
    CHECK(composed(left, right) == identity);
    CHECK(composed(right, left) == identity);
    CHECK(composed(right, right) == gr_ag);
    CHECK(composed(left, left) == gr_ag);
    CHECK(images(antireverse, n) == composed(left, composed(reversed, right)));
  }
}

// Reference: independent value: the golden product tables, in which
// e_i e_j + e_j e_i = 2 g_ij, of rigid(3), conformal(3), a degenerate form,
// and seeded random form 1, and the golden complements, in which
// u ^ right(u) = antiscalar and left(u) ^ u = antiscalar: for every pair of
// blades, the plan of the geometric antiproduct is left(right(a) right(b))
// composed from golden values alone, and that of the antiwedge
// left(right(a) ^ right(b)) with the wedge written here
TEST_CASE("The antiproduct plans are the golden products conjugated by the "
          "golden complements") {
  CHECK(golden_antiproduct_mismatches(golden::gp_tables::rigid_3_gram,
                                      golden::gp_tables::rigid_3,
                                      golden::complements::n4) == 0);
  CHECK(golden_antiproduct_mismatches(golden::gp_tables::conformal_3_gram,
                                      golden::gp_tables::conformal_3,
                                      golden::complements::n5) == 0);
  CHECK(golden_antiproduct_mismatches(golden::gp_tables::degenerate_3_gram,
                                      golden::gp_tables::degenerate_3,
                                      golden::complements::n3) == 0);
  CHECK(golden_antiproduct_mismatches(golden::gp_tables::random_1_gram,
                                      golden::gp_tables::random_1,
                                      golden::complements::n3) == 0);
}

// Reference: independent value: the golden product tables of rigid(3) and
// conformal(3), in which e_i e_j + e_j e_i = 2 g_ij, and the golden
// complements, in which u ^ right(u) = antiscalar and left(u) ^ u =
// antiscalar: the typed antiproduct plans of the oriented point, line,
// plane, and motor of rigid(3) and of the vectors of conformal(3), whose
// counts and result sets are worked by hand above, read back as the golden
// composition of each pair with the signs of its blades
TEST_CASE("The typed antiproduct plans of rigid(3) and conformal(3) are the "
          "golden compositions") {
  const auto& rigid = golden::gp_tables::rigid_3;
  const auto& four = golden::complements::n4;
  CHECK(is_golden_antiproduct(product_plan<antigp, rga3, point, point>, antigp,
                              rigid, four, point, point));
  CHECK(is_golden_antiproduct(product_plan<antiwedge, rga3, point, point>,
                              antiwedge, rigid, four, point, point));
  CHECK(is_golden_antiproduct(product_plan<antigp, rga3, plane, plane>, antigp,
                              rigid, four, plane, plane));
  CHECK(is_golden_antiproduct(product_plan<antiwedge, rga3, plane, plane>,
                              antiwedge, rigid, four, plane, plane));
  CHECK(is_golden_antiproduct(product_plan<antiwedge, rga3, line, plane>,
                              antiwedge, rigid, four, line, plane));
  CHECK(is_golden_antiproduct(product_plan<antigp, rga3, motor, point>, antigp,
                              rigid, four, motor, point));
  CHECK(is_golden_antiproduct(product_plan<antigp, rga3, motor, motor>, antigp,
                              rigid, four, motor, motor));
  constexpr const auto& vectors = core::of_grade<5, 1>;
  CHECK(
      is_golden_antiproduct(product_plan<antigp, conformal3, vectors, vectors>,
                            antigp, golden::gp_tables::conformal_3,
                            golden::complements::n5, vectors, vectors));
  CHECK(is_golden_antiproduct(
      product_plan<antiwedge, conformal3, vectors, vectors>, antiwedge,
      golden::gp_tables::conformal_3, golden::complements::n5, vectors,
      vectors));
}

// Reference: proof: the De Morgan laws right(a ^ b) = right(a) v right(b) and
// right(a b) = right(a) ⟇ right(b), and the same with the left complement,
// for every pair of blades of every signature of up to six generators listed
// here, each side computed from the plans and maps of the library and
// compared as lists of terms
TEST_CASE("The complements turn each product into its antiproduct") {
  for_each_signature([](const char* name, const auto& g) {
    constexpr std::size_t n = std::remove_cvref_t<decltype(g)>::generator_count;
    INFO(name);
    const std::vector<oriented_blade> all = all_blades(n);
    const full_plan product = full_plan_of<gp>(g);
    const full_plan antiproduct = full_plan_of<antigp>(g);
    const full_plan exterior = full_plan_of<wedge>(g);
    const full_plan antiexterior = full_plan_of<antiwedge>(g);
    const std::vector<term> right = images(right_complement, n);
    const std::vector<term> left = images(left_complement, n);
    CHECK(de_morgan_failures(product, antiproduct, right, all) == 0);
    CHECK(de_morgan_failures(exterior, antiexterior, right, all) == 0);
    CHECK(de_morgan_failures(product, antiproduct, left, all) == 0);
    CHECK(de_morgan_failures(exterior, antiexterior, left, all) == 0);
  });
}

// Reference: identity: both antiproducts are associative, checked on every
// triple of blades, the antiscalar is the identity of the geometric antiproduct
// and 1 that of the geometric product, u v right(u) = 1 and left(u) v u = 1 for
// every blade, and reverse(a b) = reverse(b) reverse(a) and antireverse(a ⟇ b)
// = antireverse(b) ⟇ antireverse(a) for every pair of blades, each of every
// signature of up to six generators listed here
TEST_CASE("The antiproducts are associative, with their identities and "
          "reversal rules") {
  for_each_signature([](const char* name, const auto& g) {
    constexpr std::size_t n = std::remove_cvref_t<decltype(g)>::generator_count;
    INFO(name);
    const std::vector<oriented_blade> all = all_blades(n);
    const full_plan product = full_plan_of<gp>(g);
    const full_plan antiproduct = full_plan_of<antigp>(g);
    const full_plan antiexterior = full_plan_of<antiwedge>(g);
    CHECK(associativity_failures(antiproduct, all) == 0);
    CHECK(associativity_failures(antiexterior, all) == 0);
    const mask_t antiscalar = (mask_t{1} << n) - 1U;
    CHECK(identity_failures(antiproduct, antiscalar, all) == 0);
    CHECK(identity_failures(product, 0, all) == 0);
    CHECK(antiwedge_unit_failures(antiexterior, n, all) == 0);
    CHECK(reversal_rule_failures(product, images(reverse, n), all) == 0);
    CHECK(reversal_rule_failures(antiproduct, images(antireverse, n), all) ==
          0);
  });
}

// Reference: proof: the closed form of the right complement's sign,
// (-1)^(k(k+1)/2 + i_1 + ... + i_k), is the sign that makes
// e_A ^ (s e_C) = e_12...n, counted here as the inversions of A then C, for
// every blade of every n from 1 to 10, so it does not depend on n
TEST_CASE("The closed form of the right complement's sign counts its "
          "inversions") {
  for (std::size_t n = 1; n <= largest_map_n; ++n) {
    INFO("n = " << n);
    int mismatches = 0;
    for (mask_t mask = 0; mask < (mask_t{1} << n); ++mask) {
      const mask_t other = (mask_t{1} << n) - 1U - mask;
      mismatches +=
          closed_form_right_sign(mask) == parity_sign(inversions(mask, other))
              ? 0
              : 1;
    }
    CHECK(mismatches == 0);
  }
}

// Reference: contract: unary_term gives the image of one canonical blade,
// its own blade for the reverse, the antireverse, and the grade involution
// and the blade of the other generators for the complements, with the sign
// of each, here in rigid(3) for e4, whose right complement is -e123, and in
// R(32,0,0) for the top generator, which the algebra of the most generators
// a mask holds accepts
TEST_CASE("unary_term gives the image of one blade") {
  CHECK(unary_term(reverse, e12, 4) == term{.blade = e12, .coefficient = -1});
  CHECK(unary_term(antireverse, e12, 4) ==
        term{.blade = e12, .coefficient = -1});
  CHECK(unary_term(involution, e4, 4) == term{.blade = e4, .coefficient = -1});
  CHECK(unary_term(right_complement, e4, 4) ==
        term{.blade = e123, .coefficient = -1});
  CHECK(unary_term(left_complement, e4, 4) ==
        term{.blade = e123, .coefficient = 1});
  constexpr mask_t top = mask_t{1} << (mask_width - 1);
  CHECK(unary_term(reverse, top, mask_width) ==
        term{.blade = top, .coefficient = 1});
  // e32 is preceded by the 31 other generators, an odd number.
  CHECK(unary_term(right_complement, top, mask_width) ==
        term{.blade = top - 1U, .coefficient = -1});
}

// Reference: contract: a mask listed twice in a set is not rejected, and
// each place that lists it gets its own term, here right(e1) = e234 for +e1
// and -e1 in rigid(3), which land on one position of the result, and a map
// of an empty set has an empty result and no terms
TEST_CASE("A repeated blade gets the map term of each place that lists it") {
  constexpr core::blade_set<3> repeated{
      {
          {
              {.mask = e1, .sign = 1},
              {.mask = e2, .sign = 1},
              {.mask = e1, .sign = -1},
          },
      },
  };
  CHECK(map_is(unary_map<right_complement, rga3, repeated>, {e134, e234},
               {
                   {.a = 0, .out = 1, .coefficient = 1},
                   {.a = 1, .out = 0, .coefficient = -1},
                   {.a = 2, .out = 1, .coefficient = -1},
               }));
  constexpr core::blade_set<0> none{};
  CHECK(unary_map<reverse, rga3, none> == typed_map<0, 0>{});
  CHECK(unary_map<left_complement, rga3, none> == typed_map<0, 0>{});
}

// Reference: contract: a typed map built at compile time is the same
// functions run at run time, so the maps of every operation over every blade
// of four and of five generators equal a run-time call with a number of
// generators that the compiler cannot know, and the antiproduct plans of
// every blade times every blade of rigid(3) and conformal(2) equal a
// run-time call on a metric that the compiler cannot know
TEST_CASE("unary_map and the antiproduct plans equal their typing run at run "
          "time") {
  const volatile std::size_t four_generators = 4;
  const volatile std::size_t five_generators = 5;
  const std::size_t four = four_generators;
  const std::size_t five = five_generators;
  CHECK(equals_map_typing(reverse, four, core::full<4>,
                          unary_map<reverse, rga3, core::full<4>>));
  CHECK(equals_map_typing(antireverse, four, core::full<4>,
                          unary_map<antireverse, rga3, core::full<4>>));
  CHECK(equals_map_typing(involution, four, core::full<4>,
                          unary_map<involution, rga3, core::full<4>>));
  CHECK(equals_map_typing(right_complement, four, core::full<4>,
                          unary_map<right_complement, rga3, core::full<4>>));
  CHECK(equals_map_typing(left_complement, four, core::full<4>,
                          unary_map<left_complement, rga3, core::full<4>>));
  CHECK(equals_map_typing(reverse, five, core::full<5>,
                          unary_map<reverse, conformal3, core::full<5>>));
  CHECK(equals_map_typing(antireverse, five, core::full<5>,
                          unary_map<antireverse, conformal3, core::full<5>>));
  CHECK(equals_map_typing(involution, five, core::full<5>,
                          unary_map<involution, conformal3, core::full<5>>));
  CHECK(equals_map_typing(
      right_complement, five, core::full<5>,
      unary_map<right_complement, conformal3, core::full<5>>));
  CHECK(
      equals_map_typing(left_complement, five, core::full<5>,
                        unary_map<left_complement, conformal3, core::full<5>>));

  constexpr const auto& all = core::full<4>;
  const core::metric<4> rigid = at_run_time(rga3::value);
  const core::metric<4> conformal = at_run_time(conformal2::value);
  CHECK(equals_typing<antigp, 8>(rigid, all, all,
                                 product_plan<antigp, rga3, all, all>));
  CHECK(equals_typing<antiwedge, 1>(rigid, all, all,
                                    product_plan<antiwedge, rga3, all, all>));
  CHECK(equals_typing<antigp, 8>(conformal, all, all,
                                 product_plan<antigp, conformal2, all, all>));
  CHECK(equals_typing<antiwedge, 1>(
      conformal, all, all, product_plan<antiwedge, conformal2, all, all>));
}

// Reference: contract: the geometric antiproduct of a pair needs as many
// terms as the geometric product of the complements of its blades, half the
// subsets of their generators together, and the antiwedge one, the capacity
// of a plan is the largest over its pairs, and each antiproduct names its
// base product
TEST_CASE("The capacities of the antiproducts are those of their base "
          "products over the complements") {
  using core::detail::base_operation;
  using core::detail::pair_capacity;
  // The complements of e1 and e2 in four generators, e234 and e134, hold
  // all four generators.
  CHECK(pair_capacity(antigp, e1, e2, 4) == 8);
  // The complement of e1234 is the scalar.
  CHECK(pair_capacity(antigp, e1234, e1234, 4) == 1);
  CHECK(pair_capacity(antigp, e123, e12, 4) == 2);
  CHECK(pair_capacity(antigp, e1, e1, 3) == 2);
  CHECK(pair_capacity(antiwedge, e1, e2, 4) == 1);
  CHECK(pair_capacity(antiwedge, 0, 0, 4) == 1);
  CHECK(plan_capacity(antigp, point.blades, point.blades, 4) == 8);
  CHECK(plan_capacity(antigp, plane.blades, plane.blades, 4) == 2);
  CHECK(plan_capacity(antiwedge, motor.blades, motor.blades, 4) == 1);
  CHECK(base_operation(gp) == gp);
  CHECK(base_operation(wedge) == wedge);
  CHECK(base_operation(antigp) == gp);
  CHECK(base_operation(antiwedge) == wedge);
}

// Reference: contract: an antiproduct's terms of one pair come in strictly
// increasing canonical order, so the check of a generated plan, which
// requires that order, reports the run of e1235 ⟇ e1234 in conformal(3) left
// in the order of the product of the complements, e12345 before e123, and
// accepts it reversed
TEST_CASE("The check of a generated plan finds an antiproduct's run that "
          "was not reversed") {
  using core::detail::defects_of;
  const std::array<plan_term, 2> reversed{
      {
          {.a = 0, .b = 0, .out = e123, .coefficient = -1},
          {.a = 0, .b = 0, .out = e1234 | e5, .coefficient = 1},
      },
  };
  const std::array<plan_term, 2> not_reversed{reversed.at(1), reversed.at(0)};
  CHECK_FALSE(defects_of(reversed, 1, 1, 5).out_of_order);
  CHECK(defects_of(not_reversed, 1, 1, 5).out_of_order);
}

// Reference: contract: the check of a typed map reports a result set out of
// strictly increasing canonical order, a result sign other than +1, a
// position at or past the size of the result, a result blade that no term
// uses, a term that is not that of the input blade at its place, a term not
// on the blade its operation gives the input (its own for the sign
// operations, the other generators' for the complements), and a
// complement's sign other than the input's sign times the closed form, each
// alone, and accepts each value just within its bound. The sign check
// applies to the complements only. In four generators right(e1) = e234 and
// right(e2) = -e134, and the left complements are their negatives
TEST_CASE("The checks of a typed map find each defect at its bound") {
  using core::detail::map_defects;
  using core::detail::map_defects_of;
  // Whether `found` reports the defect `which` and no other, or none when
  // `which` is null.
  const auto none_but = [](const map_defects& found,
                           bool map_defects::* which) {
    const std::array<bool map_defects::*, 7> all{
        &map_defects::result_out_of_order, &map_defects::result_sign,
        &map_defects::beyond_result,       &map_defects::result_unused,
        &map_defects::not_per_input,       &map_defects::wrong_blade,
        &map_defects::complement_sign,
    };
    return std::ranges::all_of(all, [&](bool map_defects::* flag) {
      return found.*flag == (flag == which);
    });
  };
  const core::blade_set<2> e1_e2{
      {
          {
              {.mask = e1, .sign = 1},
              {.mask = e2, .sign = 1},
          },
      },
  };
  const auto two = [](oriented_blade first, oriented_blade second, map_term t0,
                      map_term t1) {
    return typed_map<2, 2>{.result = {{{first, second}}}, .terms = {{t0, t1}}};
  };
  const oriented_blade on_e134{.mask = e134, .sign = 1};
  const oriented_blade on_e234{.mask = e234, .sign = 1};
  const map_term e1_to_1{.a = 0, .out = 1, .coefficient = 1};
  const map_term e2_to_0{.a = 1, .out = 0, .coefficient = -1};

  // The right and left complements of e1 and e2, as generated.
  CHECK(none_but(map_defects_of(right_complement, 4, e1_e2,
                                two(on_e134, on_e234, e1_to_1, e2_to_0)),
                 nullptr));
  CHECK(none_but(map_defects_of(left_complement, 4, e1_e2,
                                two(on_e134, on_e234,
                                    {.a = 0, .out = 1, .coefficient = -1},
                                    {.a = 1, .out = 0, .coefficient = 1})),
                 nullptr));

  // The result set: canonical order, not mask order, no repeat, sign +1 on
  // either side of it.
  CHECK(none_but(
      map_defects_of(right_complement, 4, e1_e2,
                     two(on_e234, on_e134, {.a = 0, .out = 0, .coefficient = 1},
                         {.a = 1, .out = 1, .coefficient = -1})),
      &map_defects::result_out_of_order));
  const core::blade_set<2> e4_e12{
      {
          {
              {.mask = e4, .sign = 1},
              {.mask = e12, .sign = 1},
          },
      },
  };
  CHECK(none_but(
      map_defects_of(reverse, 4, e4_e12,
                     two({.mask = e4, .sign = 1}, {.mask = e12, .sign = 1},
                         {.a = 0, .out = 0, .coefficient = 1},
                         {.a = 1, .out = 1, .coefficient = -1})),
      nullptr));
  const core::blade_set<2> e1_minus_e1{
      {
          {
              {.mask = e1, .sign = 1},
              {.mask = e1, .sign = -1},
          },
      },
  };
  CHECK(none_but(
      map_defects_of(right_complement, 4, e1_minus_e1,
                     two(on_e234, on_e234, {.a = 0, .out = 0, .coefficient = 1},
                         {.a = 1, .out = 1, .coefficient = -1})),
      &map_defects::result_out_of_order));
  for (const int sign : {-1, 0, 2}) {
    CHECK(none_but(map_defects_of(right_complement, 4, e1_e2,
                                  two(on_e134, {.mask = e234, .sign = sign},
                                      e1_to_1, e2_to_0)),
                   &map_defects::result_sign));
  }

  // Positions within the result, and every result blade used.
  const typed_map<1, 2> both_on_e234{
      .result = {{{on_e234}}},
      .terms =
          {
              {
                  {.a = 0, .out = 0, .coefficient = 1},
                  {.a = 1, .out = 0, .coefficient = -1},
              },
          },
  };
  CHECK(none_but(map_defects_of(right_complement, 4, e1_minus_e1, both_on_e234),
                 nullptr));
  const typed_map<1, 2> one_past{
      .result = {{{on_e234}}},
      .terms =
          {
              {
                  {.a = 0, .out = 0, .coefficient = 1},
                  {.a = 1, .out = 1, .coefficient = -1},
              },
          },
  };
  CHECK(none_but(map_defects_of(right_complement, 4, e1_minus_e1, one_past),
                 &map_defects::beyond_result));
  const typed_map<3, 2> extra_last{
      .result = {{{on_e134, on_e234, {.mask = e1234, .sign = 1}}}},
      .terms = {{e1_to_1, e2_to_0}},
  };
  CHECK(none_but(map_defects_of(right_complement, 4, e1_e2, extra_last),
                 &map_defects::result_unused));
  const typed_map<3, 2> extra_first{
      .result = {{{{.mask = e12, .sign = 1}, on_e134, on_e234}}},
      .terms =
          {
              {
                  {.a = 0, .out = 2, .coefficient = 1},
                  {.a = 1, .out = 1, .coefficient = -1},
              },
          },
  };
  CHECK(none_but(map_defects_of(right_complement, 4, e1_e2, extra_first),
                 &map_defects::result_unused));

  // One term per input, in order: a term at place k must have a = k.
  CHECK(none_but(
      map_defects_of(
          right_complement, 4, e1_e2,
          two(on_e134, on_e234, {.a = 1, .out = 1, .coefficient = 1}, e2_to_0)),
      &map_defects::not_per_input));
  CHECK(none_but(map_defects_of(right_complement, 4, e1_e2,
                                two(on_e134, on_e234, e1_to_1,
                                    {.a = 2, .out = 0, .coefficient = -1})),
                 &map_defects::not_per_input));
  CHECK(none_but(map_defects_of(right_complement, 4, e1_e2,
                                two(on_e134, on_e234, e1_to_1,
                                    {.a = 0, .out = 0, .coefficient = -1})),
                 &map_defects::not_per_input));

  // The blade: a complement lands on the other generators, a sign operation
  // on the blade itself.
  CHECK(none_but(
      map_defects_of(right_complement, 4, e1_e2,
                     two(on_e134, on_e234, {.a = 0, .out = 0, .coefficient = 1},
                         {.a = 1, .out = 1, .coefficient = -1})),
      &map_defects::wrong_blade));
  const core::blade_set<1> just_e1{{{{.mask = e1, .sign = 1}}}};
  const typed_map<1, 1> e1_on_e234{
      .result = {{{on_e234}}},
      .terms = {{{.a = 0, .out = 0, .coefficient = 1}}},
  };
  CHECK(none_but(map_defects_of(right_complement, 4, just_e1, e1_on_e234),
                 nullptr));
  CHECK(none_but(map_defects_of(reverse, 4, just_e1, e1_on_e234),
                 &map_defects::wrong_blade));
  const typed_map<1, 1> e1_on_e1{
      .result = {{{{.mask = e1, .sign = 1}}}},
      .terms = {{{.a = 0, .out = 0, .coefficient = 1}}},
  };
  CHECK(none_but(map_defects_of(reverse, 4, just_e1, e1_on_e1), nullptr));
  CHECK(none_but(map_defects_of(right_complement, 4, just_e1, e1_on_e1),
                 &map_defects::wrong_blade));

  // The sign of a complement: the closed form times the input's sign, and
  // for the left complement times (-1)^(k(n - k)), which is -1 for a vector
  // of four generators and +1 for one of three.
  CHECK(none_but(
      map_defects_of(right_complement, 4, e1_e2,
                     two(on_e134, on_e234,
                         {.a = 0, .out = 1, .coefficient = -1}, e2_to_0)),
      &map_defects::complement_sign));
  CHECK(none_but(map_defects_of(left_complement, 4, e1_e2,
                                two(on_e134, on_e234, e1_to_1, e2_to_0)),
                 &map_defects::complement_sign));
  const typed_map<1, 1> e1_on_e23{
      .result = {{{{.mask = e23, .sign = 1}}}},
      .terms = {{{.a = 0, .out = 0, .coefficient = 1}}},
  };
  CHECK(none_but(map_defects_of(left_complement, 3, just_e1, e1_on_e23),
                 nullptr));
  const core::blade_set<1> minus_e1{{{{.mask = e1, .sign = -1}}}};
  CHECK(none_but(map_defects_of(right_complement, 4, minus_e1, e1_on_e234),
                 &map_defects::complement_sign));
  const typed_map<1, 1> minus_e1_on_e234{
      .result = {{{on_e234}}},
      .terms = {{{.a = 0, .out = 0, .coefficient = -1}}},
  };
  CHECK(
      none_but(map_defects_of(right_complement, 4, minus_e1, minus_e1_on_e234),
               nullptr));
  // A sign operation's coefficient is not checked against a closed form.
  CHECK(none_but(
      map_defects_of(reverse, 4, just_e1,
                     typed_map<1, 1>{
                         .result = {{{{.mask = e1, .sign = 1}}}},
                         .terms = {{{.a = 0, .out = 0, .coefficient = -1}}},
                     }),
      nullptr));
}
