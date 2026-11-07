#ifndef GALOOT_CORE_TRANSFORM_HPP
#define GALOOT_CORE_TRANSFORM_HPP

// Versors: the transform of a multivector by a versor, and the composition
// of two versors, in the sense of Lengyel's rigid and conformal geometric
// algebra, where a motor and a flector act through the geometric
// antiproduct ⟇.
//
//   struct rga3 {
//     static constexpr auto value = galoot::core::rigid<3>();
//   };
//   constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;
//   constexpr auto motor_layout = galoot::core::blades<
//       rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
//
//   using Point = galoot::core::mv<rga3, point_layout, double>;
//   using Motor = galoot::core::mv<rga3, motor_layout, double>;
//   Motor turn{0, 0, 0.6, 0.8, 0, 0, 0, 0};  // a rotation about the z axis
//   Motor move{0, 0, 0, 1, 1, 2, -1, 0};     // a translation by (2, 4, -2)
//   Motor both{galoot::core::compose(move, turn)};  // turn, then move
//   Point p{1, 0, 0, 1};
//   Point q = galoot::core::transform(p, both);  // about (2.28, 4.96, -2, 1)
//
// The parity of a versor is the parity of the antigrades n - k of its
// blades, where n is the number of generators and k a blade's grade: a
// motor is even and a flector odd. In rigid(3) the grade and the antigrade
// have the same parity; where n is odd they do not, and a motor of rigid(2)
// or conformal(3) has blades of odd grade.
//
// transform(x, V) applies the versor V to x. With antireverse(V) written V̰,
// which multiplies a blade of antigrade m by (-1)^(m(m-1)/2):
//
//   even V   V ⟇ x ⟇ V̰
//   odd V    -V ⟇ x̌ ⟇ V̰, where x̌, the antigrade involution of x, multiplies
//            a blade of antigrade m by (-1)^m
//
// When n is even x̌ is the grade involution of x, so an odd V gives
// -V ⟇ x̂ ⟇ V̰; when n is odd x̌ is minus the grade involution, so an odd V
// gives V ⟇ x̂ ⟇ V̰. In rigid(3) a flector F then reflects a point and keeps
// its weight positive, where the single form -F ⟇ x ⟇ F̰ would negate it.
// For a versor, an antiproduct of hyperplanes, with V ⟇ V̰ = λ𝟙 and λ not
// zero, the transform T satisfies λ^(k-1) T(a1 ∧ ... ∧ ak) = T(a1) ∧ ... ∧
// T(ak): lines and planes move as the points that span them. A unitized
// motor or flector has λ = 1; transform does not unitize, so a versor with
// λ = 2 doubles the weight of a point it moves, which is the same point.
//
// The result has x's type, its layout and orientations: transform(line, V)
// is a Line. The part of V ⟇ x ⟇ V̰ on other blades is dropped. For a versor
// it is zero; in checking mode transform checks that it is, exactly for an
// exact scalar and within its rounding error for float and double; see the
// comment of transform.
//
// compose(Q2, Q1) is Q2 ⟇ Q1, the versor that applies Q1 first and then Q2.
// Its result is on compose_layout<Alg, A, B>, the canonical set of the blades
// the antiproduct reaches, each with orientation +1, in order of grade and
// then of index list, as for a product: convert it to a named layout
// (Motor{compose(q2, q1)}) to read it under that layout's orientations.
//
// Under `using namespace std::ranges` or `using namespace std::views`, an
// unqualified transform(x, v) finds the function object std::ranges::transform
// or std::views::transform, which turns off argument-dependent lookup, so it
// does not compile; write galoot::core::transform(x, v). Under
// `using namespace std`, transform(x, v) finds this function, since
// std::transform takes iterators.
//
// Compile cost: compose is one geometric antiproduct, and transform two,
// V ⟇ x and then that with V, each a plan built once for each pair of
// layouts a program uses, so they have the antiproduct's limits, which the
// comment of geometric_antiproduct gives. Typed layouts such as a point, a
// line, a plane, a motor, or a flector are far below them. Measured on GCC 16
// and Clang 23 with libstdc++, at the default limits, the same with and
// without libstdc++'s assertions, for x a full<n> multivector of R(n,0,0)
// and V on every blade of even antigrade, the largest layout of one parity:
// transform builds up to n = 5 on Clang and n = 6 on GCC, and compose of two
// such versors up to n = 5 on Clang and n = 7, the largest tried, on GCC.
// Both build for full<5> of conformal(3). In a dense form of five generators,
// where every pair of generators is coupled, neither builds on Clang, and
// both build on GCC. A failure is reported inside a plan's generation, on
// Clang as "constexpr variable 'count' must be initialized by a constant
// expression" and on GCC as an operation count over the limit or a
// libstdc++ assertion that is not a constant expression, followed by errors
// that do not name the operation.

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#include <galoot/core/assert.hpp>
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>
#include <galoot/core/product.hpp>

