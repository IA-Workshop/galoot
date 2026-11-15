// Spikes C and F: a throwaway user-facing layer over the Spike A generator.
//
// Not library code. It has the shape decision 0021 fixes, an algebra tag that
// carries its metric value and blade sets passed as class-type template
// arguments, so that the diagnostics of plan 0001 section 9 (Spikes C and F)
// can be read from it. Products use spike::blade_product from
// records/spikes/spike-a/gen.hpp; `transform` is a stub, because only its
// constraint matters here. Included by diag.cpp, named_check.cpp,
// named_detect.cpp, and the units gen_named.py writes; compile with -I to
// records/spikes/spike-a and to this directory.
//
// The rejecting overloads come in six forms, chosen by SPIKE_FORM:
//   1 (R)  raw: the constrained overload only; misuse fails deduction or
//          constraint satisfaction
//   2 (K)  R plus a constrained catch-all whose body is
//          static_assert(false, "<literal message>")
//   3 (D)  a constrained catch-all declared `= delete`, with no message
//   4 (G)  K, but the message is computed (C++26 user-generated
//          static_assert message, P2741), naming the algebras or blades
//   5 (A)  K with a deduced return type (auto) in place of void, so that the
//          body, and with it the static_assert, is instantiated at the call
//          before the caller uses the result
//   6 (P)  A, and the body returns a placeholder of a plausible type after
//          the static_assert, so the caller's use of the result raises no
//          second error
#ifndef GALOOT_SPIKE_CDF_FIXTURE_HPP
#define GALOOT_SPIKE_CDF_FIXTURE_HPP

#include "gen.hpp"

#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>

#define SPIKE_FORM_R 1
#define SPIKE_FORM_K 2
#define SPIKE_FORM_D 3
#define SPIKE_FORM_G 4
#define SPIKE_FORM_A 5
#define SPIKE_FORM_P 6
#ifndef SPIKE_FORM
#define SPIKE_FORM SPIKE_FORM_R
#endif
#if SPIKE_FORM == SPIKE_FORM_A || SPIKE_FORM == SPIKE_FORM_P
#define SPIKE_CATCH_RET auto
#else
#define SPIKE_CATCH_RET void
#endif
#if SPIKE_FORM == SPIKE_FORM_P
#define SPIKE_PLACEHOLDER(expr) return expr
#else
#define SPIKE_PLACEHOLDER(expr) static_cast<void>(0)
#endif

namespace cdf {

using spike::mask_t;

// --- Fixed strings and blade names -------------------------------------------

template <std::size_t N> struct fixed_string {
  std::array<char, N> s{}; // with the terminating NUL
  consteval fixed_string(const char (&str)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
      s[i] = str[i];
    }
  }
  constexpr std::size_t size() const { return N - 1; }
};

// Called only to fail constant evaluation (decision 0012): the function's
// name is the message the user reads.
void blade_name_is_not_valid_in_this_algebra();

struct oriented {
  mask_t mask = 0;
  int sign = 1;
};

// "e41" names the mask {1, 4} with sign -1 relative to the canonical e14
// (CONVENTIONS 1.1, 1.6). Single-digit indices only.
template <std::size_t N>
consteval oriented parse_name(const fixed_string<N>& name, std::size_t n) {
  if (name.size() == 1 && name.s[0] == '1') {
    return {}; // the scalar
  }
  if (name.size() < 2 || name.s[0] != 'e') {
    blade_name_is_not_valid_in_this_algebra();
  }
  oriented r{};
  for (std::size_t i = 1; i < name.size(); ++i) {
    const int d = name.s[i] - '0';
    if (d < 1 || static_cast<std::size_t>(d) > n) {
      blade_name_is_not_valid_in_this_algebra();
    }
    const mask_t bit = mask_t{1} << (d - 1);
    if ((r.mask & bit) != 0) {
      blade_name_is_not_valid_in_this_algebra();
    }
    // Each index already present above d is passed by one transposition.
    if ((std::popcount(r.mask & ~(bit - 1)) & 1) != 0) {
      r.sign = -r.sign;
    }
    r.mask |= bit;
  }
  return r;
}

// --- Algebras and blade sets ---------------------------------------------------

