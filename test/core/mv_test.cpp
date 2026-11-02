// Tests of galoot/core/mv.hpp: the multivector type, its scalar concepts,
// its construction, its equality, and the checks of its layout.
//
// The expected values come from what the header states, written out here:
// a value built from a list of numbers holds them in the order given, a
// default value holds T{} in every place, two values are equal when every
// coefficient is, and a scalar concept accepts exactly the types that meet
// its listed requirements. The scalar types are the five a multivector must
// serve: std::int64_t and the test rational, which are exact; float and
// double, which round; and std::complex<double>, whose members are private.
#include <array>
#include <complex>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <new> // IWYU pragma: keep
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

#include <doctest/doctest.h>

#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
using test_support::rational;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
// R(15, 0, 1): sixteen generators, more than names can reach.
struct r15_0_1 {
  static constexpr auto value = core::R<15, 0, 1>();
};
template <std::size_t N>
struct euclidean {
  static constexpr auto value = core::R<N, 0, 0>();
};

// The objects of rigid(3), named as the published formulas name them.
inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
inline constexpr auto motor_layout =
    core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
inline constexpr auto scalar_layout = core::blades<rga3, "1">;
inline constexpr auto conformal_vector_layout =
    core::blades<cga3, "e1 e2 e3 e4 e5">;

// {1, e1, e16, e1 ^ e16}, given by mask because names have one digit per
// index: bit i - 1 stands for e_i.
inline constexpr auto sparse_blades = std::to_array<core::oriented_blade>({
    {.mask = 0, .sign = 1},
    {.mask = 0b1, .sign = 1},
    {.mask = core::mask_t{1} << 15U, .sign = 1},
    {.mask = 0b1U | (core::mask_t{1} << 15U), .sign = 1},
});
inline constexpr core::blade_set<4> sparse_layout{sparse_blades};
inline constexpr core::blade_set<0> empty_layout{};

template <class T>
using Point = core::mv<rga3, point_layout, T>;
template <class T>
using Line = core::mv<rga3, line_layout, T>;
template <class T>
using Plane = core::mv<rga3, plane_layout, T>;
template <class T>
using Motor = core::mv<rga3, motor_layout, T>;
template <class T>
using Scalar = core::mv<rga3, scalar_layout, T>;
template <class T>
using ConformalVector = core::mv<cga3, conformal_vector_layout, T>;
template <class T>
using Sparse = core::mv<r15_0_1, sparse_layout, T>;
template <class T>
using Empty = core::mv<rga3, empty_layout, T>;

// The value i as a scalar of type T; every scalar here converts exactly
// from a small int.
template <class T>
constexpr T scalar_of(int i) {
  return T(i);
}

// The coefficients 1, 2, ..., K as a scalar of type T.
template <class T, std::size_t K>
constexpr std::array<T, K> counting() {
  std::array<T, K> values{};
  for (std::size_t i = 0; i < K; ++i) {
    values[i] = scalar_of<T>(static_cast<int>(i) + 1);
  }
  return values;
}

// Whether every coefficient of x is T{}.
template <class Type>
constexpr bool all_zero(const Type& x) {
  using T = Type::scalar_type;
  // NOLINTNEXTLINE(readability-use-anyofallof): one loop reads plainly.
  for (const T& c : x.coefficients) {
    if (!(c == T{})) {
      return false;
    }
  }
  return true;
}

// A value default-initialized, as in `Line l;`, inside a constant
// evaluation, which rejects reading an indeterminate value.
template <class Type>
constexpr bool default_initialized_is_zero() {
  // NOLINTNEXTLINE(cppcoreguidelines-init-variables): default on purpose.
  Type x;
  return all_zero(x);
}

// A value default-initialized at run time over storage filled with a
// nonzero pattern, so that a coefficient left uninitialized shows it.
template <class Type>
bool default_initialized_over_a_pattern_is_zero() {
  alignas(Type) std::array<unsigned char, sizeof(Type)> storage{};
  storage.fill(0xA5);
  // Placement new into storage on the stack, which owns nothing to free.
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  const Type* x = ::new (static_cast<void*>(storage.data())) Type;
  return all_zero(*x);
}

// The properties a multivector type has whatever its scalar: it is not an
// aggregate, it is trivially copyable and standard-layout, and a nonempty
// one holds exactly its coefficients.
template <class Type, class T>
consteval bool has_value_properties() {
  static_assert(!std::is_aggregate_v<Type>);
  static_assert(std::is_trivially_copyable_v<Type>);
  static_assert(std::is_standard_layout_v<Type>);
  static_assert(Type::component_count == 0 ||
                sizeof(Type) == Type::component_count * sizeof(T));
  return true;
}