// The rounding check of transform assumes that each operation rounds once,
// to nearest, with gradual underflow. -ffast-math reassociates sums, and
// the startup code it links flushes subnormal numbers to zero, so the check
// could then reject a correct result.
#if defined(__FAST_MATH__) && GALOOT_CHECKING == 1
#error "transform: checking mode needs IEEE 754 arithmetic, not -ffast-math"
#endif

namespace galoot::core {

namespace detail {

/// The parity of the antigrades of a layout's blades: even when every blade
/// has an even antigrade, which an empty layout has vacuously; odd when
/// every blade has an odd one; mixed otherwise.
enum class Parity : std::uint8_t {
  even,
  odd,
  mixed,
};

// The parity of the antigrades n - k of the blades of `set`.
template <std::size_t K>
[[nodiscard]] constexpr Parity parity_of(const blade_set<K>& set,
                                         std::size_t n) noexcept {
  bool has_even = false;
  bool has_odd = false;
  for (const oriented_blade& blade : set.blades) {
    if (antigrade(blade.mask, n) % 2 == 0) {
      has_even = true;
    } else {
      has_odd = true;
    }
  }
  if (has_even && has_odd) {
    return Parity::mixed;
  }
  return has_odd ? Parity::odd : Parity::even;
}

/// The parity of the antigrades of the blades of S in the algebra Alg.
template <metric_like Alg, blade_set S>
inline constexpr Parity antigrade_parity =
    parity_of(S, std::remove_cvref_t<decltype(Alg::value)>::generator_count);

/// The ways an operation of this header is written, so that a message names
/// the operation as the user wrote it.
// An int, so that a compiler prints its values and not a byte.
// NOLINTNEXTLINE(performance-enum-size)
enum class TransformSpelling {
  transform_function,
  compose_function,
};

/// The only error of a transform or a composition of mismatched operands:
/// the message
/// names the operation and the cause. Each condition compares the two
/// algebras or the two scalar types themselves, so that the compiler prints
/// them. Its result type is that of the first operand, so that a variable
/// initialized from the call adds no second error.
template <TransformSpelling Spelling, class X, class Y>
constexpr std::remove_cvref_t<X> reject_transform_operands() {
  using R = std::remove_cvref_t<X>;
  using S = std::remove_cvref_t<Y>;
  using enum TransformSpelling;
  if constexpr (!std::is_same_v<typename R::algebra_type,
                                typename S::algebra_type>) {
    if constexpr (Spelling == transform_function) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "transform: the operands are in different algebras; give both "
          "types the same algebra tag");
    } else {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "compose: the operands are in different algebras; give both types "
          "the same algebra tag");
    }
  } else {
    if constexpr (Spelling == transform_function) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "transform: the operands have different scalar types; give both "
          "types the same scalar type");
    } else {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "compose: the operands have different scalar types; give both "
          "types the same scalar type");
    }
  }
  return R{};
}

// The two plans of a transform of x on X by V on V: the geometric
// antiproduct of V with x, on its result layout, the middle layout; and
// the antiproduct of that with V, whose result holds the blades that the
// transform reaches.
template <metric_like Alg, blade_set V, blade_set X>
inline constexpr const auto& sandwich_first_plan =
    product_plan<BinaryOperation::geometric_antiproduct, Alg, V, X>;

template <metric_like Alg, blade_set V, blade_set X>
inline constexpr auto sandwich_middle_layout =
    sandwich_first_plan<Alg, V, X>.result;

template <metric_like Alg, blade_set V, blade_set X>
inline constexpr const auto& sandwich_second_plan =
    product_plan<BinaryOperation::geometric_antiproduct, Alg,
                 sandwich_middle_layout<Alg, V, X>, V>;

