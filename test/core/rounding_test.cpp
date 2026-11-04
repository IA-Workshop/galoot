// Tests of rounding in the core's operations over double: the four products
// and their operators, the grade maps and the complements with ~ and !,
// sums, differences, negation, scaling, part, and truncate.
//
// The structure constants are exact, so rounding enters only through the
// scalar type. Each double result is compared, coefficient by coefficient,
// with the exact value composed from the reference tables, within a forward
// error bound fixed before the result is seen. The bound comes from running
// the same call over test_support::tracked, whose values carry the exact
// result, the magnitude M of the same computation on absolute values, and
// the count of roundings K (support/tracked.hpp). The double result must
// then lie within gamma_K M of the exact value: checked as the ratio of the
// exact error to gamma_K M, and also, with a margin of two roundings for
// the check's own arithmetic, as the distance from the double nearest the
// exact value. A result of one rounding must be that nearest double, and an
// operation with no rounding, such as a sign map or a copy, must give the
// exact value. The tracked run's own value must equal the reference
// exactly.
//
// Inputs are seeded random p / 2^27 with |p| < 2^27: exact in double and in
// the test rational, while a product of two needs up to 54 bits and so
// rounds in double. Every reference value and magnitude stays below 2^7 in
// these forms, so the rationals, with denominators of at most 2^54, never
// leave std::int64_t. Sums would be exact on such inputs, so they take wide
// inputs, p / 2^(53 + s) with |p| < 2^53 and s from 0 to 6, whose sums need
// up to 60 bits. Each test reports the largest ratio of an error to its
// bound gamma_K times the magnitude that it saw.
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <random>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>
#include <galoot/core/unary.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>
#include <support/rational.hpp>
#include <support/tracked.hpp>

namespace {

namespace core = galoot::core;
using test_support::rational;
using test_support::tracked;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga2 {
  static constexpr auto value = core::conformal<2>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
// The two forms of the reference tables beside rigid(3) and the conformal
// ones: a degenerate form that is not diagonal, and a seeded random form.
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
struct r200 {
  static constexpr auto value = core::R<2, 0, 0>();
};

template <class Alg>
constexpr std::size_t blade_count =
    std::size_t{1}
    << std::remove_cvref_t<decltype(Alg::value)>::generator_count;

// The rigid(3) objects, under their oriented names.
constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
constexpr auto line_layout = core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12 e1234 1">;
using Point = core::mv<rga3, point_layout, double>;
using Line = core::mv<rga3, line_layout, double>;
using Plane = core::mv<rga3, plane_layout, double>;
using Motor = core::mv<rga3, motor_layout, double>;

template <class Alg, std::size_t N>
using full_mv = core::mv<Alg, core::full<N>, double>;

// The same multivector type over the tracking scalar.
template <class X>
using tracked_of = core::mv<typename X::algebra_type, X::layout, tracked>;

constexpr std::int64_t input_scale = test_support::power_of_two(27);

// One input multivector three ways: over double, over tracked, and its exact
// coefficients indexed by mask, each times its blade's orientation.
template <class X>
struct sample {
  X d;
  tracked_of<X> t;
  std::vector<rational> on;
};

// p / 2^27 for a seeded p with |p| < 2^27, or, for a wide input, p / 2^(53
// + s) with |p| < 2^53 and s from 0 to 6: a sum of two wide inputs needs up
// to 60 bits, so it rounds in double.
rational draw_value(std::mt19937_64& source, bool wide = false) {
  if (wide) {
    constexpr std::int64_t wide_scale = test_support::power_of_two(53);
    std::uniform_int_distribution<std::int64_t> numerator(-(wide_scale - 1),
                                                          wide_scale - 1);
    std::uniform_int_distribution<int> shift(0, 6);
    const std::int64_t p = numerator(source);
    return rational{p, test_support::power_of_two(53 + shift(source))};
  }
  std::uniform_int_distribution<std::int64_t> numerator(-(input_scale - 1),
                                                        input_scale - 1);
  return rational{numerator(source), input_scale};
}

template <class X>
sample<X> draw(std::mt19937_64& source, bool wide = false) {
  sample<X> s{
      .d = X{},
      .t = tracked_of<X>{},
      .on = std::vector<rational>(blade_count<typename X::algebra_type>),
  };
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const rational c = draw_value(source, wide);
    const core::oriented_blade blade = X::layout.blades.at(k);
    s.d.coefficients.at(k) = test_support::nearest_double(c);
    s.t.coefficients.at(k) = tracked::input(c);
    s.on.at(blade.mask) = blade.sign < 0 ? -c : c;
  }
  return s;
}

// What a family of comparisons found.
struct tally {
  int coefficients = 0;
  int wrong_values = 0;
  int outside_bound = 0;
  int rounded = 0;
  int missing_blades = 0;
  // The largest ratio of an error to its bound, over every coefficient that
  // rounds and over those with two or more roundings, whose bound is less
  // tight than one rounding's.
  double largest_ratio = 0.0;
  double largest_deeper_ratio = 0.0;
  std::string first_problem;