// A value used as a template argument, read back.
template <auto Value>
constexpr auto as_template_argument = Value;

} // namespace

TYPE_TO_STRING_AS("std::int64_t", std::int64_t);
TYPE_TO_STRING_AS("rational", rational);
TYPE_TO_STRING_AS("float", float);
TYPE_TO_STRING_AS("double", double);
TYPE_TO_STRING_AS("std::complex<double>", std::complex<double>);

// Reference: contract: a multivector is not an aggregate, is trivially
//   copyable and standard-layout when its scalar is, holds K coefficients in
//   K times the size of its scalar for a nonempty layout, and neither a
//   one-blade multivector nor a larger one converts implicitly from its
//   scalar, and it names its scalar and algebra types, for the
//   point, line, plane, and motor of rigid(3), a conformal(3) vector, a
//   sparse set in sixteen generators, and an empty set
TEST_CASE_TEMPLATE("mv is a trivially copyable value with exactly its "
                   "coefficients",
                   T, std::int64_t, rational, float, double,
                   std::complex<double>) {
  static_assert(has_value_properties<Point<T>, T>());
  static_assert(has_value_properties<Line<T>, T>());
  static_assert(has_value_properties<Plane<T>, T>());
  static_assert(has_value_properties<Motor<T>, T>());
  static_assert(has_value_properties<Scalar<T>, T>());
  static_assert(has_value_properties<ConformalVector<T>, T>());
  static_assert(has_value_properties<Sparse<T>, T>());
  static_assert(has_value_properties<Empty<T>, T>());
  static_assert(Empty<T>{}.coefficients.empty());

  static_assert(!std::is_convertible_v<T, Scalar<T>>);
  static_assert(std::is_constructible_v<Scalar<T>, T>);
  static_assert(!std::is_convertible_v<T, Line<T>>);
  CHECK_FALSE(std::is_convertible_v<T, Scalar<T>>);
  CHECK_FALSE(std::is_convertible_v<T, Line<T>>);

  static_assert(std::same_as<typename Line<T>::scalar_type, T>);
  static_assert(std::same_as<typename Line<T>::algebra_type, rga3>);
  CHECK(Line<T>::component_count == 6);
  CHECK(Motor<T>::component_count == 8);
}

// Reference: contract: the size is the number of components times the size
//   of the scalar, 6 x 8 = 48 bytes for a line over double, 6 x 4 = 24 over
//   float, and 8 x 8 = 64 for a motor over std::int64_t, and a multivector
//   over double or the test rational, both structural, is a template argument
//   that reads back equal
TEST_CASE("mv has the sizes and template arguments of its coefficients") {
  static_assert(sizeof(Line<double>) == 48);
  static_assert(sizeof(Line<float>) == 24);
  static_assert(sizeof(Motor<std::int64_t>) == 64);

  constexpr Line<double> line{1, 2, 3, 4, 5, 6};
  static_assert(as_template_argument<line> == line);
  constexpr Point<rational> point{rational{1, 2}, rational{2}, rational{3},
                                  rational{1}};
  static_assert(as_template_argument<point> == point);
  static_assert(as_template_argument<Empty<double>{}> == Empty<double>{});
  static_assert(
      as_template_argument<Sparse<double>{1, 2, 3, 4}>.coefficients[3] == 4.0);
  CHECK(as_template_argument<line>.coefficients[5] == 6.0);
}

// Reference: contract: a default-constructed multivector, whether
//   default-initialized or value-initialized with empty braces, holds T{} in
//   every place, at compile time and at run time
TEST_CASE_TEMPLATE("mv is zero by default", T, std::int64_t, rational, float,
                   double, std::complex<double>) {
  static_assert(default_initialized_is_zero<Line<T>>());
  static_assert(default_initialized_is_zero<Motor<T>>());
  static_assert(default_initialized_is_zero<Sparse<T>>());
  static_assert(all_zero(Line<T>{}));
  static_assert(all_zero(Motor<T>{}));

  CHECK(default_initialized_over_a_pattern_is_zero<Line<T>>());
  CHECK(default_initialized_over_a_pattern_is_zero<Motor<T>>());
  CHECK(default_initialized_over_a_pattern_is_zero<Sparse<T>>());
  CHECK(all_zero(Line<T>{}));
  CHECK(all_zero(Motor<T>{}));
}