// Whether the blade at position `out` of the second plan's result is a
// blade of X, which the transform keeps.
template <metric_like Alg, blade_set V, blade_set X>
[[nodiscard]] consteval bool kept_output(std::size_t out) {
  constexpr const auto& second = sandwich_second_plan<Alg, V, X>;
  return position_of(X, second.result.blades[out].mask) < X.size();
}

// The number of terms of the second plan that reach a blade of X, when
// Kept, or another blade, when not.
template <metric_like Alg, blade_set V, blade_set X, bool Kept>
[[nodiscard]] consteval std::size_t second_term_count() {
  std::size_t count = 0;
  for (const typed_term& t : sandwich_second_plan<Alg, V, X>.terms) {
    count += kept_output<Alg, V, X>(t.out) == Kept ? 1 : 0;
  }
  return count;
}

// The number of blades of the second plan's result that X lacks: the
// blades the transform drops.
template <metric_like Alg, blade_set V, blade_set X>
[[nodiscard]] consteval std::size_t dropped_blade_count() {
  constexpr const auto& second = sandwich_second_plan<Alg, V, X>;
  std::size_t count = 0;
  for (std::size_t out = 0; out < second.result.size(); ++out) {
    count += kept_output<Alg, V, X>(out) ? 0 : 1;
  }
  return count;
}

// The position of each blade of the second plan's result among the dropped
// blades, in the order of that result; kept blades have none.
template <metric_like Alg, blade_set V, blade_set X>
[[nodiscard]] consteval auto make_dropped_places() {
  constexpr const auto& second = sandwich_second_plan<Alg, V, X>;
  std::array<std::size_t, second.result.size()> places{};
  std::size_t k = 0;
  for (std::size_t out = 0; out < second.result.size(); ++out) {
    if (!kept_output<Alg, V, X>(out)) {
      places[out] = k;
      ++k;
    }
  }
  return places;
}

// The sign that the second stage of the transform folds into each term: the
// antireverse's sign of V's blade, and -1 more for an odd V.
template <metric_like Alg, blade_set V>
[[nodiscard]] consteval coefficient_type second_stage_sign(std::size_t b) {
  constexpr std::size_t n =
      std::remove_cvref_t<decltype(Alg::value)>::generator_count;
  const coefficient_type odd = antigrade_parity<Alg, V> == Parity::odd ? -1 : 1;
  return odd * antireverse_sign(V.blades[b].mask, n);
}

// The terms of the second stage, (V ⟇ x) ⟇ V̰ with the odd sign, that reach
// the blades of X, when Kept, moved to their places in X and multiplied by
// the blade's orientation there, since c e_C = (c s) (s e_C); or the terms
// that reach the other blades, when not, at their places among the dropped
// blades.
template <metric_like Alg, blade_set V, blade_set X, bool Kept>
[[nodiscard]] consteval auto make_second_kernel() {
  constexpr const auto& second = sandwich_second_plan<Alg, V, X>;
  constexpr auto dropped_places = make_dropped_places<Alg, V, X>();
  std::array<kernel_term, second_term_count<Alg, V, X, Kept>()> terms{};
  std::size_t k = 0;
  for (const typed_term& t : second.terms) {
    if (kept_output<Alg, V, X>(t.out) != Kept) {
      continue;
    }
    const std::size_t out =
        Kept ? position_of(X, second.result.blades[t.out].mask)
             : dropped_places[t.out];
    const coefficient_type orientation = Kept ? X.blades[out].sign : 1;
    terms[k] = {
        .a = t.a,
        .b = t.b,
        .out = out,
        .coefficient =
            orientation * second_stage_sign<Alg, V>(t.b) * t.coefficient,
        .first = false,
    };
    ++k;
  }
  if constexpr (Kept) {
    mark_first_terms<X.size()>(terms);
  } else {
    mark_first_terms<dropped_blade_count<Alg, V, X>()>(terms);
  }
  return terms;
}

// The blades of the middle layout that the kept terms of the second stage
// read.
template <metric_like Alg, blade_set V, blade_set X>
[[nodiscard]] consteval auto make_middle_read() {
  constexpr const auto& second = sandwich_second_plan<Alg, V, X>;
  std::array<bool, sandwich_middle_layout<Alg, V, X>.size()> read{};
  for (const typed_term& t : second.terms) {
    if (kept_output<Alg, V, X>(t.out)) {
      read[t.a] = true;
    }
  }
  return read;
}

