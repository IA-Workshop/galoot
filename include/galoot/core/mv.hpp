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
// A value moves to a layout that holds all its blades by an explicit
// conversion, and to one that lacks some of them by truncate, which names
// the loss:
//
//   inline constexpr auto motor_layout =
//       galoot::core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
//   using Motor = galoot::core::mv<rga3, motor_layout, double>;
//
//   Motor m{axis};                                // or Motor(axis)
//   Line back = galoot::core::truncate<Line>(m);  // drops 1 and e1234
//   // Motor wrong = axis;  does not compile: conversions are explicit
//
// Since e41 = -e14, the coefficient written on e41 multiplies -e14, and no
// sign is hidden from the formula. Naming the layout and the type, as above,
// also lets Clang name them in some of its own errors, such as the
// candidates of a failed conversion; the context of the library's messages
// still shows a layout as its masks and signs.

#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>

#include <galoot/core/assert.hpp>
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
/// coefficients of the generated products; and T is not bool, whose sums
/// are not a ring's (true + true is true), though its arithmetic meets the
/// other requirements through integer promotion. The requirement that fails
/// on the fewest declarations is tested first, so that a compiler's account
/// of a failure stays short. Not checked, and required: T{} is the zero of
/// T, and +, -, * obey the ring laws. Equality is not required, so that a
/// scalar holding several lanes at once is accepted.
template <class T>
concept ring_scalar = !std::same_as<std::remove_cv_t<T>, bool> &&
                      std::semiregular<T> && requires(const T& a, const T& b) {
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

/// The multivector type, defined below.
template <metric_like Alg, blade_set Blades, std::destructible T>
struct mv;

namespace detail {

template <class X>
inline constexpr bool is_multivector_v = false;

template <metric_like Alg, blade_set Blades, std::destructible T>
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

// What is wrong with a scalar type for a multivector, if anything: the
// first fault in the order the multivector reports them, so that a scalar
// with several faults gives one error.
enum class ScalarFault : std::uint8_t {
  none,
  reference_or_qualified,
  boolean,
  not_default_initializable,
  not_copyable,
  no_ring_operations,
};

template <class T>
[[nodiscard]] consteval ScalarFault scalar_fault() noexcept {
  if constexpr (!std::same_as<T, std::remove_cvref_t<T>>) {
    return ScalarFault::reference_or_qualified;
  } else if constexpr (std::same_as<T, bool>) {
    return ScalarFault::boolean;
  } else if constexpr (!std::default_initializable<T>) {
    return ScalarFault::not_default_initializable;
  } else if constexpr (!std::copyable<T>) {
    return ScalarFault::not_copyable;
  } else if constexpr (!ring_scalar<T>) {
    return ScalarFault::no_ring_operations;
  } else {
    return ScalarFault::none;
  }
}

// Whether every mask of `source` is held by `target`, in either orientation.
template <std::size_t K, std::size_t L>
[[nodiscard]] constexpr bool holds_every_mask(const blade_set<K>& target,
                                              const blade_set<L>& source) {
  // NOLINTNEXTLINE(readability-use-anyofallof): avoids <algorithm> here.
  for (const oriented_blade& blade : source.blades) {
    if (position_of(target, blade.mask) == K) {
      return false;
    }
  }
  return true;
}

// One statement of the straight-line code that applies a plan or a map:
// coefficient times the input's coefficient at position a (times, for a
// product, the second input's at b) goes to position out of the result.
// `first` marks the first term that reaches out, which is assigned rather
// than added to the zero already there.
struct kernel_term {
  std::size_t a;
  std::size_t b;
  std::size_t out;
  coefficient_type coefficient;
  bool first;
};

// Marks the first term of each of the R positions of a result, in one pass
// in the order of the terms.
template <std::size_t R, std::size_t N>
constexpr void mark_first_terms(std::array<kernel_term, N>& terms) noexcept {
  std::array<bool, R> seen{};
  for (kernel_term& t : terms) {
    t.first = !seen[t.out];
    seen[t.out] = true;
  }
}

// The terms that copy a multivector on the layout From onto the layout To:
// one for each blade of From whose mask To holds, with the coefficient
// s_from * s_to, since c s_from e_A = (c s_from s_to) s_to e_A.
template <blade_set From, blade_set To>
[[nodiscard]] consteval std::size_t kept_count() noexcept {
  std::size_t count = 0;
  for (const oriented_blade& blade : From.blades) {
    count += position_of(To, blade.mask) < To.size() ? 1 : 0;
  }
  return count;
}

template <blade_set From, blade_set To>
[[nodiscard]] consteval auto make_identity_kernel() noexcept {
  std::array<kernel_term, kept_count<From, To>()> terms{};
  std::size_t k = 0;
  for (std::size_t a = 0; a < From.size(); ++a) {
    const std::size_t at = position_of(To, From.blades[a].mask);
    if (at < To.size()) {
      terms[k] = {
          .a = a,
          .b = 0,
          .out = at,
          .coefficient = static_cast<coefficient_type>(From.blades[a].sign) *
                         To.blades[at].sign,
          .first = false,
      };
      ++k;
    }
  }
  mark_first_terms<To.size()>(terms);
  return terms;
}

// Evaluated once for each pair of layouts a program converts between.
template <blade_set From, blade_set To>
inline constexpr auto identity_kernel = make_identity_kernel<From, To>();

// The positions of From whose masks To lacks: what a truncation drops.
template <blade_set From, blade_set To>
[[nodiscard]] consteval auto make_dropped_positions() noexcept {
  std::array<std::size_t, From.size() - kept_count<From, To>()> dropped{};
  std::size_t k = 0;
  for (std::size_t a = 0; a < From.size(); ++a) {
    if (position_of(To, From.blades[a].mask) == To.size()) {
      dropped[k] = a;
      ++k;
    }
  }
  return dropped;
}

template <blade_set From, blade_set To>
inline constexpr auto dropped_positions = make_dropped_positions<From, To>();

// One term: out = c (x y), or out = out + c (x y) after the first term of
// out, where `value` is x y for a product and x for a map. A coefficient of
// +1 or -1 is an addition or a subtraction, never a multiplication, and the
// first term is assigned, because under IEEE 754 0.0 + v is not v when v is
// -0.0, so a compiler cannot drop the addition. Each result is converted to
// T, since an arithmetic type narrower than int computes in int.
template <coefficient_type C, bool First, class T>
constexpr void accumulate(T& out, const T& value) {
  if constexpr (C == 1) {
    if constexpr (First) {
      out = value;
    } else {
      out = static_cast<T>(out + value);
    }
  } else if constexpr (C == -1) {
    if constexpr (First) {
      out = static_cast<T>(-value);
    } else {
      out = static_cast<T>(out - value);
    }
  } else {
    const T term = static_cast<T>(static_cast<T>(C) * value);
    if constexpr (First) {
      out = term;
    } else {
      out = static_cast<T>(out + term);
    }
  }
}

// The R coefficients of a result from the terms of Kernel, one statement
// each, in order, with every position and coefficient a constant; the
// positions no term reaches are zero. The result is a local array returned
// by value, not one written through a reference, so that a compiler that
// keeps the code out of line need not assume a write to it can change an
// input, and keeps the partial sums in registers.
// clang-tidy reads the parentheses that a fold expression needs as
// redundant once the fold is expanded.
// NOLINTBEGIN(readability-redundant-parentheses)
template <const auto& Kernel, std::size_t R, class T, std::size_t KX,
          std::size_t... K>
constexpr std::array<T, R> run_map(const std::array<T, KX>& x,
                                   std::index_sequence<K...> /*terms*/) {
  std::array<T, R> out{};
  (accumulate<Kernel[K].coefficient, Kernel[K].first>(out[Kernel[K].out],
                                                      x[Kernel[K].a]),
   ...);
  return out;
}

template <const auto& Kernel, std::size_t R, class T, std::size_t KX,
          std::size_t KY, std::size_t... K>
constexpr std::array<T, R> run_product(const std::array<T, KX>& x,
                                       const std::array<T, KY>& y,
                                       std::index_sequence<K...> /*terms*/) {
  std::array<T, R> out{};
  (accumulate<Kernel[K].coefficient, Kernel[K].first>(
       out[Kernel[K].out], static_cast<T>(x[Kernel[K].a] * y[Kernel[K].b])),
   ...);
  return out;
}
// NOLINTEND(readability-redundant-parentheses)

// The coefficients of a multivector, and its constructor from one value per
// component. Ts is T once for each component, so that each parameter of the
// constructor is T itself and a compiler's message about a value names T.
// Its parameters name the multivector's algebra, layout, and scalar, so that
// no two multivector types share this base and none can be compared with
// another through it.
template <class Alg, blade_set Blades, class T, class... Ts>
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
  // TODO: a one-blade value built from a source that converts only
  // explicitly gets the compiler's own error, 114 lines on GCC. A catch-all
  // with a short message would fix that, but it changes three
  // std::is_constructible answers that the tests pin, so it waits until the
  // long message becomes a problem for a user.
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

// coefficient_storage with T once for each blade of the layout.
template <class Alg, blade_set Blades, class T, class Positions>
struct storage_for;

template <class Alg, blade_set Blades, class T, std::size_t... I>
struct storage_for<Alg, Blades, T, std::index_sequence<I...>> {
  template <std::size_t>
  using same = T;
  using type = coefficient_storage<Alg, Blades, T, same<I>...>;
};

// The type the storage holds: T without reference or cv-qualification, so
// that a multivector over const double still gets as far as its own check
// of the scalar, which rejects it with a message; and, for a scalar that is
// not semiregular, which the multivector also rejects, unsigned char, so
// that making a default value of the rejected type raises no second error.
template <class T>
using stored_scalar =
    std::conditional_t<std::semiregular<std::remove_cvref_t<T>>,
                       std::remove_cvref_t<T>, unsigned char>;

template <class Alg, blade_set Blades, class T>
using storage_of = storage_for<Alg, Blades, stored_scalar<T>,
                               std::make_index_sequence<Blades.size()>>::type;

// Declared for the requires-expression below and never defined.
template <metric_like Alg, blade_set Blades, std::destructible T>
void as_multivector(const mv<Alg, Blades, T>&);

// Whether X is an mv or a class derived from one: anything that the
// constructors of mv take as a multivector, by copy or by conversion.
template <class X>
concept multivector_or_derived =
    requires(const std::remove_cvref_t<X>& x) { detail::as_multivector(x); };

// Whether a single constructor argument of type Value is not a coefficient
// of a multivector over T: neither a multivector nor a value that T can be
// made from, explicitly or not, such as a whole array of coefficients.
template <class Value, class T>
concept not_a_coefficient =
    !multivector_or_derived<Value> && !std::constructible_from<T, Value>;

// Whether a value of type Value holds several values, as an array, a
// pointer to one, or a container does: what a user who passes the
// coefficients in one argument passes.
template <class Value>
concept holds_several_values =
    std::is_array_v<std::remove_cvref_t<Value>> ||
    std::is_pointer_v<std::remove_cvref_t<Value>> ||
    requires(const std::remove_cvref_t<Value>& values) {
      values.begin();
      values.end();
    };

} // namespace detail

/// A multivector of the algebra Alg with one coefficient of type T on each
/// oriented blade of the layout Blades, in the order the layout lists them:
/// coefficients[i] multiplies Blades.blades[i].sign times the canonical blade
/// of Blades.blades[i].mask.
///
/// Requires: Alg is metric_like and T is std::destructible, or the type does
/// not match. Checked when the type is used, each with its own message, in
/// this order, so that a scalar with several faults gives one error: T is
/// not a reference and not const or volatile; T is not bool; T is default
/// constructible; T is copyable and assignable; T is a ring_scalar. Then, also
/// each with its own message: every blade of the layout names only e1 to en;
/// every sign is +1 or -1; and no mask appears twice, in either orientation. A
/// layout is a list of oriented blades and does not keep the algebra named in
/// blades<Alg, ...>: any algebra with enough generators accepts it. Generic
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
/// Values of different layouts, algebras, or scalars do not compare. The
/// type has no output operator or formatter; std::format("{}",
/// x.coefficients) prints its coefficients.
template <metric_like Alg, blade_set Blades, std::destructible T>
struct mv : detail::storage_of<Alg, Blades, T> {
private:
  // The rules a scalar and a layout must keep, each named, so that a
  // compiler that prints the condition of a failed assertion prints the
  // rule. Only the first fault of the scalar is reported.
  static constexpr detail::ScalarFault scalar_fault = detail::scalar_fault<T>();
  static constexpr bool scalar_is_not_a_reference_or_qualified =
      scalar_fault != detail::ScalarFault::reference_or_qualified;
  static constexpr bool scalar_is_not_bool =
      scalar_fault != detail::ScalarFault::boolean;
  static constexpr bool scalar_is_default_constructible =
      scalar_fault != detail::ScalarFault::not_default_initializable;
  static constexpr bool scalar_is_copyable =
      scalar_fault != detail::ScalarFault::not_copyable;
  static constexpr bool scalar_has_ring_operations =
      scalar_fault != detail::ScalarFault::no_ring_operations;
  static_assert(scalar_is_not_a_reference_or_qualified,
                "mv: the scalar type is a reference or is const or volatile; "
                "give the type itself, as in double for const double");
  static_assert(scalar_is_not_bool,
                "mv: bool cannot be the scalar type, since true + true is "
                "true; use an integer type such as int");
  static_assert(scalar_is_default_constructible,
                "mv: the scalar type must be default constructible, "
                "copyable, and assignable; give it a default constructor "
                "that makes its zero");
  static_assert(scalar_is_copyable,
                "mv: the scalar type must be copyable and assignable, as a "
                "number is; give it copy construction and copy assignment, "
                "which a move-only type or a class with a const member "
                "lacks");
  static_assert(scalar_has_ring_operations,
                "mv: the scalar type must support +, -, *, unary - and "
                "construction from an integer coefficient; use a number type "
                "such as double or an integer type");

  static constexpr bool layout_names_only_generators_of_the_algebra =
      detail::blades_in_algebra<
          std::remove_cvref_t<decltype(Alg::value)>::generator_count, Blades>;
  static constexpr bool layout_signs_are_plus_or_minus_one =
      detail::signs_valid<Blades>;
  static constexpr bool layout_holds_each_blade_once =
      detail::distinct_masks<Blades>;
  static_assert(layout_names_only_generators_of_the_algebra,
                "mv: the layout names a generator that the algebra does not "
                "have; use a layout whose blades are all in this algebra");
  static_assert(layout_signs_are_plus_or_minus_one,
                "mv: the layout holds a sign other than +1 or -1; give each "
                "blade of a layout written out by hand the sign +1 or -1");
  static_assert(layout_holds_each_blade_once,
                "mv: the layout holds one blade twice, perhaps in both "
                "orientations; list each blade once");

public:
  /// The algebra tag.
  using algebra_type = Alg;
  /// The scalar type of the coefficients.
  using scalar_type = T;
  /// The layout: the oriented blades the coefficients are stored on.
  static constexpr auto layout = Blades;
  /// The number of coefficients, one per blade of the layout.
  static constexpr std::size_t component_count = Blades.size();

  /// Zero: T{} in every place.
  constexpr mv() = default;

  /// From exactly one value per component, in the order the layout lists
  /// the blades: Line{0, 0, 1, 0, 0, 0}. With braces, each value converts
  /// to T as in brace initialization: an integer literal is accepted for a
  /// floating scalar, and a variable that would narrow is rejected (an error
  /// on Clang; on GCC a warning, an error when warnings are errors). With
  /// parentheses, Line(0, 0, 1, 0, 0, 0), values convert as for any
  /// constructor and may narrow without a diagnostic. Explicit when the
  /// layout holds one blade, so that a scalar never converts silently into
  /// a multivector.
  using detail::storage_of<Alg, Blades, T>::storage_of;

  /// From a multivector of the same algebra and scalar type on another
  /// layout, every blade of which this layout holds, in either orientation:
  /// Motor{line}, Motor(line), or static_cast<Motor>(line). Each coefficient
  /// moves to the place of its blade here, negated where the two layouts
  /// give the blade opposite orientations, so the value is unchanged; the
  /// blades the source lacks are zero. Explicit, as every conversion between
  /// layouts is: Motor m = line; does not compile. The comment at the end of
  /// the declaration's line is for the reader of a compiler's error: both
  /// compilers show this line when they skip the constructor in such an
  /// implicit conversion.
  template <blade_set From>
  // A requires-clause needs the parentheses around a function call.
  // NOLINTNEXTLINE(readability-redundant-parentheses)
    requires(detail::holds_every_mask(Blades, From))
  constexpr explicit mv(const mv<Alg, From, T>& source) { // write Type{x}
    this->coefficients =
        detail::run_map<detail::identity_kernel<From, Blades>, Blades.size()>(
            source.coefficients,
            std::make_index_sequence<
                detail::identity_kernel<From, Blades>.size()>{});
  }

  /// Rejects, with one error, a conversion from a layout with a blade that
  /// this layout lacks: it would lose that blade's coefficient. Such a
  /// conversion is named, truncate<Type>(x). An implicit conversion, such
  /// as const Line l = motor;, reaches no constructor and is the compiler's
  /// own error, which shows this declaration's line and the comment at its
  /// end. Because this constructor exists, std::is_constructible_v<Line,
  /// Motor> is true; std::is_convertible_v<Motor, Line> is false.
  template <blade_set From>
    requires(!detail::holds_every_mask(Blades, From))
  constexpr explicit mv(const mv<Alg, From, T>& /*x*/) { // truncate<Type>(x)
    static_assert(false, "mv: the conversion drops blades of its input; it "
                         "is lossy, so request it with truncate<Type>(x)");
  }

  /// Rejects any other number of values, other than none and a single
  /// argument that is not a coefficient, with one error that prints the
  /// number given and the number of components, as "3 == 6". A single
  /// multivector, or a value of a class derived from one, is copied or
  /// converted by the constructors above instead. Because this constructor
  /// exists, std::is_constructible_v<Line, double, double, double> is true.
  template <class... Values>
    requires(sizeof...(Values) > 0 && sizeof...(Values) != component_count &&
             !(sizeof...(Values) == 1 &&
               (detail::multivector_or_derived<Values> && ...)) &&
             !(sizeof...(Values) == 1 &&
               (detail::not_a_coefficient<Values, T> && ...)))
  constexpr explicit(sizeof...(Values) == 1) mv(Values&&... /*values*/) {
    static_assert(sizeof...(Values) == component_count,
                  "mv: give one value for each component, in the order the "
                  "layout lists them");
  }

  /// Rejects, with one error, a single argument that is neither a
  /// multivector nor a value that T can be made from: one that holds
  /// several values, such as a std::array of the coefficients,
  /// Line{values}, with the message for coefficients given in one argument,
  /// and any other, such as a std::complex<double> for a double layout, with
  /// the message that the value does not convert. Because this constructor
  /// exists, std::is_constructible_v<Line, std::array<double, 6>> is true.
  template <class Value>
    requires detail::not_a_coefficient<Value, T>
  constexpr explicit mv(Value&& /*value*/) {
    if constexpr (detail::holds_several_values<Value>) {
      static_assert(false, "mv: give the coefficients one by one, not as one "
                           "array or braced list; write one value for each "
                           "component, in the order the layout lists them");
    } else {
      static_assert(false, "mv: the value does not convert to the scalar "
                           "type; give one value of the scalar type for each "
                           "component");
    }
  }

  /// Rejects, with one error, coefficients given as a braced list in a
  /// second pair of braces, Line{{0, 0, 1, 0, 0, 0}}, which an aggregate
  /// would take, or as a C array of T: a braced list binds to an array
  /// parameter and to nothing else, so that is the parameter this rejection
  /// needs. Explicit, as every single-argument constructor is, so a C array
  /// converts to no multivector implicitly; Line l = {{...}}; is then the
  /// compiler's error, which names the explicit constructor. Because this
  /// constructor and the one above exist, std::is_constructible_v<Line,
  /// double (&)[6]> is true; std::is_convertible_v is false.
  template <std::size_t N>
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  constexpr explicit mv(const std::remove_cvref_t<T> (& /*values*/)[N]) {
    static_assert(false, "mv: give the coefficients one by one, not as one "
                         "array or braced list; write one value for each "
                         "component, in the order the layout lists them");
  }
};

namespace detail {

// Whether std::numeric_limits<T> describes an integer type with its range,
// or a floating type of radix 2 with its digits and exponent range. A
// program's own specialization may define fewer members; a member it lacks
// makes the answer no, not an error.
// TODO: a specialization that defines only some members is no longer
// checked beyond this test. A conforming specialization defines every
// member, so this matters only for non-conforming code; revisit if a user
// reports it.
template <class T>
concept integer_limits = requires {
  requires std::numeric_limits<T>::is_specialized;
  requires std::numeric_limits<T>::is_integer;
  { std::numeric_limits<T>::is_signed } -> std::convertible_to<bool>;
  { std::numeric_limits<T>::digits } -> std::convertible_to<int>;
  { std::numeric_limits<T>::min() } -> std::convertible_to<std::int64_t>;
  { std::numeric_limits<T>::max() } -> std::convertible_to<std::uint64_t>;
};
template <class T>
concept binary_floating_limits = requires {
  requires std::numeric_limits<T>::is_specialized;
  requires !std::numeric_limits<T>::is_exact;
  requires std::numeric_limits<T>::radix == 2;
  { std::numeric_limits<T>::digits } -> std::convertible_to<int>;
  { std::numeric_limits<T>::max_exponent } -> std::convertible_to<int>;
};

// Whether the integer c converts to T exactly, as far as
// std::numeric_limits<T> tells. A signed integer type holds c when c lies
// from min() to max(); an unsigned one, whose arithmetic is modular, when
// |c| is at most max(). A floating type of radix 2 holds c when the odd
// part of |c| has at most `digits` bits and |c| is below 2^max_exponent: it
// holds every integer up to 2^digits, and above that the ones that are a
// smaller integer times a power of two. A type that its limits describe as
// neither, such as a type without a specialization or an exact type that is
// not an integer type, is not checked.
template <class T>
[[nodiscard]] constexpr bool holds_exactly(coefficient_type c) noexcept {
  using limits = std::numeric_limits<T>;
  const std::uint64_t magnitude =
      c < 0 ? std::uint64_t{0} - static_cast<std::uint64_t>(c)
            : static_cast<std::uint64_t>(c);
  if constexpr (integer_limits<T>) {
    if constexpr (limits::is_signed) {
      // A signed type of 63 or more digits holds every std::int64_t.
      return limits::digits >= 63 ||
             (c >= static_cast<coefficient_type>(limits::min()) &&
              c <= static_cast<coefficient_type>(limits::max()));
    } else {
      return magnitude <= static_cast<std::uint64_t>(limits::max());
    }
  } else if constexpr (binary_floating_limits<T>) {
    // No shift by `digits`, which may be 64 or more.
    const int odd_bits =
        magnitude == 0
            ? 0
            : std::bit_width(magnitude >> static_cast<unsigned>(
                                              std::countr_zero(magnitude)));
    return odd_bits <= limits::digits &&
           std::bit_width(magnitude) <= limits::max_exponent;
  } else {
    return true;
  }
}

template <class T, std::size_t R, std::size_t N>
[[nodiscard]] constexpr bool
all_held_exactly(const typed_plan<R, N>& plan) noexcept {
  // NOLINTNEXTLINE(readability-use-anyofallof): avoids <algorithm> here.
  for (const typed_term& t : plan.terms) {
    if (!holds_exactly<T>(t.coefficient)) {
      return false;
    }
  }
  return true;
}

// Whether every coefficient of the plan of Op over A and B converts to T
// exactly, evaluated once for each plan and scalar type a program uses.
template <BinaryOperation Op, metric_like Alg, blade_set A, blade_set B,
          class T>
// clang-tidy reads the operation, once substituted, as a C-style cast of its
// value, which the source does not have.
// NOLINTBEGIN(modernize-avoid-c-style-cast)
inline constexpr bool coefficients_exact =
    all_held_exactly<T>(product_plan<Op, Alg, A, B>);
// NOLINTEND(modernize-avoid-c-style-cast)

// The terms of the plan of Op over A and B, with each output moved to the
// place of its blade in the layout Target and multiplied by that blade's
// sign, since c e_C = (c s) (s e_C). Target holds every blade of the
// plan's result.
template <BinaryOperation Op, metric_like Alg, blade_set A, blade_set B,
          blade_set Target>
[[nodiscard]] consteval auto make_product_kernel() noexcept {
  constexpr const auto& plan = product_plan<Op, Alg, A, B>;
  std::array<kernel_term, plan.terms.size()> terms{};
  for (std::size_t k = 0; k < terms.size(); ++k) {
    const typed_term& t = plan.terms[k];
    const std::size_t at = position_of(Target, plan.result.blades[t.out].mask);
    terms[k] = {
        .a = t.a,
        .b = t.b,
        .out = at,
        .coefficient = Target.blades[at].sign * t.coefficient,
        .first = false,
    };
  }
  mark_first_terms<Target.size()>(terms);
  return terms;
}

template <BinaryOperation Op, metric_like Alg, blade_set A, blade_set B,
          blade_set Target>
// As for coefficients_exact.
// NOLINTBEGIN(modernize-avoid-c-style-cast)
inline constexpr auto product_kernel =
    make_product_kernel<Op, Alg, A, B, Target>();
// NOLINTEND(modernize-avoid-c-style-cast)

// The terms of the map Op of S, onto Target as for a product.
template <UnaryOperation Op, metric_like Alg, blade_set S, blade_set Target>
[[nodiscard]] consteval auto make_map_kernel() noexcept {
  constexpr const auto& map = unary_map<Op, Alg, S>;
  std::array<kernel_term, map.terms.size()> terms{};
  for (std::size_t k = 0; k < terms.size(); ++k) {
    const map_term& t = map.terms[k];
    const std::size_t at = position_of(Target, map.result.blades[t.out].mask);
    terms[k] = {
        .a = t.a,
        .b = 0,
        .out = at,
        .coefficient = Target.blades[at].sign * t.coefficient,
        .first = false,
    };
  }
  mark_first_terms<Target.size()>(terms);
  return terms;
}

template <UnaryOperation Op, metric_like Alg, blade_set S, blade_set Target>
// As for coefficients_exact.
// NOLINTNEXTLINE(modernize-avoid-c-style-cast)
inline constexpr auto map_kernel = make_map_kernel<Op, Alg, S, Target>();

/// The product Op of x and y, written onto the layout Target: the plan of Op
/// over the layouts of x and y (product_plan) applied to their coefficients
/// by straight-line code, one statement for each term of the plan, in the
/// order of the plan's terms, with every position and coefficient a
/// constant of the code. Each term adds c (x_a y_b) to its output, a
/// coefficient c of +1 or -1 as an addition or a subtraction, and the first
/// term of each output is assigned, not added to zero, so a lone term of
/// -0.0 gives -0.0; ring_scalar's unary minus serves a first term of -1.
/// Positions of Target that no term reaches are zero.
///
/// Requires: Target holds every blade of the plan's result, in either
/// orientation, and every coefficient of the plan converts to T exactly
/// where std::numeric_limits<T> can tell (a signed integer type when c is
/// within min() to max(), an unsigned one, whose arithmetic is modular, when
/// |c| is at most max(), and a binary floating type when the odd part of |c|
/// fits its digits and |c| is below 2^max_exponent): each checked at compile
/// time with its own message. A scalar without
/// a specialization, such as std::complex<float>, must hold the coefficients
/// exactly without a check. The code is one fold expression over the terms, and
/// Clang 23 stops a fold at 2,048 terms; no plan that Clang 23 builds at its
/// default limits has more than 1,024 terms. Measured at the default limits, at
/// -O0 and -O2, with and without _GLIBCXX_ASSERTIONS: apply builds wherever its
/// plan builds. The plans build up to full<6> times full<6> (4,096 terms) on
/// GCC 16, and up to full<5> times full<5> on Clang 23, with one exception
/// there: the geometric antiproduct of full<5> in conformal(3) builds only
/// with _GLIBCXX_ASSERTIONS, which libstdc++ defines by itself only without
/// optimization, because without it each subscript is a function call that
/// Clang counts, and the plan then needs more than Clang's default steps
/// (-fconstexpr-steps=1200000 is enough). product_plan's comment gives the
/// counts, taken with _GLIBCXX_ASSERTIONS.
/// Guarantees: the coefficient of each blade of Target is the sum over the
/// terms on it, multiplied by the blade's sign in Target, evaluated in the
/// order stated; with an exact scalar the result is exact. A constant
/// evaluation and a run-time call give the same result.
template <BinaryOperation Op, blade_set Target, metric_like Alg, blade_set A,
          blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, Target, T> apply(const mv<Alg, A, T>& x,
                                                 const mv<Alg, B, T>& y) {
  constexpr bool target_holds_result =
      holds_every_mask(Target, product_plan<Op, Alg, A, B>.result);
  constexpr bool scalar_holds_every_coefficient_of_the_product =
      coefficients_exact<Op, Alg, A, B, T>;
  static_assert(target_holds_result,
                "apply: the target layout does not hold every blade of the "
                "product");
  static_assert(scalar_holds_every_coefficient_of_the_product,
                "apply: the scalar type cannot hold one of the product's "
                "exact coefficients; use a scalar type that holds larger "
                "integers exactly");

  mv<Alg, Target, T> out;
  if constexpr (target_holds_result &&
                scalar_holds_every_coefficient_of_the_product) {
    out.coefficients =
        run_product<product_kernel<Op, Alg, A, B, Target>, Target.size()>(
            x.coefficients, y.coefficients,
            std::make_index_sequence<
                product_kernel<Op, Alg, A, B, Target>.size()>{});
  }
  return out;
}

/// The product Op of x and y on the blade set of its result, the plan's
/// result set: apply<Op, product_plan<Op, Alg, A, B>.result>(x, y).
template <BinaryOperation Op, metric_like Alg, blade_set A, blade_set B,
          ring_scalar T>
[[nodiscard]] constexpr mv<Alg, product_plan<Op, Alg, A, B>.result, T>
apply(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  return apply<Op, product_plan<Op, Alg, A, B>.result>(x, y);
}

/// The map Op of x, written onto the layout Target, as for a product: one
/// statement for each blade of x, its coefficient of +1 or -1 an
/// assignment of the coefficient or of its negation.
///
/// Requires: Target holds every blade of the map's result, checked at
/// compile time with its message. Measured as for a product: apply builds
/// wherever the map builds, up to full<8> for the complements and full<7>
/// for the other maps on Clang 23, and full<9> on GCC 16.
/// Guarantees: as for a product.
template <UnaryOperation Op, blade_set Target, metric_like Alg, blade_set S,
          ring_scalar T>
[[nodiscard]] constexpr mv<Alg, Target, T> apply(const mv<Alg, S, T>& x) {
  constexpr bool target_holds_result =
      holds_every_mask(Target, unary_map<Op, Alg, S>.result);
  static_assert(target_holds_result,
                "apply: the target layout does not hold every blade of the "
                "map");

  mv<Alg, Target, T> out;
  if constexpr (target_holds_result) {
    out.coefficients = run_map<map_kernel<Op, Alg, S, Target>, Target.size()>(
        x.coefficients,
        std::make_index_sequence<map_kernel<Op, Alg, S, Target>.size()>{});
  }
  return out;
}

/// The map Op of x on the blade set of its result, the map's result set.
template <UnaryOperation Op, metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, unary_map<Op, Alg, S>.result, T>
apply(const mv<Alg, S, T>& x) {
  return apply<Op, unary_map<Op, Alg, S>.result>(x);
}

// Checks that every coefficient that a truncation drops is zero. The
// message is on the line of the check, which a failed constant evaluation
// shows.
template <class T, std::size_t K, std::size_t D>
constexpr void check_dropped(const std::array<T, K>& c,
                             const std::array<std::size_t, D>& dropped) {
  for (const std::size_t at : dropped) {
    GALOOT_ASSERT(c[at] == T{}, "truncate: a dropped coefficient is not zero");
  }
}

// Whether Target, with any cv-qualification and reference, is a multivector
// type of the algebra Alg and the scalar type T.
template <class Target, class Alg, class T>
concept multivector_of =
    multivector<Target> &&
    std::same_as<typename std::remove_cvref_t<Target>::algebra_type, Alg> &&
    std::same_as<typename std::remove_cvref_t<Target>::scalar_type, T>;

} // namespace detail

/// x on the layout To: each blade of To that x holds, in either
/// orientation, takes x's coefficient, negated where the two layouts give
/// the blade opposite orientations; every other blade of To is zero; and
/// the blades of x that To lacks are dropped. It drops components, not
/// digits: no coefficient is rounded. truncate<Line>(motor) names the
/// target by its type and truncate<line_layout>(motor) by its layout. The
/// type must be an mv type itself, not a class derived from one; to reach
/// such a class, truncate to its layout and make the class from the result,
/// which needs a constructor of the class's own, since an inherited
/// constructor does not take a value of the base.
///
/// Requires, for an exact_scalar T with ==: every coefficient that it drops
/// is zero. Checked by GALOOT_ASSERT in every constant evaluation, which
/// then fails, and at run time when GALOOT_CHECKING is 1, which then aborts
/// with the message; with GALOOT_CHECKING 0 the coefficients are dropped
/// unchecked at run time. For a rounding scalar nothing is checked: a
/// coefficient that should be zero may be a rounding error away from it,
/// and truncate has no error bound to test it against; the operations that
/// drop part of their own result check it against their own bound.
/// Guarantees: the result is exact, for any scalar.
template <blade_set To, metric_like Alg, blade_set From, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, To, T> truncate(const mv<Alg, From, T>& x) {
  mv<Alg, To, T> kept;
  kept.coefficients =
      detail::run_map<detail::identity_kernel<From, To>, To.size()>(
          x.coefficients,
          std::make_index_sequence<detail::identity_kernel<From, To>.size()>{});
  if constexpr (exact_scalar<T> && std::equality_comparable<T>) {
    detail::check_dropped(x.coefficients, detail::dropped_positions<From, To>);
  }
  return kept;
}

/// truncate<Target::layout>(x) for a multivector type Target of x's algebra
/// and scalar type, with any cv-qualification and reference.
template <class Target, metric_like Alg, blade_set From, ring_scalar T>
  requires detail::multivector_of<Target, Alg, T>
[[nodiscard]] constexpr std::remove_cvref_t<Target>
truncate(const mv<Alg, From, T>& x) {
  return truncate<std::remove_cvref_t<Target>::layout>(x);
}

/// Rejects a target that is not an mv type of x's algebra and scalar type,
/// such as double, an mv of another algebra or scalar, or a class derived
/// from an mv, with one error. It returns a default value of the target,
/// where it has one, so that a use of the result raises no second error.
template <class Target, metric_like Alg, blade_set From, ring_scalar T>
  requires(!detail::multivector_of<Target, Alg, T>)
constexpr auto truncate(const mv<Alg, From, T>& x) {
  static_assert(false, "truncate: the target must be an mv type of the same "
                       "algebra and scalar type, not a class derived from "
                       "one; truncate changes only which blades are held");
  if constexpr (std::default_initializable<std::remove_cvref_t<Target>>) {
    return std::remove_cvref_t<Target>{};
  } else {
    return x;
  }
}

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
  absent_from_ordered_layout,
  other_orientation,
  other_orientation_in_ordered_layout,
};