namespace {

// Constant evaluations that each write into one place of a zero plane of
// rationals. GCC 16.2.1 was seen to let such a write change what a later zero
// value of the same type holds at run time, when every place shares one
// initializer: writes at places 2 and 3 did, from that place on, and writes
// at places 0 and 1 did not. Every place is written, so the test does not
// depend on which places trigger it. Each is a constexpr variable rather than
// a static_assert, so that the evaluation takes place whatever becomes of
// the assertions.
constexpr bool plane_written_at(std::size_t place) {
  Plane<rational> plane{};
  plane.coefficients.at(place) = rational{7};
  return plane.coefficients.at(place) == rational{7};
}
constexpr bool plane_written = plane_written_at(0) && plane_written_at(1) &&
                               plane_written_at(2) && plane_written_at(3);

} // namespace

// Reference: contract: a multivector is zero by default also after a
//   constant evaluation in the same program wrote into a zero value of its
//   type, here a plane over the test rational, whose zero 0/1 has a nonzero
//   denominator
TEST_CASE("mv stays zero by default after a constant evaluation wrote into "
          "one") {
  CHECK(plane_written);
  const Plane<rational> plane{};
  CHECK(all_zero(plane));
  CHECK(default_initialized_over_a_pattern_is_zero<Plane<rational>>());
  for (const rational& c : plane.coefficients) {
    CHECK(c.denominator == 1);
  }
}

// Reference: contract: a multivector is constructed from exactly one value
//   per component, which lands in the place of that component in the order
//   the layout lists them, so Motor{1, ..., 8} holds 1 to 8 at positions 0
//   to 7, at compile time and, from values read through volatile, at run
//   time
TEST_CASE_TEMPLATE("mv takes one value per component in layout order", T,
                   std::int64_t, rational, float, double,
                   std::complex<double>) {
  constexpr auto k = [](int i) { return scalar_of<T>(i); };
  static_assert(
      Motor<T>{k(1), k(2), k(3), k(4), k(5), k(6), k(7), k(8)}.coefficients ==
      counting<T, 8>());
  static_assert(Line<T>{k(1), k(2), k(3), k(4), k(5), k(6)}.coefficients ==
                counting<T, 6>());
  static_assert(Scalar<T>{k(7)}.coefficients[0] == k(7));

  volatile int one = 1;
  const auto v = [&one](int i) { return scalar_of<T>(i * one); };
  const Motor<T> motor{v(1), v(2), v(3), v(4), v(5), v(6), v(7), v(8)};
  CHECK(motor.coefficients == counting<T, 8>());
  const Line<T> line{v(1), v(2), v(3), v(4), v(5), v(6)};
  CHECK(line.coefficients == counting<T, 6>());
}

// Reference: contract: values convert to the scalar as in brace
//   initialization, so integer literals give a floating layout the same
//   values as floating literals, and a float layout takes float literals
TEST_CASE("mv takes integer literals for a floating scalar") {
  static_assert(Line<double>{1, 2, 3, 4, 5, 6} ==
                Line<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0});
  constexpr Line<float> halves{0.5F, 1.0F, 1.5F, 2.0F, 2.5F, 3.0F};
  static_assert(halves.coefficients ==
                std::array<float, 6>{0.5F, 1.0F, 1.5F, 2.0F, 2.5F, 3.0F});
  static_assert(Point<double>{0, 0, 1, 1}.coefficients[2] == 1.0);

  const volatile double half = 0.5;
  const Line<double> line{half, 1, 1.5, 2, 2.5, 3};
  CHECK(line.coefficients ==
        std::array<double, 6>{0.5, 1.0, 1.5, 2.0, 2.5, 3.0});
}

