// Tests of galoot/core/mv.hpp: the multivector type, its scalar concepts,
// its construction, its equality, the checks of its layout, named access,
// the straight-line code that applies a product's plan or a map, the
// conversions between layouts, and truncate.
//
// The expected values come from what the header states, written out here:
// a value built from a list of numbers holds them in the order given, a
// default value holds T{} in every place, two values are equal when every
// coefficient is, and a scalar concept accepts exactly the types that meet
// its listed requirements. The scalar types are the five a multivector must
// serve: std::int64_t and the test rational, which are exact; float and
// double, which round; and std::complex<double>, whose members are private.
#include <array>
#include <cmath>
#include <complex>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <new> // IWYU pragma: keep
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// For the POSIX truncate, a name that truncate must coexist with.
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>
#include <support/rational.hpp>
#include <support/scalar_faults.hpp>
#include <unistd.h> // IWYU pragma: keep

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
  // Value-initialized with (), not {}: GCC 16.2.1 can give a {} of an array
  // of a type whose zero is not all zero bits, such as the rational, the
  // wrong value at run time when a constant evaluation of the same
  // declaration in the program has written into it first.
  std::array<T, K> values = std::array<T, K>();
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

namespace {

// Whether a == b is a valid expression, tested as such: the standard
// equality_comparable_with also asks for a common reference, which two
// unrelated class types never have, so it cannot tell whether == exists.
template <class A, class B>
concept has_equality_with = requires(const A& a, const B& b) { a == b; };

} // namespace

// Reference: contract: multivectors of different layouts, algebras, or
//   scalars are different types and have no ==, even when they hold the
//   same number of coefficients of one scalar, tested on the expression a == b
//   itself
TEST_CASE("mv compares only with its own type") {
  using Bivectors = core::mv<rga3, core::of_grade<4, 2>, double>;
  using FourEuclidean =
      core::mv<euclidean<4>, core::blades<euclidean<4>, "e1 e2 e3 e4">, double>;
  static_assert(has_equality_with<Line<double>, Line<double>>);
  static_assert(!has_equality_with<Point<double>, FourEuclidean>);
  static_assert(!has_equality_with<Line<double>, Bivectors>);
  static_assert(!has_equality_with<Line<double>, Line<float>>);
  CHECK(has_equality_with<Line<double>, Line<double>>);
  CHECK_FALSE(has_equality_with<Point<double>, FourEuclidean>);
  CHECK_FALSE(has_equality_with<Line<double>, Bivectors>);
  CHECK_FALSE(has_equality_with<Line<double>, Line<float>>);

  // The standard concept says no for any two class types without a common
  // reference, whatever == does; these record its answer.
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
//   and unary - that construct from coefficient_type, other than bool, and
//   rejects a type that lacks any one of them and bool, and exact_scalar adds
//   that std::numeric_limits says the type is exact, which it says for the
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
  static_assert(!core::ring_scalar<bool>);
  static_assert(!core::ring_scalar<const bool>);

  static_assert(core::exact_scalar<std::int64_t>);
  static_assert(core::exact_scalar<int>);
  static_assert(core::exact_scalar<rational>);
  static_assert(!core::exact_scalar<float>);
  static_assert(!core::exact_scalar<double>);
  static_assert(!core::exact_scalar<std::complex<double>>);
  static_assert(!core::exact_scalar<std::string>);
  static_assert(!core::exact_scalar<bool>);

  // The same answers at run time, for runs that set the assertions aside.
  CHECK(core::ring_scalar<double>);
  CHECK(core::ring_scalar<rational>);
  CHECK_FALSE(core::ring_scalar<std::string>);
  CHECK_FALSE(core::ring_scalar<without_multiplication>);
  CHECK_FALSE(core::ring_scalar<without_coefficient_conversion>);
  CHECK_FALSE(core::ring_scalar<without_unary_minus>);
  CHECK_FALSE(core::ring_scalar<not_copyable>);
  CHECK_FALSE(core::ring_scalar<bool>);
  CHECK(core::exact_scalar<rational>);
  CHECK_FALSE(core::exact_scalar<double>);
}

namespace {

// The five requirements of ring_scalar after semiregular and not bool, each
// written out from its statement, so that a test scalar can be shown to
// fail exactly one of them.
template <class T>
concept adds = requires(const T& a, const T& b) {
  { a + b } -> std::convertible_to<T>;
};
template <class T>
concept subtracts = requires(const T& a, const T& b) {
  { a - b } -> std::convertible_to<T>;
};
template <class T>
concept multiplies = requires(const T& a, const T& b) {
  { a * b } -> std::convertible_to<T>;
};
template <class T>
concept negates = requires(const T& a) {
  { -a } -> std::convertible_to<T>;
};
template <class T>
concept made_from_coefficients = std::constructible_from<T, std::int64_t>;

// How many of the five requirements T meets.
template <class T>
inline constexpr int requirements_met =
    (adds<T> ? 1 : 0) + (subtracts<T> ? 1 : 0) + (multiplies<T> ? 1 : 0) +
    (negates<T> ? 1 : 0) + (made_from_coefficients<T> ? 1 : 0);

} // namespace

