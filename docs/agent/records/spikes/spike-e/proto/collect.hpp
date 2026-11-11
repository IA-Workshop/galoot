#ifndef SPIKE_E_PROTO_COLLECT_HPP
#define SPIKE_E_PROTO_COLLECT_HPP

// A collection pass over the two kernels of the library's transform, run at
// compile time, and the straight-line code that applies its result.
//
// transform(x, v) is two kernels: the first gives the middle coefficients
// w_m = sum c1 v_a x_b, the second the result y_o = sum c2 w_m v_b over the
// terms that reach x's blades. Composed, every result is
//
//   y_o = sum over (j, p <= q) of C[o][j][p][q] v_p v_q x_j,
//
// a polynomial linear in x and quadratic in v, so y = M(v) x with each entry
// M[o][j] = sum C[o][j][p][q] v_p v_q. The pass computes C exactly from the
// kernels' terms, then:
//
// - divides each entry by the greatest common divisor of its coefficients,
//   the entry's scale, applied once to the entry's sum;
// - writes each entry with a positive coefficient, carrying a minus sign into
//   the result's sum, so that a sum starts with an assignment, not a
//   negation, where it can;
// - shares an entry that equals another (after its scale and sign) and each
//   product v_p v_q among all entries.
//
// The result is three kernels: the products, the entries from the products,
// and y from the entries and x. The products and entries read only v, so a
// batch of objects under one versor computes them once.
//
// Assumptions that the library's transform does not make: the scalar's
// multiplication commutes (v_p v_q and v_q v_p are one product), and a sum
// may be evaluated in another order and grouping than the two antiproducts'
// (the result differs in rounding, not in exact value).

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

#include <galoot/core/transform.hpp>