// Reference: contract: two multivectors of one type are equal exactly when
//   every coefficient is, so changing any one place of a line or a motor
//   makes them unequal
TEST_CASE_TEMPLATE("mv equality compares every coefficient", T, std::int64_t,
                   rational, float, double, std::complex<double>) {
  const auto differs_at_each_place = []<class Type>(const Type& base) {
    int unequal = 0;
    for (std::size_t i = 0; i < Type::component_count; ++i) {
      Type changed = base;
      changed.coefficients.at(i) = scalar_of<T>(100);
      CAPTURE(i);
      CHECK(changed != base);
      CHECK_FALSE(changed == base);
      unequal += changed != base ? 1 : 0;
    }
    return unequal;
  };

  const auto k = [](int i) { return scalar_of<T>(i); };
  const Line<T> line{k(1), k(2), k(3), k(4), k(5), k(6)};
  const Motor<T> motor{k(1), k(2), k(3), k(4), k(5), k(6), k(7), k(8)};
  CHECK(line == Line<T>{k(1), k(2), k(3), k(4), k(5), k(6)});
  CHECK(motor == Motor<T>{k(1), k(2), k(3), k(4), k(5), k(6), k(7), k(8)});
  CHECK(differs_at_each_place(line) == 6);
  CHECK(differs_at_each_place(motor) == 8);
  CHECK(Empty<T>{} == Empty<T>{});
}

// Reference: contract: with a floating scalar, equality is IEEE equality of
//   each coefficient, so -0.0 equals +0.0 and a NaN equals nothing, itself
//   included
TEST_CASE("mv equality over double is IEEE equality") {
  constexpr double nan = std::numeric_limits<double>::quiet_NaN();
  static_assert(Point<double>{-0.0, 0, 0, 1} == Point<double>{0.0, 0, 0, 1});
  static_assert(Point<double>{nan, 0, 0, 1} != Point<double>{nan, 0, 0, 1});

  const volatile double zero = 0.0;
  const Point<double> negative{-zero, 0, 0, 1};
  const Point<double> positive{zero, 0, 0, 1};
  CHECK(negative == positive);
  const volatile double nan_input = nan;
  const Point<double> not_a_number{nan_input, 0, 0, 1};
  const Point<double> copy = not_a_number;
  CHECK(not_a_number != copy);
}

// Reference: contract: multivectors of different layouts, algebras, or
//   scalars are different types and have no ==, even when they hold the
//   same number of coefficients of one scalar
TEST_CASE("mv compares only with its own type") {
  using Bivectors = core::mv<rga3, core::of_grade<4, 2>, double>;
  using FourEuclidean =
      core::mv<euclidean<4>, core::blades<euclidean<4>, "e1 e2 e3 e4">, double>;
  static_assert(std::equality_comparable<Line<double>>);
  static_assert(!std::equality_comparable_with<Line<double>, Bivectors>);
  static_assert(!std::equality_comparable_with<Point<double>, FourEuclidean>);
  static_assert(!std::equality_comparable_with<Line<double>, Line<float>>);
  CHECK(Bivectors{} == Bivectors{});
}

namespace {

// Types that meet every requirement of ring_scalar but one. The concepts
// read only the types of the operations, so each returns a default value;
// [[maybe_unused]] says that no call is evaluated.
struct without_multiplication {
  without_multiplication() = default;
  explicit without_multiplication(std::int64_t /*value*/) {}
  [[maybe_unused]] friend without_multiplication
  operator+(const without_multiplication& /*a*/,
            const without_multiplication& /*b*/) {
    return {};
  }
  [[maybe_unused]] friend without_multiplication
  operator-(const without_multiplication& /*a*/,
            const without_multiplication& /*b*/) {
    return {};
  }
  [[maybe_unused]] friend without_multiplication
  operator-(const without_multiplication& /*a*/) {
    return {};
  }
};
struct without_coefficient_conversion {
  [[maybe_unused]] friend without_coefficient_conversion
  operator+(const without_coefficient_conversion& /*a*/,
            const without_coefficient_conversion& /*b*/) {
    return {};
  }
  [[maybe_unused]] friend without_coefficient_conversion
  operator-(const without_coefficient_conversion& /*a*/,
            const without_coefficient_conversion& /*b*/) {
    return {};
  }
  [[maybe_unused]] friend without_coefficient_conversion
  operator*(const without_coefficient_conversion& /*a*/,
            const without_coefficient_conversion& /*b*/) {
    return {};
  }
  [[maybe_unused]] friend without_coefficient_conversion
  operator-(const without_coefficient_conversion& /*a*/) {
    return {};
  }
};
struct without_unary_minus {
  without_unary_minus() = default;
  explicit without_unary_minus(std::int64_t /*value*/) {}
  [[maybe_unused]] friend without_unary_minus
  operator+(const without_unary_minus& /*a*/,
            const without_unary_minus& /*b*/) {
    return {};
  }
  [[maybe_unused]] friend without_unary_minus
  operator-(const without_unary_minus& /*a*/,
            const without_unary_minus& /*b*/) {
    return {};
  }
  [[maybe_unused]] friend without_unary_minus
  operator*(const without_unary_minus& /*a*/,
            const without_unary_minus& /*b*/) {
    return {};
  }
};
struct not_copyable {
  not_copyable() = default;
  explicit not_copyable(std::int64_t /*value*/) {}
  not_copyable(const not_copyable&) = delete;
  not_copyable& operator=(const not_copyable&) = delete;
  not_copyable(not_copyable&&) = default;
  not_copyable& operator=(not_copyable&&) = default;
  ~not_copyable() = default;
  [[maybe_unused]] friend not_copyable operator+(const not_copyable& /*a*/,
                                                 const not_copyable& /*b*/) {
    return {};
  }
  [[maybe_unused]] friend not_copyable operator-(const not_copyable& /*a*/,
                                                 const not_copyable& /*b*/) {
    return {};
  }
  [[maybe_unused]] friend not_copyable operator*(const not_copyable& /*a*/,
                                                 const not_copyable& /*b*/) {
    return {};
  }
  [[maybe_unused]] friend not_copyable operator-(const not_copyable& /*a*/) {
    return {};
  }
};

} // namespace

