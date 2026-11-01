#ifndef ANTIPRODUCT_SPIKE_REDUCED_ANTIPRODUCT_HPP
#define ANTIPRODUCT_SPIKE_REDUCED_ANTIPRODUCT_HPP

// Geometric antiproduct plans by conjugation inside the generators a use is
// coupled to, a prototype built on the plan generator of galoot/core/plan.hpp.
//
// Call two generators coupled when the form pairs them, g_ij != 0 for i != j,
// and let C be the generators of the input blades together with every
// generator coupled to them, directly or through others. The rest, Q, is
// orthogonal to C. For blades e_A and e_B on C,
//
//   e_A ⟇ e_B = γ (-1)^(|Q|(|C| + |A|)) τ(C, Q)
//               Σ_E a_E τ(E, Q) e_(E ∪ Q),
//
// where Σ_E a_E e_E is the antiproduct of e_A and e_B in the algebra on C
// alone (complements taken inside C), γ = e_Q e_Q is a scalar, the square of
// the blade of the generators outside C, and τ(X, Y) is the sign of
// reordering e_X e_Y (reorder_sign). So the antiproduct is the same
// conjugation of blade_product as before, run on complements inside C rather
// than inside all n generators, with one scalar and per-blade signs added.
// Its cost follows the size of C, not n.

#include <array>
#include <bit>
#include <cstddef>
#include <span>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace antiproduct_spike {

namespace core = galoot::core;
namespace detail = galoot::core::detail;
using core::mask_t;
using core::coefficient_type;
using detail::plan_term;

// (-1)^k.
[[nodiscard]] constexpr coefficient_type parity_sign(int k) noexcept {
  return k % 2 == 0 ? 1 : -1;
}

// The generators of `used` and every generator coupled to one of them,
// directly or through others: one row of the form for each generator reached.
template <std::size_t N>
[[nodiscard]] constexpr mask_t coupled_closure(const core::metric<N>& g,
                                               mask_t used) noexcept {
  mask_t closure = used;
  mask_t frontier = used;
  while (frontier != 0) {
    const auto i = static_cast<std::size_t>(std::countr_zero(frontier));
    frontier &= frontier - 1;
    for (std::size_t j = 0; j < N; ++j) {
      const mask_t bit = mask_t{1} << j;
      if (j != i && g.gram[i][j] != 0 && (closure & bit) == 0) {
        closure |= bit;
        frontier |= bit;
      }
    }
  }
  return closure;
}

// Where a use splits the generators: C, the closure of the used generators,
// and Q, the rest, with the capacity a product of a component of Q with
// itself needs.
struct reduction {
  mask_t inside;
  mask_t outside;
  std::size_t component_capacity;
};

template <std::size_t N>
[[nodiscard]] constexpr reduction reduction_of(const core::metric<N>& g,
                                               mask_t used) noexcept {
  reduction r{};
  r.inside = coupled_closure(g, used);
  r.outside = detail::low_bits(N) & ~r.inside;
  r.component_capacity = 1;
  for (mask_t rest = r.outside; rest != 0;) {
    const mask_t component = coupled_closure(g, rest & (~rest + 1U));
    rest &= ~component;
    const std::size_t capacity = detail::product_capacity(component, component);
    if (capacity > r.component_capacity) {
      r.component_capacity = capacity;
    }
  }
  return r;
}

// γ = e_Q e_Q for Q a union of components: (-1)^(|Q|(|Q|-1)/2) times the
// product of det_K = (-1)^(|K|(|K|-1)/2) e_K e_K over the components K of Q,
// each square a blade_product of the component with itself.
template <std::size_t Capacity, std::size_t N>
[[nodiscard]] constexpr coefficient_type
spectator_square(const core::metric<N>& g, mask_t outside) noexcept {
  coefficient_type det = 1;
  for (mask_t rest = outside; rest != 0 && det != 0;) {
    const mask_t component = coupled_closure(g, rest & (~rest + 1U));
    rest &= ~component;
    const auto square = detail::blade_product<Capacity>(g, component, component);
    const coefficient_type value =
        square.size() == 0 ? 0 : square.terms[0].coefficient;
    const int k = core::grade(component);
    det = detail::multiply_checked(det, parity_sign(k * (k - 1) / 2) * value);
  }
  const int q = core::grade(outside);
  return parity_sign(q * (q - 1) / 2) * det;
}

// The capacity of the pairs' products inside C.
[[nodiscard]] constexpr std::size_t
reduced_capacity(std::span<const core::oriented_blade> as,
                 std::span<const core::oriented_blade> bs,
                 mask_t inside) noexcept {
  std::size_t capacity = 1;
  for (const core::oriented_blade& x : as) {
    for (const core::oriented_blade& y : bs) {
      const std::size_t pair =
          detail::product_capacity(inside & ~x.mask, inside & ~y.mask);
      if (pair > capacity) {
        capacity = pair;
      }
    }
  }
  return capacity;
}

