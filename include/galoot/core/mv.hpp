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
#include <limits>
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

template <class T, std::size_t I>
using component = T;

// The coefficients of a multivector, and its constructor from one value per
// component. They are a base class because the constructor's parameter list,
// one T for each position 0 to K - 1, is a pack expansion over the
// positions, which needs a template that has them as parameters. The first
// parameter is the multivector type itself, so that no two multivector types
// share this base and none can be compared with another through it.
template <class Multivector, class T, class Positions>
struct coefficient_storage;

template <class Multivector, class T, std::size_t... I>
struct coefficient_storage<Multivector, T, std::index_sequence<I...>> {
  // T{} in every place, written out once per position. The shorter `{}`
  // gives the same value, but GCC 16 then reuses one shared initializer for
  // every place, and a constant evaluation that writes into one value of the
  // type changes what a later `{}` of the type gives at run time: with a
  // rational scalar whose T{} is 0/1, the places from the one written on
  // became 0/0.
  std::array<T, sizeof...(I)> coefficients{zero_at(I)...};

  // T{}, for the place `position`.
  static constexpr T zero_at(std::size_t /*position*/) { return T{}; }

  constexpr coefficient_storage() = default;

  // Not declared for an empty layout, where it would be the default
  // constructor. Brace initialization applies the language's rule for
  // narrowing to each value, as for an aggregate.
  constexpr explicit(sizeof...(I) == 1)
      coefficient_storage(component<T, I>... values)
    requires(sizeof...(I) > 0)
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
/// layout.
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
struct mv
    : detail::coefficient_storage<mv<Alg, Blades, T>, T,
                                  std::make_index_sequence<Blades.size()>> {
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
  using detail::coefficient_storage<
      mv, T, std::make_index_sequence<Blades.size()>>::coefficient_storage;

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

} // namespace galoot::core

#endif // GALOOT_CORE_MV_HPP
