// Tests of galoot/core/unary.hpp: the reverse, the antireverse, the grade
// involution, and the right and left complements of a multivector, and the
// operators ~ and !.
//
// The expected values come from the reference's tables of the signs that
// the grade operations give each blade and of the complements of each blade.
// The relations among the operations are checked on random integer
// multivectors, on which every operation here is exact.
#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include <string>
#include <type_traits>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/unary.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>

namespace {

namespace core = galoot::core;

// The unary operations read only the number of generators of an algebra, not
// its metric, so one form of each size serves.
struct euclidean1 {
  static constexpr auto value = core::R<1, 0, 0>();
};
struct euclidean2 {
  static constexpr auto value = core::R<2, 0, 0>();
};
struct euclidean3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
struct euclidean6 {
  static constexpr auto value = core::R<6, 0, 0>();
};
struct euclidean8 {
  static constexpr auto value = core::R<8, 0, 0>();
};
struct mixed12 {
  static constexpr auto value = core::R<6, 6, 0>();
};
struct rigid15 {
  static constexpr auto value = core::R<15, 0, 1>();
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
inline constexpr auto motor_layout =
    core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
// Every blade of rigid(3) under its oriented name.
inline constexpr auto oriented_layout =
    core::blades<rga3, "1 e1 e2 e3 e4 e23 e31 e12 e41 e42 e43 e423 e431 "
                       "e412 e321 e1234">;
// A flat point of conformal(3), a point wedged with e5.
inline constexpr auto flat_point_layout = core::blades<cga3, "e15 e25 e35 e45">;

using Point = core::mv<rga3, point_layout, std::int64_t>;
using Line = core::mv<rga3, line_layout, std::int64_t>;
using Plane = core::mv<rga3, plane_layout, std::int64_t>;
using Motor = core::mv<rga3, motor_layout, std::int64_t>;
using Oriented = core::mv<rga3, oriented_layout, std::int64_t>;
using FlatPoint = core::mv<cga3, flat_point_layout, std::int64_t>;

// Sparse sets of a large algebra, given by their masks: bit i - 1 stands for
// e_i. {1, e1, e_n, e1 e_n} and {e1, e2, e_n}.
template <std::size_t N>
inline constexpr core::mask_t last_generator = core::mask_t{1} << (N - 1);
template <std::size_t N>
inline constexpr core::blade_set<4> sparse_a{
    {
        {
            {.mask = 0, .sign = 1},
            {.mask = 0x1, .sign = 1},
            {.mask = last_generator<N>, .sign = 1},
            {.mask = core::mask_t{0x1} | last_generator<N>, .sign = 1},
        },
    },
};
template <std::size_t N>
inline constexpr core::blade_set<3> sparse_b{
    {
        {
            {.mask = 0x1, .sign = 1},
            {.mask = 0x2, .sign = 1},
            {.mask = last_generator<N>, .sign = 1},
        },
    },
};

// The number of generators of the algebra of a multivector type.
template <class X>
inline constexpr std::size_t generators =
    std::remove_cvref_t<decltype(X::algebra_type::value)>::generator_count;

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

// The name of a blade, e followed by the indices of its generators, or 1,
// with a minus sign for an orientation of -1, so that a failure names the
// blade it is about.
std::string blade_name(core::mask_t mask, int sign = 1) {
  std::string name = sign < 0 ? "-" : "";
  if (mask == 0) {
    return name + "1";
  }
  name += 'e';
  for (unsigned i = 0; i < 32; ++i) {
    if (((mask >> i) & 1U) != 0) {
      name += std::to_string(i + 1);
      if (i >= 9) {
        name += ' ';
      }
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
  const auto differs_at = [&](core::mask_t mask) {
    return value_on(x, mask) != value_on(y, mask);
  };
  const auto report = [&](core::mask_t mask) {
    return blade_name(mask) + ": " + std::to_string(value_on(x, mask)) +
           " against " + std::to_string(value_on(y, mask));
  };
  for (const core::oriented_blade& blade : X::layout.blades) {
    if (differs_at(blade.mask)) {
      return report(blade.mask);
    }
  }
  for (const core::oriented_blade& blade : Y::layout.blades) {
    if (differs_at(blade.mask)) {
      return report(blade.mask);
    }
  }
  return "";
}

// 1, with a failure that shows `what`, when `wrong`; otherwise 0.
int failure_if(bool wrong, const std::string& what) {
  if (wrong) {
    FAIL_CHECK(what);
    return 1;
  }
  return 0;
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

// The number of places, over every basis blade of the layout of X and every
// place of the result, where the reverse, the antireverse, the grade
// involution, or ~ differs from the sign that `signs`, indexed by mask, gives
// the blade. Each keeps the layout of X, and the coefficient at the blade's
// own place is the sign whatever the blade's orientation there, since
// reverse(s e_A) = s reverse(e_A).
template <class X>
int sign_mismatches(std::span<const golden::blade_signs> signs) {
  int mismatches = 0;
  for (std::size_t i = 0; i < X::component_count; ++i) {
    const core::oriented_blade blade = X::layout.blades.at(i);
    const golden::blade_signs& row = signs[blade.mask];
    if (row.blade != blade.mask) {
      mismatches +=
          failure_if(true, "no sign entry for " + blade_name(blade.mask));
      continue;
    }
    X x{};
    x.coefficients.at(i) = 1;
    const X reversed = core::reverse(x);
    const X antireversed = core::antireverse(x);
    const X involuted = core::grade_involution(x);
    const X tilde = ~x;
    for (std::size_t k = 0; k < X::component_count; ++k) {
      const auto want = [&](int sign) { return k == i ? sign : 0; };
      const auto at = [&](const char* op, std::int64_t got) {
        return std::string(op) + " of " + blade_name(blade.mask, blade.sign) +
               " at place " + std::to_string(k) + " is " + std::to_string(got);
      };
      mismatches += failure_if(reversed.coefficients.at(k) != want(row.reverse),
                               at("reverse", reversed.coefficients.at(k)));
      mismatches +=
          failure_if(antireversed.coefficients.at(k) != want(row.antireverse),
                     at("antireverse", antireversed.coefficients.at(k)));
      mismatches +=
          failure_if(involuted.coefficients.at(k) != want(row.involution),
                     at("grade_involution", involuted.coefficients.at(k)));
      mismatches += failure_if(tilde.coefficients.at(k) != want(row.reverse),
                               at("~x", tilde.coefficients.at(k)));
    }
  }
  return mismatches;
}

// The number of coefficients, over every basis blade s e_A of the layout of
// X and every blade of the algebra, where right_complement, left_complement,
// or ! differs from s times the complement that `complements`, indexed by
// mask, gives e_A.
template <class X>
int complement_mismatches(
    std::span<const golden::complement_pair> complements) {
  int mismatches = 0;
  for (std::size_t i = 0; i < X::component_count; ++i) {
    const core::oriented_blade blade = X::layout.blades.at(i);
    const golden::complement_pair& row = complements[blade.mask];
    if (row.blade != blade.mask) {
      mismatches +=
          failure_if(true, "no complement entry for " + blade_name(blade.mask));
      continue;
    }
    X x{};
    x.coefficients.at(i) = 1;
    const auto right = core::right_complement(x);
    const auto left = core::left_complement(x);
    const auto bang = !x;
    static_assert(std::same_as<decltype(bang), decltype(right)>);
    for (core::mask_t m = 0; m < complements.size(); ++m) {
      const auto want = [&](const golden::signed_blade& c) {
        return m == c.blade ? std::int64_t{blade.sign} * c.sign : 0;
      };
      const auto of = [&](const char* op, std::int64_t got) {
        return std::string(op) + "(" + blade_name(blade.mask, blade.sign) +
               ") on " + blade_name(m) + " is " + std::to_string(got);
      };
      mismatches += failure_if(value_on(right, m) != want(row.right),
                               of("right_complement", value_on(right, m)));
      mismatches += failure_if(value_on(left, m) != want(row.left),
                               of("left_complement", value_on(left, m)));
      mismatches += failure_if(value_on(bang, m) != want(row.right),
                               of("!x", value_on(bang, m)));
    }
  }
  return mismatches;
}

// Whether the layout of X is exactly the masks of `order`, each with sign
// +1, in that order.
template <class X>
bool has_canonical_layout(std::span<const golden::mask> order) {
  if (X::component_count != order.size()) {
    return false;
  }
  for (std::size_t k = 0; k < order.size(); ++k) {
    if (X::layout.blades.at(k).mask != order[k] ||
        X::layout.blades.at(k).sign != 1) {
      return false;
    }
  }
  return true;
}

// (-1)^(gr ag) for the blade of `mask` in an algebra of n generators: the
// sign that turns the right complement of the blade into its left one.
int complement_parity(core::mask_t mask, std::size_t n) {
  const int k = std::popcount(mask);
  const int m = static_cast<int>(n) - k;
  return (k * m) % 2 == 0 ? 1 : -1;
}

// x with each coefficient multiplied by complement_parity of its blade.
template <class X>
X by_complement_parity(X x) {
  for (std::size_t k = 0; k < X::component_count; ++k) {
    x.coefficients.at(k) *=
        complement_parity(X::layout.blades.at(k).mask, generators<X>);
  }
  return x;
}

// Checks the relations among the unary operations on random values of X.
template <class X>
void check_relations(std::mt19937_64& source) {
  for (int k = 0; k < 4; ++k) {
    const X x = random_integers<X>(source);
    CHECK(difference(core::reverse(core::reverse(x)), x) == "");
    CHECK(difference(~~x, x) == "");
    CHECK(difference(core::antireverse(core::antireverse(x)), x) == "");
    CHECK(difference(core::grade_involution(core::grade_involution(x)), x) ==
          "");
    CHECK(difference(core::left_complement(core::right_complement(x)), x) ==
          "");
    CHECK(difference(core::right_complement(core::left_complement(x)), x) ==
          "");
    CHECK(difference(core::right_complement(core::right_complement(x)),
                     by_complement_parity(x)) == "");
    CHECK(difference(core::left_complement(core::left_complement(x)),
                     by_complement_parity(x)) == "");
    CHECK(difference(core::antireverse(x),
                     core::left_complement(
                         core::reverse(core::right_complement(x)))) == "");
  }
}

} // namespace

// Reference: independent value: for each blade of the algebras of 3, 4, and
//   5 generators and of rigid(3) stored under its oriented names, where an
//   oriented name lists generators in the order of their product, the reverse
//   of a grade-k blade is (-1)^(k(k-1)/2) times it, the antireverse of a
//   blade of antigrade m is (-1)^(m(m-1)/2) times it, and the grade involution
//   of a grade-k blade is (-1)^k times it, each on the input's own layout, and
//   ~a is the reverse reverse(a)
TEST_CASE("the involutions of each basis blade have the reference signs") {
  CHECK(sign_mismatches<core::mv<euclidean3, core::full<3>, std::int64_t>>(
            golden::grade_signs::n3) == 0);
  CHECK(sign_mismatches<core::mv<rga3, core::full<4>, std::int64_t>>(
            golden::grade_signs::n4) == 0);
  CHECK(sign_mismatches<core::mv<cga3, core::full<5>, std::int64_t>>(
            golden::grade_signs::n5) == 0);

  const auto& names = golden::oriented_names::rigid_3;
  REQUIRE(names.size() == Oriented::component_count);
  for (std::size_t k = 0; k < names.size(); ++k) {
    CAPTURE(names.at(k).name);
    REQUIRE(Oriented::layout.blades.at(k).mask == names.at(k).blade.blade);
    REQUIRE(Oriented::layout.blades.at(k).sign == names.at(k).blade.sign);
  }
  CHECK(sign_mismatches<Oriented>(golden::grade_signs::n4) == 0);
}

// Reference: independent value: for each blade of the algebras of 3, 4, and
//   5 generators and of rigid(3) stored under its oriented names, where an
//   oriented name lists generators in the order of their product, the right
//   and left complements satisfy u ^ right(u) = antiscalar and
//   left(u) ^ u = antiscalar, and !a is the right complement
//   right_complement(a), the result holding the complements in canonical
//   order, since results are ordered by grade, then by index list
TEST_CASE("the complements of each basis blade are the reference complements") {
  using X4 = core::mv<rga3, core::full<4>, std::int64_t>;
  using X5 = core::mv<cga3, core::full<5>, std::int64_t>;
  CHECK(
      complement_mismatches<core::mv<euclidean3, core::full<3>, std::int64_t>>(
          golden::complements::n3) == 0);
  CHECK(complement_mismatches<X4>(golden::complements::n4) == 0);
  CHECK(complement_mismatches<X5>(golden::complements::n5) == 0);

  const auto& names = golden::oriented_names::rigid_3;
  REQUIRE(names.size() == Oriented::component_count);
  for (std::size_t k = 0; k < names.size(); ++k) {
    CAPTURE(names.at(k).name);
    REQUIRE(Oriented::layout.blades.at(k).mask == names.at(k).blade.blade);
    REQUIRE(Oriented::layout.blades.at(k).sign == names.at(k).blade.sign);
  }
  CHECK(complement_mismatches<Oriented>(golden::complements::n4) == 0);

  CHECK(has_canonical_layout<decltype(core::right_complement(X4{}))>(
      golden::result_order::n4));
  CHECK(has_canonical_layout<decltype(core::left_complement(X5{}))>(
      golden::result_order::n5));
  CHECK(has_canonical_layout<decltype(core::right_complement(Oriented{}))>(
      golden::result_order::n4));
}

// Reference: independent value: by hand from the definitions, the reverse,
//   the antireverse, and the grade involution send each blade to itself
//   times a sign, so they keep the input's layout, while a complement holds
//   the complements of the input's blades in canonical order with sign +1:
//   the vectors of rigid(3) go to the trivectors, a line to the bivectors, a
//   plane to the vectors, a motor to the scalar, the six bivectors, and
//   e1234, and a flat point e15 e25 e35 e45 of conformal(3) to e123 e124 e134
//   e234
TEST_CASE("the sign maps keep the layout and the complements are canonical") {
  using Trivectors = core::mv<rga3, core::of_grade<4, 3>, std::int64_t>;
  using Bivectors = core::mv<rga3, core::of_grade<4, 2>, std::int64_t>;
  using Vectors = core::mv<rga3, core::of_grade<4, 1>, std::int64_t>;
  using Even =
      core::mv<rga3, core::blades<rga3, "1 e12 e13 e14 e23 e24 e34 e1234">,
               std::int64_t>;
  using FlatComplement =
      core::mv<cga3, core::blades<cga3, "e123 e124 e134 e234">, std::int64_t>;
  const Point p{};
  const Line l{};
  const Plane g{};
  const Motor m{};
  const FlatPoint f{};
  static_assert(std::same_as<decltype(core::reverse(l)), Line>);
  static_assert(std::same_as<decltype(core::antireverse(l)), Line>);
  static_assert(std::same_as<decltype(core::grade_involution(l)), Line>);
  static_assert(std::same_as<decltype(~l), Line>);
  static_assert(std::same_as<decltype(core::reverse(m)), Motor>);
  static_assert(std::same_as<decltype(core::antireverse(m)), Motor>);
  static_assert(std::same_as<decltype(core::grade_involution(m)), Motor>);
  static_assert(std::same_as<decltype(~m), Motor>);
  static_assert(std::same_as<decltype(core::reverse(f)), FlatPoint>);
  static_assert(std::same_as<decltype(core::antireverse(f)), FlatPoint>);
  static_assert(std::same_as<decltype(core::grade_involution(f)), FlatPoint>);
  static_assert(std::same_as<decltype(~f), FlatPoint>);

  static_assert(std::same_as<decltype(core::right_complement(p)), Trivectors>);
  static_assert(std::same_as<decltype(core::left_complement(p)), Trivectors>);
  static_assert(std::same_as<decltype(!p), Trivectors>);
  static_assert(std::same_as<decltype(core::right_complement(l)), Bivectors>);
  static_assert(std::same_as<decltype(!g), Vectors>);
  static_assert(std::same_as<decltype(core::left_complement(g)), Vectors>);
  static_assert(std::same_as<decltype(!m), Even>);
  static_assert(std::same_as<decltype(core::left_complement(m)), Even>);
  static_assert(std::same_as<decltype(!f), FlatComplement>);
  static_assert(
      std::same_as<decltype(core::left_complement(f)), FlatComplement>);
}

// Reference: independent value: by hand in rigid(3), the right complement of
//   the right complement is (-1)^(gr ag) times the input, so !!p is -p for a
//   point, whose grade 1 and antigrade 3 give -1, and !!l is l for a line,
//   whose grade and antigrade are 2, and the reverse of a line is its
//   negation and of a point the point itself; contract: !a is a
//   multivector, which does not convert to bool
TEST_CASE("! applied twice is the input up to a sign by grade") {
  const Point p{2, 3, 5, 7};
  const Line l{1, -2, 3, -4, 5, -6};
  CHECK(value_on(!!p, 0b0001) == -2);
  CHECK(value_on(!!p, 0b0010) == -3);
  CHECK(value_on(!!p, 0b0100) == -5);
  CHECK(value_on(!!p, 0b1000) == -7);
  CHECK(difference(!!l, l) == "");
  CHECK(~p == p);
  CHECK(~l == Line{-1, 2, -3, 4, -5, 6});
  static_assert(!std::is_constructible_v<bool, decltype(!p)>);
  static_assert(!std::is_convertible_v<decltype(!p), bool>);
}

// Reference: identity: the reverse, the antireverse, and the grade
//   involution are involutions, the left complement of the right complement
//   is the identity and so is the right complement of the left complement,
//   right(right(u)) and left(left(u)) are (-1)^(gr ag) u, and the antireverse
//   is left(reverse(right(u))), on random integer multivectors of every
//   blade of algebras of 1 to 6 generators, of rigid(3) under its oriented
//   names, and of sparse sets in 8, 12, and 16 generators
TEST_CASE("the involutions and complements satisfy their relations") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  check_relations<core::mv<euclidean1, core::full<1>, std::int64_t>>(source);
  check_relations<core::mv<euclidean2, core::full<2>, std::int64_t>>(source);
  check_relations<core::mv<euclidean3, core::full<3>, std::int64_t>>(source);
  check_relations<core::mv<rga3, core::full<4>, std::int64_t>>(source);
  check_relations<core::mv<cga3, core::full<5>, std::int64_t>>(source);
  check_relations<core::mv<euclidean6, core::full<6>, std::int64_t>>(source);
  check_relations<Oriented>(source);
  check_relations<Line>(source);
  check_relations<FlatPoint>(source);
  check_relations<core::mv<euclidean8, sparse_a<8>, std::int64_t>>(source);
  check_relations<core::mv<euclidean8, sparse_b<8>, std::int64_t>>(source);
  check_relations<core::mv<mixed12, sparse_a<12>, std::int64_t>>(source);
  check_relations<core::mv<mixed12, sparse_b<12>, std::int64_t>>(source);
  check_relations<core::mv<rigid15, sparse_a<16>, std::int64_t>>(source);
  check_relations<core::mv<rigid15, sparse_b<16>, std::int64_t>>(source);
}

// Reference: contract: an unqualified call reverse(x) of a multivector, in
//   code that has using namespace std and the standard algorithms, finds the
//   library's reverse by argument-dependent lookup beside std::reverse, which
//   takes two iterators, and gives the same type and value as the qualified
//   call
TEST_CASE("an unqualified reverse finds the multivector reverse") {
  using namespace std;
  array<int, 3> digits{1, 2, 3};
  // NOLINTNEXTLINE(modernize-use-ranges): the iterator form is under test.
  reverse(digits.begin(), digits.end());
  CHECK(digits == array<int, 3>{3, 2, 1});

  const Line l{1, -2, 3, -4, 5, -6};
  static_assert(same_as<decltype(reverse(l)), Line>);
  CHECK(reverse(l) == core::reverse(l));
}

// Under a using-directive for std::ranges, an unqualified reverse names the
// function object std::ranges::reverse, which turns off argument-dependent
// lookup, so the library's reverse is not found and the call does not
// compile; the library's comment says to qualify the call there.
namespace ranges_user {

using namespace std::ranges;

template <class X>
concept unqualified_reverse_compiles = requires(const X& x) { reverse(x); };

} // namespace ranges_user

// Reference: contract: under using namespace std::ranges, the function
//   object std::ranges::reverse hides the library's reverse from an
//   unqualified call, as the library documents, while a qualified call works
TEST_CASE("std::ranges::reverse hides an unqualified reverse") {
  static_assert(!ranges_user::unqualified_reverse_compiles<Line>);
  const Line l{1, -2, 3, -4, 5, -6};
  CHECK(core::reverse(l) == Line{-1, 2, -3, 4, -5, 6});
}

// A scope that sees galoot::core's operators by a using-directive, as a
// user's code may, with types of its own: one without operators and one
// with its own ~ and !. The library's operators must leave both alone.
namespace {
namespace user_code {

using namespace galoot::core;

struct widget {};

struct gadget {
  friend int operator~(gadget /*a*/) { return 1; }
  friend int operator!(gadget /*a*/) { return 2; }
};

template <class X>
concept has_tilde = requires(X x) { ~x; };
template <class X>
concept has_bang = requires(X x) { !x; };

// Reference: contract: with galoot::core's operators in scope by a
//   using-directive, a type of the user's without ~ and ! still has none, a
//   type with its own keeps them, and a multivector has both
TEST_CASE("the unary operators leave other types alone") {
  static_assert(!has_tilde<widget>);
  static_assert(!has_bang<widget>);
  static_assert(std::same_as<decltype(~gadget{}), int>);
  static_assert(std::same_as<decltype(!gadget{}), int>);
  CHECK((~gadget{}) == 1);
  CHECK((!gadget{}) == 2);
  static_assert(has_tilde<Line>);
  static_assert(has_bang<Line>);
}

} // namespace user_code
} // namespace
