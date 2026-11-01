// A throwaway multivector layer over the real core headers, for comparing the
// diagnostics that a first-time user sees for the likely mistakes of WU8. It
// is not library code and nothing under include/ depends on it.
//
// Two spellings of a declared type:
//   mv<Alg, blades<4, "e41 e42">>  the blade set as masks and signs (planned)
//   mvn<Alg, "e41 e42">            the names as written, parsed in the algebra
//
// Four forms of rejecting misuse, chosen by APX_FORM:
//   'R'  constraints only
//   'P'  a constrained catch-all whose body is static_assert(false, "literal")
//        and, for a deduced return type, a placeholder return
//   'S'  as P, but the static_assert tests the failed condition itself, so a
//        compiler may print the condition with the user's arguments
//   'T'  as P, but the body names an undefined class template whose template
//        arguments are the user's terms, and has no literal message
#ifndef APX_FIXTURE_HPP
#define APX_FIXTURE_HPP

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

#ifndef APX_FORM
#define APX_FORM 'R'
#endif

namespace apx {

namespace gc = galoot::core;
using gc::detail::name_list;

template <class T>
concept ring_scalar = std::regular<T> && requires(const T a, const T b) {
  { a + b } -> std::convertible_to<T>;
  { a - b } -> std::convertible_to<T>;
  { a * b } -> std::convertible_to<T>;
};

template <class Alg>
inline constexpr std::size_t n_of =
    std::remove_cvref_t<decltype(Alg::value)>::generator_count;

// Whether every blade of the set names only generators of the algebra.
template <class Alg, gc::blade_set Set>
constexpr bool blades_fit() {
  const gc::mask_t outside = ~((gc::mask_t{1} << n_of<Alg>) - 1);
  for (const gc::oriented_blade b : Set.blades) {
    if ((b.mask & outside) != 0) {
      return false;
    }
  }
  return true;
}

// Whether every blade of From is in To with the same orientation.
template <gc::blade_set To, gc::blade_set From>
constexpr bool covers() {
  for (const gc::oriented_blade f : From.blades) {
    bool found = false;
    for (const gc::oriented_blade t : To.blades) {
      found = found || t == f;
    }
    if (!found) {
      return false;
    }
  }
  return true;
}

template <class From, class To>
struct lossy_conversion_from_to;

template <name_list Name, class Type>
struct no_component_named;

template <class Left, class Right>
struct comparison_of_different_types;

template <class Type, std::size_t Given>
struct coefficient_count_differs;

// The masks-and-signs spelling.
template <gc::metric_like Alg, gc::blade_set B, class T = double>
#if APX_FORM == 'R'
  requires ring_scalar<T> && (blades_fit<Alg, B>())
#endif
struct mv {
#if APX_FORM != 'R'
  static_assert(ring_scalar<T>,
                "mv: the scalar type must have +, - and * and compare with ==");
  static_assert(blades_fit<Alg, B>(),
                "mv: a blade of the set names a generator that the algebra "
                "does not have");
#endif
  using algebra = Alg;
  using scalar = T;
  static constexpr auto set = B;

  std::array<T, B.size()> c;

  friend constexpr bool operator==(const mv&, const mv&) = default;

#if APX_FORM == 'P'
  template <gc::blade_set To>
    requires(!covers<To, B>())
  constexpr operator mv<Alg, To, T>() const {
    static_assert(false, "conversion drops components of its input: it is "
                         "lossy, so request it with truncate<Type>(x)");
    return {};
  }
#elif APX_FORM == 'S'
  template <gc::blade_set To>
    requires(!covers<To, B>())
  constexpr operator mv<Alg, To, T>() const {
    static_assert(covers<To, B>(), "conversion drops components of its input: "
                                   "it is lossy, so request it with "
                                   "truncate<Type>(x)");
    return {};
  }
#elif APX_FORM == 'T'
  template <gc::blade_set To>
    requires(!covers<To, B>())
  constexpr operator mv<Alg, To, T>() const {
    return lossy_conversion_from_to<mv, mv<Alg, To, T>>{};
  }
#endif
};

// The names-as-written spelling.
template <class Alg, name_list Names>
inline constexpr auto names_set = gc::blades<n_of<Alg>, Names>;

template <gc::metric_like Alg, name_list Names, class T = double>
#if APX_FORM == 'R'
  requires ring_scalar<T>
#endif
struct mvn {
#if APX_FORM != 'R'
  static_assert(ring_scalar<T>,
                "mv: the scalar type must have +, - and * and compare with ==");
#endif
  using algebra = Alg;
  using scalar = T;
  static constexpr auto set = names_set<Alg, Names>;

  std::array<T, names_set<Alg, Names>.size()> c;

