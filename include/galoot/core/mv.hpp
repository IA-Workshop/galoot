#ifndef GALOOT_CORE_MV_HPP
#define GALOOT_CORE_MV_HPP

// Multivectors: values of an algebra that hold one coefficient of a scalar
// type on each oriented blade of a blade set, the layout of the type.
//
// A coefficient stored on the oriented blade s e_A multiplies s e_A, so a
// type can hold the coefficients of a published formula as they are written.
// In rigid(3) a line stores its direction on e41, e42, e43 and its moment on
// e23, e31, e12:
//
//   struct rga3 {
//     static constexpr auto value = galoot::core::rigid<3>();
//   };
//   inline constexpr auto line_layout =
//       galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
//   using Line = galoot::core::mv<rga3, line_layout, double>;
//
//   Line axis{0, 0, 1, 0, 0, 0};  // the z axis through the origin
//   Line none;                     // every coefficient zero
//
// Since e41 = -e14, the coefficient written on e41 multiplies -e14, and no
// sign is hidden from the formula. Naming the layout and the type, as above,
// also lets Clang print those names in its diagnostics.

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>

#include <galoot/core/blade_product.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace galoot::core {

/// A scalar type a multivector can hold.
///
/// Requires of T: std::semiregular (default construction, copies, and
/// assignment); a + b, a - b, a * b, and -a convertible to T; and T
/// constructible from coefficient_type, the type of the exact integer
/// coefficients of the generated products. The requirement that fails on
/// the fewest declarations is tested first, so that a compiler's account of
/// a failure stays short. Not checked, and required: T{} is the zero of T,
/// and +, -, * obey the ring laws. Equality is not required, so that a
/// scalar holding several lanes at once is accepted.
template <class T>
concept ring_scalar = std::semiregular<T> && requires(const T& a, const T& b) {
  { a + b } -> std::convertible_to<T>;
  { a - b } -> std::convertible_to<T>;
  { a * b } -> std::convertible_to<T>;
  { -a } -> std::convertible_to<T>;
} && std::constructible_from<T, coefficient_type>;

/// A ring scalar whose arithmetic is exact, as std::numeric_limits says:
/// the integer types, and a type of the user's whose numeric_limits
/// specialization sets is_exact. A floating type, and any type without a
/// specialization, such as std::complex<double>, is treated as rounding.
template <class T>
concept exact_scalar = ring_scalar<T> && std::numeric_limits<T>::is_exact;

template <metric_like Alg, blade_set Blades, std::semiregular T>
struct mv;

namespace detail {

template <class X>
inline constexpr bool is_multivector_v = false;

template <metric_like Alg, blade_set Blades, std::semiregular T>
inline constexpr bool is_multivector_v<mv<Alg, Blades, T>> = true;

} // namespace detail

/// Whether X, with any cv-qualification and reference removed, is a
/// specialization of mv. It tests the type only and is never an error.
template <class X>
concept multivector = detail::is_multivector_v<std::remove_cvref_t<X>>;

namespace detail {

// Whether no mask appears twice in `set`, in either orientation. Every pair
// is compared, so the cost grows with the square of the size of the set.
template <std::size_t K>
[[nodiscard]] constexpr bool masks_distinct(const blade_set<K>& set) noexcept {
  for (std::size_t i = 0; i < K; ++i) {
    for (std::size_t j = i + 1; j < K; ++j) {
      if (set.blades[i].mask == set.blades[j].mask) {
        return false;
      }
    }
  }
  return true;
}

// The checks of a layout, each evaluated once for each set (and algebra)
// that a program uses, however many types share it.
template <blade_set S>
inline constexpr bool distinct_masks = masks_distinct(S);

template <std::size_t N, blade_set S>
inline constexpr bool blades_in_algebra = all_in_algebra(S, N);

// The coefficients of a multivector, and its constructor from one value per
// component. Ts is T once for each component, so that each parameter of the
// constructor is T itself and a compiler's message about a value names T.
// The first parameter is the multivector type itself, so that no two
// multivector types share this base and none can be compared with another
// through it.
template <class Multivector, class T, class... Ts>
struct coefficient_storage {
  // T{} in every place, one initializer per place. GCC 16.2.1 was seen to
  // compile the shorter `coefficients{}` wrongly: after a constant evaluation
  // wrote into place 2 or a later place of one value of the type, a zero of
  // the type made later at run time held zero bits from that place on. For a
  // scalar whose T{} is not all zero bits, such as a rational's 0/1, that is
  // a wrong value. One initializer per place was compiled correctly.
  std::array<T, sizeof...(Ts)> coefficients{Ts{}...};