  void note(const std::string& what) {
    if (first_problem.empty()) {
      first_problem = what;
    }
  }
};

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

std::string text(const rational& x) {
  return std::format("{}/{}", x.numerator, x.denominator);
}

// Compares the double result d and the tracked result t of one call with
// the exact coefficients `expected`, indexed by mask. Each coefficient of
// the result must be within its bound, and its tracked value must equal the
// expected value; a blade the result lacks must be zero in `expected`. With
// `exact`, the operation must not round at all.
template <class R, class RT>
void compare(const R& d, const RT& t, const std::vector<rational>& expected,
             bool exact, const std::string& what, tally& out) {
  static_assert(std::is_same_v<RT, tracked_of<R>>);
  std::vector<bool> held(expected.size());
  for (std::size_t k = 0; k < R::component_count; ++k) {
    const core::oriented_blade blade = R::layout.blades.at(k);
    const double dk =
        blade.sign < 0 ? -d.coefficients.at(k) : d.coefficients.at(k);
    const tracked tk =
        blade.sign < 0 ? -t.coefficients.at(k) : t.coefficients.at(k);
    const rational& e = expected.at(blade.mask);
    const std::string where = what + " on " + blade_name(blade.mask);
    held.at(blade.mask) = true;
    ++out.coefficients;
    if (tk.value != e) {
      ++out.wrong_values;
      out.note(where + ": the tracked value is " + text(tk.value) +
               ", the reference " + text(e));
    }
    if (exact && tk.roundings != 0) {
      ++out.rounded;
      out.note(where + std::format(": {} roundings", tk.roundings));
    }
    if (!test_support::within_rounding_bound(dk, e, tk.roundings,
                                             tk.magnitude)) {
      ++out.outside_bound;
      out.note(where + std::format(": {} is outside the bound of ", dk) +
               text(e) +
               std::format(" with K = {} and magnitude ", tk.roundings) +
               text(tk.magnitude));
    }
    if (tk.roundings > 0) {
      const double ratio =
          test_support::bound_ratio(dk, e, tk.roundings, tk.magnitude);
      out.largest_ratio = std::max(out.largest_ratio, ratio);
      if (tk.roundings > 1) {
        out.largest_deeper_ratio = std::max(out.largest_deeper_ratio, ratio);
      }
    }
  }
  for (std::size_t m = 0; m < expected.size(); ++m) {
    if (!held.at(m) && expected.at(m) != rational{}) {
      ++out.missing_blades;
      out.note(what + ": the result lacks " +
               blade_name(static_cast<core::mask_t>(m)) + ", which is " +
               text(expected.at(m)));
    }
  }
}

void check_tally(const tally& t, const std::string& family) {
  MESSAGE(std::format("{}: {} coefficients, largest ratio of error to bound "
                      "{:.6f}, {:.6f} with two or more roundings",
                      family, t.coefficients, t.largest_ratio,
                      t.largest_deeper_ratio));
  INFO("first problem: ", t.first_problem);
  CHECK(t.coefficients > 0);
  CHECK(t.wrong_values == 0);
  CHECK(t.outside_bound == 0);
  CHECK(t.rounded == 0);
  CHECK(t.missing_blades == 0);
  CHECK(t.largest_ratio <= test_support::ratio_limit);
}

// Draws `samples` pairs of the types X and Y, applies `op` over double and
// over tracked, and compares both with `reference` of the exact inputs.
template <class X, class Y, class Op, class Reference>
void check_binary(std::mt19937_64& source, int samples, const Op& op,
                  const Reference& reference, bool exact,
                  const std::string& what, tally& out, bool wide = false) {
  for (int s = 0; s < samples; ++s) {
    const sample<X> a = draw<X>(source, wide);
    const sample<Y> b = draw<Y>(source, wide);
    compare(op(a.d, b.d), op(a.t, b.t), reference(a.on, b.on), exact, what,
            out);
  }
}

template <class X, class Op, class Reference>
void check_unary(std::mt19937_64& source, int samples, const Op& op,
                 const Reference& reference, bool exact,
                 const std::string& what, tally& out) {
  for (int s = 0; s < samples; ++s) {
    const sample<X> a = draw<X>(source);
    compare(op(a.d), op(a.t), reference(a.on), exact, what, out);
  }
}

using coefficients = std::vector<rational>;

// The product of a and b, by mask, from a product table indexed by
// (mask of a) times the number of blades plus (mask of b): each pair of
// blades adds its terms, and for the wedge only a pair with no generator in
// common adds, and only its term on the union of their generators. `found`
// is cleared when an entry is not where its index says.
coefficients table_product(std::span<const golden::product> table,
                           const coefficients& a, const coefficients& b,
                           bool wedge, bool& found) {
  const std::size_t size = a.size();
  coefficients out(size);
  for (std::size_t i = 0; i < size; ++i) {
    for (std::size_t j = 0; j < size; ++j) {
      const golden::product& entry = table[(i * size) + j];
      found = found && entry.a == i && entry.b == j;
      if (a[i] == rational{} || b[j] == rational{}) {
        continue;
      }
      for (std::size_t k = 0; k < entry.size; ++k) {
        const golden::term& t = entry.terms.at(k);
        if (wedge && ((i & j) != 0 || t.blade != (i | j))) {
          continue;
        }
        out.at(t.blade) =
            out.at(t.blade) + (a[i] * b[j] * rational{t.coefficient});
      }
    }
  }
  return out;
}

// The right or the left complement of x, by mask, from a complement table.
coefficients complement(std::span<const golden::complement_pair> table,
                        const coefficients& x, bool right) {
  coefficients out(x.size());
  for (std::size_t m = 0; m < x.size(); ++m) {
    const golden::signed_blade c = right ? table[m].right : table[m].left;
    out.at(c.blade) = rational{c.sign} * x[m];
  }
  return out;
}

// x with each blade's coefficient times the sign a grade map gives it.
coefficients grade_map(std::span<const golden::blade_signs> table,
                       const coefficients& x, int golden::blade_signs::* sign) {
  coefficients out(x.size());
  for (std::size_t m = 0; m < x.size(); ++m) {
    out.at(m) = rational{table[m].*sign} * x[m];
  }
  return out;
}

// The four products over one pair of types, by function and by operator,
// against the product table and, for the antiproducts, the complement table:
// a ⟇ b = left(right(a) ⟑ right(b)) and a v b = left(right(a) ^ right(b)).
template <class X, class Y>
void check_products(std::mt19937_64& source, int samples,
                    std::span<const golden::product> table,
                    const std::string& form, tally& out, bool& found) {
  const auto gp = [&](const coefficients& a, const coefficients& b) {
    return table_product(table, a, b, false, found);
  };
  const auto wedge = [&](const coefficients& a, const coefficients& b) {
    return table_product(table, a, b, true, found);
  };
  check_binary<X, Y>(
      source, samples,
      [](const auto& x, const auto& y) {
        return core::geometric_product(x, y);
      },
      gp, false, form + ", geometric_product", out);
  check_binary<X, Y>(
      source, samples, [](const auto& x, const auto& y) { return x * y; }, gp,
      false, form + ", a * b", out);
  check_binary<X, Y>(
      source, samples,
      [](const auto& x, const auto& y) { return core::wedge(x, y); }, wedge,
      false, form + ", wedge", out);
  check_binary<X, Y>(
      source, samples, [](const auto& x, const auto& y) { return x ^ y; },
      wedge, false, form + ", a ^ b", out);
}

// GeometricAntiproduct false leaves out the geometric antiproduct, for
// layouts whose plan the compiler cannot build at its default limits.
template <class X, class Y, bool GeometricAntiproduct = true>
void check_antiproducts(std::mt19937_64& source, int samples,
                        std::span<const golden::product> table,
                        std::span<const golden::complement_pair> complements,
                        const std::string& form, tally& out, bool& found) {
  const auto anti = [&](bool wedge) {
    return [&, wedge](const coefficients& a, const coefficients& b) {
      return complement(complements,
                        table_product(table, complement(complements, a, true),
                                      complement(complements, b, true), wedge,
                                      found),
                        false);
    };
  };
  if constexpr (GeometricAntiproduct) {
    check_binary<X, Y>(
        source, samples,
        [](const auto& x, const auto& y) {
          return core::geometric_antiproduct(x, y);
        },
        anti(false), false, form + ", geometric_antiproduct", out);
    check_binary<X, Y>(
        source, samples, [](const auto& x, const auto& y) { return x % y; },
        anti(false), false, form + ", a % b", out);
  }
  check_binary<X, Y>(
      source, samples,
      [](const auto& x, const auto& y) { return core::antiwedge(x, y); },
      anti(true), false, form + ", antiwedge", out);
  check_binary<X, Y>(
      source, samples, [](const auto& x, const auto& y) { return x & y; },
      anti(true), false, form + ", a & b", out);
}

// The five maps and ~ and !, each of which must give the exact value.
template <class X>
void check_maps(std::mt19937_64& source, int samples,
                std::span<const golden::blade_signs> signs,
                std::span<const golden::complement_pair> complements,
                const std::string& form, tally& out) {
  const auto by = [&](int golden::blade_signs::* sign) {
    return
        [&, sign](const coefficients& a) { return grade_map(signs, a, sign); };
  };
  const auto right = [&](const coefficients& a) {
    return complement(complements, a, true);
  };
  const auto left = [&](const coefficients& a) {
    return complement(complements, a, false);
  };
  check_unary<X>(
      source, samples, [](const auto& x) { return core::reverse(x); },
      by(&golden::blade_signs::reverse), true, form + ", reverse", out);
  check_unary<X>(
      source, samples, [](const auto& x) { return ~x; },
      by(&golden::blade_signs::reverse), true, form + ", ~a", out);
  check_unary<X>(
      source, samples, [](const auto& x) { return core::antireverse(x); },
      by(&golden::blade_signs::antireverse), true, form + ", antireverse", out);
  check_unary<X>(
      source, samples, [](const auto& x) { return core::grade_involution(x); },
      by(&golden::blade_signs::involution), true, form + ", grade_involution",
      out);
  check_unary<X>(
      source, samples, [](const auto& x) { return core::right_complement(x); },
      right, true, form + ", right_complement", out);
  check_unary<X>(
      source, samples, [](const auto& x) { return !x; }, right, true,
      form + ", !a", out);
  check_unary<X>(
      source, samples, [](const auto& x) { return core::left_complement(x); },
      left, true, form + ", left_complement", out);
}

// a + b and a - b on wide inputs, against the sum and the difference of the
// exact coefficients on each blade: one rounding on a blade both operands
// hold, and none on a blade only one holds.
template <class X, class Y>
void check_sums(std::mt19937_64& source, int samples, const std::string& form,
                tally& out) {
  const auto plus = [](const coefficients& a, const coefficients& b) {
    coefficients r(a.size());
    for (std::size_t m = 0; m < a.size(); ++m) {
      r.at(m) = a[m] + b[m];
    }
    return r;
  };
  const auto minus = [](const coefficients& a, const coefficients& b) {
    coefficients r(a.size());
    for (std::size_t m = 0; m < a.size(); ++m) {
      r.at(m) = a[m] - b[m];
    }
    return r;
  };
  check_binary<X, Y>(
      source, samples, [](const auto& x, const auto& y) { return x + y; }, plus,
      false, form + ", a + b", out, true);
  check_binary<X, Y>(
      source, samples, [](const auto& x, const auto& y) { return x - y; },
      minus, false, form + ", a - b", out, true);
}

// Negation, part, and truncate, which must give the exact value: the
// negated coefficients, and the coefficients of the target's blades.
template <class X, auto Target>
void check_copies(std::mt19937_64& source, int samples, const std::string& form,
                  tally& out) {
  const auto negated = [](const coefficients& a) {
    coefficients r(a.size());
    for (std::size_t m = 0; m < a.size(); ++m) {
      r.at(m) = -a[m];
    }
    return r;
  };
  const auto kept = [](const coefficients& a) {
    coefficients r(a.size());
    for (const core::oriented_blade& blade : Target.blades) {
      r.at(blade.mask) = a.at(blade.mask);
    }
    return r;
  };
  check_unary<X>(
      source, samples, [](const auto& x) { return -x; }, negated, true,
      form + ", -a", out);
  check_unary<X>(
      source, samples, [](const auto& x) { return core::part<Target>(x); },
      kept, true, form + ", part", out);
  // truncate drops only zeros here, as its contract asks, so its input is
  // the part it keeps, converted back to X.
  check_unary<X>(
      source, samples,
      [](const auto& x) {
        using Y = std::remove_cvref_t<decltype(x)>;
        return core::truncate<Target>(Y{core::part<Target>(x)});
      },
      kept, true, form + ", truncate", out);
}

// The seed source, its seed logged on failure.
std::mt19937_64 seeded_source() {
  return std::mt19937_64(doctest::getContextOptions()->rand_seed);
}

} // namespace

