// Tests of galoot/core/linear.hpp: the sum and the difference of two
// multivectors, scaling by a scalar, and negation.
//
// The expected values are written out here from the definitions. A sum adds
// the coefficients that the two operands hold on each blade. Two values of
// one type sum to that type. Two different layouts sum to the blades of
// either, each once, in order of grade and then of index list, with
// orientation +1, so a coefficient c stored on an oriented blade s e_A
// becomes s c on e_A. An oriented name stands for the canonical blade of its
// indices times the parity of their inversions: in rigid(3) e41 = -e14,
// e42 = -e24, e43 = -e34, e31 = -e13, e423 = e234, e431 = -e134,
// e412 = e124, and e321 = -e123. The identities are checked on integer
// coefficients small enough that every sum and product is exact.
#include <cmath>
#include <complex>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <type_traits>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>

#include <doctest/doctest.h>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga2 {
  static constexpr auto value = core::conformal<2>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};

// The objects of rigid(3), named as the published formulas name them, and
// two layouts that hold a line's blades under other orientations.
inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
inline constexpr auto motor_layout =
    core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
inline constexpr auto antiscalar_layout = core::blades<rga3, "e1234">;
inline constexpr auto other_line_layout =
    core::blades<rga3, "e14 e24 e34 e32 e31 e21">;

// Layouts of conformal(3): a vector, a flat point (a point wedged with e5),
// and a vector listed backwards beside e54 = -e45.
inline constexpr auto conformal_vector_layout =
    core::blades<cga3, "e1 e2 e3 e4 e5">;
inline constexpr auto flat_point_layout = core::blades<cga3, "e15 e25 e35 e45">;
inline constexpr auto conformal_wide_layout =
    core::blades<cga3, "e5 e4 e3 e2 e1 e54">;

template <class T>
using Point = core::mv<rga3, point_layout, T>;
template <class T>
using Line = core::mv<rga3, line_layout, T>;
template <class T>
using Plane = core::mv<rga3, plane_layout, T>;
template <class T>
using Motor = core::mv<rga3, motor_layout, T>;
template <class T>
using Antiscalar = core::mv<rga3, antiscalar_layout, T>;
template <class T>
using OtherLine = core::mv<rga3, other_line_layout, T>;
template <class T>
using Bivectors = core::mv<rga3, core::of_grade<4, 2>, T>;
template <class T>
using ConformalVector = core::mv<cga3, conformal_vector_layout, T>;
template <class T>
using FlatPoint = core::mv<cga3, flat_point_layout, T>;
template <class T>
using ConformalWide = core::mv<cga3, conformal_wide_layout, T>;

// The coefficient of x on the canonical blade of this mask: the stored
// coefficient times the orientation of the blade in the layout, or zero when
// the layout lacks the blade.
template <class X>
std::int64_t value_on(const X& x, core::mask_t mask) {
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const core::oriented_blade blade = X::layout.blades.at(k);
    if (blade.mask == mask) {
      return blade.sign * x.coefficients.at(k);
    }
  }
  return 0;
}

// The name of a blade, e followed by the indices of its generators, or 1, so
// that a failure names the blade it is about.
std::string blade_name(core::mask_t mask) {
  if (mask == 0) {
    return "1";
  }
  std::string name = "e";
  for (unsigned i = 0; i < 32; ++i) {
    if (((mask >> i) & 1U) != 0) {
      name += std::to_string(i + 1);
    }
  }
  return name;
}

// Empty when x and y have the same coefficient on every canonical blade that
// either layout holds; otherwise the first blade where they differ, with
// both values. Comparing blade by blade does not need the two layouts to be
// one type.
template <class X, class Y>
std::string difference(const X& x, const Y& y) {
  const auto report = [&](core::mask_t mask) {
    return blade_name(mask) + ": " + std::to_string(value_on(x, mask)) +
           " against " + std::to_string(value_on(y, mask));
  };
  for (const core::oriented_blade& blade : X::layout.blades) {
    if (value_on(x, blade.mask) != value_on(y, blade.mask)) {
      return report(blade.mask);
    }
  }
  for (const core::oriented_blade& blade : Y::layout.blades) {
    if (value_on(x, blade.mask) != value_on(y, blade.mask)) {
      return report(blade.mask);
    }
  }
  return "";
}

template <class X>
X random_integers(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> digit(-9, 9);
  X x{};
  for (auto& c : x.coefficients) {
    c = digit(source);
  }
  return x;
}