  constexpr coefficient_storage() = default;

  // Not declared for an empty layout, where it would be the default
  // constructor.
  constexpr explicit(sizeof...(Ts) == 1) coefficient_storage(Ts... values)
    requires(sizeof...(Ts) > 0)
      : coefficients{values...} {}

  // Declared only for a scalar with ==, so that a scalar without one, such
  // as one holding several lanes, still gives a multivector type, and
  // generic code that asks is told it has no ==. Written out rather than
  // defaulted: with a defaulted body, GCC 16 failed to compile a test of
  // std::equality_comparable on such a multivector, inside std::array's ==.
  [[nodiscard]] constexpr bool
  operator==(const coefficient_storage& other) const
    requires std::equality_comparable<T>
  {
    return coefficients == other.coefficients;
  }
};

// coefficient_storage with T once for each of K components.
template <class Multivector, class T, class Positions>
struct storage_for;

template <class Multivector, class T, std::size_t... I>
struct storage_for<Multivector, T, std::index_sequence<I...>> {
  template <std::size_t>
  using same = T;
  using type = coefficient_storage<Multivector, T, same<I>...>;
};

template <class Multivector, class T, std::size_t K>
using storage_of =
    storage_for<Multivector, T, std::make_index_sequence<K>>::type;

} // namespace detail

/// A multivector of the algebra Alg with one coefficient of type T on each
/// oriented blade of the layout Blades, in the order the layout lists them:
/// coefficients[i] multiplies Blades.blades[i].sign times the canonical blade
/// of Blades.blades[i].mask.
///
/// Requires: Alg is metric_like and T is std::semiregular, or the type does
/// not match. Checked when the type is used, each with its own message: T is
/// a ring_scalar; every blade of the layout names only e1 to en; every sign
/// is +1 or -1; and no mask appears twice, in either orientation. Generic
/// code that only names mv<A, B, std::string> is told that it is a type; it
/// fails when used. The repeated-mask check compares every pair of blades,
/// so the compiler's limits on constant evaluation bound the size of a
/// layout. Measured at the default limits, alike at -O0 and -O2 and with or
/// without _GLIBCXX_ASSERTIONS: mv<R(n,0,0), full<n>, double> builds for n
/// up to 8 (256 blades) on Clang 23 and up to 10 (1,024 blades) on GCC 16.
/// Guarantees: not an aggregate; trivially copyable and standard-layout when
/// T is, with sizeof equal to the number of blades times sizeof(T) for a
/// nonempty layout; usable as a template argument when T is a structural
/// type, as double is (std::complex<double> is not, since its members are
/// private); never allocates. A default-constructed value holds T{} in every
/// place. Two values of one type compare with == and != coefficient by
/// coefficient, exactly; with a floating scalar that is IEEE equality, so
/// -0.0 equals +0.0 and a coefficient that is a NaN makes them unequal.
/// Values of different layouts, algebras, or scalars do not compare.
template <metric_like Alg, blade_set Blades, std::semiregular T>
struct mv : detail::storage_of<mv<Alg, Blades, T>, T, Blades.size()> {
  static_assert(ring_scalar<T>,
                "mv: the scalar type must support +, -, *, unary - and "
                "construction from an integer coefficient");
  static_assert(
      detail::blades_in_algebra<
          std::remove_cvref_t<decltype(Alg::value)>::generator_count, Blades>,
      "mv: the blade set names a generator beyond e_n");
  static_assert(detail::signs_valid<Blades>,
                "mv: the blade set holds a sign other than +1 or -1");
  static_assert(detail::distinct_masks<Blades>,
                "mv: the blade set holds one blade twice, perhaps in both "
                "orientations");