// Reference: independent value: by hand, the tracked run of the geometric
//   product of two vectors x and y of R(2,0,0) has the scalar part
//   x1 y1 + x2 y2 and the e12 part x1 y2 - x2 y1, each two roundings deep
//   (a product, then a sum) with the magnitude |x1 y1| + |x2 y2| and
//   |x1 y2| + |x2 y1|
TEST_CASE("a tracked run counts the roundings of a product") {
  using V = core::mv<r200, core::of_grade<2, 1>, tracked>;
  const V x{tracked::input(rational{1, 2}), tracked::input(rational{-3, 4})};
  const V y{tracked::input(rational{5, 8}), tracked::input(rational{1, 8})};
  const auto p = core::geometric_product(x, y);
  static_assert(decltype(p)::component_count == 2);
  const tracked scalar = p.coefficients.at(0);
  const tracked e12 = p.coefficients.at(1);
  CHECK(scalar.value == rational{7, 32});
  CHECK(scalar.magnitude == rational{13, 32});
  CHECK(scalar.roundings == 2);
  CHECK(e12.value == rational{17, 32});
  CHECK(e12.magnitude == rational{17, 32});
  CHECK(e12.roundings == 2);
}

// Reference: proof: in the standard model fl(a op b) = (a op b)(1 + d) with
//   |d| <= 2^-53, a double sum of products with at most K roundings on any
//   path differs from its exact value by at most gamma_K = K u / (1 - K u)
//   times its magnitude (Higham, Accuracy and Stability of Numerical
//   Algorithms, 2nd ed., Lemma 3.1 and equation (3.5)), checked with two
//   roundings more for the check's own rounding;
//   independent value: the exact products from the tables, which satisfy
//   e_i e_j + e_j e_i = 2 g_ij, with the wedge the term on the union of two
//   blades with no generator in common, for the products of rigid(3),
//   conformal(2), conformal(3), a degenerate form, and a random form, on
//   seeded inputs
TEST_CASE("the products round within their forward error bounds") {
  INFO("seed: ", doctest::getContextOptions()->rand_seed);
  std::mt19937_64 source = seeded_source();
  tally out;
  bool found = true;
  check_products<full_mv<rga3, 4>, full_mv<rga3, 4>>(
      source, 200, golden::gp_tables::rigid_3, "rigid(3)", out, found);
  check_products<Motor, Point>(source, 200, golden::gp_tables::rigid_3,
                               "rigid(3) motor and point", out, found);
  check_products<Line, Plane>(source, 200, golden::gp_tables::rigid_3,
                              "rigid(3) line and plane", out, found);
  check_products<full_mv<cga2, 4>, full_mv<cga2, 4>>(
      source, 200, golden::gp_tables::conformal_2, "conformal(2)", out, found);
  check_products<full_mv<cga3, 5>, full_mv<cga3, 5>>(
      source, 40, golden::gp_tables::conformal_3, "conformal(3)", out, found);
  check_products<full_mv<degenerate3, 3>, full_mv<degenerate3, 3>>(
      source, 200, golden::gp_tables::degenerate_3, "degenerate", out, found);
  check_products<full_mv<random1, 3>, full_mv<random1, 3>>(
      source, 200, golden::gp_tables::random_1, "random", out, found);
  CHECK(found);
  check_tally(out, "products");
}