template <class A>
concept algebra = requires {
  A::value.g.size();
  { A::name } -> std::convertible_to<std::string_view>;
};

struct rga3 { // R(3,0,1), CONVENTIONS 1.3
  static constexpr spike::metric<4> value = [] {
    auto m = spike::euclidean<4>();
    m.g[3][3] = 0;
    return m;
  }();
  static constexpr std::string_view name = "rga3";
};
struct r400 { // R(4,0,0): same n as rga3, a different algebra
  static constexpr spike::metric<4> value = spike::euclidean<4>();
  static constexpr std::string_view name = "r400";
};

template <std::size_t K> struct blade_set {
  std::array<mask_t, K> mask{};
  std::array<int, K> sign{};
  static constexpr std::size_t size = K;
};

template <algebra Alg, fixed_string... Names>
inline constexpr blade_set<sizeof...(Names)> blades_of = {
    {parse_name(Names, Alg::value.g.size()).mask...},
    {parse_name(Names, Alg::value.g.size()).sign...}};

template <std::size_t K>
constexpr std::size_t index_of(const blade_set<K>& s, oriented b) {
  for (std::size_t i = 0; i < K; ++i) {
    if (s.mask[i] == b.mask && s.sign[i] == b.sign) {
      return i;
    }
  }
  return K;
}

template <std::size_t K> constexpr bool one_parity(const blade_set<K>& s) {
  for (std::size_t i = 0; i < K; ++i) {
    if ((std::popcount(s.mask[i]) & 1) != (std::popcount(s.mask[0]) & 1)) {
      return false;
    }
  }
  return true;
}

// Every blade of From is in To with the same orientation.
template <std::size_t I, std::size_t J>
constexpr bool subset(const blade_set<I>& from, const blade_set<J>& to) {
  for (std::size_t i = 0; i < I; ++i) {
    if (index_of(to, {from.mask[i], from.sign[i]}) == J) {
      return false;
    }
  }
  return true;
}

// The alternative to get<"e41"> for Spike F: x["e41"]. The parameter type
// is fixed by the multivector, so a string literal converts to it through a
// consteval constructor that finds the blade or fails constant evaluation.
void blade_is_not_in_this_blade_set();

template <class Alg, blade_set B> struct name_in {
  std::size_t index = 0;
  template <std::size_t N>
  consteval name_in(const char (&name)[N])
      : index(index_of(B, parse_name(fixed_string<N>(name),
                                     Alg::value.g.size()))) {
    if (index == B.size) {
      blade_is_not_in_this_blade_set();
    }
  }
};

// --- C++26 computed messages (form G) -------------------------------------------

struct message {
  std::array<char, 160> buf{};
  std::size_t n = 0;
  constexpr void add(std::string_view s) {
    for (char ch : s) {
      buf[n++] = ch;
    }
  }
  constexpr void add_blade(mask_t m) {
    add("e");
    for (int i = 0; i < 16; ++i) {
      if ((m >> i & 1U) != 0) {
        buf[n++] = static_cast<char>('1' + i);
      }
    }
  }
  constexpr std::size_t size() const { return n; }
  constexpr const char* data() const { return buf.data(); }
};

template <blade_set From, blade_set To> consteval message lossy() {
  message m{};
  m.add("conversion drops");
  for (std::size_t i = 0; i < From.size; ++i) {
    if (index_of(To, {From.mask[i], From.sign[i]}) == To.size) {
      m.add(" ");
      m.add(From.sign[i] < 0 ? "-" : "");
      m.add_blade(From.mask[i]);
    }
  }
  m.add(": request it with truncate<Blades>(x)");
  return m;
}

// --- Scalars and multivectors ----------------------------------------------------

template <class T>
concept ring_scalar = std::copyable<T> && requires(T a, T b, std::int64_t k) {
  { a + b } -> std::convertible_to<T>;
  { a - b } -> std::convertible_to<T>;
  { a * b } -> std::convertible_to<T>;
  static_cast<T>(k);
};

