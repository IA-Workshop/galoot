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

namespace {

namespace core = galoot::core;
using core::coefficient_type;
using core::mask_t;
using core::oriented_blade;
using core::detail::generate_gp;
using core::detail::gp_plan;
using core::detail::plan_capacity;
using core::detail::plan_term;
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

// Whether generate_gp, called directly, writes the same plan as gp_plan.
template <std::size_t Capacity, std::size_t N, std::size_t KA, std::size_t KB,
          std::size_t T>
constexpr bool equals_generation(const core::metric<N>& g,
                                 const core::blade_set<KA>& as,
                                 const core::blade_set<KB>& bs,
                                 const std::array<plan_term, T>& plan) {
  std::array<plan_term, T> again{};
  const std::size_t count =
      generate_gp<Capacity>(g, as.blades, bs.blades, again);
  return count == T && again == plan;
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
static_assert(gp_plan<rga3, point, point> ==
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
static_assert(gp_plan<rga3, line, plane>.size() == 15);
static_assert(follows_diagonal_rule(rga3::value, line, plane,
                                    gp_plan<rga3, line, plane>));
static_assert(gp_plan<rga3, motor, point>.size() == 28);
static_assert(follows_diagonal_rule(rga3::value, motor, point,
                                    gp_plan<rga3, motor, point>));
static_assert(gp_plan<rga3, point, motor>.size() == 28);
static_assert(follows_diagonal_rule(rga3::value, point, motor,
                                    gp_plan<rga3, point, motor>));

// Reference: contract: a coefficient stored on s e_A multiplies s e_A, so
// the plan of oriented blades is the plan of their masks with each
// coefficient times s_a s_b, and an oriented name lists generators in the
// order of their product, so e41 = -e14 and e31 = -e13. A line times a line
// has 6 x 6 - 3 x 3 = 27 terms, as e4 e4 = 0
static_assert(gp_plan<rga3, line, line>.size() == 27);
static_assert(folds_signs(
    gp_plan<rga3, line, line>,
    gp_plan<rga3, with_plus_signs(line), with_plus_signs(line)>, line, line));
static_assert(folds_signs(
    gp_plan<rga3, line, plane>,
    gp_plan<rga3, with_plus_signs(line), with_plus_signs(plane)>, line, plane));

// Reference: independent value: the sparse sets {1, e1, e_n, e1 ^ e_n} and
// {e1, e2, e_n} in R(n - 1, 0, 1), where e_n e_n = 0, give ten terms worked
// by hand, at n = 8, 12, and 16, so that a plan in a large algebra costs
// what its sets use
static_assert(gp_plan<rigid_n<8>, sparse_first<8>, sparse_second<8>> ==
              sparse_terms<8>());
static_assert(gp_plan<rigid_n<12>, sparse_first<12>, sparse_second<12>> ==
              sparse_terms<12>());
static_assert(gp_plan<rigid_n<16>, sparse_first<16>, sparse_second<16>> ==
              sparse_terms<16>());

// Reference: contract: the plan is generate_gp's output, so the memoized
// plans of every blade of rigid(3), R(4,0,0), and R(1,3,0) times every blade
// equal a direct call of generate_gp in a constant evaluation
static_assert(
    equals_generation<8>(rga3::value, core::full<4>, core::full<4>,
                         gp_plan<rga3, core::full<4>, core::full<4>>));
static_assert(
    equals_generation<8>(r400::value, core::full<4>, core::full<4>,
                         gp_plan<r400, core::full<4>, core::full<4>>));
static_assert(
    equals_generation<8>(r130::value, core::full<4>, core::full<4>,
                         gp_plan<r130, core::full<4>, core::full<4>>));

// Reference: contract: a plan term is a trivially copyable aggregate, and a
// plan is a std::array of them
static_assert(std::is_aggregate_v<plan_term>);
static_assert(std::is_trivially_copyable_v<plan_term>);
static_assert(std::is_same_v<decltype(gp_plan<rga3, point, point>),
                             const std::array<plan_term, 15>>);

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
  plan.counted = generate_gp<Capacity>(g, as, bs, {});
  plan.terms.resize(plan.counted);
  plan.written = generate_gp<Capacity>(g, as, bs, plan.terms);
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

// Two uses of one plan, in two functions, which must share one evaluation.
std::size_t point_terms() { return gp_plan<rga3, point, point>.size(); }
coefficient_type point_scalar() {
  return gp_plan<rga3, point, point>.front().coefficient;
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
    constexpr std::size_t capacity = plan_capacity(all.blades, all.blades);
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

  check_properties(gp_plan<rga3, point, point>, 4, 4, 4);
  check_properties(gp_plan<rga3, line, line>, 6, 6, 4);
  check_properties(gp_plan<rga3, line, plane>, 6, 4, 4);
  check_properties(gp_plan<rga3, motor, point>, 8, 4, 4);
  check_properties(gp_plan<rga3, point, motor>, 4, 8, 4);
  check_properties(gp_plan<rigid_n<16>, sparse_first<16>, sparse_second<16>>, 4,
                   3, 16);
}

// Reference: contract: gp_plan and generate_gp are one constexpr function,
// so the plans of every blade times every blade of rigid(3), R(4,0,0), and
// R(1,3,0) built in a constant evaluation equal the same function run at run
// time on a metric that the compiler cannot know
TEST_CASE("gp_plan equals generate_gp run at run time") {
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
  CHECK(same(read_at_run_time(rga3::value), gp_plan<rga3, all, all>));
  CHECK(same(read_at_run_time(r400::value), gp_plan<r400, all, all>));
  CHECK(same(read_at_run_time(r130::value), gp_plan<r130, all, all>));
}

// Reference: contract: generate_gp writes as many terms as fit in its
// output and returns the count of all, so an empty output counts and a
// short one is filled without a write past its end
TEST_CASE("generate_gp counts every term and writes only what fits") {
  const plan_term untouched{.a = 9, .b = 9, .out = 0, .coefficient = 9};
  std::array<plan_term, 5> storage{};
  storage.fill(untouched);
  const std::size_t count = generate_gp<2>(
      rga3::value, point.blades, point.blades, std::span(storage).first(3));
  CHECK(count == 15);
  CHECK(storage.at(0) == plan_term{.a = 0, .b = 0, .out = 0, .coefficient = 1});
  CHECK(storage.at(1) ==
        plan_term{.a = 0, .b = 1, .out = e12, .coefficient = 1});
  CHECK(storage.at(2) ==
        plan_term{.a = 0, .b = 2, .out = e13, .coefficient = 1});
  CHECK(storage.at(3) == untouched);
  CHECK(storage.at(4) == untouched);

  CHECK(generate_gp<2>(rga3::value, point.blades, point.blades, {}) == 15);
  std::array<plan_term, 16> roomy{};
  roomy.fill(untouched);
  CHECK(generate_gp<2>(rga3::value, point.blades, point.blades, roomy) == 15);
  CHECK(std::ranges::equal(std::span(roomy).first(15),
                           gp_plan<rga3, point, point>));
  CHECK(roomy.at(15) == untouched);
}

// Reference: independent value: a product of no blades has no terms, so a
// plan with an empty set is empty and its capacity is the smallest, 1
TEST_CASE("A plan with an empty blade set is empty") {
  constexpr core::blade_set<0> none{};
  CHECK(gp_plan<rga3, none, point>.empty());
  CHECK(gp_plan<rga3, point, none>.empty());
  CHECK(gp_plan<rga3, none, none>.empty());
  CHECK(generate_gp<1>(rga3::value, none.blades, point.blades, {}) == 0);
  CHECK(plan_capacity(none.blades, point.blades) == 1);
  CHECK(plan_capacity(point.blades, none.blades) == 1);
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
  CHECK(gp_plan<r300, repeated, just_e1> ==
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
  CHECK(plan_capacity(point.blades, point.blades) ==
        largest_pair_bound(point, point));
  CHECK(plan_capacity(point.blades, point.blades) == 2);
  CHECK(plan_capacity(line.blades, plane.blades) == 8);
  CHECK(plan_capacity(motor.blades, point.blades) == 8);
  CHECK(plan_capacity(sparse_first<16>.blades, sparse_second<16>.blades) == 4);
  CHECK(plan_capacity(sparse_first<16>.blades, sparse_second<16>.blades) ==
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
  CHECK(plan_capacity(middle.blades, just_e1.blades) == 4);
  CHECK(plan_capacity(just_e1.blades, middle.blades) == 4);
  CHECK(plan_capacity(just_e1.blades, just_e1.blades) == 1);
  constexpr core::blade_set<1> just_e2{{{{.mask = e2, .sign = 1}}}};
  CHECK(plan_capacity(just_e1.blades, just_e2.blades) == 2);
}

// Reference: independent value: in R(32,0,0), the widest algebra a mask
// holds, e32 e32 = 1, so a blade on the top bit is within the algebra
TEST_CASE("gp_plan accepts the generators of the widest algebra") {
  constexpr mask_t top = mask_t{1} << (mask_width - 1);
  constexpr core::blade_set<2> tops{
      {
          {
              {.mask = e1, .sign = 1},
              {.mask = top, .sign = -1},
          },
      },
  };
  CHECK(gp_plan<widest, tops, tops> ==
        std::array<plan_term, 4>{{
            {.a = 0, .b = 0, .out = 0, .coefficient = 1},
            {.a = 0, .b = 1, .out = e1 | top, .coefficient = -1},
            {.a = 1, .b = 0, .out = e1 | top, .coefficient = 1},
            {.a = 1, .b = 1, .out = 0, .coefficient = 1},
        }});
}

// Reference: contract: a plan is generated once for each algebra and pair of
// sets, so two functions that read the point times point plan of rigid(3)
// read one value, its 15 terms with e1 e1 = 1 first
TEST_CASE("Two uses of one plan read the same plan") {
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