// The number of terms of the first plan that the first stage keeps: every
// term, when All, or those whose output the kept terms of the second stage
// read.
template <metric_like Alg, blade_set V, blade_set X, bool All>
[[nodiscard]] consteval std::size_t first_term_count() {
  constexpr auto read = make_middle_read<Alg, V, X>();
  std::size_t count = 0;
  for (const typed_term& t : sandwich_first_plan<Alg, V, X>.terms) {
    count += All || read[t.out] ? 1 : 0;
  }
  return count;
}

// The terms of the first stage, V ⟇ x̌ for an odd V and V ⟇ x for an even
// one, the involution's sign of x's blade folded into each term: every term
// of the first plan, when All, or those the kept terms of the second stage
// read, in the plan's order. A term's output is its place in the middle
// layout, so both selections give each blade they reach the same sum in the
// same order.
template <metric_like Alg, blade_set V, blade_set X, bool All>
[[nodiscard]] consteval auto make_first_kernel() {
  constexpr std::size_t n =
      std::remove_cvref_t<decltype(Alg::value)>::generator_count;
  constexpr bool odd = antigrade_parity<Alg, V> == Parity::odd;
  constexpr const auto& first = sandwich_first_plan<Alg, V, X>;
  constexpr auto read = make_middle_read<Alg, V, X>();
  std::array<kernel_term, first_term_count<Alg, V, X, All>()> terms{};
  std::size_t k = 0;
  for (const typed_term& t : first.terms) {
    if (!All && !read[t.out]) {
      continue;
    }
    const coefficient_type sign =
        odd && antigrade(X.blades[t.b].mask, n) % 2 != 0 ? -1 : 1;
    terms[k] = {
        .a = t.a,
        .b = t.b,
        .out = t.out,
        .coefficient = sign * t.coefficient,
        .first = false,
    };
    ++k;
  }
  mark_first_terms<sandwich_middle_layout<Alg, V, X>.size()>(terms);
  return terms;
}

// Each kernel is evaluated once for each pair of layouts a program uses. As
// for coefficients_exact.
// NOLINTBEGIN(modernize-avoid-c-style-cast)
template <metric_like Alg, blade_set V, blade_set X, bool All>
inline constexpr auto first_kernel = make_first_kernel<Alg, V, X, All>();

template <metric_like Alg, blade_set V, blade_set X, bool Kept>
inline constexpr auto second_kernel = make_second_kernel<Alg, V, X, Kept>();
// NOLINTEND(modernize-avoid-c-style-cast)

// The middle coefficients of a transform, by the first kernel of All.
template <metric_like Alg, blade_set V, blade_set X, bool All, class T>
[[nodiscard]] constexpr auto
transform_middle(const std::array<T, V.size()>& v,
                 const std::array<T, X.size()>& x) {
  constexpr const auto& kernel = first_kernel<Alg, V, X, All>;
  return run_product<kernel, sandwich_middle_layout<Alg, V, X>.size()>(
      v, x, std::make_index_sequence<kernel.size()>{});
}

// The coefficients that the second kernel of Kept gives from the middle
// coefficients, on X when Kept and on the dropped blades when not.
template <metric_like Alg, blade_set V, blade_set X, bool Kept, class T,
          std::size_t M>
[[nodiscard]] constexpr auto transform_outer(const std::array<T, M>& middle,
                                             const std::array<T, V.size()>& v) {
  constexpr const auto& kernel = second_kernel<Alg, V, X, Kept>;
  constexpr std::size_t size =
      Kept ? X.size() : dropped_blade_count<Alg, V, X>();
  return run_product<kernel, size>(middle, v,
                                   std::make_index_sequence<kernel.size()>{});
}

// Checks that every coefficient a transform drops is zero, one comparison
// each. The message is on the line of the check, which a failed constant
// evaluation shows.
template <class T, std::size_t D>
constexpr void check_transform_dropped(const std::array<T, D>& dropped) {
  for (const T& d : dropped) {
    GALOOT_ASSERT(d == T{}, "transform: v does not act as a versor on x");
  }
}

/// A scalar whose rounding transform checks: an IEEE 754 binary
/// floating-point type that rounds to nearest, as its numeric_limits say,
/// such as float and double.
template <class T>
concept rounding_checked_scalar =
    ring_scalar<T> && std::numeric_limits<T>::is_iec559 &&
    std::numeric_limits<T>::round_style == std::round_to_nearest;