namespace collect {

namespace core = galoot::core;
namespace detail = galoot::core::detail;
using coefficient = core::coefficient_type;
using term = detail::kernel_term;

inline constexpr std::size_t none = static_cast<std::size_t>(-1);

constexpr coefficient magnitude(coefficient c) { return c < 0 ? -c : c; }

constexpr coefficient gcd(coefficient a, coefficient b) {
  while (b != 0) {
    const coefficient r = a % b;
    a = b;
    b = r;
  }
  return a;
}

// The pass's working result, on fixed bounds: X * X entries of V * V
// products each. Entry m's coefficients are rows[m * V * V + p * V + q],
// p <= q.
template <std::size_t V, std::size_t X>
struct summary {
  std::array<coefficient, X * X * V * V> rows;
  std::array<coefficient, X * X> scales;
  // For result o and input j, at o * X + j: the entry, or none, and the
  // sign that carries it into y_o.
  std::array<std::size_t, X * X> entry_of;
  std::array<coefficient, X * X> sign_of;
  // For the pair p * V + q: the product's place, or none.
  std::array<std::size_t, V * V> product_of;
  std::size_t composed;
  std::size_t products;
  std::size_t entries;
  std::size_t entry_terms;
  std::size_t apply_terms;
};

// The pass, from the first kernel (onto M middle places) and the kept second
// kernel (onto X's places) of a transform by a versor on V places.
template <std::size_t V, std::size_t X, std::size_t M, std::size_t N1,
          std::size_t N2>
consteval summary<V, X> summarize(const std::array<term, N1>& first,
                                  const std::array<term, N2>& second) {
  constexpr std::size_t pairs = V * V;
  summary<V, X> s{};

  // The first kernel's terms by middle place, so that each second term
  // meets only the first terms it multiplies.
  std::array<std::size_t, M + 1> start{};
  for (const term& t : first) {
    ++start[t.out + 1];
  }
  for (std::size_t m = 0; m < M; ++m) {
    start[m + 1] += start[m];
  }
  std::array<std::size_t, N1> by_middle{};
  std::array<std::size_t, M> fill{};
  for (std::size_t k = 0; k < N1; ++k) {
    const std::size_t m = first[k].out;
    by_middle[start[m] + fill[m]] = k;
    ++fill[m];
  }

  // The composed terms, collected: C[o][j][p][q] += c1 c2.
  std::array<coefficient, X * X * V * V> c{};
  for (const term& t2 : second) {
    for (std::size_t k = start[t2.a]; k < start[t2.a + 1]; ++k) {
      const term& t1 = first[by_middle[k]];
      const std::size_t p = t1.a < t2.b ? t1.a : t2.b;
      const std::size_t q = t1.a < t2.b ? t2.b : t1.a;
      c[(((t2.out * X) + t1.b) * pairs) + (p * V) + q] +=
          t1.coefficient * t2.coefficient;
      ++s.composed;
    }
  }

  // Entries: scaled, signed, and shared when equal. A fingerprint of each
  // row is compared before the row itself.
  std::array<coefficient, X * X> prints{};
  for (std::size_t cell = 0; cell < X * X; ++cell) {
    const std::size_t at = cell * pairs;
    coefficient g = 0;
    bool positive = false;
    for (std::size_t pq = 0; pq < pairs; ++pq) {
      g = gcd(g, magnitude(c[at + pq]));
      positive = positive || c[at + pq] > 0;
    }
    if (g == 0) {
      s.entry_of[cell] = none;
      continue;
    }
    const coefficient sign = positive ? 1 : -1;
    coefficient print = 0;
    for (std::size_t pq = 0; pq < pairs; ++pq) {
      c[at + pq] = sign * c[at + pq] / g;
      print += c[at + pq] * static_cast<coefficient>((pq * 7919U) + 1U);
    }
    std::size_t found = none;
    for (std::size_t m = 0; m < s.entries && found == none; ++m) {
      if (prints[m] != print || s.scales[m] != g) {
        continue;
      }
      bool same = true;
      for (std::size_t pq = 0; pq < pairs && same; ++pq) {
        same = s.rows[(m * pairs) + pq] == c[at + pq];
      }
      found = same ? m : none;
    }
    if (found == none) {
      found = s.entries;
      ++s.entries;
      prints[found] = print;
      s.scales[found] = g;
      for (std::size_t pq = 0; pq < pairs; ++pq) {
        s.rows[(found * pairs) + pq] = c[at + pq];
        s.entry_terms += c[at + pq] != 0 ? 1 : 0;
      }
    }
    s.entry_of[cell] = found;
    s.sign_of[cell] = sign;
    ++s.apply_terms;
  }

  // Products: each pair that some entry reads, once.
  for (std::size_t pq = 0; pq < pairs; ++pq) {
    s.product_of[pq] = none;
    for (std::size_t m = 0; m < s.entries; ++m) {
      if (s.rows[(m * pairs) + pq] != 0) {
        s.product_of[pq] = s.products;
        ++s.products;
        break;
      }
    }
  }
  return s;
}

template <core::metric_like Alg, core::blade_set V, core::blade_set X>
inline constexpr auto transform_summary =
    summarize<V.size(), X.size(),
              detail::sandwich_middle_layout<Alg, V, X>.size()>(
        detail::first_kernel<Alg, V, X, false>,
        detail::second_kernel<Alg, V, X, true>);

template <std::size_t P, std::size_t ET, std::size_t E, std::size_t AT>
struct collected {
  // v_a v_b onto product out.
  std::array<term, P> products;
  // coefficient times product a onto entry out.
  std::array<term, ET> entry_terms;
  std::array<coefficient, E> scales;
  // sign times entry a times x_b onto result out.
  std::array<term, AT> apply_terms;
};

// The summary S of a versor on V places and an x on X places written as
// three kernels, each sum starting from a positive term where it has one.
template <const auto& S, std::size_t V, std::size_t X>
consteval auto make_collected() {
  constexpr const auto& s = S;
  constexpr std::size_t n = V;
  constexpr std::size_t pairs = n * n;
  collected<s.products, s.entry_terms, s.entries, s.apply_terms> out{};

  for (std::size_t pq = 0; pq < pairs; ++pq) {
    if (s.product_of[pq] != none) {
      out.products[s.product_of[pq]] = {.a = pq / n,
                                        .b = pq % n,
                                        .out = s.product_of[pq],
                                        .coefficient = 1,
                                        .first = true};
    }
  }

  std::size_t k = 0;
  for (std::size_t m = 0; m < s.entries; ++m) {
    const std::size_t at = m * pairs;
    std::size_t lead = 0;
    while (s.rows[at + lead] <= 0) {
      ++lead;
    }
    out.entry_terms[k] = {.a = s.product_of[lead],
                          .b = 0,
                          .out = m,
                          .coefficient = s.rows[at + lead],
                          .first = true};
    ++k;
    for (std::size_t pq = 0; pq < pairs; ++pq) {
      if (pq != lead && s.rows[at + pq] != 0) {
        out.entry_terms[k] = {.a = s.product_of[pq],
                              .b = 0,
                              .out = m,
                              .coefficient = s.rows[at + pq],
                              .first = false};
        ++k;
      }
    }
    out.scales[m] = s.scales[m];
  }

  k = 0;
  for (std::size_t o = 0; o < X; ++o) {
    std::size_t lead = none;
    for (std::size_t j = 0; j < X && lead == none; ++j) {
      const std::size_t cell = (o * X) + j;
      if (s.entry_of[cell] != none && s.sign_of[cell] > 0) {
        lead = j;
      }
    }
    bool first = true;
    for (std::size_t pass = 0; pass < 2; ++pass) {
      for (std::size_t j = 0; j < X; ++j) {
        const std::size_t cell = (o * X) + j;
        if (s.entry_of[cell] == none || (pass == 0) != (j == lead)) {
          continue;
        }
        out.apply_terms[k] = {.a = s.entry_of[cell],
                              .b = j,
                              .out = o,
                              .coefficient = s.sign_of[cell],
                              .first = first};
        first = false;
        ++k;
      }
    }
  }
  return out;
}

template <core::metric_like Alg, core::blade_set V, core::blade_set X>
inline constexpr auto collected_transform =
    make_collected<transform_summary<Alg, V, X>, V.size(), X.size()>();

// Each kernel apart, so that it can be a template argument of its own.
template <core::metric_like Alg, core::blade_set V, core::blade_set X>
inline constexpr auto product_terms =
    collected_transform<Alg, V, X>.products;

template <core::metric_like Alg, core::blade_set V, core::blade_set X>
inline constexpr auto entry_terms = collected_transform<Alg, V, X>.entry_terms;

template <core::metric_like Alg, core::blade_set V, core::blade_set X>
inline constexpr auto entry_scales = collected_transform<Alg, V, X>.scales;

template <core::metric_like Alg, core::blade_set V, core::blade_set X>
inline constexpr auto apply_terms = collected_transform<Alg, V, X>.apply_terms;

// Operation counts of the collected kernels by the counting rule's classes:
// a product of two values, an addition or subtraction, a negation, a
// doubling (scale 2, as x + x), and a product by another literal.
struct counts {
  std::size_t setup_mul;
  std::size_t setup_add;
  std::size_t setup_neg;
  std::size_t setup_scl;
  std::size_t setup_literal;
  std::size_t apply_mul;
  std::size_t apply_add;
  std::size_t apply_neg;
  std::size_t composed;
};

template <core::metric_like Alg, core::blade_set V, core::blade_set X>
consteval counts count() {
  constexpr const auto& c = collected_transform<Alg, V, X>;
  counts n{};
  n.composed = transform_summary<Alg, V, X>.composed;
  n.setup_mul = c.products.size();
  for (const term& t : c.entry_terms) {
    n.setup_add += t.first ? 0 : 1;
    n.setup_neg += t.first && t.coefficient < 0 ? 1 : 0;
    n.setup_literal += magnitude(t.coefficient) != 1 ? 1 : 0;
  }
  for (const coefficient scale : c.scales) {
    n.setup_scl += scale == 2 ? 1 : 0;
    n.setup_literal += scale > 2 ? 1 : 0;
  }
  for (const term& t : c.apply_terms) {
    n.apply_mul += 1;
    n.apply_add += t.first ? 0 : 1;
    n.apply_neg += t.first && t.coefficient < 0 ? 1 : 0;
  }
  return n;
}

// The straight-line code. Always inlined, so that a benchmark of it times
// the arithmetic and not a call.
template <coefficient C, bool First, class T>
[[gnu::always_inline]] constexpr void add_term(T& out, const T& value) {
  if constexpr (C == 1) {
    out = First ? value : static_cast<T>(out + value);
  } else if constexpr (C == -1) {
    out = First ? static_cast<T>(-value) : static_cast<T>(out - value);
  } else {
    const auto product = static_cast<T>(static_cast<T>(C) * value);
    out = First ? product : static_cast<T>(out + product);
  }
}

template <coefficient S, class T>
[[gnu::always_inline]] constexpr T scaled(const T& x) {
  if constexpr (S == 1) {
    return x;
  } else if constexpr (S == 2) {
    return static_cast<T>(x + x);
  } else {
    return static_cast<T>(static_cast<T>(S) * x);
  }
}

template <const auto& Terms, std::size_t R, class T, std::size_t KX,
          std::size_t KY, std::size_t... K>
[[gnu::always_inline]] constexpr std::array<T, R>
run_products(const std::array<T, KX>& x, const std::array<T, KY>& y,
             std::index_sequence<K...> /*terms*/) {
  std::array<T, R> out = detail::zero_places<T>(std::make_index_sequence<R>{});
  (add_term<Terms[K].coefficient, Terms[K].first>(
       out[Terms[K].out], static_cast<T>(x[Terms[K].a] * y[Terms[K].b])),
   ...);
  return out;
}

template <const auto& Terms, std::size_t R, class T, std::size_t KX,
          std::size_t... K>
[[gnu::always_inline]] constexpr std::array<T, R>
run_sums(const std::array<T, KX>& x, std::index_sequence<K...> /*terms*/) {
  std::array<T, R> out = detail::zero_places<T>(std::make_index_sequence<R>{});
  (add_term<Terms[K].coefficient, Terms[K].first>(out[Terms[K].out],
                                                  x[Terms[K].a]),
   ...);
  return out;
}

template <const auto& Scales, class T, std::size_t E, std::size_t... M>
[[gnu::always_inline]] constexpr std::array<T, E>
run_scales(const std::array<T, E>& e, std::index_sequence<M...> /*entries*/) {
  return {scaled<Scales[M]>(e[M])...};
}

/// The entries of M(v), the part of a collected transform of x on X by v on
/// V that reads only v.
template <core::metric_like Alg, core::blade_set V, core::blade_set X,
          class T>
[[gnu::always_inline]] constexpr auto
setup(const std::array<T, V.size()>& v) {
  constexpr const auto& products = product_terms<Alg, V, X>;
  constexpr const auto& sums = entry_terms<Alg, V, X>;
  constexpr const auto& scales = entry_scales<Alg, V, X>;
  const auto p = run_products<products, products.size()>(
      v, v, std::make_index_sequence<products.size()>{});
  const auto e = run_sums<sums, scales.size()>(
      p, std::make_index_sequence<sums.size()>{});
  return run_scales<scales>(e, std::make_index_sequence<scales.size()>{});
}

/// M(v) x from the entries that setup gave.
template <core::metric_like Alg, core::blade_set V, core::blade_set X,
          class T, std::size_t E>
[[gnu::always_inline]] constexpr std::array<T, X.size()>
apply(const std::array<T, E>& entries, const std::array<T, X.size()>& x) {
  constexpr const auto& terms = apply_terms<Alg, V, X>;
  return run_products<terms, X.size()>(entries, x,
                                       std::make_index_sequence<terms.size()>{});
}

} // namespace collect

#endif // SPIKE_E_PROTO_COLLECT_HPP