// Reference: contract: ring_scalar accepts a semiregular type with +, -, *,
//   and unary - that construct from coefficient_type, and rejects a type
//   that lacks any one of them, and exact_scalar adds that
//   std::numeric_limits says the type is exact, which it says for the
//   integer types and the test rational, and not for floating types or a
//   type without a specialization, such as std::complex<double>
TEST_CASE("The scalar concepts accept the types that meet their "
          "requirements") {
  static_assert(core::ring_scalar<std::int64_t>);
  static_assert(core::ring_scalar<int>);
  static_assert(core::ring_scalar<short>);
  static_assert(core::ring_scalar<float>);
  static_assert(core::ring_scalar<double>);
  static_assert(core::ring_scalar<long double>);
  static_assert(core::ring_scalar<std::complex<double>>);
  static_assert(core::ring_scalar<rational>);

  static_assert(!core::ring_scalar<int*>);
  static_assert(!core::ring_scalar<std::string>);
  static_assert(!core::ring_scalar<without_multiplication>);
  static_assert(!core::ring_scalar<without_coefficient_conversion>);
  static_assert(!core::ring_scalar<without_unary_minus>);
  static_assert(!core::ring_scalar<not_copyable>);

  static_assert(core::exact_scalar<std::int64_t>);
  static_assert(core::exact_scalar<int>);
  static_assert(core::exact_scalar<rational>);
  static_assert(!core::exact_scalar<float>);
  static_assert(!core::exact_scalar<double>);
  static_assert(!core::exact_scalar<std::complex<double>>);
  static_assert(!core::exact_scalar<std::string>);

  // The same answers at run time, for runs that set the assertions aside.
  CHECK(core::ring_scalar<double>);
  CHECK(core::ring_scalar<rational>);
  CHECK_FALSE(core::ring_scalar<std::string>);
  CHECK_FALSE(core::ring_scalar<without_multiplication>);
  CHECK_FALSE(core::ring_scalar<without_coefficient_conversion>);
  CHECK_FALSE(core::ring_scalar<without_unary_minus>);
  CHECK_FALSE(core::ring_scalar<not_copyable>);
  CHECK(core::exact_scalar<rational>);
  CHECK_FALSE(core::exact_scalar<double>);
}

// Reference: contract: multivector holds for a specialization of mv with any
//   cv-qualification and reference, and for nothing else
TEST_CASE("multivector names exactly the specializations of mv") {
  static_assert(core::multivector<Line<double>>);
  static_assert(core::multivector<const Line<double>>);
  static_assert(core::multivector<const Line<double>&>);
  static_assert(core::multivector<Line<double>&&>);
  static_assert(core::multivector<volatile Empty<rational>>);
  static_assert(!core::multivector<std::array<double, 4>>);
  static_assert(!core::multivector<core::blade_set<4>>);
  static_assert(!core::multivector<decltype(line_layout)>);
  static_assert(!core::multivector<double>);
  CHECK(core::multivector<Motor<float>&>);
}