// The count of roundings of each of the R coefficients of a kernel's
// result, from the counts of its operands' coefficients x and y. Each term
// rounds its product after the counts of both factors, and rounds again when
// its coefficient is not +1 or -1, since it then multiplies by it; the first
// term of a result is assigned, and each later one is added or subtracted,
// which rounds after the larger of the two counts.
template <std::size_t R, std::size_t N, std::size_t KX, std::size_t KY>
[[nodiscard]] consteval std::array<int, R>
kernel_roundings(const std::array<kernel_term, N>& kernel,
                 const std::array<int, KX>& x, const std::array<int, KY>& y) {
  std::array<int, R> out{};
  for (const kernel_term& t : kernel) {
    int term = x[t.a] + y[t.b] + 1;
    if (t.coefficient != 1 && t.coefficient != -1) {
      ++term;
    }
    if (t.first) {
      out[t.out] = term;
    } else {
      out[t.out] = (out[t.out] < term ? term : out[t.out]) + 1;
    }
  }
  return out;
}

/// The count of roundings K of each coefficient that a transform of x on X
/// by v on V drops, in the order of the dropped blades: the order of the
/// second antiproduct's result, without X's blades. The inputs count none.
// As for coefficients_exact.
// NOLINTBEGIN(modernize-avoid-c-style-cast)
template <metric_like Alg, blade_set V, blade_set X>
inline constexpr std::array<int, dropped_blade_count<Alg, V, X>()>
    transform_dropped_roundings =
        kernel_roundings<dropped_blade_count<Alg, V, X>()>(
            second_kernel<Alg, V, X, false>,
            kernel_roundings<sandwich_middle_layout<Alg, V, X>.size()>(
                first_kernel<Alg, V, X, true>, std::array<int, V.size()>{},
                std::array<int, X.size()>{}),
            std::array<int, V.size()>{});
// NOLINTEND(modernize-avoid-c-style-cast)

// The tolerance of a dropped coefficient d with K roundings is
// fl(fl(gamma_(K+m)) P), where gamma_k = k u / (1 - k u), u is the unit
// roundoff, m this margin, and P the padded magnitude computed below.
// Derivation (Higham, Accuracy and Stability of Numerical Algorithms, 2nd
// ed., SIAM, 2002: the model (2.8), with fl(a op b) = (a op b)(1 + δ) + η,
// |δ| <= u, |η| <= λu, δη = 0, and η = 0 for + and -, pp. 56-57; Lemma 3.1
// and Lemma 3.3, pp. 63 and 67):
// - Run on |v| and |x| with |c| for each coefficient c, and with the
//   smallest normal number λ added to every rounded product, the kernels
//   give the exact padded magnitude P. By induction over the operations,
//   |d - e| <= gamma_K P for the exact value e: a sum adds no η, and a
//   product's |η| <= λu <= gamma_K λ, which its padding covers. For a versor
//   e = 0.
// - The computed magnitude rounds two times per product (the product and
//   the padding's addition) and once per sum, at most 2K times on a path,
//   each on nonnegative numbers at least λ (1 - u)^2, so it is at least
//   (1 - u)^(2K) P; and fl(gamma_k) >= (1 - u) gamma_k, since k u and
//   1 - k u are exact.
// - (1 - u)^(2K+1) gamma_(K+1) >= gamma_K when (K + 1)(2K + 1) u < 1, by
//   multiplying out, so margin 1 makes the exact product of the two computed
//   factors at least gamma_K P >= |d|, and rounding to nearest, which is
//   monotone, keeps |d| <= the computed tolerance. Margin 0 fails this,
//   since both factors may round down.
inline constexpr int transform_rounding_margin = 1;

// The magnitudes of the R coefficients of a kernel's result, from the
// magnitudes x and y of its operands' coefficients: each term is the product
// of the two, plus λ, then, when its coefficient c is not +1 or -1, |c|
// times that, plus λ, and the terms are summed in the kernel's order. λ,
// the smallest normal number of T, covers the absolute error of a product
// that underflows, as derived above.
template <class T, std::size_t R, std::size_t N, std::size_t KX, std::size_t KY>
[[nodiscard]] constexpr std::array<T, R>
padded_magnitude(const std::array<kernel_term, N>& kernel,
                 const std::array<T, KX>& x, const std::array<T, KY>& y) {
  constexpr T padding = std::numeric_limits<T>::min();
  std::array<T, R> out{};
  for (const kernel_term& t : kernel) {
    T term = static_cast<T>(static_cast<T>(x[t.a] * y[t.b]) + padding);
    if (t.coefficient != 1 && t.coefficient != -1) {
      const auto c =
          static_cast<T>(t.coefficient < 0 ? -t.coefficient : t.coefficient);
      term = static_cast<T>(static_cast<T>(c * term) + padding);
    }
    out[t.out] = t.first ? term : static_cast<T>(out[t.out] + term);
  }
  return out;
}