template <algebra Alg, blade_set B, class T = double> struct mv {
  std::array<T, B.size> c{};

  constexpr T& operator[](name_in<Alg, B> name) { return c[name.index]; }
  constexpr const T& operator[](name_in<Alg, B> name) const {
    return c[name.index];
  }

  // Conversions between blade sets are explicit and never lossy (STYLE S8,
  // CONVENTIONS 4.5). A conversion function, not a constructor, keeps mv an
  // aggregate.
  template <blade_set To>
    requires(subset(B, To))
  explicit constexpr operator mv<Alg, To, T>() const {
    mv<Alg, To, T> r{};
    for (std::size_t i = 0; i < B.size; ++i) {
      r.c[index_of(To, {B.mask[i], B.sign[i]})] = c[i];
    }
    return r;
  }
#if SPIKE_FORM == SPIKE_FORM_K || SPIKE_FORM == SPIKE_FORM_A ||    \
    SPIKE_FORM == SPIKE_FORM_P
  template <blade_set To>
    requires(!subset(B, To))
  constexpr operator mv<Alg, To, T>() const {
    static_assert(false, "conversion drops blades of its input: this is a "
                         "lossy conversion, request it with truncate<Blades>(x)");
  }
#elif SPIKE_FORM == SPIKE_FORM_D
  template <blade_set To>
    requires(!subset(B, To))
  constexpr operator mv<Alg, To, T>() const = delete;
#elif SPIKE_FORM == SPIKE_FORM_G
  template <blade_set To>
    requires(!subset(B, To))
  constexpr operator mv<Alg, To, T>() const {
    static_assert(false, lossy<B, To>());
  }
#endif
};

template <class X> inline constexpr bool is_mv = false;
template <class Alg, blade_set B, class T>
inline constexpr bool is_mv<mv<Alg, B, T>> = true;
template <class X>
concept multivector = is_mv<X>;

template <class X> struct mv_traits;
template <class Alg, blade_set B, class T> struct mv_traits<mv<Alg, B, T>> {
  using algebra_type = Alg;
  using scalar_type = T;
  static constexpr auto blades = B;
};

template <class X, class Y>
concept same_algebra = std::same_as<typename mv_traits<X>::algebra_type,
                                    typename mv_traits<Y>::algebra_type>;

// --- gp ---------------------------------------------------------------------------

template <class Alg, blade_set A, blade_set B> consteval auto gp_blades() {
  constexpr std::size_t n = Alg::value.g.size();
  constexpr auto seen = [] {
    std::array<bool, (std::size_t{1} << n)> s{};
    for (std::size_t i = 0; i < A.size; ++i) {
      for (std::size_t j = 0; j < B.size; ++j) {
        const auto r = spike::blade_product<n>(Alg::value, A.mask[i], B.mask[j]);
        for (std::size_t k = 0; k < r.size; ++k) {
          s[r.t[k].mask] = true;
        }
      }
    }
    return s;
  }();
  constexpr std::size_t count = [&] {
    std::size_t c = 0;
    for (bool b : seen) {
      c += b ? 1 : 0;
    }
    return c;
  }();
  blade_set<count> r{};
  std::size_t k = 0;
  for (std::size_t m = 0; m < seen.size(); ++m) {
    if (seen[m]) {
      r.mask[k] = static_cast<mask_t>(m);
      r.sign[k] = 1;
      ++k;
    }
  }
  return r;
}

template <class Alg, blade_set A, blade_set B, ring_scalar T>
constexpr mv<Alg, gp_blades<Alg, A, B>(), T> gp(const mv<Alg, A, T>& a,
                                               const mv<Alg, B, T>& b) {
  constexpr auto out = gp_blades<Alg, A, B>();
  constexpr std::size_t n = Alg::value.g.size();
  mv<Alg, out, T> r{};
  for (std::size_t i = 0; i < A.size; ++i) {
    for (std::size_t j = 0; j < B.size; ++j) {
      const auto p = spike::blade_product<n>(Alg::value, A.mask[i], B.mask[j]);
      for (std::size_t k = 0; k < p.size; ++k) {
        const auto c = p.t[k].coef * A.sign[i] * B.sign[j];
        r.c[index_of(out, {p.t[k].mask, 1})] =
            r.c[index_of(out, {p.t[k].mask, 1})] +
            static_cast<T>(c) * a.c[i] * b.c[j];
      }
    }
  }
  return r;
}

