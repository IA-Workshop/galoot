// The reduced antiproduct plan against the conjugated one of plan.hpp: at
// compile time for the typed rigid(3) pairs embedded in R(n-1,0,1) and for
// full sets of small algebras, and at run time for every pair of single
// blades, with both signs, in every R(p,q,r) with n <= 6 and in the
// non-diagonal forms of the signature set.

#include <array>
#include <cstddef>
#include <cstdio>
#include <span>
#include <vector>

#include "reduced_antiproduct.hpp"

namespace core = galoot::core;
namespace detail = galoot::core::detail;
namespace spike = antiproduct_spike;

namespace {

// The rigid(3) sets with e4 read as e_n: masks and the signs of the
// oriented names.
template <std::size_t N>
struct rigid_sets {
  static constexpr core::mask_t en = core::mask_t{1} << (N - 1);
  static constexpr core::blade_set<4> point{{{{1, 1}, {2, 1}, {4, 1}, {en, 1}}}};
  static constexpr core::blade_set<6> line{{{{en | 1, -1}, {en | 2, -1},
                                             {en | 4, -1}, {6, 1}, {5, -1},
                                             {3, 1}}}};
  static constexpr core::blade_set<4> plane{{{{en | 6, 1}, {en | 5, -1},
                                              {en | 3, 1}, {7, -1}}}};
  static constexpr core::blade_set<8> motor{{{{0, 1}, {en | 1, -1},
                                              {en | 2, -1}, {en | 4, -1},
                                              {6, 1}, {5, -1}, {3, 1},
                                              {en | 7, 1}}}};
};

template <std::size_t N>
struct rigid_alg {
  static constexpr auto value = core::R<N - 1, 0, 1>();
};

template <std::size_t N>
consteval bool rigid_pairs_agree() {
  using S = rigid_sets<N>;
  using Alg = rigid_alg<N>;
  constexpr auto anti = detail::BinaryOperation::antigp;
  return spike::reduced_plan<Alg, S::point, S::point> ==
             detail::product_plan<anti, Alg, S::point, S::point> &&
         spike::reduced_plan<Alg, S::line, S::plane> ==
             detail::product_plan<anti, Alg, S::line, S::plane> &&
         spike::reduced_plan<Alg, S::motor, S::point> ==
             detail::product_plan<anti, Alg, S::motor, S::point> &&
         spike::reduced_plan<Alg, S::point, S::motor> ==
             detail::product_plan<anti, Alg, S::point, S::motor>;
}

constexpr auto anti = detail::BinaryOperation::antigp;

// RUNTIME_ONLY leaves out the compile-time comparisons, for mutants.
#ifndef RUNTIME_ONLY
static_assert(rigid_pairs_agree<4>());
static_assert(rigid_pairs_agree<5>());
static_assert(rigid_pairs_agree<8>());

struct conformal2 {
  static constexpr auto value = core::conformal<2>();
};
struct conformal3 {
  static constexpr auto value = core::conformal<3>();
};
static_assert(spike::reduced_plan<rigid_alg<4>, core::full<4>, core::full<4>> ==
              detail::product_plan<anti, rigid_alg<4>, core::full<4>,
                                   core::full<4>>);
static_assert(spike::reduced_plan<conformal2, core::full<4>, core::full<4>> ==
              detail::product_plan<anti, conformal2, core::full<4>,
                                   core::full<4>>);
// A use on e1, e2, and the origin of conformal(3): the closure adds the point
// at infinity and leaves e3 outside.
constexpr core::blade_set<3> conformal_use{{{{1, 1}, {2, -1}, {8, 1}}}};
constexpr core::blade_set<2> conformal_other{{{{3, 1}, {9, 1}}}};
static_assert(spike::reduction_of(conformal3::value, 11).inside == 27);
static_assert(spike::reduced_plan<conformal3, conformal_use, conformal_other> ==
              detail::product_plan<anti, conformal3, conformal_use,
                                   conformal_other>);
#endif

constexpr std::size_t capacity = 32; // product_capacity of n = 6 generators

template <std::size_t N>
int compare_all_pairs(const core::metric<N>& g, const char* name) {
  int failures = 0;
  std::array<detail::plan_term, capacity> reduced{};
  std::array<detail::plan_term, capacity> conjugated{};
  for (core::mask_t a = 0; a < (core::mask_t{1} << N); ++a) {
    for (core::mask_t b = 0; b < (core::mask_t{1} << N); ++b) {
      for (int sign : {1, -1}) {
        const std::array<core::oriented_blade, 1> as{{{a, sign}}};
        const std::array<core::oriented_blade, 1> bs{{{b, 1}}};
        const spike::reduction split = spike::reduction_of(g, a | b);
        const auto gamma =
            spike::spectator_square<capacity>(g, split.outside);
        const std::size_t r = spike::generate_reduced_plan<capacity>(
            g, split.inside, gamma, as, bs, reduced);
        const std::size_t c =
            detail::generate_plan<anti, capacity>(g, as, bs, conjugated);
        bool same = r == c;
        for (std::size_t k = 0; same && k < r; ++k) {
          same = reduced[k] == conjugated[k];
        }
        if (!same) {
          if (failures < 3) {
            std::printf("%s: e_A %u, e_B %u, sign %d differ\n", name,
                        static_cast<unsigned>(a), static_cast<unsigned>(b),
                        sign);
          }
          ++failures;
        }
      }
    }
  }
  return failures;
}

template <std::size_t N>
int all_diagonal_forms(int& forms) {
  int failures = 0;
  for (std::size_t p = 0; p <= N; ++p) {
    for (std::size_t q = 0; p + q <= N; ++q) {
      core::metric<N> g{};
      for (std::size_t i = 0; i < p + q; ++i) {
        g.gram[i][i] = i < p ? 1 : -1;
      }
      failures += compare_all_pairs(g, "R(p,q,r)");
      ++forms;
    }
  }
  return failures;
}

template <std::size_t N>
core::metric<N> block_form(std::initializer_list<std::array<int, 3>> entries) {
  core::metric<N> g{};
  for (const auto& [i, j, x] : entries) {
    g.gram[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] = x;
    g.gram[static_cast<std::size_t>(j)][static_cast<std::size_t>(i)] = x;
  }
  return g;
}

} // namespace