template <class T, std::size_t K>
[[nodiscard]] constexpr std::array<T, K>
absolute_values(const std::array<T, K>& a) {
  std::array<T, K> out{};
  for (std::size_t i = 0; i < K; ++i) {
    out[i] = a[i] < T{} ? static_cast<T>(-a[i]) : a[i];
  }
  return out;
}

/// The tolerance of each coefficient that a transform of x on X by v on V
/// drops, in the order of transform_dropped_roundings: fl(fl(gamma_(K+1)) P),
/// with K the coefficient's count of roundings and P its padded magnitude,
/// so that a coefficient a versor drops, zero but for rounding, is within it.
template <metric_like Alg, blade_set V, blade_set X, rounding_checked_scalar T>
[[nodiscard]] constexpr std::array<T, dropped_blade_count<Alg, V, X>()>
transform_dropped_tolerances(const std::array<T, V.size()>& v,
                             const std::array<T, X.size()>& x) {
  constexpr std::size_t dropped = dropped_blade_count<Alg, V, X>();
  constexpr const auto& roundings = transform_dropped_roundings<Alg, V, X>;
  constexpr T u = std::numeric_limits<T>::epsilon() / 2;
  constexpr bool margin_suffices = [] {
    // NOLINTNEXTLINE(readability-use-anyofallof): avoids <algorithm> here.
    for (const int k : roundings) {
      const auto count = static_cast<std::int64_t>(k);
      if (static_cast<T>((count + 1) * ((2 * count) + 1)) * u >= T{1}) {
        return false;
      }
    }
    return true;
  }();
  static_assert(margin_suffices,
                "transform: a dropped coefficient has too many roundings "
                "for the margin of the rounding check");

  const auto abs_v = absolute_values(v);
  const auto middle =
      padded_magnitude<T, sandwich_middle_layout<Alg, V, X>.size()>(
          first_kernel<Alg, V, X, true>, abs_v, absolute_values(x));
  const auto magnitude = padded_magnitude<T, dropped>(
      second_kernel<Alg, V, X, false>, middle, abs_v);

  std::array<T, dropped> tolerances{};
  for (std::size_t j = 0; j < dropped; ++j) {
    const auto k = static_cast<T>(roundings[j] + transform_rounding_margin);
    const T gamma = static_cast<T>((k * u) / (T{1} - (k * u)));
    tolerances[j] = static_cast<T>(gamma * magnitude[j]);
  }
  return tolerances;
}

// Checks that every coefficient a transform drops is within its tolerance,
// one comparison each, and that the tolerance is finite: a NaN or an
// infinite coefficient, or one whose magnitude overflows, cannot be
// verified, and fails. The message is on the line of the check, which a
// failed constant evaluation shows.
template <rounding_checked_scalar T, std::size_t D>
constexpr void check_transform_rounded(const std::array<T, D>& dropped,
                                       const std::array<T, D>& tolerances) {
  for (std::size_t j = 0; j < D; ++j) {
    const T size = dropped[j] < T{} ? static_cast<T>(-dropped[j]) : dropped[j];
    const bool within =
        size <= tolerances[j] && tolerances[j] <= std::numeric_limits<T>::max();
    GALOOT_ASSERT(within, "transform: v is not a versor on x within rounding");
  }
}

} // namespace detail

// clang-tidy reads the operation, once substituted, as a C-style cast of its
// value, which the source does not have.
// NOLINTBEGIN(modernize-avoid-c-style-cast)

/// The layout of compose(q2, q1) for q2 on the layout A and q1 on the layout
/// B in the algebra Alg: the blades of the geometric antiproduct of A and B,
/// each with orientation +1, in order of grade and then of index list. Two
/// rigid(3) motors give 1, e12, e13, e14, e23, e24, e34, and e1234.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto compose_layout = geometric_antiproduct_layout<Alg, A, B>;

// NOLINTEND(modernize-avoid-c-style-cast)