struct name_lookup {
  NameLookup result;
  // The position in the layout when result is found; otherwise K.
  std::size_t at;
};

// Whether every blade of `layout` has sign +1 and the blades come in
// canonical order, by grade and then by index list, as in every layout that
// full, of_grade, or a product generates: then the name of each blade lists
// its indices in increasing order.
template <std::size_t K>
[[nodiscard]] constexpr bool in_canonical_order(const blade_set<K>& layout) {
  for (std::size_t at = 0; at < K; ++at) {
    if (layout.blades[at].sign != 1 ||
        (at > 0 &&
         !canonical_less(layout.blades[at - 1].mask, layout.blades[at].mask))) {
      return false;
    }
  }
  return true;
}

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
      if (layout.blades[at].sign == parsed.blade.sign) {
        return {.result = NameLookup::found, .at = at};
      }
      const NameLookup result =
          in_canonical_order(layout)
              ? NameLookup::other_orientation_in_ordered_layout
              : NameLookup::other_orientation;
      return {.result = result, .at = at};
    }
  }
  const NameLookup result = in_canonical_order(layout)
                                ? NameLookup::absent_from_ordered_layout
                                : NameLookup::absent;
  return {.result = result, .at = K};
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
/// with one error whose message names the cause: a blade the layout lacks
/// in either orientation; one it holds with the other orientation (use the
/// name the layout gives and negate the value; for a layout in canonical
/// order, such as one that full, of_grade, or a product generates, which
/// lists no names of its own, the message says how its blades are named);
/// an index above n; a text that is not a name; a name with a repeated
/// generator; more than one name; or an algebra of more than nine
/// generators, where names are not read and coefficients are reached by
/// position. Because the rejecting overload exists, a
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
  } else if constexpr (cause == absent_from_ordered_layout) {
    static_assert(false, "get: the type has no blade of this name, in either "
                         "orientation; its layout does not hold it");
  } else if constexpr (cause == other_orientation_in_ordered_layout) {
    static_assert(false, "get: the type stores this blade with the other "
                         "orientation; its layout names each blade by its "
                         "indices in increasing order, as e12 for e21; use "
                         "that name and negate the value");
  } else {
    static_assert(false, "get: the type stores this blade with the other "
                         "orientation; use the name its layout gives it and "
                         "negate the value");
  }
  return std::forward_like<X>(x.coefficients[0]);
}

} // namespace galoot::core

#endif // GALOOT_CORE_MV_HPP