int main() {
  int forms = 0;
  int failures = 0;
  failures += all_diagonal_forms<1>(forms);
  failures += all_diagonal_forms<2>(forms);
  failures += all_diagonal_forms<3>(forms);
  failures += all_diagonal_forms<4>(forms);
  failures += all_diagonal_forms<5>(forms);
  failures += all_diagonal_forms<6>(forms);
  failures += compare_all_pairs(core::conformal<1>(), "conformal(1)");
  failures += compare_all_pairs(core::conformal<2>(), "conformal(2)");
  failures += compare_all_pairs(core::conformal<3>(), "conformal(3)");
  failures += compare_all_pairs(core::conformal<4>(), "conformal(4)");
  failures += compare_all_pairs(block_form<2>({{0, 1, 1}}), "Witt 1");
  failures += compare_all_pairs(block_form<4>({{0, 1, 1}, {2, 3, 1}}),
                                "Witt 2");
  failures += compare_all_pairs(
      block_form<6>({{0, 1, 1}, {2, 3, 1}, {4, 4, 1}}), "Witt 2 + e+ + e0");
  failures += compare_all_pairs(block_form<3>({{0, 1, 1}}), "degenerate");
  failures += compare_all_pairs(
      block_form<3>({{0, 0, 1}, {0, 1, 1}, {1, 2, 1}, {2, 2, -1}}),
      "radical off the generators");
  failures += compare_all_pairs(
      block_form<6>({{0, 0, 1}, {1, 1, 1}, {2, 3, -1}, {5, 5, -1}}),
      "conformal(2) + null + negative");
  failures += compare_all_pairs(
      block_form<5>({{0, 0, 2}, {1, 1, -3}, {3, 3, 5}, {2, 4, 1}}),
      "diag(2,-3,.,5) + Witt");
  forms += 4 + 7;
  std::printf("%d forms, %d differing pairs\n", forms, failures);
  return failures == 0 ? 0 : 1;
}