template <class X, class Y>
concept can_add = requires(X x, Y y) { x + y; };
template <class X, class Y>
concept can_subtract = requires(X x, Y y) { x - y; };

} // namespace

// Reference: independent value: by hand, the sum and the difference of two
//   values of one type add and subtract their coefficients place by place,
//   for a line and a motor of rigid(3) and a flat point of conformal(3), at
//   compile time and at run time; contract: the result has the operands'
//   type
TEST_CASE("A sum of two values of one type is that type, place by place") {
  using L = Line<std::int64_t>;
  using M = Motor<std::int64_t>;
  using F = FlatPoint<std::int64_t>;
  static_assert(std::same_as<decltype(L{} + L{}), L>);
  static_assert(std::same_as<decltype(L{} - L{}), L>);
  static_assert(std::same_as<decltype(M{} + M{}), M>);
  static_assert(std::same_as<decltype(M{} - M{}), M>);
  static_assert(std::same_as<decltype(F{} + F{}), F>);
  static_assert(std::same_as<decltype(F{} - F{}), F>);

  constexpr L a{1, 2, 3, 4, 5, 6};
  constexpr L b{10, 20, 30, 40, 50, 60};
  static_assert((a + b) == L{11, 22, 33, 44, 55, 66});
  static_assert((b - a) == L{9, 18, 27, 36, 45, 54});
  static_assert((M{1, 2, 3, 4, 5, 6, 7, 8} + M{8, 7, 6, 5, 4, 3, 2, 1}) ==
                M{9, 9, 9, 9, 9, 9, 9, 9});
  static_assert((F{1, 2, 3, 4} - F{4, 3, 2, 1}) == F{-3, -1, 1, 3});

  const volatile std::int64_t one = 1;
  const L c{one, 2, 3, 4, 5, 6};
  CHECK((c + b) == L{11, 22, 33, 44, 55, 66});
  CHECK((c - b) == L{-9, -18, -27, -36, -45, -54});
  const F f{one, 2, 3, 4};
  CHECK((f + f) == F{2, 4, 6, 8});
}

namespace {

// A line plus the antiscalar, and a point plus a plane, of rigid(3), in
// canonical order.
template <class T>
using LineAndAntiscalar =
    core::mv<rga3, core::blades<rga3, "e12 e13 e14 e23 e24 e34 e1234">, T>;
template <class T>
using PointAndPlane =
    core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4 e123 e124 e134 e234">, T>;

template <class Alg>
concept has_sum_layout =
    requires { core::sum_layout<Alg, core::full<2>, core::of_grade<2, 0>>; };

} // namespace

// Reference: independent value: by hand in rigid(3), the line
//   vx e41 + vy e42 + vz e43 + mx e23 + my e31 + mz e12 plus a e1234 holds
//   mz on e12, -my on e13, -vx on e14, mx on e23, -vy on e24, -vz on e34, and
//   a on e1234, and the point x e1 + y e2 + z e3 + w e4 plus the plane
//   gx e423 + gy e431 + gz e412 + gw e321 holds x, y, z, w, then -gw on
//   e123, gz on e124, -gy on e134, and gx on e234; contract: a sum or a
//   difference of two layouts, in either order, has the canonical set of
//   the blades of both, which sum_layout names
TEST_CASE("A sum of two layouts holds the blades of both in canonical order") {
  using L = Line<std::int64_t>;
  using I = Antiscalar<std::int64_t>;
  using LI = LineAndAntiscalar<std::int64_t>;
  using P = Point<std::int64_t>;
  using G = Plane<std::int64_t>;
  using PG = PointAndPlane<std::int64_t>;
  static_assert(std::same_as<decltype(L{} + I{}), LI>);
  static_assert(std::same_as<decltype(I{} + L{}), LI>);
  static_assert(std::same_as<decltype(L{} - I{}), LI>);
  static_assert(std::same_as<decltype(I{} - L{}), LI>);
  static_assert(std::same_as<decltype(P{} + G{}), PG>);
  static_assert(std::same_as<decltype(G{} - P{}), PG>);
  static_assert(core::sum_layout<rga3, line_layout, antiscalar_layout> ==
                core::blades<rga3, "e12 e13 e14 e23 e24 e34 e1234">);
  static_assert(core::sum_layout<rga3, plane_layout, point_layout> ==
                core::blades<rga3, "e1 e2 e3 e4 e123 e124 e134 e234">);
  static_assert(has_sum_layout<rga3>);
  static_assert(!has_sum_layout<int>);

  // vx, vy, vz, mx, my, mz = 1 to 6, and a = 7.
  static_assert((L{1, 2, 3, 4, 5, 6} + I{7}) == LI{6, -5, -1, 4, -2, -3, 7});

  const volatile std::int64_t one = 1;
  const L l{one, 2, 3, 4, 5, 6};
  const I i{7 * one};
  CHECK((l + i) == LI{6, -5, -1, 4, -2, -3, 7});
  CHECK((i + l) == LI{6, -5, -1, 4, -2, -3, 7});
  CHECK((l - i) == LI{6, -5, -1, 4, -2, -3, -7});
  CHECK((i - l) == LI{-6, 5, 1, -4, 2, 3, 7});

  // x, y, z, w = 1 to 4, and gx, gy, gz, gw = 5 to 8.
  const P p{one, 2, 3, 4};
  const G g{5 * one, 6, 7, 8};
  CHECK((p + g) == PG{1, 2, 3, 4, -8, 7, -6, 5});
  CHECK((g + p) == PG{1, 2, 3, 4, -8, 7, -6, 5});
  CHECK((g - p) == PG{-1, -2, -3, -4, -8, 7, -6, 5});
  CHECK((p - g) == PG{1, 2, 3, 4, 8, -7, 6, -5});
}