namespace {

// Whether mv accepts the set: asking whether the type is trivially copyable
// instantiates it, which runs every check of its layout and fails to
// compile if one fails.
template <class Alg, core::blade_set Set>
constexpr bool accepted =
    std::is_trivially_copyable_v<core::mv<Alg, Set, double>>;

template <std::size_t N, std::size_t... K>
consteval bool every_grade_accepted(std::index_sequence<K...> /*grades*/) {
  return (accepted<euclidean<N>, core::of_grade<N, static_cast<int>(K)>> &&
          ...);
}

// The sets that full and of_grade build in n generators, and a merge of the
// vectors with the antiscalar.
template <std::size_t N>
consteval bool generated_sets_accepted() {
  return accepted<euclidean<N>, core::full<N>> &&
         every_grade_accepted<N>(std::make_index_sequence<N + 1>{}) &&
         accepted<euclidean<N>,
                  core::merge<core::of_grade<N, 1>,
                              core::of_grade<N, static_cast<int>(N)>>>;
}

} // namespace

// Reference: contract: the checks of a layout accept every set that the
//   constructors of blade sets build, since those keep every blade within
//   the algebra, every sign +1 or -1, and every mask once: full<n>, each
//   of_grade<n, k>, a merge, and named sets, for n from 1 to 8
TEST_CASE("mv accepts every set the blade-set constructors build") {
  static_assert(generated_sets_accepted<1>());
  static_assert(generated_sets_accepted<2>());
  static_assert(generated_sets_accepted<3>());
  static_assert(generated_sets_accepted<4>());
  static_assert(generated_sets_accepted<5>());
  static_assert(generated_sets_accepted<6>());
  static_assert(generated_sets_accepted<7>());
  static_assert(generated_sets_accepted<8>());

  static_assert(accepted<euclidean<1>, core::blades<euclidean<1>, "1 e1">>);
  static_assert(accepted<euclidean<2>, core::blades<euclidean<2>, "e21 1">>);
  static_assert(
      accepted<euclidean<3>, core::blades<euclidean<3>, "e3 e31 e123">>);
  static_assert(accepted<rga3, motor_layout>);
  static_assert(accepted<rga3, core::merge<line_layout, plane_layout>>);
  static_assert(
      accepted<euclidean<5>, core::blades<euclidean<5>, "e54 e1 e12345">>);
  static_assert(accepted<euclidean<6>, core::blades<euclidean<6>, "e6 e654">>);
  static_assert(
      accepted<euclidean<7>, core::blades<euclidean<7>, "e7 e71 e1234567">>);
  static_assert(
      accepted<euclidean<8>, core::blades<euclidean<8>, "e87654321 e8 e18">>);
  static_assert(accepted<cga3, core::blades<cga3, "e45 e54321">>);
  static_assert(accepted<r15_0_1, sparse_layout>);
  static_assert(accepted<rga3, empty_layout>);
  CHECK(accepted<rga3, line_layout>);
}

namespace {

// A layout of conformal(3) with e45, the blade of the origin and infinity.
inline constexpr auto conformal_layout =
    core::blades<cga3, "e1 e45 e321 e12345">;
template <class T>
using ConformalBlades = core::mv<cga3, conformal_layout, T>;

// The oriented blade that a name stands for, read here apart from the
// header: the mask holds the indices the name lists, and the sign is that of
// the permutation that sorts them, the parity of the pairs out of order.
constexpr core::oriented_blade blade_of(std::string_view name) {
  if (name == "1") {
    return {.mask = 0, .sign = 1};
  }
  core::mask_t mask = 0;
  int inversions = 0;
  for (std::size_t p = 1; p < name.size(); ++p) {
    mask |= core::mask_t{1} << static_cast<unsigned>(name[p] - '1');
    for (std::size_t q = p + 1; q < name.size(); ++q) {
      inversions += name[p] > name[q] ? 1 : 0;
    }
  }
  return {.mask = mask, .sign = inversions % 2 == 0 ? 1 : -1};
}

// Whether get<Name> reads and writes exactly position `at` of Type, through
// an lvalue, a const lvalue, and an rvalue, whose result is an rvalue.
template <class Type, core::detail::name_list Name>
constexpr bool get_reaches(std::size_t at) {
  using T = Type::scalar_type;
  static_assert(std::is_same_v<decltype(get<Name>(std::declval<Type&>())), T&>);
  static_assert(std::is_same_v<decltype(get<Name>(std::declval<const Type&>())),
                               const T&>);
  static_assert(std::is_same_v<decltype(get<Name>(std::declval<Type>())), T&&>);

  Type x{};
  get<Name>(x) = T(7);
  bool only_there = true;
  for (std::size_t i = 0; i < Type::component_count; ++i) {
    only_there = only_there && x.coefficients.at(i) == (i == at ? T(7) : T{});
  }
  const Type& read_only = x;
  const T moved = get<Name>(Type{x});
  return only_there && get<Name>(read_only) == T(7) && moved == T(7);
}

// Whether Name stands for the blade (mask, sign), by the reading above, and
// is at position `at` of Type's layout, where has_blade finds it and get
// reaches it.
template <class Type, core::detail::name_list Name>
constexpr bool named_at(std::size_t at, core::mask_t mask, int sign) {
  const core::oriented_blade expected{.mask = mask, .sign = sign};
  return blade_of(Name.text()) == expected &&
         Type::layout.blades.at(at) == expected &&
         core::has_blade<Type, Name> && get_reaches<Type, Name>(at);
}

} // namespace