/// The composition of two versors, q2 ⟇ q1: the versor that applies q1 first
/// and then q2, so that transform(x, compose(q2, q1)) is
/// transform(transform(x, q1), q2). The composition of two motors is a
/// motor, of a motor and a flector a flector, and of two flectors a motor,
/// since antigrades add modulo 2 under the antiproduct.
///
/// Requires: q2 and q1 have the same algebra and the same scalar type; each
/// has one parity, every blade of even antigrade or every blade of odd
/// antigrade; and the scalar holds every exact coefficient of the
/// antiproduct where its numeric_limits can tell, as for
/// geometric_antiproduct. Otherwise the call is rejected at compile time
/// with a message that names compose. For the antiproduct of anything, write
/// geometric_antiproduct or %.
/// Guarantees: on compose_layout<Alg, A, B>. Never checks: it drops nothing.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, compose_layout<Alg, A, B>, T>
compose(const mv<Alg, A, T>& q2, const mv<Alg, B, T>& q1) {
  constexpr bool each_operand_has_one_parity =
      detail::antigrade_parity<Alg, A> != detail::Parity::mixed &&
      detail::antigrade_parity<Alg, B> != detail::Parity::mixed;
  static_assert(each_operand_has_one_parity,
                "compose: an operand holds blades of both parities, even and "
                "odd antigrade; compose versors that are each even or odd");
  constexpr bool scalar_holds_every_coefficient_of_the_composition =
      detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,
                                 Alg, A, B, T>;
  static_assert(scalar_holds_every_coefficient_of_the_composition,
                "compose: the scalar type cannot hold one of the "
                "composition's exact coefficients; use a scalar type that "
                "holds larger integers exactly");
  if constexpr (each_operand_has_one_parity &&
                scalar_holds_every_coefficient_of_the_composition) {
    return detail::apply<detail::BinaryOperation::geometric_antiproduct,
                         compose_layout<Alg, A, B>>(q2, q1);
  } else {
    return {};
  }
}