// The plan of the geometric antiproduct over `as` and `bs`, whose masks lie
// in `inside`, a union of components; `spectators` is γ for the generators
// outside. Writes the first out.size() terms and returns the number of all
// terms, as generate_plan does, in the same order.
template <std::size_t Capacity, std::size_t N>
constexpr std::size_t
generate_reduced_plan(const core::metric<N>& g, mask_t inside,
                      coefficient_type spectators,
                      std::span<const core::oriented_blade> as,
                      std::span<const core::oriented_blade> bs,
                      std::span<plan_term> out) noexcept {
  if (spectators == 0) {
    return 0;
  }
  const mask_t outside = detail::low_bits(N) & ~inside;
  const int q = core::grade(outside);
  const coefficient_type constant =
      spectators * parity_sign(q * core::grade(inside)) *
      core::reorder_sign(inside, outside);
  std::size_t count = 0;
  for (std::size_t a = 0; a < as.size(); ++a) {
    for (std::size_t b = 0; b < bs.size(); ++b) {
      const mask_t first = inside & ~as[a].mask;
      const mask_t second = inside & ~bs[b].mask;
      const coefficient_type sign =
          static_cast<coefficient_type>(as[a].sign) * bs[b].sign *
          core::reorder_sign(as[a].mask, first) *
          parity_sign(q * core::grade(as[a].mask)) *
          core::reorder_sign(bs[b].mask, second);
      const auto terms = detail::blade_product<Capacity>(g, first, second);
      for (std::size_t k = 0; k < terms.size(); ++k) {
        const detail::term& base = terms.terms[terms.size() - 1 - k];
        const mask_t e = inside & ~base.blade;
        const coefficient_type c = base.coefficient *
                                   core::reorder_sign(e, base.blade) *
                                   core::reorder_sign(e, outside);
        if (count < out.size()) {
          out[count] = {
              .a = a,
              .b = b,
              .out = e | outside,
              .coefficient = detail::multiply_checked(constant, sign * c),
          };
        }
        ++count;
      }
    }
  }
  return count;
}

template <std::size_t K>
[[nodiscard]] constexpr mask_t used_generators(const core::blade_set<K>& s) {
  mask_t used = 0;
  for (const core::oriented_blade& x : s.blades) {
    used |= x.mask;
  }
  return used;
}

template <std::size_t Count, std::size_t Capacity, std::size_t N>
[[nodiscard]] constexpr std::array<plan_term, Count>
filled_reduced_plan(const core::metric<N>& g, mask_t inside,
                    coefficient_type spectators,
                    std::span<const core::oriented_blade> as,
                    std::span<const core::oriented_blade> bs) noexcept {
  std::array<plan_term, Count> plan{};
  generate_reduced_plan<Capacity>(g, inside, spectators, as, bs, plan);
  return plan;
}

// The data each generator brings, kept once per algebra: the mask of its
// coupling component, and for the lowest generator of each component the
// component's determinant (-1)^(|K|(|K|-1)/2) e_K e_K (1 elsewhere).
template <std::size_t N>
struct coupling_data {
  std::array<mask_t, N> component;
  std::array<coefficient_type, N> det;
};

template <std::size_t N>
[[nodiscard]] constexpr std::array<mask_t, N>
components_of(const core::metric<N>& g) noexcept {
  std::array<mask_t, N> component{};
  for (std::size_t i = 0; i < N; ++i) {
    if (component[i] == 0) {
      const mask_t k = coupled_closure(g, mask_t{1} << i);
      for (mask_t rest = k; rest != 0; rest &= rest - 1) {
        component[static_cast<std::size_t>(std::countr_zero(rest))] = k;
      }
    }
  }
  return component;
}

template <std::size_t N>
[[nodiscard]] constexpr std::size_t
largest_component_capacity(const std::array<mask_t, N>& component) noexcept {
  std::size_t capacity = 1;
  for (const mask_t k : component) {
    const std::size_t c = detail::product_capacity(k, k);
    if (c > capacity) {
      capacity = c;
    }
  }
  return capacity;
}

template <std::size_t Capacity, std::size_t N>
[[nodiscard]] constexpr std::array<coefficient_type, N>
component_dets(const core::metric<N>& g,
               const std::array<mask_t, N>& component) noexcept {
  std::array<coefficient_type, N> det{};
  for (std::size_t i = 0; i < N; ++i) {
    const mask_t k = component[i];
    det[i] = 1;
    if (static_cast<std::size_t>(std::countr_zero(k)) == i) {
      const auto square = detail::blade_product<Capacity>(g, k, k);
      const int size = core::grade(k);
      det[i] = parity_sign(size * (size - 1) / 2) *
               (square.size() == 0 ? 0 : square.terms[0].coefficient);
    }
  }
  return det;
}