// Reference: proof: the forward error bound gamma_K times the magnitude, as
//   for the products (Higham, Lemma 3.1);
//   independent value: a ⟇ b = left(right(a) ⟑ right(b)) and
//   a v b = left(right(a) ^ right(b)), composed from the product tables,
//   which satisfy e_i e_j + e_j e_i = 2 g_ij, and the complement tables,
//   which satisfy u ^ right(u) = antiscalar and left(u) ^ u = antiscalar,
//   with the geometric antiproduct of conformal(3) taken on pairs of grade
//   sets that together reach every pair of grades rather than on two full
//   multivectors
TEST_CASE("the antiproducts round within their forward error bounds") {
  INFO("seed: ", doctest::getContextOptions()->rand_seed);
  std::mt19937_64 source = seeded_source();
  tally out;
  bool found = true;
  check_antiproducts<full_mv<rga3, 4>, full_mv<rga3, 4>>(
      source, 200, golden::gp_tables::rigid_3, golden::complements::n4,
      "rigid(3)", out, found);
  check_antiproducts<Motor, Point>(source, 200, golden::gp_tables::rigid_3,
                                   golden::complements::n4,
                                   "rigid(3) motor and point", out, found);
  check_antiproducts<Point, Plane>(source, 200, golden::gp_tables::rigid_3,
                                   golden::complements::n4,
                                   "rigid(3) point and plane", out, found);
  check_antiproducts<full_mv<cga2, 4>, full_mv<cga2, 4>>(
      source, 200, golden::gp_tables::conformal_2, golden::complements::n4,
      "conformal(2)", out, found);
  check_antiproducts<full_mv<cga3, 5>, full_mv<cga3, 5>, false>(
      source, 40, golden::gp_tables::conformal_3, golden::complements::n5,
      "conformal(3)", out, found);
  // The geometric antiproduct of two full<5> values of conformal(3) needs
  // more constant-evaluation steps than Clang allows by default when
  // libstdc++'s assertions are off, so it is taken on pairs of grade sets
  // that together reach every pair of grades.
  using C5 = full_mv<cga3, 5>;
  using Ends =
      core::mv<cga3, core::merge<core::of_grade<5, 0>, core::of_grade<5, 5>>,
               double>;
  using OneFour =
      core::mv<cga3, core::merge<core::of_grade<5, 1>, core::of_grade<5, 4>>,
               double>;
  using TwoThree =
      core::mv<cga3, core::merge<core::of_grade<5, 2>, core::of_grade<5, 3>>,
               double>;
  constexpr auto even_grades =
      core::merge<core::merge<core::of_grade<5, 0>, core::of_grade<5, 2>>,
                  core::of_grade<5, 4>>;
  constexpr auto odd_grades =
      core::merge<core::merge<core::of_grade<5, 1>, core::of_grade<5, 3>>,
                  core::of_grade<5, 5>>;
  using Even = core::mv<cga3, even_grades, double>;
  using Odd = core::mv<cga3, odd_grades, double>;
  const std::span<const golden::product> c3 = golden::gp_tables::conformal_3;
  const std::span<const golden::complement_pair> n5 = golden::complements::n5;
  check_antiproducts<OneFour, TwoThree>(
      source, 40, c3, n5, "conformal(3) grades 1 and 4 with 2 and 3", out,
      found);
  check_antiproducts<Ends, C5>(
      source, 40, c3, n5, "conformal(3) grades 0 and 5 with all", out, found);
  check_antiproducts<C5, Ends>(
      source, 40, c3, n5, "conformal(3) all with grades 0 and 5", out, found);
  check_antiproducts<Even, Even>(
      source, 40, c3, n5, "conformal(3) even with even grades", out, found);
  check_antiproducts<Odd, Odd>(source, 40, c3, n5,
                               "conformal(3) odd with odd grades", out, found);
  check_antiproducts<Even, Odd>(
      source, 40, c3, n5, "conformal(3) even with odd grades", out, found);
  check_antiproducts<full_mv<degenerate3, 3>, full_mv<degenerate3, 3>>(
      source, 200, golden::gp_tables::degenerate_3, golden::complements::n3,
      "degenerate", out, found);
  check_antiproducts<full_mv<random1, 3>, full_mv<random1, 3>>(
      source, 200, golden::gp_tables::random_1, golden::complements::n3,
      "random", out, found);
  CHECK(found);
  check_tally(out, "antiproducts");
}