/// The transform of x by the versor v: v ⟇ x ⟇ antireverse(v) when v is
/// even, every blade of even antigrade, and -v ⟇ x̌ ⟇ antireverse(v) when v
/// is odd, where x̌ multiplies a blade of antigrade m by (-1)^m; the header's
/// comment gives the forms for even and odd n. The result is on x's layout,
/// with x's orientations, so transform(point, motor) is a point; the part of
/// the antiproduct on blades x lacks is dropped. An empty v gives zero. The
/// coefficients are computed by straight-line code from the terms of the two
/// antiproducts that reach x's blades, with the signs of the antireverse, of
/// the involution, and of an odd v folded into the terms' coefficients;
/// under IEEE 754 a negation is exact, so this is the value of the
/// antiproducts written out, bit for bit. transform does not unitize: for
/// v ⟇ antireverse(v) = λ𝟙 it multiplies a point's weight by λ.
///
/// Requires: x and v have the same algebra and the same scalar type; v has
/// one parity, every blade of even antigrade or every blade of odd antigrade;
/// and the scalar holds every exact coefficient of the two antiproducts the
/// transform composes, v ⟇ x and that with v, where its numeric_limits can
/// tell, as for geometric_antiproduct. Otherwise the call is rejected at
/// compile time with a message that names transform.
///
/// Checks, for an exact_scalar T with ==, that every coefficient it drops is
/// zero, which holds when v acts as a versor on x: in rigid(3) for any v of
/// one parity and x a point, a line, a plane, a motor, or a flector, and in
/// any algebra for a versor and an x of whole grades. A v that is not a
/// versor may leave the blades of x, as v = 1 + 𝟙 does for the scalar in
/// R(4,0,0). The message, "transform: v does not act as a versor on x",
/// also comes for a versor when x's layout holds only some of the blades of
/// a grade: in R(4,0,0), v = 1 + e12 + 2 e13 + 3 e14, with
/// v ⟇ antireverse(v) = 15𝟙, moves x on e1 alone onto e2, e3, and e4 as
/// well; give x every blade of its grades. The check is a GALOOT_ASSERT in
/// every constant evaluation, which then fails, and at run time when
/// GALOOT_CHECKING is 1, which then aborts with the message; it compares
/// each dropped coefficient with zero once, after computing the terms that
/// reach the dropped blades. With GALOOT_CHECKING 0 those terms are not
/// computed at run time.
///
/// For an IEEE 754 binary floating-point scalar that rounds to nearest, as
/// its numeric_limits say (float and double), it checks in the same modes
/// that every dropped coefficient d is within the forward error bound of
/// this operation: |d| <= fl(fl(gamma_(K+1)) P), where gamma_k =
/// k u / (1 - k u) for the unit roundoff u, K is the number of roundings on
/// d's path, and P the same computation on the absolute values of v and x
/// with the absolute values of the terms' coefficients, the smallest normal
/// number added after each product to cover a product that underflows. A
/// coefficient that a versor drops is zero but for rounding, and within
/// that bound. The bound takes v and x as exact: a v with rounding error of
/// its own, from its construction, is only nearly a versor, and where what
/// transform drops is zero only because v is a versor, as for the
/// antiscalar part of a conformal(3) vector, that error can exceed the
/// bound and the check aborts. In rigid(3), for the five layouts above,
/// what transform drops is zero for every v of one parity, so it cannot.
/// A NaN or infinite dropped coefficient, or one whose P overflows, fails
/// the check. On failure the message is "transform: v is not a versor on x
/// within rounding". The bound assumes that each operation rounds once to
/// the scalar's own precision, to nearest, with gradual underflow; a build
/// that flushes subnormal numbers to zero is outside it, and a checking
/// build under -ffast-math does not compile. Other scalars that are not
/// exact, such as std::complex<double>, are not checked.
/// Guarantees: on x's layout; with an exact scalar the result is exact, and
/// the result is the same with and without the check.
template <metric_like Alg, blade_set X, blade_set V, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, X, T> transform(const mv<Alg, X, T>& x,
                                                const mv<Alg, V, T>& v) {
  constexpr bool versor_has_one_parity =
      detail::antigrade_parity<Alg, V> != detail::Parity::mixed;
  static_assert(versor_has_one_parity,
                "transform: the versor holds blades of both parities, even "
                "and odd antigrade; a versor is even, as a motor, or odd, as "
                "a flector");
  constexpr bool scalar_holds_every_coefficient_of_the_transform =
      detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,
                                 Alg, V, X, T> &&
      detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,
                                 Alg, detail::sandwich_middle_layout<Alg, V, X>,
                                 V, T>;
  static_assert(scalar_holds_every_coefficient_of_the_transform,
                "transform: the scalar type cannot hold one of the "
                "transform's exact coefficients; use a scalar type that holds "
                "larger integers exactly");

  mv<Alg, X, T> out;
  if constexpr (versor_has_one_parity &&
                scalar_holds_every_coefficient_of_the_transform) {
    if constexpr ((exact_scalar<T> && std::equality_comparable<T>) ||
                  detail::rounding_checked_scalar<T>) {
      if (detail::checking_now()) {
        const auto middle = detail::transform_middle<Alg, V, X, true>(
            v.coefficients, x.coefficients);
        out.coefficients =
            detail::transform_outer<Alg, V, X, true>(middle, v.coefficients);
        const auto dropped =
            detail::transform_outer<Alg, V, X, false>(middle, v.coefficients);
        if constexpr (detail::rounding_checked_scalar<T>) {
          detail::check_transform_rounded(
              dropped, detail::transform_dropped_tolerances<Alg, V, X>(
                           v.coefficients, x.coefficients));
        } else {
          detail::check_transform_dropped(dropped);
        }
        return out;
      }
    }
    out.coefficients = detail::transform_outer<Alg, V, X, true>(
        detail::transform_middle<Alg, V, X, false>(v.coefficients,
                                                   x.coefficients),
        v.coefficients);
  }
  return out;
}

/// Rejects a transform whose operands are not in one algebra with one scalar
/// type, with one error that names the cause. Because it exists, a
/// requires-expression that asks whether such a call is valid is told yes;
/// ask the operand types instead.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> transform(const X& /*x*/, const Y& /*v*/) {
  return detail::reject_transform_operands<
      detail::TransformSpelling::transform_function, X, Y>();
}

/// Rejects a composition whose operands are not in one algebra with one
/// scalar type, with one error that names the cause. Because it exists, a
/// requires-expression that asks whether such a call is valid is told yes;
/// ask the operand types instead.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> compose(const X& /*q2*/, const Y& /*q1*/) {
  return detail::reject_transform_operands<
      detail::TransformSpelling::compose_function, X, Y>();
}

} // namespace galoot::core

#endif // GALOOT_CORE_TRANSFORM_HPP