// Catch-alls for gp. Mutually exclusive with the overload above and with each
// other: different algebras (case 1), or one algebra and a scalar type without
// the arithmetic gp needs (case 5).
#if SPIKE_FORM == SPIKE_FORM_K || SPIKE_FORM == SPIKE_FORM_A ||    \
    SPIKE_FORM == SPIKE_FORM_P
template <multivector X, multivector Y>
  requires(!same_algebra<X, Y>)
SPIKE_CATCH_RET gp(const X&, const Y&) {
  static_assert(false, "gp: the operands belong to different algebras");
  SPIKE_PLACEHOLDER(X{});
}
template <multivector X, multivector Y>
  requires(same_algebra<X, Y> &&
           !(std::same_as<typename mv_traits<X>::scalar_type,
                          typename mv_traits<Y>::scalar_type> &&
             ring_scalar<typename mv_traits<X>::scalar_type>))
SPIKE_CATCH_RET gp(const X&, const Y&) {
  static_assert(false, "gp: the scalar type must be the same for both "
                       "operands and support +, -, * and conversion from an "
                       "integer coefficient");
  SPIKE_PLACEHOLDER(X{});
}
#elif SPIKE_FORM == SPIKE_FORM_D
template <multivector X, multivector Y>
  requires(!same_algebra<X, Y>)
void gp(const X&, const Y&) = delete;
template <multivector X, multivector Y>
  requires(same_algebra<X, Y> &&
           !(std::same_as<typename mv_traits<X>::scalar_type,
                          typename mv_traits<Y>::scalar_type> &&
             ring_scalar<typename mv_traits<X>::scalar_type>))
void gp(const X&, const Y&) = delete;
#elif SPIKE_FORM == SPIKE_FORM_G
template <multivector X, multivector Y> consteval message algebras_differ() {
  message m{};
  m.add("gp: the operands belong to different algebras, ");
  m.add(mv_traits<X>::algebra_type::name);
  m.add(" and ");
  m.add(mv_traits<Y>::algebra_type::name);
  return m;
}
template <multivector X, multivector Y>
  requires(!same_algebra<X, Y>)
SPIKE_CATCH_RET gp(const X&, const Y&) {
  static_assert(false, algebras_differ<X, Y>());
}
template <multivector X, multivector Y>
  requires(same_algebra<X, Y> &&
           !(std::same_as<typename mv_traits<X>::scalar_type,
                          typename mv_traits<Y>::scalar_type> &&
             ring_scalar<typename mv_traits<X>::scalar_type>))
SPIKE_CATCH_RET gp(const X&, const Y&) {
  static_assert(false, "gp: the scalar type must be the same for both "
                       "operands and support +, -, * and conversion from an "
                       "integer coefficient");
}
#endif

// --- transform (stub) ---------------------------------------------------------------

// CONVENTIONS 4.4: a versor with blades of both parities is rejected.
template <class Alg, blade_set X, blade_set V, ring_scalar T>
  requires(one_parity(V))
constexpr mv<Alg, X, T> transform(const mv<Alg, X, T>& x,
                                  const mv<Alg, V, T>&) {
  return x;
}
#if SPIKE_FORM == SPIKE_FORM_K || SPIKE_FORM == SPIKE_FORM_A ||    \
    SPIKE_FORM == SPIKE_FORM_P
template <class Alg, blade_set X, blade_set V, class T>
  requires(!one_parity(V))
SPIKE_CATCH_RET transform([[maybe_unused]] const mv<Alg, X, T>& x,
                          const mv<Alg, V, T>&) {
  static_assert(false, "transform: the versor has blades of both parities, "
                       "so it is neither even nor odd (CONVENTIONS 4.4)");
  SPIKE_PLACEHOLDER(x);
}
#elif SPIKE_FORM == SPIKE_FORM_D
template <class Alg, blade_set X, blade_set V, class T>
  requires(!one_parity(V))
void transform(const mv<Alg, X, T>&, const mv<Alg, V, T>&) = delete;
#elif SPIKE_FORM == SPIKE_FORM_G
template <blade_set V> consteval message mixed_parity() {
  message m{};
  m.add("transform: the versor has blades of both parities:");
  for (std::size_t i = 0; i < V.size; ++i) {
    m.add(" ");
    m.add_blade(V.mask[i]);
    m.add((std::popcount(V.mask[i]) & 1) != 0 ? " (odd)" : " (even)");
  }
  return m;
}
template <class Alg, blade_set X, blade_set V, class T>
  requires(!one_parity(V))