// Reference: independent value: the reverse of a grade-k blade is
//   (-1)^(k(k-1)/2) times it, the antireverse uses the antigrade, and the
//   grade involution multiplies by (-1)^k, and the complements satisfy
//   u ^ right(u) = antiscalar and left(u) ^ u = antiscalar, each a sign
//   and a permutation of the blades, so over double each gives the exact
//   value
TEST_CASE("the grade maps and the complements are exact over double") {
  INFO("seed: ", doctest::getContextOptions()->rand_seed);
  std::mt19937_64 source = seeded_source();
  tally out;
  check_maps<full_mv<rga3, 4>>(source, 100, golden::grade_signs::n4,
                               golden::complements::n4, "rigid(3)", out);
  check_maps<Line>(source, 100, golden::grade_signs::n4,
                   golden::complements::n4, "rigid(3) line", out);
  check_maps<Motor>(source, 100, golden::grade_signs::n4,
                    golden::complements::n4, "rigid(3) motor", out);
  check_maps<full_mv<cga2, 4>>(source, 100, golden::grade_signs::n4,
                               golden::complements::n4, "conformal(2)", out);
  check_maps<full_mv<cga3, 5>>(source, 100, golden::grade_signs::n5,
                               golden::complements::n5, "conformal(3)", out);
  check_maps<full_mv<degenerate3, 3>>(source, 100, golden::grade_signs::n3,
                                      golden::complements::n3, "degenerate",
                                      out);
  check_maps<full_mv<random1, 3>>(source, 100, golden::grade_signs::n3,
                                  golden::complements::n3, "random", out);
  check_tally(out, "grade maps and complements");
}