// Reference: contract: ring_scalar is one concept whose requirements after
//   semiregular and not bool are a + b, a - b, a * b, -a, and construction
//   from the integer coefficient type, so a semiregular scalar that lacks any
//   one of them is rejected, and each scalar the multivector's messages are
//   tested on lacks the one its name gives and meets the other four
TEST_CASE("Each fault scalar lacks exactly one requirement of ring_scalar") {
  using namespace test_support;
  static_assert(std::same_as<core::coefficient_type, std::int64_t>);

  static_assert(std::semiregular<lacks_addition>);
  static_assert(!adds<lacks_addition>);
  static_assert(requirements_met<lacks_addition> == 4);
  static_assert(!core::ring_scalar<lacks_addition>);

  static_assert(std::semiregular<lacks_subtraction>);
  static_assert(!subtracts<lacks_subtraction>);
  static_assert(requirements_met<lacks_subtraction> == 4);
  static_assert(!core::ring_scalar<lacks_subtraction>);

  static_assert(std::semiregular<lacks_multiplication>);
  static_assert(!multiplies<lacks_multiplication>);
  static_assert(requirements_met<lacks_multiplication> == 4);
  static_assert(!core::ring_scalar<lacks_multiplication>);

  static_assert(std::semiregular<lacks_unary_minus>);
  static_assert(!negates<lacks_unary_minus>);
  static_assert(requirements_met<lacks_unary_minus> == 4);
  static_assert(!core::ring_scalar<lacks_unary_minus>);

  static_assert(std::semiregular<lacks_integer_construction>);
  static_assert(!made_from_coefficients<lacks_integer_construction>);
  static_assert(requirements_met<lacks_integer_construction> == 4);
  static_assert(!core::ring_scalar<lacks_integer_construction>);

  // A scalar that meets all five is a ring scalar, so the five are all that
  // the fault scalars miss.
  static_assert(requirements_met<std::int64_t> == 5);
  static_assert(requirements_met<rational> == 5);
  CHECK(requirements_met<lacks_addition> == 4);
  CHECK_FALSE(core::ring_scalar<lacks_integer_construction>);
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
// is at position `at` of Type's layout, where has_name finds it and get
// reaches it.
template <class Type, core::detail::name_list Name>
constexpr bool named_at(std::size_t at, core::mask_t mask, int sign) {
  const core::oriented_blade expected{.mask = mask, .sign = sign};
  return blade_of(Name.text()) == expected &&
         Type::layout.blades.at(at) == expected && core::has_name<Type, Name> &&
         get_reaches<Type, Name>(at);
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

// Reference: contract: has_name is true only for a name the layout holds
//   with that orientation, and false, never an error, for the other
//   orientation, a blade the layout lacks, a name with an index above n, a
//   text that is not a name, two names, and a type that is not a
//   multivector
TEST_CASE("has_name answers for every name without an error") {
  static_assert(!core::has_name<Line<double>, "e14">);
  static_assert(!core::has_name<Line<double>, "e24">);
  static_assert(!core::has_name<Line<double>, "e34">);
  static_assert(!core::has_name<Line<double>, "e13">);
  static_assert(!core::has_name<Line<double>, "e1">);
  static_assert(!core::has_name<Line<double>, "e5">);
  static_assert(!core::has_name<Line<double>, "x1">);
  static_assert(!core::has_name<Line<double>, "e0">);
  static_assert(!core::has_name<Line<double>, "e11">);
  static_assert(!core::has_name<Line<double>, "e1 e2">);
  static_assert(!core::has_name<Line<double>, "">);
  static_assert(!core::has_name<Empty<double>, "1">);
  static_assert(!core::has_name<std::array<double, 6>, "e41">);
  static_assert(core::has_name<const Line<double>&, "e41">);
  static_assert(core::has_name<Motor<float>, "1">);
  CHECK(core::has_name<Line<double>, "e12">);
}

// Reference: independent value: an oriented name stands for the canonical
//   blade of its indices times the parity of their inversions, so in rigid(3)
//   e234 is e423 and e4321 is e1234, while e14 = -e41, e32 = -e23,
//   e324 = -e423, e21 = -e12, and e2134 = -e1234; contract: has_name is true
//   exactly for a name that get reads, a name that stands for a blade of the
//   layout with the orientation the layout gives it however the name is
//   spelled, and false, never an error, for a blade that the layout holds
//   with the other orientation and for a blade that it lacks
TEST_CASE("has_name is true for the oriented names that get reads") {
  using Bivectors = core::mv<rga3, core::of_grade<4, 2>, std::int64_t>;
  static_assert(core::has_name<Line<std::int64_t>, "e41">);
  static_assert(core::has_name<Plane<std::int64_t>, "e423">);
  static_assert(core::has_name<Bivectors, "e12">);
  static_assert(core::has_name<const Motor<std::int64_t>&, "e1234">);

  // Another spelling of a blade with the layout's orientation.
  static_assert(core::has_name<Plane<std::int64_t>, "e234">);
  static_assert(core::has_name<Motor<std::int64_t>, "e4321">);

  // A blade that the layout holds, named with the other orientation.
  static_assert(!core::has_name<Line<std::int64_t>, "e14">);
  static_assert(!core::has_name<Line<std::int64_t>, "e32">);
  static_assert(!core::has_name<Plane<std::int64_t>, "e324">);
  static_assert(!core::has_name<Bivectors, "e21">);
  static_assert(!core::has_name<Motor<std::int64_t>, "e2134">);

  // A blade that the layout lacks, a text that is not a name, and a type
  // that is not a multivector.
  static_assert(!core::has_name<Line<std::int64_t>, "e1">);
  static_assert(!core::has_name<Line<std::int64_t>, "e1234">);
  static_assert(!core::has_name<Line<std::int64_t>, "e5">);
  static_assert(!core::has_name<Line<std::int64_t>, "x1">);
  static_assert(!core::has_name<std::int64_t, "1">);

  // get reads the place of each accepted name: the plane's e423 is its
  // first place, the motor's e1234 its last.
  constexpr Plane<std::int64_t> plane{1, 2, 3, 4};
  constexpr Motor<std::int64_t> motor{1, 2, 3, 4, 5, 6, 7, 8};
  static_assert(get<"e234">(plane) == 1);
  static_assert(get<"e4321">(motor) == 8);

  const volatile std::int64_t one = 1;
  const Plane<std::int64_t> at_run_time{one, 2, 3, 4};
  CHECK(get<"e234">(at_run_time) == 1);
  CHECK(core::has_name<Plane<std::int64_t>, "e234">);
  CHECK_FALSE(core::has_name<Plane<std::int64_t>, "e324">);
}

// Reference: contract: the rejecting overload of get and the constructor for
//   a wrong count make generic code that probes a call or a construction
//   told yes, while has_name answers no, as their comments say
TEST_CASE("Probes of get and of construction answer yes for a misuse") {
  static_assert(requires(Line<double>& line) { get<"e1">(line); });
  static_assert(!core::has_name<Line<double>, "e1">);
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

namespace {

// Whether mv<rga3, line_layout, T> names a type, as generic code asks.
template <class T>
concept names_a_line = requires { typename core::mv<rga3, line_layout, T>; };

// A scalar without a default constructor, which is not semiregular.
struct without_default {
  explicit without_default(std::int64_t /*value*/) {}
};

} // namespace

// Reference: contract: mv's template head requires only a destructible
//   scalar, so generic code that asks whether mv names a type over void is
//   told no, while a scalar that mv rejects in its body with a message, such
//   as one without a default constructor, std::string, const double, a
//   reference, or bool, is told yes and fails only when used, as the comment
//   of mv says
TEST_CASE("mv's template head asks for a destructible scalar") {
  static_assert(names_a_line<double>);
  static_assert(!names_a_line<void>);
  static_assert(names_a_line<without_default>);
  static_assert(names_a_line<std::string>);
  static_assert(names_a_line<const double>);
  static_assert(names_a_line<double&>);
  static_assert(names_a_line<bool>);
  CHECK(names_a_line<double>);
  CHECK_FALSE(names_a_line<void>);
  CHECK(names_a_line<without_default>);
}

namespace {

// R(9, 0, 0), the widest algebra whose blades have names, and a layout of
// its two blades that need the index 9.
inline constexpr auto nine_layout =
    core::blades<euclidean<9>, "e19 e9 e123456789">;
using Nine = core::mv<euclidean<9>, nine_layout, double>;

} // namespace

// Reference: independent value: in R(9, 0, 0) e19 is the mask of bits 0 and
//   8 with sign +1, e9 bit 8, and e123456789 every bit, read by the test's
//   own inversion count; contract: get reads names for n up to 9, the
//   largest n whose indices are single digits
TEST_CASE("get reads names in an algebra of nine generators") {
  static_assert(named_at<Nine, "e19">(0, 0b100000001, 1));
  static_assert(named_at<Nine, "e9">(1, 0b100000000, 1));
  static_assert(named_at<Nine, "e123456789">(2, 0b111111111, 1));
  static_assert(!core::has_name<Nine, "e91">); // the other orientation
  Nine x{1, 2, 3};
  get<"e9">(x) = 5.0;
  CHECK(get<"e19">(x) == 1.0);
  CHECK(get<"e9">(x) == 5.0);
  CHECK(core::has_name<Nine, "e123456789">);
}

// Applying plans and maps, converting between layouts, and truncating.

namespace {

using core::coefficient_type;
using core::detail::apply;
using core::detail::BinaryOperation;
using core::detail::product_plan;
using core::detail::unary_map;
using core::detail::UnaryOperation;

// The forms the kernel is checked over, beside rga3 and cga3 above.
struct r1_3 {
  static constexpr auto value = core::R<1, 3, 0>();
};
struct cga2 {
  static constexpr auto value = core::conformal<2>();
};
struct degenerate3 {
  static constexpr auto value = core::from_gram({
      {0, 1, 0},
      {1, 0, 0},
      {0, 0, 0},
  });
};
struct random1 {
  static constexpr auto value = core::from_gram({
      {2, 0, -3},
      {0, 1, -3},
      {-3, -3, -2},
  });
};
// One generator with e1 e1 = 2, so that its product has the coefficient 2.
struct doubled1 {
  static constexpr auto value = core::from_gram({{2}});
};

// The product of the plan's terms taken one at a time, each added to a
// result that starts at zero: out[t.out] += c (x[t.a] y[t.b]).
template <BinaryOperation Op, class Alg, core::blade_set A, core::blade_set B,
          class T>
constexpr auto product_by_loop(const std::array<T, A.size()>& x,
                               const std::array<T, B.size()>& y) {
  constexpr const auto& plan = product_plan<Op, Alg, A, B>;
  // Value-initialized with (), not {}, as in counting.
  std::array<T, plan.result.size()> out = std::array<T, plan.result.size()>();
  for (const core::detail::typed_term& t : plan.terms) {
    out.at(t.out) =
        out.at(t.out) + (T(t.coefficient) * (x.at(t.a) * y.at(t.b)));
  }
  return out;
}

// The map's terms taken in the same way: out[t.out] += c x[t.a].
template <UnaryOperation Op, class Alg, core::blade_set S, class T>
constexpr auto map_by_loop(const std::array<T, S.size()>& x) {
  constexpr const auto& map = unary_map<Op, Alg, S>;
  // Value-initialized with (), not {}, as in counting.
  std::array<T, map.result.size()> out = std::array<T, map.result.size()>();
  for (const core::detail::map_term& t : map.terms) {
    out.at(t.out) = out.at(t.out) + (T(t.coefficient) * x.at(t.a));
  }
  return out;
}

// A multivector of integer type with every coefficient drawn from -9 to 9.
template <class Type>
Type random_integers(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> draw(-9, 9);
  Type x{};
  for (std::int64_t& c : x.coefficients) {
    c = draw(source);
  }
  return x;
}

// The number of inputs, over every pair of basis blades of Set and 16 pairs
// of random inputs, where apply<Op> and the loop over the plan differ.
template <BinaryOperation Op, class Alg, core::blade_set Set>
int product_mismatches(std::mt19937_64& source) {
  using X = core::mv<Alg, Set, std::int64_t>;
  const auto differs = [](const X& x, const X& y) {
    return apply<Op>(x, y).coefficients !=
           product_by_loop<Op, Alg, Set, Set>(x.coefficients, y.coefficients);
  };
  int mismatches = 0;
  for (std::size_t i = 0; i < Set.size(); ++i) {
    for (std::size_t j = 0; j < Set.size(); ++j) {
      X x{};
      X y{};
      x.coefficients.at(i) = 1;
      y.coefficients.at(j) = 1;
      mismatches += differs(x, y) ? 1 : 0;
    }
  }
  for (int k = 0; k < 16; ++k) {
    const X x = random_integers<X>(source);
    const X y = random_integers<X>(source);
    mismatches += differs(x, y) ? 1 : 0;
  }
  return mismatches;
}

template <UnaryOperation Op, class Alg, core::blade_set Set>
int map_mismatches(std::mt19937_64& source) {
  using X = core::mv<Alg, Set, std::int64_t>;
  const auto differs = [](const X& x) {
    return apply<Op>(x).coefficients !=
           map_by_loop<Op, Alg, Set>(x.coefficients);
  };
  int mismatches = 0;
  for (std::size_t i = 0; i < Set.size(); ++i) {
    X x{};
    x.coefficients.at(i) = 1;
    mismatches += differs(x) ? 1 : 0;
  }
  for (int k = 0; k < 16; ++k) {
    mismatches += differs(random_integers<X>(source)) ? 1 : 0;
  }
  return mismatches;
}

// The mismatches of every binary operation and every unary map in one form.
template <class Alg, core::blade_set Set>
int form_mismatches(std::mt19937_64& source) {
  using enum BinaryOperation;
  using enum UnaryOperation;
  return product_mismatches<geometric_product, Alg, Set>(source) +
         product_mismatches<wedge, Alg, Set>(source) +
         product_mismatches<geometric_antiproduct, Alg, Set>(source) +
         product_mismatches<antiwedge, Alg, Set>(source) +
         map_mismatches<reverse, Alg, Set>(source) +
         map_mismatches<antireverse, Alg, Set>(source) +
         map_mismatches<grade_involution, Alg, Set>(source) +
         map_mismatches<right_complement, Alg, Set>(source) +
         map_mismatches<left_complement, Alg, Set>(source);
}

} // namespace

// Reference: proof: an exhaustive check over every pair of basis blades of
//   every blade of rigid(3), R(4,0,0), R(1,3,0), and conformal(2), and of
//   the degenerate form and seeded random form 1 in three generators, for
//   the geometric product, the wedge, both antiproducts, and the five unary
//   maps; identity: on seeded random integer inputs as well, the
//   straight-line code equals the loop that adds each term of the plan,
//   c x_a y_b, to its output from zero
TEST_CASE("apply equals the loop over the terms of the plan") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  CHECK(form_mismatches<rga3, core::full<4>>(source) == 0);
  CHECK(form_mismatches<euclidean<4>, core::full<4>>(source) == 0);
  CHECK(form_mismatches<r1_3, core::full<4>>(source) == 0);
  CHECK(form_mismatches<cga2, core::full<4>>(source) == 0);
  CHECK(form_mismatches<degenerate3, core::full<3>>(source) == 0);
  CHECK(form_mismatches<random1, core::full<3>>(source) == 0);
}

namespace {

// Whether each term of a kernel is marked first exactly when no earlier
// term has its output, and the first term of each output has the lowest a,
// then the lowest b, of the terms on that output.
template <std::size_t N>
constexpr bool
first_terms_marked(const std::array<core::detail::kernel_term, N>& kernel) {
  for (std::size_t k = 0; k < N; ++k) {
    bool earlier = false;
    for (std::size_t j = 0; j < k; ++j) {
      earlier = earlier || kernel.at(j).out == kernel.at(k).out;
    }
    if (kernel.at(k).first == earlier) {
      return false;
    }
    for (std::size_t j = 0; j < N; ++j) {
      const bool lower =
          kernel.at(j).a < kernel.at(k).a ||
          (kernel.at(j).a == kernel.at(k).a && kernel.at(j).b < kernel.at(k).b);
      if (kernel.at(k).first && kernel.at(j).out == kernel.at(k).out && lower) {
        return false;
      }
    }
  }
  return true;
}

constexpr core::oriented_blade plus(core::mask_t mask) {
  return {.mask = mask, .sign = 1};
}

} // namespace

// Reference: contract: each output's first term in the order of the plan is
//   assigned and every later one is added or subtracted, so the first term
//   of each output of point times point is the one of lowest a, then lowest
//   b, and, under IEEE 754, -0.0 + 0.0 is +0.0 while -0.0 - 0.0 is -0.0, so an
//   output whose first term is (-1)(+0.0) keeps -0.0 where adding to zero
//   gives +0.0, and -0.0 in an input otherwise gives equal values
TEST_CASE("apply assigns the first term of each output and adds the rest") {
  constexpr const auto& plan = product_plan<BinaryOperation::geometric_product,
                                            rga3, point_layout, point_layout>;
  static_assert(first_terms_marked(
      core::detail::product_kernel<BinaryOperation::geometric_product, rga3,
                                   point_layout, point_layout, plan.result>));
  static_assert(first_terms_marked(
      core::detail::product_kernel<
          BinaryOperation::geometric_product, rga3, motor_layout, point_layout,
          product_plan<BinaryOperation::geometric_product, rga3, motor_layout, point_layout>.result>));

  constexpr Point<double> minus_one{-1.0, 0.0, 0.0, 0.0};
  constexpr Point<double> zero{};
  constexpr auto product =
      apply<BinaryOperation::geometric_product>(minus_one, zero);
  static_assert(std::signbit(get<"e12">(product)));
  static_assert(!std::signbit(get<"1">(product)));
  static_assert(!std::signbit(
      product_by_loop<BinaryOperation::geometric_product, rga3, point_layout,
                      point_layout>(minus_one.coefficients, zero.coefficients)
          .at(1)));
  // The reverse negates e12 and e23 of a line's canonical blades, as a
  // first term of -1.
  constexpr auto reversed = apply<UnaryOperation::reverse>(Line<double>{});
  static_assert(std::signbit(get<"e12">(reversed)));
  static_assert(std::signbit(get<"e23">(reversed)));
  static_assert(!std::signbit(get<"e13">(reversed)));

  const volatile double negative_zero = -0.0;
  const Point<double> p{1.0, negative_zero, 3.0, 1.0};
  const Point<double> q{4.0, 5.0, 6.0, 1.0};
  CHECK(
      apply<BinaryOperation::geometric_product>(p, q) ==
      apply<BinaryOperation::geometric_product>(Point<double>{1, 0, 3, 1}, q));
  CHECK(std::signbit(get<"e12">(apply<BinaryOperation::geometric_product>(
      Point<double>{-1.0 * (1.0 + negative_zero), 0, 0, 0}, zero))));
  CHECK(first_terms_marked(
      core::detail::product_kernel<BinaryOperation::geometric_product, rga3,
                                   point_layout, point_layout, plan.result>));
  CHECK(plan.result.blades.at(1) == plus(0b0011));
}

// Reference: contract: a term with a coefficient other than +1 or -1
//   contributes c (x_a y_b), the product of the inputs first, so that in a
//   form with e1 e1 = 2 the product of the largest double and 0.25 is half
//   the largest double, where (c x_a) y_b would overflow to infinity
TEST_CASE("apply multiplies the inputs before the coefficient") {
  using One = core::mv<doubled1, core::blades<doubled1, "e1">, double>;
  constexpr double largest = std::numeric_limits<double>::max();
  static_assert(get<"1">(apply<BinaryOperation::geometric_product>(
                    One{largest}, One{0.25})) == largest / 2);
  const volatile double quarter = 0.25;
  CHECK(get<"1">(apply<BinaryOperation::geometric_product>(
            One{largest}, One{quarter})) == largest / 2);
}

// Reference: independent value: by hand, from e1 e1 = e2 e2 = e3 e3 = 1 and
//   e4 e4 = 0, for p = (1, 2, 3, 1) and q = (4, 5, 6, 1): p q has the scalar
//   p1 q1 + p2 q2 + p3 q3 = 32 and on each e_ij the value p_i q_j - p_j q_i,
//   (-3, -6, -3, -3, -3, -3) on e12, e13, e14, e23, e24, e34, which is also
//   p ^ q, and as a line on e41 e42 e43 e23 e31 e12 that is (3, 3, 3, -3,
//   6, -3), the direction q - p and the moment, the cross product
//   (1, 2, 3) x (4, 5, 6) = (-3, 6, -3), where results are ordered by grade,
//   then by index list
TEST_CASE("apply gives the products of two points of rigid(3)") {
  constexpr Point<double> p{1, 2, 3, 1};
  constexpr Point<double> q{4, 5, 6, 1};
  constexpr auto product = apply<BinaryOperation::geometric_product>(p, q);
  constexpr auto wedge = apply<BinaryOperation::wedge>(p, q);
  static_assert(decltype(product)::layout.blades == std::to_array({
                                                        plus(0),
                                                        plus(0b0011),
                                                        plus(0b0101),
                                                        plus(0b1001),
                                                        plus(0b0110),
                                                        plus(0b1010),
                                                        plus(0b1100),
                                                    }));
  static_assert(product.coefficients ==
                std::array<double, 7>{32, -3, -6, -3, -3, -3, -3});
  static_assert(wedge.coefficients ==
                std::array<double, 6>{-3, -6, -3, -3, -3, -3});
  static_assert(Line<double>{wedge} == Line<double>{3, 3, 3, -3, 6, -3});
  static_assert(apply<BinaryOperation::wedge, line_layout>(p, q) ==
                Line<double>{3, 3, 3, -3, 6, -3});

  const volatile double one = 1.0;
  const Point<double> pv{one, 2 * one, 3 * one, one};
  const Point<double> qv{4 * one, 5 * one, 6 * one, one};
  CHECK(apply<BinaryOperation::geometric_product>(pv, qv).coefficients ==
        std::array<double, 7>{32, -3, -6, -3, -3, -3, -3});
  CHECK(Line<double>{apply<BinaryOperation::wedge>(pv, qv)} ==
        Line<double>{3, 3, 3, -3, 6, -3});
}

// Reference: independent value: by hand, from e4 . e5 = -1, e4 e4 = e5 e5 =
//   0, and e45 = e4 ^ e5 in conformal(3), for v = (1, 2, 3, 4, 5) and
//   w = (1, 0, 0, 1, 1): the scalar is 1 + (-1)(4 + 5) = -8, e45 gets
//   v4 w5 - v5 w4 = -1, and e23 is in the result set and is zero, so v w is
//   (-8, -2, -3, -3, -4, 0, 2, 2, 3, 3, -1) on 1, e12, e13, e14, e15, e23,
//   e24, e25, e34, e35, e45
TEST_CASE("apply gives the geometric product of two conformal(3) vectors") {
  constexpr ConformalVector<double> v{1, 2, 3, 4, 5};
  constexpr ConformalVector<double> w{1, 0, 0, 1, 1};
  constexpr auto product = apply<BinaryOperation::geometric_product>(v, w);
  static_assert(decltype(product)::layout.blades == std::to_array({
                                                        plus(0),
                                                        plus(0b00011),
                                                        plus(0b00101),
                                                        plus(0b01001),
                                                        plus(0b10001),
                                                        plus(0b00110),
                                                        plus(0b01010),
                                                        plus(0b10010),
                                                        plus(0b01100),
                                                        plus(0b10100),
                                                        plus(0b11000),
                                                    }));
  static_assert(product.coefficients ==
                std::array<double, 11>{-8, -2, -3, -3, -4, 0, 2, 2, 3, 3, -1});
  static_assert(get<"e45">(product) == -1.0);

  const volatile double one = 1.0;
  const ConformalVector<double> vv{one, 2 * one, 3 * one, 4 * one, 5 * one};
  CHECK(apply<BinaryOperation::geometric_product>(vv, w).coefficients ==
        std::array<double, 11>{-8, -2, -3, -3, -4, 0, 2, 2, 3, 3, -1});
}

// Reference: independent value: the reverse multiplies a blade of grade 2 by
//   (-1)^(2 (2 - 1) / 2) = -1, so the reverse of a line is its negation, and
//   on the line l = (1, 2, 3, 4, 5, 6) of e41 e42 e43 e23 e31 e12, whose
//   canonical blades e12, e13, e14, e23, e24, e34 hold (6, -5, -1, 4, -2, -3)
//   by e41 = -e14, e31 = -e13, e42 = -e24, and e43 = -e34, the reverse there
//   is (-6, 5, 1, -4, 2, 3)
TEST_CASE("apply gives the reverse of a line") {
  constexpr Line<double> l{1, 2, 3, 4, 5, 6};
  constexpr auto canonical = apply<UnaryOperation::reverse>(l);
  static_assert(canonical.coefficients ==
                std::array<double, 6>{-6, 5, 1, -4, 2, 3});
  static_assert(apply<UnaryOperation::reverse, line_layout>(l) ==
                Line<double>{-1, -2, -3, -4, -5, -6});

  const volatile double one = 1.0;
  const Line<double> lv{one, 2 * one, 3, 4, 5, 6};
  CHECK(apply<UnaryOperation::reverse>(lv).coefficients ==
        std::array<double, 6>{-6, 5, 1, -4, 2, 3});
  CHECK(apply<UnaryOperation::reverse, line_layout>(lv) ==
        Line<double>{-1, -2, -3, -4, -5, -6});
}

namespace {

// The right or left complement of the blade of `mask`, read from a table of
// the complements of every blade.
golden::signed_blade
complement_of(std::span<const golden::complement_pair> complements,
              core::mask_t mask, bool right) {
  for (const golden::complement_pair& pair : complements) {
    if (pair.blade == mask) {
      return right ? pair.right : pair.left;
    }
  }
  return {.blade = 0, .sign = 0};
}

// A product of x on the layout A with y on the layout B, composed from a
// table of the products of every pair of blades of n generators, listed in
// order of a, then b: the value on each mask, where a coefficient on the
// oriented blade s e_M multiplies s e_M. With complements, it is the
// antiproduct left(right(a) right(b)) instead, through the same table. A
// table entry out of its place makes the result empty.
template <class T, std::size_t KA, std::size_t KB>
std::vector<T>
composed_product(std::span<const golden::product> table, std::size_t n,
                 const core::blade_set<KA>& a_layout,
                 const core::blade_set<KB>& b_layout,
                 const std::array<T, KA>& x, const std::array<T, KB>& y,
                 std::span<const golden::complement_pair> complements = {}) {
  const bool anti = !complements.empty();
  const std::size_t blades = std::size_t{1} << n;
  std::vector<T> out(blades, T{});
  for (std::size_t i = 0; i < KA; ++i) {
    for (std::size_t j = 0; j < KB; ++j) {
      core::mask_t a = a_layout.blades.at(i).mask;
      core::mask_t b = b_layout.blades.at(j).mask;
      int sign = a_layout.blades.at(i).sign * b_layout.blades.at(j).sign;
      if (anti) {
        const golden::signed_blade ra = complement_of(complements, a, true);
        const golden::signed_blade rb = complement_of(complements, b, true);
        a = ra.blade;
        b = rb.blade;
        sign *= ra.sign * rb.sign;
      }
      const golden::product& entry = table[(a * blades) + b];
      if (entry.a != a || entry.b != b) {
        return {};
      }
      for (std::size_t k = 0; k < entry.size; ++k) {
        core::mask_t on = entry.terms.at(k).blade;
        int c = sign * entry.terms.at(k).coefficient;
        if (anti) {
          const golden::signed_blade left =
              complement_of(complements, on, false);
          on = left.blade;
          c *= left.sign;
        }
        out.at(on) = out.at(on) + (T(c) * (x.at(i) * y.at(j)));
      }
    }
  }
  return out;
}

// Whether the multivector r, on a result layout whose signs are all +1,
// holds exactly the values `expected` gives each mask: its own blades' and
// zero on every other.
template <class Type>
bool holds_composed(const Type& r,
                    const std::vector<typename Type::scalar_type>& expected) {
  using T = Type::scalar_type;
  if (expected.empty()) {
    return false;
  }
  std::vector<T> remaining = expected;
  for (std::size_t k = 0; k < Type::component_count; ++k) {
    const core::oriented_blade blade = Type::layout.blades.at(k);
    if (blade.sign != 1 ||
        !(remaining.at(blade.mask) == r.coefficients.at(k))) {
      return false;
    }
    remaining.at(blade.mask) = T{};
  }
  // NOLINTNEXTLINE(readability-use-anyofallof): one loop reads plainly.
  for (const T& value : remaining) {
    if (!(value == T{})) {
      return false;
    }
  }
  return true;
}

// A multivector of rationals with numerators from -9 to 9 and denominators
// 2, 3, and 5 in turn.
template <class Type>
Type random_fractions(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> numerator(-9, 9);
  constexpr std::array<std::int64_t, 3> denominators{2, 3, 5};
  Type x{};
  for (std::size_t k = 0; k < Type::component_count; ++k) {
    x.coefficients.at(k) = rational{numerator(source), denominators.at(k % 3)};
  }
  return x;
}

// Whether apply<geometric_product> of `pairs` pairs of random fractions on
// full<n> in the form Alg equals their product composed from `table`.
template <class Alg, std::size_t N>
bool matches_table(std::span<const golden::product> table,
                   std::mt19937_64& source, int pairs) {
  using X = core::mv<Alg, core::full<N>, rational>;
  for (int k = 0; k < pairs; ++k) {
    const X x = random_fractions<X>(source);
    const X y = random_fractions<X>(source);
    if (!holds_composed(apply<BinaryOperation::geometric_product>(x, y),
                        composed_product(table, N, X::layout, X::layout,
                                         x.coefficients, y.coefficients))) {
      return false;
    }
  }
  return true;
}

} // namespace

// Reference: independent value: the golden product tables, in which
//   e_i e_j + e_j e_i = 2 g_ij, of rigid(3), conformal(3), a degenerate form,
//   and seeded random form 1: the geometric product of two multivectors of
//   every blade with seeded random fractions, composed pair by pair from
//   the table, is the product apply computes, exactly, over the test
//   rational
TEST_CASE("apply gives the products the golden tables compose") {
  static_assert(rga3::value.gram == golden::gp_tables::rigid_3_gram);
  static_assert(cga3::value.gram == golden::gp_tables::conformal_3_gram);
  static_assert(degenerate3::value.gram ==
                golden::gp_tables::degenerate_3_gram);
  static_assert(random1::value.gram == golden::gp_tables::random_1_gram);

  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  CHECK(matches_table<rga3, 4>(golden::gp_tables::rigid_3, source, 8));
  CHECK(matches_table<cga3, 5>(golden::gp_tables::conformal_3, source, 4));
  CHECK(matches_table<degenerate3, 3>(golden::gp_tables::degenerate_3, source,
                                      8));
  CHECK(matches_table<random1, 3>(golden::gp_tables::random_1, source, 8));
}

namespace {

// The largest magnitude of a coefficient of a plan, and the largest number
// of its terms on one output.
template <std::size_t R, std::size_t N>
constexpr coefficient_type
largest_coefficient(const core::detail::typed_plan<R, N>& plan) {
  coefficient_type largest = 0;
  for (const core::detail::typed_term& t : plan.terms) {
    const coefficient_type c =
        t.coefficient < 0 ? -t.coefficient : t.coefficient;
    largest = c > largest ? c : largest;
  }
  return largest;
}

template <std::size_t R, std::size_t N>
constexpr std::size_t
most_terms_on_one_output(const core::detail::typed_plan<R, N>& plan) {
  std::size_t most = 0;
  for (std::size_t r = 0; r < R; ++r) {
    std::size_t count = 0;
    for (const core::detail::typed_term& t : plan.terms) {
      count += t.out == r ? 1 : 0;
    }
    most = count > most ? count : most;
  }
  return most;
}

// The bound on the inputs of the five-scalar test: every coefficient, and
// for std::complex<double> each of its parts, is an integer of magnitude at
// most 2^4.
constexpr std::int64_t input_bound = 16;
constexpr std::int64_t two_to_16 = 65'536;

// Whether the plan's products of such inputs are exact in every scalar: a
// partial sum of an output is at most (terms) (largest coefficient) 2^8,
// and for std::complex<double> twice that, since a part of a product of
// two complex numbers is a sum of two real products. At most 48 terms with
// coefficients of magnitude 2 give 48 * 2 * 2^8 * 2 < 2^16 < 2^24, which
// float, and so every other scalar here, holds exactly.
template <std::size_t R, std::size_t N>
constexpr bool
exact_for_bounded_inputs(const core::detail::typed_plan<R, N>& plan) {
  return largest_coefficient(plan) <= 2 &&
         most_terms_on_one_output(plan) <= 48 &&
         std::int64_t{48} * 2 * input_bound * input_bound * 2 < two_to_16;
}

// The value of a scalar of type T with the integer parts re and im; im is
// zero for every type but std::complex<double>.
template <class T>
T scalar_from(std::int64_t re, std::int64_t im) {
  if constexpr (std::same_as<T, std::complex<double>>) {
    return {static_cast<double>(re), static_cast<double>(im)};
  } else {
    return T(re);
  }
}

// Inputs of integer parts within the bound, as a multivector over T, from
// the real parts `re` and the imaginary parts `im`.
template <class Type, std::size_t K>
Type bounded_input(const std::array<std::int64_t, K>& re,
                   const std::array<std::int64_t, K>& im) {
  using T = Type::scalar_type;
  Type x{};
  for (std::size_t k = 0; k < K; ++k) {
    x.coefficients.at(k) = scalar_from<T>(re.at(k), im.at(k));
  }
  return x;
}

// Whether apply<Op>(x, y) over T equals the product composed in
// std::int64_t from `table` (and, for an antiproduct, `complements`),
// converted to T: for std::complex<double>, by bilinearity, the real part
// is P(re x, re y) - P(im x, im y) and the imaginary part P(re x, im y) +
// P(im x, re y).
template <BinaryOperation Op, class T, class A, class B, std::size_t KA,
          std::size_t KB>
bool exact_in(std::span<const golden::product> table, std::size_t n,
              std::span<const golden::complement_pair> complements,
              const std::array<std::int64_t, KA>& x_re,
              const std::array<std::int64_t, KA>& x_im,
              const std::array<std::int64_t, KB>& y_re,
              const std::array<std::int64_t, KB>& y_im) {
  const auto compose = [&](const auto& x, const auto& y) {
    return composed_product(table, n, A::layout, B::layout, x, y, complements);
  };
  const std::vector<std::int64_t> rr = compose(x_re, y_re);
  const std::vector<std::int64_t> ii = compose(x_im, y_im);
  const std::vector<std::int64_t> ri = compose(x_re, y_im);
  const std::vector<std::int64_t> ir = compose(x_im, y_re);
  if (rr.empty() || ii.empty() || ri.empty() || ir.empty()) {
    return false;
  }
  std::vector<T> expected(rr.size());
  for (std::size_t m = 0; m < rr.size(); ++m) {
    expected.at(m) = scalar_from<T>(rr.at(m) - ii.at(m), ri.at(m) + ir.at(m));
  }
  const auto r =
      apply<Op>(bounded_input<A>(x_re, x_im), bounded_input<B>(y_re, y_im));
  return holds_composed(r, expected);
}

// Integer inputs within the bound, real and imaginary parts.
constexpr std::array<std::int64_t, 4> point_re{16, -7, 3, 1};
constexpr std::array<std::int64_t, 4> point_im{-2, 5, -16, 9};
constexpr std::array<std::int64_t, 4> other_re{-4, 11, 16, -1};
constexpr std::array<std::int64_t, 4> other_im{7, -3, 0, 14};
constexpr std::array<std::int64_t, 6> line_re{1, -16, 5, 9, -2, 13};
constexpr std::array<std::int64_t, 6> line_im{3, 4, -8, 0, 16, -5};
constexpr std::array<std::int64_t, 8> motor_re{2, -3, 16, 7, -11, 1, 0, -6};
constexpr std::array<std::int64_t, 8> motor_im{-9, 4, 1, -16, 6, 2, 12, 3};
constexpr std::array<std::int64_t, 5> vector_re{3, -12, 7, 16, -5};
constexpr std::array<std::int64_t, 5> vector_im{-1, 8, 0, -16, 10};
constexpr std::array<std::int64_t, 5> other_vector_re{-16, 2, 9, -4, 1};
constexpr std::array<std::int64_t, 5> other_vector_im{6, -7, 13, 5, -2};

// For a scalar other than std::complex<double>, the imaginary parts are 0.
template <class T, std::size_t K>
std::array<std::int64_t, K> imaginary(const std::array<std::int64_t, K>& im) {
  return std::same_as<T, std::complex<double>> ? im
                                               : std::array<std::int64_t, K>{};
}

} // namespace

// Reference: independent value: the golden product tables of rigid(3) and
//   conformal(3), in which e_i e_j + e_j e_i = 2 g_ij, and the golden
//   complements, u ^ right(u) = antiscalar and left(u) ^ u = antiscalar,
//   compose point times point, line times plane, motor times point, motor
//   antiproduct point, and vector times vector in std::int64_t, and with
//   inputs
//   of integer parts at most 2^4, plans whose coefficients are at most 2 and
//   that have at most 48 terms on an output, every partial sum is below 2^16
//   < 2^24, so each of the five scalars computes the composed value exactly
TEST_CASE_TEMPLATE("apply is exact over the five scalars on bounded inputs", T,
                   std::int64_t, rational, float, double,
                   std::complex<double>) {
  using enum BinaryOperation;
  static_assert(exact_for_bounded_inputs(
      product_plan<geometric_product, rga3, point_layout, point_layout>));
  static_assert(exact_for_bounded_inputs(
      product_plan<geometric_product, rga3, line_layout, plane_layout>));
  static_assert(exact_for_bounded_inputs(
      product_plan<geometric_product, rga3, motor_layout, point_layout>));
  static_assert(exact_for_bounded_inputs(
      product_plan<geometric_antiproduct, rga3, motor_layout, point_layout>));
  static_assert(exact_for_bounded_inputs(
      product_plan<geometric_product, cga3, conformal_vector_layout,
                   conformal_vector_layout>));

  const std::span<const golden::product> rigid = golden::gp_tables::rigid_3;
  const std::span<const golden::product> conformal =
      golden::gp_tables::conformal_3;
  const std::span<const golden::complement_pair> complements =
      golden::complements::n4;
  const std::array<std::int64_t, 4> plane_re{-5, 0, 12, 16};
  const std::array<std::int64_t, 4> plane_im{8, -16, 1, -3};
  CHECK(exact_in<geometric_product, T, Point<T>, Point<T>>(
      rigid, 4, {}, point_re, imaginary<T>(point_im), other_re,
      imaginary<T>(other_im)));
  CHECK(exact_in<geometric_product, T, Line<T>, Plane<T>>(
      rigid, 4, {}, line_re, imaginary<T>(line_im), plane_re,
      imaginary<T>(plane_im)));
  CHECK(exact_in<geometric_product, T, Motor<T>, Point<T>>(
      rigid, 4, {}, motor_re, imaginary<T>(motor_im), point_re,
      imaginary<T>(point_im)));
  CHECK(exact_in<geometric_antiproduct, T, Motor<T>, Point<T>>(
      rigid, 4, complements, motor_re, imaginary<T>(motor_im), point_re,
      imaginary<T>(point_im)));
  CHECK(exact_in<geometric_product, T, ConformalVector<T>, ConformalVector<T>>(
      conformal, 5, {}, vector_re, imaginary<T>(vector_im), other_vector_re,
      imaginary<T>(other_vector_im)));
}

namespace {

// Diagonal forms whose products have large coefficients: in a diagonal
// form e12 e12 = -g11 g22 and e1234 e1234 = +g11 g22 g33 g44.
template <int G1, int G2>
struct diagonal2 {
  static constexpr auto value = core::from_gram({
      {G1, 0},
      {0, G2},
  });
};
template <int G1, int G2, int G3, int G4>
struct diagonal4 {
  static constexpr auto value = core::from_gram({
      {G1, 0, 0, 0},
      {0, G2, 0, 0},
      {0, 0, G3, 0},
      {0, 0, 0, G4},
  });
};

// Whether the plan of e12 e12 in diag(G1, G2), or of e1234 e1234 in
// diag(G1, G2, G3, G4), has only coefficients that T holds exactly.
template <int G1, int G2, class T>
constexpr bool e12_exact =
    core::detail::coefficients_exact<BinaryOperation::geometric_product,
                                     diagonal2<G1, G2>,
                                     core::blades<diagonal2<G1, G2>, "e12">,
                                     core::blades<diagonal2<G1, G2>, "e12">, T>;
template <int G1, int G2, int G3, int G4, class T>
constexpr bool e1234_exact = core::detail::coefficients_exact<
    BinaryOperation::geometric_product, diagonal4<G1, G2, G3, G4>,
    core::blades<diagonal4<G1, G2, G3, G4>, "e1234">,
    core::blades<diagonal4<G1, G2, G3, G4>, "e1234">, T>;

// A scalar whose std::numeric_limits describe a binary floating type of 11
// digits and an exponent range up to 2^16, the half precision of IEEE 754,
// so that the check's exponent bound can be seen; it computes in float, and
// the check reads only its limits.
struct half_like {
  float value = 0;
  half_like() = default;
  explicit half_like(std::int64_t v) : value(static_cast<float>(v)) {}
  [[maybe_unused]] friend half_like operator+(half_like a, half_like b) {
    return half_like{a.value + b.value};
  }
  [[maybe_unused]] friend half_like operator-(half_like a, half_like b) {
    return half_like{a.value - b.value};
  }
  [[maybe_unused]] friend half_like operator*(half_like a, half_like b) {
    return half_like{a.value * b.value};
  }
  [[maybe_unused]] friend half_like operator-(half_like a) {
    return half_like{-a.value};
  }

private:
  explicit half_like(float v) : value(v) {}
};

} // namespace

/// half_like reads as the half precision of IEEE 754.
template <>
struct std::numeric_limits<half_like> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_exact = false;
  static constexpr int radix = 2;
  static constexpr int digits = 11;
  static constexpr int max_exponent = 16;
};

namespace {

// The scalar of e1234 e1234 in diag(G1, G2, G3, G4), computed by apply over
// T.
template <int G1, int G2, int G3, int G4, class T>
constexpr T e1234_squared() {
  using Diagonal = diagonal4<G1, G2, G3, G4>;
  using E1234 = core::mv<Diagonal, core::blades<Diagonal, "e1234">, T>;
  return get<"1">(
      apply<BinaryOperation::geometric_product>(E1234{T(1)}, E1234{T(1)}));
}

// The scalar of e12 e12 in diag(G1, G2), computed by apply over T.
template <int G1, int G2, class T>
constexpr T e12_squared() {
  using E12 =
      core::mv<diagonal2<G1, G2>, core::blades<diagonal2<G1, G2>, "e12">, T>;
  return get<"1">(
      apply<BinaryOperation::geometric_product>(E12{T(1)}, E12{T(1)}));
}

} // namespace

// Reference: independent value: by hand, e12 e12 = -g11 g22 and e1234 e1234
//   = g11 g22 g33 g44 in a diagonal form, 4096^2 = 2^24, 24929 * 673 =
//   16,777,217 = 2^24 + 1, 4095 * 4097 = 2^24 - 1, 8192 * 4096 = 2^25,
//   8192^3 * 16384 = 2^53, and 32767^4 = 1,152,780,773,560,811,521 is odd
//   and lies between 2^53 and 2^63, and a binary float of p digits holds an
//   integer exactly when its odd part has at most p bits, so float (24
//   digits) holds 2^24 - 1, 2^24, and 2^25 and not 2^24 + 1, double (53)
//   holds 2^53 and not 32767^4, and long double (64 digits on x86-64) every
//   one of them, and short holds -32768 = -(128 * 256) to 32767 = 7 * 31 *
//   151 = 217 * 151 and not -32769 = -(9 * 3641) or 32768, and a half
//   precision float (11 digits, values below 2^16) holds 2047 and 2^15 and
//   not 2049 or 2^16; contract: a
//   product whose plan has a coefficient that the scalar type cannot hold
//   exactly is rejected
TEST_CASE("apply checks that the scalar holds the plan's coefficients") {
  // 2^24 and 2^53, written out.
  constexpr std::int64_t two_to_24 = 16'777'216;
  constexpr std::int64_t two_to_53 = 9'007'199'254'740'992;
  static_assert(std::int64_t{4096} * 4096 == two_to_24);
  static_assert(std::int64_t{24929} * 673 == two_to_24 + 1);
  static_assert(std::int64_t{8192} * 8192 * 8192 * 16384 == two_to_53);
  static_assert(std::int64_t{32767} * 32767 * 32767 * 32767 ==
                1'152'780'773'560'811'521);
  static_assert(217 * 151 == 32767 && 7 * 31 * 151 == 32767);
  static_assert(128 * 256 == 32768 && 9 * 3641 == 32769);
  static_assert(std::int64_t{4095} * 4097 == two_to_24 - 1);
  static_assert(std::int64_t{8192} * 4096 == 2 * two_to_24);

  static_assert(e12_exact<4096, 4096, float>);
  static_assert(e12_exact<4095, 4097, float>);
  static_assert(e12_exact<8192, 4096, float>);
  static_assert(!e12_exact<24929, 673, float>);
  static_assert(e12_exact<24929, 673, double>);
  static_assert(e12_exact<24929, 673, int>);
  static_assert(e12_exact<24929, 673, std::int64_t>);
  static_assert(e1234_exact<8192, 8192, 8192, 16384, double>);
  static_assert(!e1234_exact<32767, 32767, 32767, 32767, double>);
  static_assert(e1234_exact<32767, 32767, 32767, 32767, std::int64_t>);
  static_assert(e1234_exact<32767, 32767, 32767, 32767, rational>);
  static_assert(e1234_exact<32767, 32767, 32767, 32767, long double>);
  static_assert(e12_exact<24929, 673, long double>);
  // Half precision: 2047 = 23 * 89 has 11 bits and 2049 = 3 * 683 twelve;
  // 2^15 = 128 * 256 is below 2^16 and 2^16 = 256 * 256 is not.
  static_assert(23 * 89 == 2047 && 3 * 683 == 2049);
  static_assert(e12_exact<23, 89, half_like>);
  static_assert(!e12_exact<3, 683, half_like>);
  static_assert(e12_exact<128, 256, half_like>);
  static_assert(!e12_exact<256, 256, half_like>);
  static_assert(!e12_exact<4096, 4096, short>);
  static_assert(e12_exact<217, 151, short>);
  static_assert(e12_exact<128, 256, short>);
  static_assert(!e12_exact<9, 3641, short>);
  static_assert(e1234_exact<1, 7, 31, 151, short>);
  static_assert(!e1234_exact<1, 1, 128, 256, short>);
  static_assert(e12_exact<4096, 4096, std::complex<double>>);

  static_assert(e12_squared<4096, 4096, float>() == -16777216.0F);
  static_assert(e12_squared<24929, 673, double>() == -16777217.0);
  static_assert(e12_squared<217, 151, short>() == -32767);
  static_assert(e12_squared<128, 256, short>() == -32768);
  static_assert(e12_squared<8192, 4096, float>() == -33554432.0F);
  static_assert(e12_squared<24929, 673, long double>() == -16777217.0L);
  static_assert(e1234_squared<8192, 8192, 8192, 16384, double>() ==
                9007199254740992.0);
  static_assert(e1234_squared<32767, 32767, 32767, 32767, std::int64_t>() ==
                1'152'780'773'560'811'521);
  CHECK(e12_squared<4096, 4096, float>() == -16777216.0F);
  CHECK(e12_squared<24929, 673, std::int64_t>() == -16777217);
  // The same answers at run time, for runs that set the assertions aside.
  CHECK(e12_exact<4096, 4096, float>);
  CHECK_FALSE(e12_exact<24929, 673, float>);
  CHECK(e12_exact<24929, 673, double>);
  CHECK(e1234_exact<8192, 8192, 8192, 16384, double>);
  CHECK_FALSE(e1234_exact<32767, 32767, 32767, 32767, double>);
  CHECK(e1234_exact<32767, 32767, 32767, 32767, rational>);
  CHECK_FALSE(e12_exact<4096, 4096, short>);
  CHECK(e12_exact<217, 151, short>);
  CHECK(e12_exact<128, 256, short>);
  CHECK_FALSE(e12_exact<9, 3641, short>);
  CHECK(e1234_exact<1, 7, 31, 151, short>);
  CHECK_FALSE(e1234_exact<1, 1, 128, 256, short>);
  CHECK(e12_exact<4095, 4097, float>);
  CHECK(e12_exact<8192, 4096, float>);
  CHECK(e1234_exact<32767, 32767, 32767, 32767, long double>);
  CHECK(e12_squared<24929, 673, long double>() == -16777217.0L);
  CHECK(e12_exact<128, 256, half_like>);
  CHECK_FALSE(e12_exact<256, 256, half_like>);
  CHECK_FALSE(e12_exact<3, 683, half_like>);
}

namespace {

// More layouts of rigid(3): a flector, holding the odd blades of a point and
// a plane, and a line whose bivectors all have the other orientation.
inline constexpr auto flector_layout =
    core::blades<rga3, "e1 e2 e3 e4 e423 e431 e412 e321">;
inline constexpr auto other_line_layout =
    core::blades<rga3, "e14 e24 e34 e32 e31 e21">;
template <class T>
using Flector = core::mv<rga3, flector_layout, T>;
template <class T>
using OtherLine = core::mv<rga3, other_line_layout, T>;
// A point of R(4,0,0), which has the blades of a rigid(3) point.
template <class T>
using EuclideanPoint =
    core::mv<euclidean<4>, core::blades<euclidean<4>, "e1 e2 e3 e4">, T>;

// x moved onto the layout of To by the test's own reading of the rule: the
// coefficient c on s e_M is c s s' on s' e_M, where To holds e_M with the
// sign s'; the blades of To that x lacks are zero.
template <class To, class From>
constexpr To converted_by_hand(const From& x) {
  using T = From::scalar_type;
  To out{};
  for (std::size_t i = 0; i < From::component_count; ++i) {
    for (std::size_t j = 0; j < To::component_count; ++j) {
      const core::oriented_blade from = From::layout.blades.at(i);
      const core::oriented_blade to = To::layout.blades.at(j);
      if (from.mask == to.mask) {
        out.coefficients.at(j) = from.sign * to.sign == 1
                                     ? x.coefficients.at(i)
                                     : T(-x.coefficients.at(i));
      }
    }
  }
  return out;
}

// Whether To{x}, To(x), and static_cast<To>(x) all give the value the rule
// gives, for x with the coefficients 1, 2, ..., K.
template <class To, class From>
constexpr bool converts() {
  using T = From::scalar_type;
  From x{};
  for (std::size_t i = 0; i < From::component_count; ++i) {
    x.coefficients.at(i) = scalar_of<T>(static_cast<int>(i) + 1);
  }
  const To expected = converted_by_hand<To>(x);
  return To{x} == expected && To(x) == expected &&
         static_cast<To>(x) == expected;
}

} // namespace

// Reference: contract: a conversion to a layout that holds every blade of the
//   source puts each coefficient on its blade, multiplied by the product of
//   the blade's signs in the two layouts, since c s e_M = (c s s') s' e_M,
//   with the other blades of the target zero, in all three spellings, for
//   every pair of the rigid(3) layouts of a scalar, point, line, plane,
//   motor, flector, and a line of the other orientations, where the target
//   holds the source, and an implicit conversion between layouts does not
//   compile; contract: a source of another scalar type or another algebra
//   reaches a constructor of its own, which rejects it with a message, so
//   generic code that probes the construction is told yes
TEST_CASE_TEMPLATE("A conversion between layouts keeps every coefficient", T,
                   std::int64_t, rational, float, double,
                   std::complex<double>) {
  static_assert(converts<Motor<T>, Scalar<T>>());
  static_assert(converts<Motor<T>, Line<T>>());
  static_assert(converts<Motor<T>, OtherLine<T>>());
  static_assert(converts<Line<T>, OtherLine<T>>());
  static_assert(converts<OtherLine<T>, Line<T>>());
  static_assert(converts<Flector<T>, Point<T>>());
  static_assert(converts<Flector<T>, Plane<T>>());
  static_assert(converts<Motor<T>, Motor<T>>());
  constexpr auto k = [](int i) { return scalar_of<T>(i); };
  static_assert(Line<T>{OtherLine<T>{k(1), k(2), k(3), k(4), k(5), k(6)}} ==
                Line<T>{k(-1), k(-2), k(-3), k(-4), k(5), k(-6)});

  static_assert(!std::is_convertible_v<Line<T>, Motor<T>>);
  static_assert(!std::is_convertible_v<Point<T>, Flector<T>>);
  static_assert(std::is_constructible_v<Line<T>, Line<rational>>);
  static_assert(std::is_constructible_v<Point<T>, EuclideanPoint<T>>);
  static_assert(!std::is_convertible_v<Line<rational>, Line<T>> ||
                std::same_as<T, rational>);
  static_assert(!std::is_convertible_v<EuclideanPoint<T>, Point<T>>);

  CHECK(converts<Motor<T>, Line<T>>());
  CHECK(converts<Flector<T>, Plane<T>>());
  CHECK(converts<Line<T>, OtherLine<T>>());
}

// Reference: independent value: the canonical blades of point ^ point are
//   e12, e13, e14, e23, e24, e34, and on a line of e41 e42 e43 e23 e31 e12
//   the wedge of p = (1, 2, 3, 1) and q = (4, 5, 6, 1) is (3, 3, 3, -3, 6,
//   -3), the direction q - p and the moment p x q; contract: the result of
//   a product converts to a named layout that holds its blades
TEST_CASE("A product's result converts to a named layout") {
  constexpr auto wedge = apply<BinaryOperation::wedge>(
      Point<double>{1, 2, 3, 1}, Point<double>{4, 5, 6, 1});
  static_assert(Line<double>{wedge} == Line<double>{3, 3, 3, -3, 6, -3});
  static_assert(Line<double>(wedge) == static_cast<Line<double>>(wedge));
  static_assert(Motor<double>{wedge} ==
                Motor<double>{0, 3, 3, 3, -3, 6, -3, 0});
  static_assert(converts<Line<double>, std::remove_cvref_t<decltype(wedge)>>());
  CHECK(Line<double>{wedge} == Line<double>{3, 3, 3, -3, 6, -3});
}

namespace {

// Whether the result of each binary operation of A and B, onto a layout
// Target that holds it, equals the canonical result converted to Target.
template <class Target, class A, class B>
constexpr bool onto_equals_converted(const A& a, const B& b) {
  using enum BinaryOperation;
  constexpr auto layout = Target::layout;
  return apply<geometric_product, layout>(a, b) ==
             Target{apply<geometric_product>(a, b)} &&
         apply<wedge, layout>(a, b) == Target{apply<wedge>(a, b)} &&
         apply<geometric_antiproduct, layout>(a, b) ==
             Target{apply<geometric_antiproduct>(a, b)} &&
         apply<antiwedge, layout>(a, b) == Target{apply<antiwedge>(a, b)};
}

// A layout of every blade of rigid(3), named as the published formulas
// name them, which holds every result.
inline constexpr auto every_layout = core::blades<
    rga3, "1 e1 e2 e3 e4 e41 e42 e43 e23 e31 e12 e423 e431 e412 e321 e1234">;
template <class T>
using Every = core::mv<rga3, every_layout, T>;

} // namespace

// Reference: proof: for the rigid(3) pairs of point and point, line and
//   plane, motor and point, point and motor, and line and line, over every
//   binary operation, the product written onto a named layout that holds
//   its result equals the canonical result converted to that layout, as do
//   the even results onto a motor and the odd onto a flector
TEST_CASE("apply onto a target layout equals the converted result") {
  constexpr Point<double> p{1, 2, 3, 1};
  constexpr Point<double> q{4, -5, 6, 2};
  constexpr Line<double> l{1, -2, 3, 4, 5, -6};
  constexpr Plane<double> g{2, 0, -1, 3};
  constexpr Motor<double> m{1, 2, -3, 4, 5, 6, -7, 8};
  static_assert(onto_equals_converted<Every<double>>(p, q));
  static_assert(onto_equals_converted<Every<double>>(l, g));
  static_assert(onto_equals_converted<Every<double>>(m, p));
  static_assert(onto_equals_converted<Every<double>>(p, m));
  static_assert(onto_equals_converted<Every<double>>(l, l));
  static_assert(apply<BinaryOperation::geometric_product, motor_layout>(p, q) ==
                Motor<double>{apply<BinaryOperation::geometric_product>(p, q)});
  static_assert(
      apply<BinaryOperation::geometric_product, flector_layout>(m, p) ==
      Flector<double>{apply<BinaryOperation::geometric_product>(m, p)});
  static_assert(apply<UnaryOperation::right_complement, plane_layout>(p) ==
                Plane<double>{apply<UnaryOperation::right_complement>(p)});

  const volatile double one = 1.0;
  const Motor<double> mv{one, 2, -3, 4, 5, 6, -7, 8};
  CHECK(onto_equals_converted<Every<double>>(mv, p));
  CHECK(apply<BinaryOperation::geometric_product, flector_layout>(mv, p) ==
        Flector<double>{apply<BinaryOperation::geometric_product>(mv, p)});
}

// Reference: contract: the wedge of a line and a plane of rigid(3) has no
//   term, since each pair of their blades shares a generator, so its result
//   holds no coefficient, it is made by default, and two such results are
//   equal
TEST_CASE("apply gives an empty result where no term exists") {
  constexpr auto none = apply<BinaryOperation::wedge>(
      Line<double>{1, 2, 3, 4, 5, 6}, Plane<double>{1, 2, 3, 4});
  static_assert(decltype(none)::component_count == 0);
  static_assert(none == std::remove_cvref_t<decltype(none)>{});
  CHECK(none.coefficients.empty());
  CHECK(none == std::remove_cvref_t<decltype(none)>{});
}

namespace {

// A value on the layout e42 e43, and values on e1 alone and on e2 alone, of
// rigid(3).
template <class T>
using E42E43 = core::mv<rga3, core::blades<rga3, "e42 e43">, T>;
template <class T>
using E1 = core::mv<rga3, core::blades<rga3, "e1">, T>;
template <class T>
using E2 = core::mv<rga3, core::blades<rga3, "e2">, T>;

// Constant evaluations of the code that the two cases below run, with the
// same template arguments, over the test rational, whose zero 0/1 is not all
// zero bits. GCC 16.2.1 was seen to compile a zero array made at run time
// wrongly once a constant evaluation of the same declaration had written
// first into place 2 of it or a later place: from that place on, the
// run-time zero held zero bits, which for the rational is 0/0. The
// conversion's first write lands on place 2 of the motor, and the wedge's on
// place 6. Each is a constexpr variable at namespace scope, so that the
// evaluation takes place whatever becomes of the cases' assertions.
constexpr Motor<rational> e42_e43_on_motor =
    Motor<rational>{E42E43<rational>{rational{1}, rational{2}}};
constexpr Motor<rational> e1_wedge_e2_on_motor =
    apply<BinaryOperation::wedge, motor_layout>(E1<rational>{rational{3}},
                                                E2<rational>{rational{5}});

} // namespace

// Reference: contract: a conversion to a layout that holds every blade of
//   the source puts each coefficient on its blade, multiplied by the
//   product of the blade's signs in the two layouts, and gives zero on the
//   other blades, also at run time after a constant evaluation of the same
//   conversion in the program: e42 and e43 stand for -e24 and -e34 in both
//   layouts, so each product of signs is +1, and 1 e42 + 2 e43 on the motor
//   layout 1 e41 e42 e43 e23 e31 e12 e1234 is (0, 0, 1, 2, 0, 0, 0, 0), each
//   zero 0/1
TEST_CASE("A conversion is zero on the blades its source lacks after a "
          "constant evaluation of it") {
  constexpr Motor<rational> expected{rational{0}, rational{0}, rational{1},
                                     rational{2}, rational{0}, rational{0},
                                     rational{0}, rational{0}};
  static_assert(e42_e43_on_motor == expected);

  const volatile std::int64_t one = 1;
  const Motor<rational> m{E42E43<rational>{rational{one}, rational{2}}};
  CHECK(m == expected);
  for (std::size_t place = 0; place < m.coefficients.size(); ++place) {
    CAPTURE(place);
    CHECK(m.coefficients.at(place).denominator == 1);
  }
}

// Reference: contract: apply onto a target layout gives each blade of the
//   target the sum of the terms on it, and zero on a blade that no term
//   reaches, also at run time after a constant evaluation of the same
//   product in the program: e1 ^ e2 = e12, which the motor layout holds at
//   place 6 with sign +1, so (3 e1) ^ (5 e2) on it is (0, 0, 0, 0, 0, 0, 15,
//   0), each zero 0/1, and no term reaches e1234 at place 7
TEST_CASE("apply onto a wider target is zero where no term reaches after a "
          "constant evaluation of it") {
  constexpr Motor<rational> expected{rational{0},  rational{0}, rational{0},
                                     rational{0},  rational{0}, rational{0},
                                     rational{15}, rational{0}};
  static_assert(e1_wedge_e2_on_motor == expected);

  const volatile std::int64_t three = 3;
  const Motor<rational> m = apply<BinaryOperation::wedge, motor_layout>(
      E1<rational>{rational{three}}, E2<rational>{rational{5}});
  CHECK(m == expected);
  for (std::size_t place = 0; place < m.coefficients.size(); ++place) {
    CAPTURE(place);
    CHECK(m.coefficients.at(place).denominator == 1);
  }
}

namespace {

// A motor with nothing but a line in it, and one with a scalar and an
// antiscalar as well, over T.
template <class T>
constexpr Motor<T> motor_of_line(int scalar, int antiscalar) {
  const auto k = [](int i) { return scalar_of<T>(i); };
  return Motor<T>{k(scalar), k(1), k(2), k(3), k(4), k(5), k(6), k(antiscalar)};
}

template <class T>
constexpr Line<T> counting_line() {
  const auto k = [](int i) { return scalar_of<T>(i); };
  return Line<T>{k(1), k(2), k(3), k(4), k(5), k(6)};
}

} // namespace

// Reference: contract: truncate keeps each blade of the target that the
//   input holds, with the sign of the conversion, and gives zero for a blade
//   of the target that the input lacks, and truncate<Line>(m) and
//   truncate<line_layout>(m) are the same, at compile time and at run time,
//   while over a rounding scalar a nonzero dropped coefficient is kept
//   out without a check
TEST_CASE_TEMPLATE("truncate keeps the blades of its target", T, std::int64_t,
                   rational, float, double, std::complex<double>) {
  static_assert(core::truncate<Line<T>>(motor_of_line<T>(0, 0)) ==
                counting_line<T>());
  static_assert(core::truncate<line_layout>(motor_of_line<T>(0, 0)) ==
                counting_line<T>());
  static_assert(core::truncate<const Line<T>&>(motor_of_line<T>(0, 0)) ==
                counting_line<T>());
  static_assert(core::truncate<OtherLine<T>>(motor_of_line<T>(0, 0)) ==
                OtherLine<T>{counting_line<T>()});
  constexpr auto k = [](int i) { return scalar_of<T>(i); };
  static_assert(core::truncate<Flector<T>>(Point<T>{k(1), k(2), k(3), k(4)}) ==
                Flector<T>{Point<T>{k(1), k(2), k(3), k(4)}});
  static_assert(core::truncate<Line<T>>(Motor<T>{}) == Line<T>{});
  static_assert(std::same_as<decltype(core::truncate<const Line<T>&>(
                                 motor_of_line<T>(0, 0))),
                             Line<T>>);

  const volatile int one = 1;
  const Motor<T> motor = motor_of_line<T>(0 * one, 0);
  CHECK(core::truncate<Line<T>>(motor) == counting_line<T>());
  CHECK(core::truncate<line_layout>(motor) == counting_line<T>());
  const Point<T> point{k(one), k(2), k(3), k(4)};
  CHECK(core::truncate<Flector<T>>(point) == Flector<T>{point});
  if constexpr (!core::exact_scalar<T>) {
    const Motor<T> nonzero = motor_of_line<T>(7 * one, 1);
    CHECK(core::truncate<Line<T>>(nonzero) == counting_line<T>());
  }
}

namespace {

// Whether truncating the motor with the given scalar and antiscalar to a
// line is a constant expression: a constant evaluation that fails, as a
// failed check fails it, makes the template argument invalid, so the answer
// is no.
template <class T, int Scalar, int Antiscalar = 0>
concept truncates_in_a_constant_evaluation = requires {
  typename std::integral_constant<
      bool, core::truncate<line_layout>(motor_of_line<T>(Scalar, Antiscalar)) ==
                counting_line<T>()>;
};

} // namespace

// Reference: contract: truncate checks that the part it drops is zero, for
//   an exact scalar, in every constant evaluation, so dropping a nonzero
//   scalar or a nonzero antiscalar of a motor, the first and the last
//   dropped places, over std::int64_t or the test rational is no constant
//   expression, while over a rounding scalar nothing is checked
TEST_CASE("truncate checks what it drops over an exact scalar") {
  static_assert(truncates_in_a_constant_evaluation<std::int64_t, 0>);
  static_assert(!truncates_in_a_constant_evaluation<std::int64_t, 7>);
  static_assert(truncates_in_a_constant_evaluation<rational, 0>);
  static_assert(!truncates_in_a_constant_evaluation<rational, 7>);
  static_assert(!truncates_in_a_constant_evaluation<std::int64_t, 0, 7>);
  static_assert(!truncates_in_a_constant_evaluation<rational, 0, 7>);
  static_assert(truncates_in_a_constant_evaluation<double, 0, 7>);
  static_assert(truncates_in_a_constant_evaluation<double, 7>);
  static_assert(truncates_in_a_constant_evaluation<float, 7>);
  static_assert(truncates_in_a_constant_evaluation<std::complex<double>, 7>);
  CHECK_FALSE(truncates_in_a_constant_evaluation<std::int64_t, 7>);
  CHECK_FALSE(truncates_in_a_constant_evaluation<std::int64_t, 0, 7>);
  CHECK(truncates_in_a_constant_evaluation<double, 7>);
}

namespace {

// A conformal(3) layout that holds a vector's blades, in another order and
// with e54 = -e45 beside them.
inline constexpr auto conformal_wide_layout =
    core::blades<cga3, "e5 e4 e3 e2 e1 e54">;
template <class T>
using ConformalWide = core::mv<cga3, conformal_wide_layout, T>;

// Whether truncating a wide conformal value over std::int64_t whose e54 is
// E54 back to a vector is a constant expression.
template <int E54>
concept conformal_truncates = requires {
  typename std::integral_constant<
      bool, core::truncate<conformal_vector_layout>(
                ConformalWide<std::int64_t>{5, 4, 3, 2, 1, E54}) ==
                ConformalVector<std::int64_t>{1, 2, 3, 4, 5}>;
};

} // namespace

// Reference: contract: in conformal(3) a vector converts to a layout that
//   holds its blades, each coefficient moving to the place of its blade,
//   and truncating back gives the vector, with e54 = -e45 dropped only when
//   it is zero, at compile time and, from values read through volatile, at
//   run time alike
TEST_CASE("Conversion and truncate agree at compile time and run time in "
          "conformal(3)") {
  constexpr ConformalVector<double> v{1, 2, 3, 4, 5};
  constexpr ConformalWide<double> wide{v};
  static_assert(wide == ConformalWide<double>{5, 4, 3, 2, 1, 0});
  static_assert(core::truncate<ConformalVector<double>>(wide) == v);
  static_assert(core::truncate<conformal_vector_layout>(
                    ConformalWide<std::int64_t>{5, 4, 3, 2, 1, 0}) ==
                ConformalVector<std::int64_t>{1, 2, 3, 4, 5});
  static_assert(conformal_truncates<0>);
  static_assert(!conformal_truncates<7>);

  const volatile double one = 1.0;
  const ConformalVector<double> vv{one, 2 * one, 3 * one, 4 * one, 5 * one};
  const ConformalWide<double> wide_at_run_time{vv};
  CHECK(wide_at_run_time == wide);
  CHECK(core::truncate<ConformalVector<double>>(wide_at_run_time) == v);
  CHECK(core::truncate<conformal_vector_layout>(
            ConformalWide<double>{5, 4, 3, 2, 1, one}) == v);
}

// Reference: contract: truncate and get are called without a namespace, as
//   argument-dependent lookup finds them, beside the POSIX truncate of
//   unistd.h and std::get of tuple, for a scalar from namespace std
TEST_CASE("truncate is found beside the names it shares") {
  const Motor<std::complex<double>> motor{};
  const Line<std::complex<double>> line = truncate<line_layout>(motor);
  CHECK(line == Line<std::complex<double>>{});
  CHECK(get<"e41">(truncate<Line<std::complex<double>>>(motor)) ==
        std::complex<double>{});
  const std::tuple<int> one{1};
  CHECK(std::get<0>(one) == 1);
}

// Reference: contract: part keeps each blade of the target that the input
//   holds, with the sign of the conversion, gives zero for a blade of the
//   target that the input lacks, takes the target by its type or by its
//   layout, and drops the rest whatever it holds, so where the dropped part
//   is zero it equals truncate in type and value, and where it is not zero,
//   a motor's scalar 7 and antiscalar 9, it gives the line all the same,
//   over every scalar, at compile time and at run time
TEST_CASE_TEMPLATE("part keeps the blades of its target", T, std::int64_t,
                   rational, float, double, std::complex<double>) {
  static_assert(std::same_as<decltype(core::part<Line<T>>(Motor<T>{})),
                             decltype(core::truncate<Line<T>>(Motor<T>{}))>);
  static_assert(
      std::same_as<decltype(core::part<line_layout>(Motor<T>{})), Line<T>>);
  static_assert(
      std::same_as<decltype(core::part<const Line<T>&>(Motor<T>{})), Line<T>>);
  static_assert(core::part<Line<T>>(motor_of_line<T>(0, 0)) ==
                core::truncate<Line<T>>(motor_of_line<T>(0, 0)));
  static_assert(core::part<line_layout>(motor_of_line<T>(0, 0)) ==
                counting_line<T>());
  static_assert(core::part<OtherLine<T>>(motor_of_line<T>(0, 0)) ==
                OtherLine<T>{counting_line<T>()});
  static_assert(core::part<Motor<T>>(counting_line<T>()) ==
                Motor<T>{counting_line<T>()});
  constexpr auto k = [](int i) { return scalar_of<T>(i); };
  static_assert(core::part<Flector<T>>(Point<T>{k(1), k(2), k(3), k(4)}) ==
                Flector<T>{Point<T>{k(1), k(2), k(3), k(4)}});

  // A dropped part that is not zero.
  static_assert(core::part<Line<T>>(motor_of_line<T>(7, 9)) ==
                counting_line<T>());
  static_assert(core::part<line_layout>(motor_of_line<T>(7, 9)) ==
                counting_line<T>());

  const volatile int one = 1;
  const Motor<T> zero_rest = motor_of_line<T>(0 * one, 0);
  CHECK(core::part<Line<T>>(zero_rest) == core::truncate<Line<T>>(zero_rest));
  CHECK(core::part<line_layout>(zero_rest) ==
        core::truncate<line_layout>(zero_rest));
  const Motor<T> nonzero = motor_of_line<T>(7 * one, 9);
  CHECK(core::part<Line<T>>(nonzero) == counting_line<T>());
  CHECK(core::part<line_layout>(nonzero) == counting_line<T>());
}

namespace {

// Whether taking the part on a line of the motor with the given scalar and
// antiscalar is a constant expression, as truncates_in_a_constant_evaluation
// asks of truncate.
template <class T, int Scalar, int Antiscalar = 0>
concept parts_in_a_constant_evaluation = requires {
  typename std::integral_constant<
      bool, core::part<line_layout>(motor_of_line<T>(Scalar, Antiscalar)) ==
                counting_line<T>()>;
};

} // namespace

// Reference: contract: part never checks what it drops, so over an exact
//   scalar, std::int64_t or the test rational, the part on a line of a motor
//   whose scalar or antiscalar is 7 is a constant expression, while truncate
//   of the same motor fails its check and is not one
TEST_CASE("part never checks what it drops, where truncate does") {
  static_assert(parts_in_a_constant_evaluation<std::int64_t, 0>);
  static_assert(parts_in_a_constant_evaluation<std::int64_t, 7>);
  static_assert(parts_in_a_constant_evaluation<std::int64_t, 0, 7>);
  static_assert(parts_in_a_constant_evaluation<rational, 7>);
  static_assert(parts_in_a_constant_evaluation<rational, 0, 7>);
  static_assert(!truncates_in_a_constant_evaluation<std::int64_t, 7>);
  static_assert(!truncates_in_a_constant_evaluation<std::int64_t, 0, 7>);
  static_assert(!truncates_in_a_constant_evaluation<rational, 7>);
  static_assert(!truncates_in_a_constant_evaluation<rational, 0, 7>);
  CHECK(parts_in_a_constant_evaluation<std::int64_t, 7>);
  CHECK_FALSE(truncates_in_a_constant_evaluation<std::int64_t, 7>);
}

// Reference: contract: part is called without a namespace, as
//   argument-dependent lookup finds it, for a scalar from namespace std
TEST_CASE("part is found by argument-dependent lookup") {
  const Motor<std::complex<double>> motor{};
  const Line<std::complex<double>> line = part<line_layout>(motor);
  CHECK(line == Line<std::complex<double>>{});
  CHECK(get<"e41">(part<Line<std::complex<double>>>(motor)) ==
        std::complex<double>{});
}

namespace {

// A class derived from a line, as a user might declare a named type.
struct DerivedLine : Line<double> {
  using Line<double>::Line;
};

} // namespace

// Reference: contract: a value of a class derived from a multivector copies
//   into its base and converts as its base does, rather than reaching the
//   constructor that rejects a wrong number of values
TEST_CASE("A class derived from a multivector copies and converts") {
  constexpr DerivedLine derived{1, 2, 3, 4, 5, 6};
  constexpr Line<double> copied(derived);
  static_assert(copied == Line<double>{1, 2, 3, 4, 5, 6});
  static_assert(Line<double>{derived} == copied);
  static_assert(Motor<double>{derived} == Motor<double>{copied});
  CHECK(Line<double>(derived) == copied);
}

namespace {

// A class derived from a line over float, of the scalar type that the
// constructor which rejects another scalar type takes.
struct DerivedFloatLine : Line<float> {
  using Line<float>::Line;
};

} // namespace

// Reference: contract: a source of the same algebra and scalar type still
//   reaches the copy and converting constructors, as a const value, a
//   value that is not const, and a temporary, also of a class derived from
//   a multivector, so the constructor that rejects another algebra or
//   scalar type takes none of them, while one coefficient of a one-blade
//   layout is still a coefficient, and the probes of a source of another
//   scalar type answer alike for a class derived from one
TEST_CASE("A source of one algebra and scalar type still converts") {
  const Line<double> line{1, 2, 3, 4, 5, 6};
  // Not const, so that a constructor taking a non-const source would be
  // the one chosen.
  // NOLINTNEXTLINE(misc-const-correctness)
  Line<double> mutable_line{1, 2, 3, 4, 5, 6};
  const Motor<double> expected{0, 1, 2, 3, 4, 5, 6, 0};
  CHECK(Motor<double>{line} == expected);
  CHECK(Motor<double>{mutable_line} == expected);
  CHECK(Motor<double>(Line<double>{1, 2, 3, 4, 5, 6}) == expected);
  CHECK(static_cast<Motor<double>>(mutable_line) == expected);
  CHECK(Line<double>{mutable_line} == line);

  // Not const, as above.
  // NOLINTNEXTLINE(misc-const-correctness)
  DerivedLine derived{1, 2, 3, 4, 5, 6};
  CHECK(Line<double>{derived} == line);
  CHECK(Line<double>(DerivedLine{1, 2, 3, 4, 5, 6}) == line);
  CHECK(Motor<double>{derived} == expected);
  CHECK(static_cast<Motor<double>>(derived) == expected);
  const Line<double> copy = derived;
  CHECK(copy == line);

  static_assert(Scalar<double>{2.0} == Scalar<double>{2.0});
  CHECK(Scalar<double>{2.0}.coefficients[0] == 2.0);

  static_assert(std::is_constructible_v<Line<double>, DerivedFloatLine>);
  static_assert(!std::is_convertible_v<DerivedFloatLine, Line<double>>);
  static_assert(std::is_constructible_v<Line<float>, DerivedFloatLine>);
}

// Reference: contract: the helpers of conversion and of the coefficient
//   check answer as their comments state: a class derived from an mv is
//   taken as a multivector, an array or a complex number is no coefficient
//   of a double layout while an integer is one of a rational layout, an
//   array, a pointer, or a container holds several values and a number does
//   not, std::numeric_limits
//   describes int and short as integer types and float and double as binary
//   floating types, and neither for the test rational or std::complex, and a
//   motor truncated to a line drops the scalar and the antiscalar, its
//   places 0 and 7
TEST_CASE("The helpers of conversion and exactness answer as stated") {
  namespace detail = core::detail;
  static_assert(detail::multivector_or_derived<const DerivedLine&>);
  static_assert(detail::multivector_or_derived<Line<double>>);
  static_assert(!detail::multivector_or_derived<std::array<double, 6>>);
  static_assert(detail::not_a_coefficient<std::array<double, 6>, double>);
  static_assert(!detail::not_a_coefficient<int, double>);
  static_assert(!detail::not_a_coefficient<int, rational>);
  static_assert(detail::not_a_coefficient<std::complex<double>, double>);
  static_assert(detail::holds_several_values<std::array<double, 6>>);
  static_assert(detail::holds_several_values<const double*>);
  // A C array is what the check is for.
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  static_assert(detail::holds_several_values<double (&)[4]>);
  static_assert(!detail::holds_several_values<std::complex<double>>);
  static_assert(!detail::holds_several_values<double>);
  static_assert(!detail::not_a_coefficient<Motor<double>, double>);
  static_assert(detail::integer_limits<int> && detail::integer_limits<short>);
  static_assert(!detail::integer_limits<double> &&
                !detail::integer_limits<rational>);
  static_assert(detail::binary_floating_limits<float> &&
                detail::binary_floating_limits<double>);
  static_assert(!detail::binary_floating_limits<int> &&
                !detail::binary_floating_limits<rational> &&
                !detail::binary_floating_limits<std::complex<double>>);
  static_assert(detail::multivector_of<const Line<double>&, rga3, double>);
  static_assert(!detail::multivector_of<Line<double>, cga3, double>);
  static_assert(!detail::multivector_of<Line<float>, rga3, double>);
  static_assert(!detail::multivector_of<double, rga3, double>);
  static_assert(detail::dropped_positions<motor_layout, line_layout> ==
                std::array<std::size_t, 2>{0, 7});
  static_assert(detail::dropped_positions<line_layout, motor_layout>.empty());
  static_assert(detail::holds_every_mask(motor_layout, line_layout));
  static_assert(!detail::holds_every_mask(line_layout, motor_layout));
  CHECK(detail::holds_every_mask(motor_layout, other_line_layout));
}

namespace {

// An overload set that takes either a point or a span of doubles, as a
// user's function might.
[[maybe_unused]] constexpr int takes(const Point<double>& /*point*/) {
  return 1;
}
[[maybe_unused]] constexpr int takes(std::span<const double> /*values*/) {
  return 2;
}

// Whether a C array of doubles reaches the span overload.
constexpr bool array_takes_the_span() {
  // A C array is the case under test.
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  const double raw[4] = {1, 2, 3, 1};
  return takes(raw) == 2;
}

} // namespace

// Reference: contract: every constructor that takes one argument is
//   explicit, the one that rejects coefficients given as a C array or
//   braced list among them, so a C array of the scalar type converts to no
//   multivector implicitly, and an overload set of a point and a span of
//   doubles called with a C array chooses the span
TEST_CASE("A C array converts to no multivector implicitly") {
  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  static_assert(!std::is_convertible_v<double (&)[4], Point<double>>);
  static_assert(!std::is_convertible_v<const double (&)[4], Point<double>>);
  static_assert(!std::is_convertible_v<double (&)[6], Point<double>>);
  static_assert(!std::is_convertible_v<double (&)[1], Scalar<double>>);
  static_assert(std::is_constructible_v<Point<double>, double (&)[4]>);
  CHECK_FALSE(std::is_convertible_v<double (&)[4], Point<double>>);
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  static_assert(array_takes_the_span());
  CHECK(array_takes_the_span());
}

// Reference: contract: generic code that probes a construction is told yes
//   for each misuse that a constructor rejects with a message, a lossy
//   conversion, a whole array of coefficients, a wrong count, and a source
//   from another algebra or of another scalar type, and no for an implicit
//   conversion, which no constructor takes, since each is explicit
TEST_CASE("Probes of conversion answer as the constructors' comments say") {
  static_assert(std::is_constructible_v<Line<double>, Motor<double>>);
  static_assert(!std::is_convertible_v<Motor<double>, Line<double>>);
  static_assert(std::is_constructible_v<Point<double>, std::array<double, 4>>);
  static_assert(std::is_constructible_v<Line<double>, double, double, double>);
  static_assert(std::is_constructible_v<Point<double>, EuclideanPoint<double>>);
  static_assert(std::is_constructible_v<Line<double>, Line<float>>);
  static_assert(!std::is_convertible_v<EuclideanPoint<double>, Point<double>>);
  static_assert(!std::is_convertible_v<Line<float>, Line<double>>);
  // An integer for a rational, whose constructor from it is explicit, is a
  // coefficient: one is a wrong count for a line, and a one-blade value
  // cannot be made from it implicitly.
  static_assert(std::is_constructible_v<Line<rational>, int>);
  static_assert(!std::is_constructible_v<Scalar<rational>, int>);
  CHECK(std::is_constructible_v<Line<double>, Motor<double>>);
  CHECK(std::is_constructible_v<Line<rational>, int>);
}