template <core::metric_like Alg>
[[nodiscard]] consteval auto make_coupling() {
  static constexpr auto component = components_of(Alg::value);
  static constexpr std::size_t capacity = largest_component_capacity(component);
  static constexpr auto det = component_dets<capacity>(Alg::value, component);
  return coupling_data<Alg::value.generator_count>{component, det};
}

/// The coupling components of the algebra of Alg, computed once per algebra.
template <core::metric_like Alg>
inline constexpr auto coupling = make_coupling<Alg>();

// C and γ for a plan whose blades use the generators `used`.
struct plan_split {
  mask_t inside;
  coefficient_type spectators;
};

// From the per-algebra data: one step per used generator and one per
// component outside C.
template <std::size_t N>
[[nodiscard]] constexpr plan_split split_from(const coupling_data<N>& c,
                                              mask_t used) noexcept {
  mask_t inside = 0;
  for (mask_t rest = used; rest != 0; rest &= rest - 1) {
    inside |= c.component[static_cast<std::size_t>(std::countr_zero(rest))];
  }
  const mask_t outside = detail::low_bits(N) & ~inside;
  coefficient_type det = 1;
  for (mask_t rest = outside; rest != 0 && det != 0;) {
    const auto i = static_cast<std::size_t>(std::countr_zero(rest));
    det = detail::multiply_checked(det, c.det[i]);
    rest &= ~c.component[i];
  }
  const int q = core::grade(outside);
  return {.inside = inside, .spectators = parity_sign(q * (q - 1) / 2) * det};
}

// Per plan (PerAlgebra false: the closure and the spectators' squares from the
// rows of the form) or from the per-algebra data (true).
template <bool PerAlgebra, core::metric_like Alg, mask_t Used>
[[nodiscard]] consteval plan_split split_for() {
  if constexpr (PerAlgebra) {
    return split_from(coupling<Alg>, Used);
  } else {
    static constexpr reduction split = reduction_of(Alg::value, Used);
    static constexpr coefficient_type spectators =
        spectator_square<split.component_capacity>(Alg::value, split.outside);
    return {.inside = split.inside, .spectators = spectators};
  }
}

// As make_product_plan for the geometric antiproduct, with the same checks
// and typing, every local static constexpr.
template <bool PerAlgebra, core::metric_like Alg, core::blade_set A,
          core::blade_set B>
[[nodiscard]] consteval auto make_reduced_plan() {
  static constexpr std::size_t n = Alg::value.generator_count;
  static constexpr bool in_range =
      detail::all_in_algebra(A, n) && detail::all_in_algebra(B, n);
  static constexpr bool valid_signs =
      detail::signs_valid<A> && detail::signs_valid<B>;
  static_assert(in_range, "reduced plan: a blade set names a generator "
                          "beyond e_n");
  static_assert(valid_signs, "reduced plan: a blade set holds a sign other "
                             "than +1 or -1");
  if constexpr (in_range && valid_signs) {
    static constexpr plan_split split =
        split_for<PerAlgebra, Alg, used_generators(A) | used_generators(B)>();
    static constexpr coefficient_type spectators = split.spectators;
    static constexpr std::size_t capacity =
        reduced_capacity(A.blades, B.blades, split.inside);
    static constexpr std::size_t count = generate_reduced_plan<capacity>(
        Alg::value, split.inside, spectators, A.blades, B.blades, {});
    static constexpr auto generated = filled_reduced_plan<count, capacity>(
        Alg::value, split.inside, spectators, A.blades, B.blades);

    static constexpr detail::plan_defects defects =
        detail::defects_of(generated, A.size(), B.size(), n);
    static_assert(!defects.zero_coefficient &&
                      !defects.beyond_first && !defects.beyond_second &&
                      !defects.beyond_n && !defects.out_of_order,
                  "reduced plan: a generated term is defective");

    static constexpr auto found = detail::collected(generated);
    static constexpr auto plan = detail::typed<found.size>(generated, found);
    static constexpr detail::typed_defects typing =
        detail::typed_defects_of(plan.result, plan.terms);
    static_assert(!typing.result_out_of_order && !typing.result_sign &&
                      !typing.beyond_result && !typing.result_unused &&
                      !typing.out_of_order,
                  "reduced plan: the typing is defective");
    return plan;
  } else {
    return detail::typed_plan<0, 0>{};
  }
}

/// The reduced plan, with C and γ computed for the plan.
template <core::metric_like Alg, core::blade_set A, core::blade_set B>
inline constexpr auto reduced_plan = make_reduced_plan<false, Alg, A, B>();

/// The reduced plan, with C and γ from the per-algebra coupling data.
template <core::metric_like Alg, core::blade_set A, core::blade_set B>
inline constexpr auto cached_plan = make_reduced_plan<true, Alg, A, B>();

} // namespace antiproduct_spike

#endif // ANTIPRODUCT_SPIKE_REDUCED_ANTIPRODUCT_HPP