// Reference: proof: a sum or a difference of two doubles rounds once, so
//   its error is at most gamma_1 times the sum of the magnitudes, and a blade
//   that only one operand holds is copied exactly, on inputs with 53-bit
//   significands and exponents that differ; independent value: the exact
//   coefficients added on each blade
TEST_CASE("sums and differences round within their forward error bounds") {
  INFO("seed: ", doctest::getContextOptions()->rand_seed);
  std::mt19937_64 source = seeded_source();
  tally out;
  check_sums<full_mv<rga3, 4>, full_mv<rga3, 4>>(source, 200, "rigid(3)", out);
  check_sums<Line, Motor>(source, 200, "rigid(3) line and motor", out);
  check_sums<Point, Plane>(source, 200, "rigid(3) point and plane", out);
  check_sums<full_mv<cga3, 5>, full_mv<cga3, 5>>(source, 200, "conformal(3)",
                                                 out);
  using Vector = core::mv<cga3, core::of_grade<5, 1>, double>;
  using Bivector = core::mv<cga3, core::of_grade<5, 2>, double>;
  check_sums<Vector, Bivector>(source, 200, "conformal(3) vector and bivector",
                               out);
  check_sums<full_mv<cga2, 4>, full_mv<cga2, 4>>(source, 200, "conformal(2)",
                                                 out);
  check_sums<full_mv<degenerate3, 3>, full_mv<degenerate3, 3>>(
      source, 200, "degenerate", out);
  check_sums<full_mv<random1, 3>, full_mv<random1, 3>>(source, 200, "random",
                                                       out);
  check_tally(out, "sums and differences");
}