  /// The algebra tag.
  using algebra_type = Alg;
  /// The layout: the oriented blades the coefficients are stored on.
  static constexpr auto layout = Blades;
  /// The number of coefficients, one per blade of the layout.
  static constexpr std::size_t component_count = Blades.size();

  /// Zero: T{} in every place.
  constexpr mv() = default;

  /// From exactly one value per component, in the order the layout lists
  /// the blades: Line{0, 0, 1, 0, 0, 0}. Each value converts to T as in
  /// brace initialization, so an integer literal is accepted for a floating
  /// scalar and a variable that would narrow is an error. Explicit when the
  /// layout holds one blade, so that a scalar never converts silently into
  /// a multivector.
  using detail::storage_of<mv, T, Blades.size()>::storage_of;

  /// Rejects any other number of values, other than none and a single
  /// multivector, with one error that prints the number given and the
  /// number of components, as "3 == 6". Because this constructor exists,
  /// std::is_constructible_v<Line, double, double, double> is true.
  template <class... Values>
    requires(sizeof...(Values) > 0 && sizeof...(Values) != component_count &&
             !(sizeof...(Values) == 1 && (multivector<Values> && ...)))
  constexpr explicit(sizeof...(Values) == 1) mv(Values&&... /*values*/) {
    static_assert(sizeof...(Values) == component_count,
                  "mv: give one value for each component, in the order the "
                  "layout lists them");
  }
};

namespace detail {

// What a blade name means for a layout: the place of the blade it names, or
// why it names none there.
enum class NameLookup : std::uint8_t {
  found,
  wide_algebra,
  not_one_name,
  not_a_name,
  repeated_generator,
  index_above_n,
  absent,
  other_orientation,
};

struct name_lookup {
  NameLookup result;
  // The position in the layout when result is found; otherwise K.
  std::size_t at;
};

// The blade that `text` names in an algebra of n generators, looked up in
// `layout` by its mask, and compared by its sign. The parser is the one
// blades uses, so a name means here what it means there; it reports an
// invalid name as a value, so a lookup never fails to compile.
template <std::size_t K>
[[nodiscard]] constexpr name_lookup
look_up_name(const blade_set<K>& layout, std::size_t n, std::string_view text) {
  if (n > max_compact_generators) {
    return {.result = NameLookup::wide_algebra, .at = K};
  }
  if (name_count(text) != 1) {
    return {.result = NameLookup::not_one_name, .at = K};
  }

  std::size_t from = 0;
  const parsed_name parsed = parse_name(next_name(text, from), n);
  switch (parsed.error) {
  case NameError::none:
    break;
  case NameError::index_above_n:
    return {.result = NameLookup::index_above_n, .at = K};
  case NameError::repeated_generator:
    return {.result = NameLookup::repeated_generator, .at = K};
  default:
    return {.result = NameLookup::not_a_name, .at = K};
  }

  for (std::size_t at = 0; at < K; ++at) {
    if (layout.blades[at].mask == parsed.blade.mask) {
      const NameLookup result = layout.blades[at].sign == parsed.blade.sign
                                    ? NameLookup::found
                                    : NameLookup::other_orientation;
      return {.result = result, .at = at};
    }
  }
  return {.result = NameLookup::absent, .at = K};
}

// The lookup of Name in the multivector type X, evaluated once for each type
// and name a program uses.
template <class X, name_list Name>
inline constexpr name_lookup name_lookup_v = look_up_name(
    X::layout,
    std::remove_cvref_t<decltype(X::algebra_type::value)>::generator_count,
    Name.text());

// The type of a coefficient of x, with the value category of x: a reference
// into an lvalue, a const reference into a const one, and an rvalue
// reference into an rvalue.
template <class X>
using coefficient_reference = decltype(std::forward_like<X>(
    std::declval<std::remove_reference_t<X>&>().coefficients[0]));

} // namespace detail