// Reference: independent value: an oriented name lists generators in the
//   order of their product, read here by the parity of its inversions: in
//   rigid(3) e41 = -e14, e42 = -e24, e43 = -e34, e23 = +e23, e31 = -e13,
//   e12 = +e12, e423 = +e234, e431 = -e134, e412 = +e124, e321 = -e123, and
//   e1234 = +e1234, as the conventions' table of rigid(3) gives them, and in
//   conformal(3) e45 = +e45 and e321 = -e123; contract: get reads and writes
//   the place of the name in the layout, with the value category of its
//   argument
TEST_CASE_TEMPLATE("get reaches the coefficient of each oriented name", T,
                   std::int64_t, rational, float, double,
                   std::complex<double>) {
  static_assert(named_at<Line<T>, "e41">(0, 0b1001, -1));
  static_assert(named_at<Line<T>, "e42">(1, 0b1010, -1));
  static_assert(named_at<Line<T>, "e43">(2, 0b1100, -1));
  static_assert(named_at<Line<T>, "e23">(3, 0b0110, 1));
  static_assert(named_at<Line<T>, "e31">(4, 0b0101, -1));
  static_assert(named_at<Line<T>, "e12">(5, 0b0011, 1));
  static_assert(named_at<Plane<T>, "e423">(0, 0b1110, 1));
  static_assert(named_at<Plane<T>, "e431">(1, 0b1101, -1));
  static_assert(named_at<Plane<T>, "e412">(2, 0b1011, 1));
  static_assert(named_at<Plane<T>, "e321">(3, 0b0111, -1));
  static_assert(named_at<Motor<T>, "1">(0, 0, 1));
  static_assert(named_at<Motor<T>, "e41">(1, 0b1001, -1));
  static_assert(named_at<Motor<T>, "e12">(6, 0b0011, 1));
  static_assert(named_at<Motor<T>, "e1234">(7, 0b1111, 1));
  static_assert(named_at<Point<T>, "e1">(0, 0b0001, 1));
  static_assert(named_at<Point<T>, "e4">(3, 0b1000, 1));
  static_assert(named_at<ConformalBlades<T>, "e1">(0, 0b00001, 1));
  static_assert(named_at<ConformalBlades<T>, "e45">(1, 0b11000, 1));
  static_assert(named_at<ConformalBlades<T>, "e321">(2, 0b00111, -1));
  static_assert(named_at<ConformalBlades<T>, "e12345">(3, 0b11111, 1));

  // The same at run time, with positions the compiler cannot know.
  const volatile std::size_t zero = 0;
  CHECK(named_at<Line<T>, "e41">(zero + 0, 0b1001, -1));
  CHECK(named_at<Line<T>, "e31">(zero + 4, 0b0101, -1));
  CHECK(named_at<Plane<T>, "e321">(zero + 3, 0b0111, -1));
  CHECK(named_at<Motor<T>, "e1234">(zero + 7, 0b1111, 1));
  CHECK(named_at<ConformalBlades<T>, "e45">(zero + 1, 0b11000, 1));
}

// Reference: contract: get reads and writes a coefficient of a value at run
//   time, called without a namespace, also beside std::get and with a scalar
//   from namespace std
TEST_CASE("get reads and writes a line at run time") {
  const volatile double two = 2.0;
  Line<double> axis{0, 0, 1, 0, 0, 0};
  get<"e23">(axis) = two;
  CHECK(axis == Line<double>{0, 0, 1, 2, 0, 0});
  CHECK(get<"e43">(axis) == 1.0);
  CHECK(get<"e23">(std::as_const(axis)) == 2.0);

  Line<std::complex<double>> complex_axis{};
  get<"e43">(complex_axis) = std::complex<double>{0.0, two};
  CHECK(complex_axis.coefficients[2] == std::complex<double>{0.0, 2.0});
  const std::tuple<int, double> pair{1, 2.0};
  CHECK(std::get<1>(pair) == 2.0);
}

