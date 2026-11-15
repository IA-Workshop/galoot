// Spike A: a throwaway constexpr generator of geometric-product plans.
//
// Not library code. It exists to measure the cost and limits of constant
// evaluation (plan 0001, section 9, Spike A). Blades are wedge monomials
// (decision 0003); coefficients are integers.
#ifndef GALOOT_SPIKE_A_GEN_HPP
#define GALOOT_SPIKE_A_GEN_HPP

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>

namespace spike {

using mask_t = std::uint32_t;
using coef_t = std::int64_t;

template <std::size_t N> struct metric {
  std::array<std::array<int, N>, N> g{};
};

template <std::size_t K> struct blade_set {
  std::array<mask_t, K> masks{};
};

struct term {
  mask_t mask = 0;
  coef_t coef = 0;
};

// A sparse multivector of capacity Cap, merged by linear search. Cap = 2^N
// always suffices; the capacity is a precondition, not checked.
template <std::size_t Cap> struct terms {
  std::array<term, Cap> t{};
  std::size_t size = 0;

  constexpr void add(mask_t m, coef_t c) {
    if (c == 0) {
      return;
    }
    for (std::size_t i = 0; i < size; ++i) {
      if (t[i].mask == m) {
        t[i].coef += c;
        if (t[i].coef == 0) {
          t[i] = t[--size];
        }
        return;
      }
    }
    t[size++] = {m, c};
  }
};

constexpr coef_t parity_sign(int k) { return (k & 1) != 0 ? -1 : 1; }

// out += scale * e_i x, by e_i x = e_i contract x + e_i wedge x.
template <std::size_t N, std::size_t Cap>
constexpr void vec_mul(const metric<N>& m, std::size_t i, const terms<Cap>& x,
                       coef_t scale, terms<Cap>& out) {
  const mask_t bit = mask_t{1} << i;
  for (std::size_t k = 0; k < x.size; ++k) {
    const mask_t b = x.t[k].mask;
    const coef_t c = x.t[k].coef * scale;

    // Wedge: e_i passes the generators of b below it.
    if ((b & bit) == 0) {
      out.add(b | bit, parity_sign(std::popcount(b & (bit - 1))) * c);
    }

    // Left contraction: sum over p of (-1)^p g(i, b_p) times b without b_p.
    int p = 0;
    for (std::size_t j = 0; j < N; ++j) {
      if ((b >> j & 1U) != 0) {
        if (m.g[i][j] != 0) {
          out.add(b & ~(mask_t{1} << j), parity_sign(p) * m.g[i][j] * c);
        }
        ++p;
      }
    }
  }
}

// out += scale * e_A x, with e_A the wedge monomial of a. For the lowest
// index i of a and the rest r, e_A = e_i wedge e_r = e_i e_r - e_i contract
// e_r, and the contraction is a sum of lower wedge monomials.
template <std::size_t N, std::size_t Cap>
constexpr void blade_mul(const metric<N>& m, mask_t a, const terms<Cap>& x,
                         coef_t scale, terms<Cap>& out) {
  if (a == 0) {
    for (std::size_t k = 0; k < x.size; ++k) {
      out.add(x.t[k].mask, x.t[k].coef * scale);
    }
    return;
  }
  const auto i = static_cast<std::size_t>(std::countr_zero(a));
  const mask_t rest = a & (a - 1);

  terms<Cap> y{};
  blade_mul(m, rest, x, 1, y);
  vec_mul(m, i, y, scale, out);

  int p = 0;
  for (std::size_t j = 0; j < N; ++j) {
    if ((rest >> j & 1U) != 0) {
      if (m.g[i][j] != 0) {
        blade_mul(m, rest & ~(mask_t{1} << j), x,
                  -parity_sign(p) * m.g[i][j] * scale, out);
      }
      ++p;
    }
  }
}

// Cap is the scratch capacity. 2^N always suffices; a smaller Cap that a
// product overruns fails constant evaluation (out-of-bounds access).
template <std::size_t N, std::size_t Cap = (std::size_t{1} << N)>
constexpr terms<Cap> blade_product(const metric<N>& m, mask_t a, mask_t b) {
  terms<Cap> x{};
  x.add(b, 1);
  terms<Cap> out{};
  blade_mul(m, a, x, 1, out);
  return out;
}

// -DSPIKE_PLAN_LAMBDA=consteval makes the plan lambda consteval (an
// experiment on GCC -O2 cost; see research note 0008).
#ifndef SPIKE_PLAN_LAMBDA
#define SPIKE_PLAN_LAMBDA
#endif

// One term of a plan: inputs by index into their blade sets, output by mask.
struct plan_term {
  std::uint32_t a = 0;
  std::uint32_t b = 0;
  mask_t out = 0;
  coef_t coef = 0;
};

// Writes every term of e_A e_B for A in as and B in bs into out, and returns
// the number of terms. With an empty out it only counts (first pass).
template <std::size_t N, std::size_t Cap = (std::size_t{1} << N)>
constexpr std::size_t generate(const metric<N>& m, std::span<const mask_t> as,
                               std::span<const mask_t> bs,
                               std::span<plan_term> out) {
  std::size_t count = 0;
  for (std::size_t ia = 0; ia < as.size(); ++ia) {
    for (std::size_t ib = 0; ib < bs.size(); ++ib) {
      const auto r = blade_product<N, Cap>(m, as[ia], bs[ib]);
      for (std::size_t k = 0; k < r.size; ++k) {
        if (!out.empty()) {
          out[count] = {static_cast<std::uint32_t>(ia),
                        static_cast<std::uint32_t>(ib), r.t[k].mask,
                        r.t[k].coef};
        }
        ++count;
      }
    }
  }
  return count;
}

template <auto M, auto A, auto B, std::size_t Cap = (std::size_t{1} << M.g.size())>
inline constexpr std::size_t plan_size =
    generate<M.g.size(), Cap>(M, std::span<const mask_t>(A.masks),
             std::span<const mask_t>(B.masks), {});

template <auto M, auto A, auto B, std::size_t Cap = (std::size_t{1} << M.g.size())>
inline constexpr auto plan = []() SPIKE_PLAN_LAMBDA {
  std::array<plan_term, plan_size<M, A, B, Cap>> out{};
  generate<M.g.size(), Cap>(M, std::span<const mask_t>(A.masks),
           std::span<const mask_t>(B.masks), std::span<plan_term>(out));
  return out;
}();

// --- Signatures and blade sets ---------------------------------------------

template <std::size_t N> constexpr metric<N> euclidean() {
  metric<N> m{};
  for (std::size_t i = 0; i < N; ++i) {
    m.g[i][i] = 1;
  }
  return m;
}

// conformal(N - 2), CONVENTIONS 1.4: the last two generators are null with
// g(N-2, N-1) = -1.
template <std::size_t N> constexpr metric<N> conformal() {
  metric<N> m{};
  for (std::size_t i = 0; i + 2 < N; ++i) {
    m.g[i][i] = 1;
  }
  m.g[N - 2][N - 1] = -1;
  m.g[N - 1][N - 2] = -1;
  return m;
}

// Every entry nonzero: the worst case for the contraction recursion.
template <std::size_t N> constexpr metric<N> dense() {
  metric<N> m{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      m.g[i][j] = i == j ? 2 : 1;
    }
  }
  return m;
}

template <std::size_t N> constexpr blade_set<(std::size_t{1} << N)> all() {
  blade_set<(std::size_t{1} << N)> s{};
  for (std::size_t i = 0; i < s.masks.size(); ++i) {
    s.masks[i] = static_cast<mask_t>(i);
  }
  return s;
}

} // namespace spike

#endif // GALOOT_SPIKE_A_GEN_HPP