  friend constexpr bool operator==(const mvn&, const mvn&) = default;

#if APX_FORM == 'P'
  template <name_list To>
    requires(!covers<names_set<Alg, To>, set>())
  constexpr operator mvn<Alg, To, T>() const {
    static_assert(false, "conversion drops components of its input: it is "
                         "lossy, so request it with truncate<Type>(x)");
    return {};
  }
#elif APX_FORM == 'S'
  template <name_list To>
    requires(!covers<names_set<Alg, To>, set>())
  constexpr operator mvn<Alg, To, T>() const {
    static_assert(covers<names_set<Alg, To>, set>(),
                  "conversion drops components of its input: it is lossy, so "
                  "request it with truncate<Type>(x)");
    return {};
  }
#elif APX_FORM == 'T'
  template <name_list To>
    requires(!covers<names_set<Alg, To>, set>())
  constexpr operator mvn<Alg, To, T>() const {
    return lossy_conversion_from_to<mvn, mvn<Alg, To, T>>{};
  }
#endif
};

template <class X>
concept multivector = requires {
  typename X::algebra;
  typename X::scalar;
  X::set.size();
};

// The position in X of the component named Name, or the size of X's set when
// X has no component of that name and orientation.
template <class X, name_list Name>
constexpr std::size_t position_of_name() {
  constexpr auto one = gc::blades<n_of<typename X::algebra>, Name>;
  if constexpr (one.size() != 1) {
    return X::set.size();
  } else {
    for (std::size_t i = 0; i < X::set.size(); ++i) {
      if (X::set.blades[i] == one.blades[0]) {
        return i;
      }
    }
    return X::set.size();
  }
}

template <class X, name_list Name>
concept has_component =
    multivector<X> && (position_of_name<X, Name>() < X::set.size());

// One overload taking a forwarding reference, rather than a const and a
// non-const pair.
template <name_list Name, class X>
  requires has_component<std::remove_cvref_t<X>, Name>
constexpr decltype(auto) get(X&& x) {
  constexpr std::size_t i = position_of_name<std::remove_cvref_t<X>, Name>();
  return std::forward<X>(x).c[i];
}

template <class X, class... A>
concept coefficients_for =
    multivector<X> && sizeof...(A) == X::set.size() &&
    (std::convertible_to<A, typename X::scalar> && ...);

template <class X, class... A>
  requires coefficients_for<X, A...>
constexpr X make(A... a) {
  return X{{static_cast<typename X::scalar>(a)...}};
}

#if APX_FORM == 'P'
template <name_list Name, class X>
  requires(multivector<std::remove_cvref_t<X>> &&
           !has_component<std::remove_cvref_t<X>, Name>)
constexpr auto get(X&&) {
  static_assert(false, "get<name>: the type has no component of this name "
                       "with this orientation");
  return typename std::remove_cvref_t<X>::scalar{};
}

template <class X, class Y>
  requires(multivector<X> && multivector<Y> && !std::same_as<X, Y>)
constexpr bool operator==(const X&, const Y&) {
  static_assert(false, "==: the operands have different types; compare "
                       "multivectors of one algebra, blade set and scalar");
  return false;
}

template <class X, class... A>
  requires(multivector<X> && !coefficients_for<X, A...>)
constexpr X make(A...) {
  static_assert(false, "make<Type>: give one coefficient for each component "
                       "of the type, in the order the type lists them");
  return X{};
}
#elif APX_FORM == 'S'
template <name_list Name, class X>
  requires(multivector<std::remove_cvref_t<X>> &&
           !has_component<std::remove_cvref_t<X>, Name>)
constexpr auto get(X&&) {
  static_assert(has_component<std::remove_cvref_t<X>, Name>,
                "get<name>: the type has no component of this name with this "
                "orientation");
  return typename std::remove_cvref_t<X>::scalar{};
}

template <class X, class Y>
  requires(multivector<X> && multivector<Y> && !std::same_as<X, Y>)
constexpr bool operator==(const X&, const Y&) {
  static_assert(std::same_as<typename X::algebra, typename Y::algebra>,
                "==: the operands belong to different algebras");
  static_assert(std::same_as<X, Y>,
                "==: the operands have different blade sets or scalars");
  return false;
}

template <class X, class... A>
  requires(multivector<X> && !coefficients_for<X, A...>)
constexpr X make(A...) {
  static_assert(sizeof...(A) == X::set.size(),
                "make<Type>: give one coefficient for each component of the "
                "type, in the order the type lists them");
  return X{};
}
#elif APX_FORM == 'T'
template <name_list Name, class X>
  requires(multivector<std::remove_cvref_t<X>> &&
           !has_component<std::remove_cvref_t<X>, Name>)
constexpr auto get(X&&) {
  return no_component_named<Name, std::remove_cvref_t<X>>{};
}

template <class X, class Y>
  requires(multivector<X> && multivector<Y> && !std::same_as<X, Y>)
constexpr bool operator==(const X&, const Y&) {
  return comparison_of_different_types<X, Y>{};
}

template <class X, class... A>
  requires(multivector<X> && !coefficients_for<X, A...>)
constexpr X make(A...) {
  return coefficient_count_differs<X, sizeof...(A)>{};
}
#endif

} // namespace apx

#endif // APX_FIXTURE_HPP