// Reference: contract: has_blade is true only for a name the layout holds
//   with that orientation, and false, never an error, for the other
//   orientation, a blade the layout lacks, a name with an index above n, a
//   text that is not a name, two names, and a type that is not a
//   multivector
TEST_CASE("has_blade answers for every name without an error") {
  static_assert(!core::has_blade<Line<double>, "e14">);
  static_assert(!core::has_blade<Line<double>, "e24">);
  static_assert(!core::has_blade<Line<double>, "e34">);
  static_assert(!core::has_blade<Line<double>, "e13">);
  static_assert(!core::has_blade<Line<double>, "e1">);
  static_assert(!core::has_blade<Line<double>, "e5">);
  static_assert(!core::has_blade<Line<double>, "x1">);
  static_assert(!core::has_blade<Line<double>, "e0">);
  static_assert(!core::has_blade<Line<double>, "e11">);
  static_assert(!core::has_blade<Line<double>, "e1 e2">);
  static_assert(!core::has_blade<Line<double>, "">);
  static_assert(!core::has_blade<Empty<double>, "1">);
  static_assert(!core::has_blade<std::array<double, 6>, "e41">);
  static_assert(core::has_blade<const Line<double>&, "e41">);
  static_assert(core::has_blade<Motor<float>, "1">);
  CHECK(core::has_blade<Line<double>, "e12">);
}

// Reference: contract: the rejecting overload of get and the constructor for
//   a wrong count make generic code that probes a call or a construction
//   told yes, while has_blade answers no, as their comments say
TEST_CASE("Probes of get and of construction answer yes for a misuse") {
  static_assert(requires(Line<double>& line) { get<"e1">(line); });
  static_assert(!core::has_blade<Line<double>, "e1">);
  static_assert(std::is_constructible_v<Line<double>, double, double, double>);
  CHECK(requires(Line<double>& line) { get<"e41">(line); });
}

namespace {

// A scalar of two lanes computed side by side, as a vector unit holds them:
// it has the arithmetic of a ring, lane by lane, and no ==, since two lanes
// are not one truth value.
struct two_lanes {
  double first = 0;
  double second = 0;

  two_lanes() = default;
  explicit two_lanes(std::int64_t value)
      : first(static_cast<double>(value)), second(static_cast<double>(value)) {}
  two_lanes(double a, double b) : first(a), second(b) {}

  friend two_lanes operator+(const two_lanes& a, const two_lanes& b) {
    return {a.first + b.first, a.second + b.second};
  }
  friend two_lanes operator-(const two_lanes& a, const two_lanes& b) {
    return {a.first - b.first, a.second - b.second};
  }
  friend two_lanes operator*(const two_lanes& a, const two_lanes& b) {
    return {a.first * b.first, a.second * b.second};
  }
  friend two_lanes operator-(const two_lanes& a) {
    return {-a.first, -a.second};
  }
};

} // namespace

// Reference: contract: ring_scalar does not require ==, so a scalar of
//   lanes without one is accepted, its multivector is a type that is made,
//   copied, and read, and the multivector has no == either, which generic
//   code that asks is told
TEST_CASE("mv over a scalar without == is a type without ==") {
  static_assert(core::ring_scalar<two_lanes>);
  static_assert(!std::equality_comparable<two_lanes>);
  static_assert(std::is_trivially_copyable_v<Line<two_lanes>>);
  static_assert(!std::equality_comparable<Line<two_lanes>>);

  const volatile double half = 0.5;
  Line<two_lanes> line{};
  get<"e41">(line) = two_lanes{half, -half};
  const Line<two_lanes> copy = line;
  CHECK(get<"e41">(copy).first == 0.5);
  CHECK(get<"e41">(copy).second == -0.5);
  CHECK(get<"e12">(copy).first == 0.0);
  const two_lanes e41 = get<"e41">(copy);
  const two_lanes sum = (e41 + e41) - (-e41) * two_lanes{1.0, 1.0};
  CHECK(sum.first == 1.5);
  CHECK(sum.second == -1.5);
  CHECK(core::ring_scalar<two_lanes>);
  CHECK_FALSE(std::equality_comparable<Line<two_lanes>>);
}