// Reference: independent value: by hand, a blade that both layouts hold
//   takes the sum of the two coefficients, each times its orientation: a
//   rigid(3) line with 1 to 6 on e41 e42 e43 e23 e31 e12 plus the bivectors
//   with 10 to 60 on e12 e13 e14 e23 e24 e34 holds 16, 15, 29, 44, 48, 57, a
//   line plus the same line listed as e14 e24 e34 e32 e31 e21 with 10 to 60
//   holds -54, -55, 9, -36, 18, 27, and in conformal(3) the vector 1 to 5
//   listed as e5 e4 e3 e2 e1 beside 7 on e54 = -e45, plus the vector
//   10 to 50 on e1 to e5, holds 11, 22, 33, 44, 55, and -7 on e45
TEST_CASE("A blade that both layouts hold adds both coefficients with their "
          "orientations") {
  using L = Line<std::int64_t>;
  using B = Bivectors<std::int64_t>;
  using O = OtherLine<std::int64_t>;
  using V = ConformalVector<std::int64_t>;
  using W = ConformalWide<std::int64_t>;
  using VW =
      core::mv<cga3, core::blades<cga3, "e1 e2 e3 e4 e5 e45">, std::int64_t>;
  static_assert(std::same_as<decltype(L{} + B{}), B>);
  static_assert(std::same_as<decltype(B{} - L{}), B>);
  static_assert(std::same_as<decltype(L{} + O{}), B>);
  static_assert(std::same_as<decltype(W{} + V{}), VW>);
  static_assert(std::same_as<decltype(V{} - W{}), VW>);

  const volatile std::int64_t one = 1;
  const L l{one, 2, 3, 4, 5, 6};
  const B b{10 * one, 20, 30, 40, 50, 60};
  CHECK((l + b) == B{16, 15, 29, 44, 48, 57});
  CHECK((b + l) == B{16, 15, 29, 44, 48, 57});
  CHECK((l - b) == B{-4, -25, -31, -36, -52, -63});
  CHECK((b - l) == B{4, 25, 31, 36, 52, 63});
  const O o{10 * one, 20, 30, 40, 50, 60};
  CHECK((l + o) == B{-54, -55, 9, -36, 18, 27});

  const W w{5 * one, 4, 3, 2, 1, 7};
  const V v{10 * one, 20, 30, 40, 50};
  CHECK((w + v) == VW{11, 22, 33, 44, 55, -7});
  CHECK((v - w) == VW{9, 18, 27, 36, 45, 7});
}