// Reference: independent value: negation, part, and truncate move or negate
//   coefficients without arithmetic on them, so over double each gives the
//   exact coefficients of the target's blades
TEST_CASE("negation, part, and truncate are exact over double") {
  INFO("seed: ", doctest::getContextOptions()->rand_seed);
  std::mt19937_64 source = seeded_source();
  tally out;
  check_copies<Motor, line_layout>(source, 200, "rigid(3) motor to line", out);
  check_copies<full_mv<rga3, 4>, plane_layout>(source, 200, "rigid(3) to plane",
                                               out);
  check_copies<full_mv<cga2, 4>, core::of_grade<4, 1>>(
      source, 200, "conformal(2) to vectors", out);
  check_copies<full_mv<degenerate3, 3>, core::of_grade<3, 2>>(
      source, 200, "degenerate to bivectors", out);
  check_copies<full_mv<random1, 3>, core::of_grade<3, 1>>(
      source, 200, "random to vectors", out);
  check_copies<full_mv<cga3, 5>, core::of_grade<5, 2>>(
      source, 200, "conformal(3) to bivectors", out);
  check_tally(out, "negation, part, truncate");
}

// Reference: proof: s * a, a * s, and a / s make one IEEE 754 operation per
//   coefficient, which rounds the exact result correctly, so each coefficient
//   equals the same operation written here, and 5 / 3 is not 5 times the
//   rounded 1/3 in double
TEST_CASE("scaling and division round each coefficient once") {
  INFO("seed: ", doctest::getContextOptions()->rand_seed);
  std::mt19937_64 source = seeded_source();
  int differences = 0;
  int compared = 0;
  const auto check_scaling = [&]<class X>(int samples) {
    for (int k = 0; k < samples; ++k) {
      const X x = draw<X>(source).d;
      double s = 0.0;
      while (s == 0.0) {
        s = test_support::nearest_double(draw_value(source));
      }
      const X left = s * x;
      const X right = x * s;
      const X quotient = x / s;
      for (std::size_t i = 0; i < X::component_count; ++i) {
        const double c = x.coefficients.at(i);
        differences += left.coefficients.at(i) == s * c ? 0 : 1;
        differences += right.coefficients.at(i) == c * s ? 0 : 1;
        differences += quotient.coefficients.at(i) == c / s ? 0 : 1;
        compared += 3;
      }
    }
  };
  check_scaling.operator()<full_mv<rga3, 4>>(200);
  check_scaling.operator()<Motor>(200);
  check_scaling.operator()<full_mv<cga2, 4>>(200);
  check_scaling.operator()<full_mv<cga3, 5>>(200);
  check_scaling.operator()<full_mv<degenerate3, 3>>(200);
  check_scaling.operator()<full_mv<random1, 3>>(200);
  CHECK(compared > 0);
  CHECK(differences == 0);

  const Point five{5.0, 10.0, 7.0, 1.0};
  const Point third = five / 3.0;
  CHECK(third.coefficients.at(0) == 5.0 / 3.0);
  CHECK(third.coefficients.at(0) != 5.0 * (1.0 / 3.0));
  CHECK(third.coefficients.at(1) == 10.0 / 3.0);
  CHECK(third.coefficients.at(2) == 7.0 / 3.0);
}