/// Whether the multivector type X, with any cv-qualification and reference,
/// holds the blade named Name with the orientation the name gives it.
///
/// Requires: nothing. False, never an error, for a type that is not a
/// multivector, a name that is not valid in its algebra, a blade the layout
/// lacks, and a blade the layout holds with the other orientation. A name
/// is "1" or e followed by generator indices, one digit each, as for blades.
template <class X, detail::name_list Name>
concept has_blade =
    multivector<X> &&
    detail::name_lookup_v<std::remove_cvref_t<X>, Name>.result ==
        detail::NameLookup::found;

/// The coefficient of x on the blade named Name: get<"e43">(line) reads it,
/// and get<"e43">(line) = 1.0 writes it. The name is oriented, as in the
/// layout: a line that stores e41 = -e14 is read with "e41". The function is
/// found by argument-dependent lookup, so the call needs no namespace.
///
/// Requires: has_blade<X, Name>. Any other name is rejected at compile time
/// with one error whose message names the cause: a blade the layout lacks,
/// one it holds with the other orientation (use the name the layout gives
/// and negate the value), an index above n, a text that is not a name, a
/// name with a repeated generator, more than one name, or an algebra of more
/// than nine generators, where names are not read and coefficients are
/// reached by position. Because the rejecting overload exists, a
/// requires-expression that asks whether get<"e1">(line) is valid is told
/// yes for any name; ask has_blade instead.
/// Guarantees: a reference to the coefficient, with the value category of
/// x: an rvalue reference for an rvalue, so the result of get on a
/// temporary must not outlive it. The position is found at compile time,
/// once for each type and name.
template <detail::name_list Name, class X>
  requires has_blade<X, Name>
// x is forwarded by std::forward_like, which gives the coefficient the value
// category of x; the check counts only std::forward.
// NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
[[nodiscard]] constexpr detail::coefficient_reference<X> get(X&& x) noexcept {
  constexpr std::size_t at =
      detail::name_lookup_v<std::remove_cvref_t<X>, Name>.at;
  return std::forward_like<X>(x.coefficients[at]);
}

/// Rejects any name that has_blade refuses, with the message for its cause.
/// Its return type is that of get, so that the message is the only error
/// and a requires-expression gets an answer without compiling the message.
template <detail::name_list Name, class X>
  requires(multivector<X> && !has_blade<X, Name>)
// NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): as for get above.
constexpr detail::coefficient_reference<X> get(X&& x) noexcept {
  using enum detail::NameLookup;
  constexpr detail::NameLookup cause =
      detail::name_lookup_v<std::remove_cvref_t<X>, Name>.result;
  if constexpr (cause == wide_algebra) {
    static_assert(false, "get: names have one digit per index, so they are "
                         "read only for n up to 9; reach the coefficient by "
                         "its position");
  } else if constexpr (cause == not_one_name) {
    static_assert(false, "get: give one blade name");
  } else if constexpr (cause == not_a_name) {
    static_assert(false, "get: a name is 1 or e followed by generator "
                         "indices, as in e41");
  } else if constexpr (cause == repeated_generator) {
    static_assert(false, "get: the name lists one generator twice; a blade "
                         "names each generator once");
  } else if constexpr (cause == index_above_n) {
    static_assert(false, "get: the name has an index above n, the number of "
                         "generators");
  } else if constexpr (cause == absent) {
    static_assert(false, "get: the type has no blade of this name; its "
                         "layout lists the names it holds");
  } else {
    static_assert(false, "get: the type stores this blade with the other "
                         "orientation; use the name its layout gives it and "
                         "negate the value");
  }
  return std::forward_like<X>(x.coefficients[0]);
}

} // namespace galoot::core

#endif // GALOOT_CORE_MV_HPP