SPIKE_CATCH_RET transform(const mv<Alg, X, T>&, const mv<Alg, V, T>&) {
  static_assert(false, mixed_parity<V>());
}
#endif

// --- Named access (Spike F) --------------------------------------------------------------

template <class Alg, blade_set B, fixed_string Name>
concept has_blade =
    index_of(B, parse_name(Name, Alg::value.g.size())) < B.size;

template <fixed_string Name, class Alg, blade_set B, class T>
  requires has_blade<Alg, B, Name>
constexpr T& get(mv<Alg, B, T>& x) {
#ifdef SPIKE_GET_INDEX_AT_RUN_TIME
  return x.c[index_of(B, parse_name(Name, Alg::value.g.size()))];
#else
  constexpr std::size_t i = index_of(B, parse_name(Name, Alg::value.g.size()));
  return x.c[i];
#endif
}
template <fixed_string Name, class Alg, blade_set B, class T>
  requires has_blade<Alg, B, Name>
constexpr const T& get(const mv<Alg, B, T>& x) {
  // A constant, so that -O0 does not search the blade set at run time.
  // -DSPIKE_GET_INDEX_AT_RUN_TIME restores the first version, in which only
  // parse_name (consteval) is constant and index_of runs at run time at -O0.
#ifdef SPIKE_GET_INDEX_AT_RUN_TIME
  return x.c[index_of(B, parse_name(Name, Alg::value.g.size()))];
#else
  constexpr std::size_t i = index_of(B, parse_name(Name, Alg::value.g.size()));
  return x.c[i];
#endif
}
#if SPIKE_FORM == SPIKE_FORM_K || SPIKE_FORM == SPIKE_FORM_A ||    \
    SPIKE_FORM == SPIKE_FORM_P
template <fixed_string Name, class Alg, blade_set B, class T>
  requires(!has_blade<Alg, B, Name>)
SPIKE_CATCH_RET get(const mv<Alg, B, T>&) {
  static_assert(false, "get<name>: no blade of this name, with this "
                       "orientation, is in the blade set");
  SPIKE_PLACEHOLDER(T{});
}
#elif SPIKE_FORM == SPIKE_FORM_D
template <fixed_string Name, class Alg, blade_set B, class T>
  requires(!has_blade<Alg, B, Name>)
void get(const mv<Alg, B, T>&) = delete;
#elif SPIKE_FORM == SPIKE_FORM_G
template <fixed_string Name, blade_set B> consteval message missing_name() {
  message m{};
  m.add("get<");
  m.add(std::string_view(Name.s.data(), Name.size()));
  m.add(">: not in the blade set {");
  for (std::size_t i = 0; i < B.size; ++i) {
    m.add(i == 0 ? "" : ", ");
    m.add(B.sign[i] < 0 ? "-" : "");
    m.add_blade(B.mask[i]);
  }
  m.add("}");
  return m;
}
template <fixed_string Name, class Alg, blade_set B, class T>
  requires(!has_blade<Alg, B, Name>)
SPIKE_CATCH_RET get(const mv<Alg, B, T>&) {
  static_assert(false, missing_name<Name, B>());
}
#endif

// --- Named types ---------------------------------------------------------------------------

namespace rga3_types {
template <class T = double>
using Vector = mv<rga3, blades_of<rga3, "e1", "e2", "e3", "e4">, T>;
// CONVENTIONS 1.6 orientation: e41 = -e14, e31 = -e13, and so on.
using Line = mv<rga3, blades_of<rga3, "e41", "e42", "e43", "e23", "e31", "e12">>;
using Point = mv<rga3, blades_of<rga3, "e1", "e2", "e3", "e4">>;
using Motor = mv<rga3, blades_of<rga3, "1", "e41", "e42", "e43", "e23",
                                 "e31", "e12", "e1234">>;
} // namespace rga3_types

} // namespace cdf

#endif // GALOOT_SPIKE_CDF_FIXTURE_HPP