// Reference: contract: a blade that only one operand holds takes that
//   operand's coefficient, or its negation, without an addition to zero, so
//   that over double -0.0 stays -0.0 and +0.0 on a blade of orientation -1
//   becomes -0.0, which an addition to zero would turn into +0.0, since
//   IEEE 754 makes 0.0 + -0.0 equal to +0.0 and 0.0 - 0.0 equal to +0.0
TEST_CASE("A blade that only one operand holds keeps the sign of a zero") {
  const volatile double zero = 0.0;
  const double minus_zero = -zero;
  REQUIRE(std::signbit(minus_zero));
  Line<double> l{};
  get<"e23">(l) = minus_zero;
  get<"e41">(l) = zero;
  const Antiscalar<double> plus{zero};
  const Antiscalar<double> minus{minus_zero};

  // e23 only from the line, -0.0; e14 from the line's +0.0 on e41 = -e14;
  // e1234 only from the antiscalar, -0.0. The line's e12 is +0.0.
  const auto sum = l + minus;
  CHECK(std::signbit(get<"e23">(sum)));
  CHECK(std::signbit(get<"e14">(sum)));
  CHECK(std::signbit(get<"e1234">(sum)));
  CHECK_FALSE(std::signbit(get<"e12">(sum)));
  const auto other_order = minus + l;
  CHECK(std::signbit(get<"e23">(other_order)));
  CHECK(std::signbit(get<"e14">(other_order)));
  CHECK(std::signbit(get<"e1234">(other_order)));

  // The second operand's +0.0 on e1234, subtracted, is -0.0.
  const auto line_minus_plus = l - plus;
  CHECK(std::signbit(get<"e1234">(line_minus_plus)));
  CHECK(std::signbit(get<"e23">(line_minus_plus)));
}

namespace {

// Commutativity, associativity, zero as the identity, and the difference as
// the inverse of the sum, on random values of the layouts A, B, and C of one
// algebra, compared blade by blade.
template <class A, class B, class C>
void check_sum_laws(std::mt19937_64& source) {
  static_assert(std::same_as<decltype(A{} + B{}), decltype(B{} + A{})>);
  for (int k = 0; k < 4; ++k) {
    const A a = random_integers<A>(source);
    const B b = random_integers<B>(source);
    const C c = random_integers<C>(source);
    CHECK(difference(a + b, b + a) == "");
    CHECK(difference((a + b) + c, a + (b + c)) == "");
    CHECK(difference(a + A{}, a) == "");
    CHECK(difference(a - a, A{}) == "");
    CHECK(difference((a + b) - b, a) == "");
    CHECK(difference((a - b) + (b - a), A{}) == "");
    CHECK(difference(a - b, a + (B{} - b)) == "");
  }
}

} // namespace

// Reference: identity: a + b = b + a, (a + b) + c = a + (b + c), a + 0 = a,
//   a - a = 0, (a + b) - b = a, (a - b) + (b - a) = 0, and
//   a - b = a + (0 - b), on random integer multivectors of every blade of
//   rigid(3) and conformal(3) and of mixed layouts of each, whose sums are
//   exact
TEST_CASE("Sums and differences satisfy the laws of a vector space") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  using R = core::mv<rga3, core::full<4>, std::int64_t>;
  using C = core::mv<cga3, core::full<5>, std::int64_t>;
  check_sum_laws<R, R, R>(source);
  check_sum_laws<C, C, C>(source);
  check_sum_laws<Line<std::int64_t>, Point<std::int64_t>, Plane<std::int64_t>>(
      source);
  check_sum_laws<Motor<std::int64_t>, OtherLine<std::int64_t>,
                 Antiscalar<std::int64_t>>(source);
  check_sum_laws<R, Line<std::int64_t>, Motor<std::int64_t>>(source);
  check_sum_laws<ConformalWide<std::int64_t>, FlatPoint<std::int64_t>,
                 ConformalVector<std::int64_t>>(source);
}

namespace {

// a x (b + c) = a x b + a x c and (b + c) x a = b x a + c x a for each
// product x of * ^ % &, on random values, compared blade by blade.
template <class X, class Y, class Z>
void check_distributive(std::mt19937_64& source) {
  for (int k = 0; k < 3; ++k) {
    const X a = random_integers<X>(source);
    const Y b = random_integers<Y>(source);
    const Z c = random_integers<Z>(source);
    CHECK(difference(a * (b + c), (a * b) + (a * c)) == "");
    CHECK(difference((b + c) * a, (b * a) + (c * a)) == "");
    CHECK(difference(a ^ (b + c), (a ^ b) + (a ^ c)) == "");
    CHECK(difference((b + c) ^ a, (b ^ a) + (c ^ a)) == "");
    CHECK(difference(a % (b + c), (a % b) + (a % c)) == "");
    CHECK(difference((b + c) % a, (b % a) + (c % a)) == "");
    CHECK(difference(a & (b + c), (a & b) + (a & c)) == "");
    CHECK(difference((b + c) & a, (b & a) + (c & a)) == "");
  }
}

} // namespace

// Reference: identity: each product is bilinear, so it distributes over a
//   sum from the left and from the right, on random integer multivectors of
//   every blade of rigid(3) and of conformal(2), a form that is not
//   diagonal, and of mixed layouts of rigid(3) and conformal(3)
TEST_CASE("Each product distributes over sums") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  using R = core::mv<rga3, core::full<4>, std::int64_t>;
  using C2 = core::mv<cga2, core::full<4>, std::int64_t>;
  check_distributive<R, R, R>(source);
  check_distributive<C2, C2, C2>(source);
  check_distributive<Motor<std::int64_t>, Line<std::int64_t>,
                     Point<std::int64_t>>(source);
  check_distributive<ConformalVector<std::int64_t>, FlatPoint<std::int64_t>,
                     ConformalWide<std::int64_t>>(source);
}

// Reference: contract: a sum or a difference of two multivectors that
//   differ in algebra or in scalar type has the rejecting overload as its
//   only candidate, so a requires-expression that asks whether it is valid
//   answers yes, while a multivector and a number have no sum or difference
TEST_CASE("The sum overloads take the right operands") {
  using R = core::mv<rga3, core::full<4>, double>;
  using F = core::mv<rga3, core::full<4>, float>;
  using C = core::mv<cga3, core::full<5>, double>;
  static_assert(can_add<R, R>);
  static_assert(can_add<R, Line<double>>);
  static_assert(can_subtract<Line<double>, R>);
  static_assert(can_add<R, F>);
  static_assert(can_add<R, C>);
  static_assert(can_subtract<R, F>);
  static_assert(can_subtract<C, R>);
  static_assert(!can_add<R, double>);
  static_assert(!can_add<double, R>);
  static_assert(!can_subtract<R, int>);
  static_assert(!can_subtract<int, R>);
  CHECK(can_add<R, F>);
  CHECK_FALSE(can_add<R, double>);
}

namespace {

// A class derived from a line, as a user might declare a named type.
struct DerivedLine : Line<std::int64_t> {
  using Line<std::int64_t>::Line;
};

} // namespace

// Reference: contract: an operand of a class derived from a multivector is
//   taken as its base, so its sum has the base's type and value
TEST_CASE("A class derived from a multivector sums as its base") {
  using L = Line<std::int64_t>;
  const volatile std::int64_t one = 1;
  const DerivedLine d{one, 2, 3, 4, 5, 6};
  const L l{10 * one, 20, 30, 40, 50, 60};
  static_assert(std::same_as<decltype(d + l), L>);
  static_assert(std::same_as<decltype(d - d), L>);
  CHECK((d + l) == L{11, 22, 33, 44, 55, 66});
  CHECK((l - d) == L{9, 18, 27, 36, 45, 54});
}

// A scope that sees galoot::core's operators by a using-directive, as a
// user's code may, with types of its own: one without operators and one
// with its own. The library's operators must leave both alone, and the
// standard library's types too.
namespace {
namespace user_code {

using namespace galoot::core;

struct widget {};

struct gadget {
  friend int operator+(gadget /*a*/, gadget /*b*/) { return 1; }
  friend int operator-(gadget /*a*/, gadget /*b*/) { return 2; }
};

template <class X, class Y>
concept has_plus = requires(X x, Y y) { x + y; };
template <class X, class Y>
concept has_minus = requires(X x, Y y) { x - y; };

// Reference: contract: with galoot::core's operators in scope by a
//   using-directive, a type of the user's without + and - still has none, a
//   type with its own keeps them, std::string and std::complex keep theirs,
//   and a multivector with such a type has none
TEST_CASE("The sum operators leave other types alone") {
  using R = mv<rga3, full<4>, double>;
  static_assert(!has_plus<widget, widget>);
  static_assert(!has_minus<widget, widget>);
  static_assert(!has_plus<R, widget>);
  static_assert(!has_minus<widget, R>);
  static_assert(!has_plus<R, gadget>);
  static_assert(std::same_as<decltype(gadget{} + gadget{}), int>);
  static_assert(std::same_as<decltype(gadget{} - gadget{}), int>);
  static_assert(
      std::same_as<decltype(std::string{} + std::string{}), std::string>);
  static_assert(
      std::same_as<decltype(std::complex<double>{} - std::complex<double>{}),
                   std::complex<double>>);
  CHECK((gadget{} + gadget{}) == 1);
  CHECK((gadget{} - gadget{}) == 2);
  static_assert(has_plus<R, R>);
  static_assert(has_minus<R, R>);
}

} // namespace user_code
} // namespace
