// Tests of galoot/core/format.hpp: the std::formatter of a multivector.
//
// std::format("{}", x) writes {name: value, name: value}: every component of
// x, zero or not, in the order of x's layout, each as the name of its blade,
// a colon and a space, and its coefficient as the scalar's own formatter
// writes it, with a comma and a space between components; an empty layout
// writes {}. A format specification, as in {:.2f}, goes to the scalar's
// formatter and applies to each coefficient. A blade prints by the name the
// algebra tag declares for it with the orientation the layout stores, in the
// tag's optional member names, a list of oriented names as blades<Alg, "...">
// reads them. Any other blade prints by its canonical name, 1 for the scalar
// and otherwise e and its indices in increasing order, preceded by - when the
// layout stores it with orientation -1, because a coefficient c there stands
// for c times minus the canonical blade.
//
// The expected names are worked out in this file from each blade's mask and
// sign, by code that shares nothing with the library, and the expected texts
// are written out by hand. rigid(3) is R(3, 0, 1), and Lengyel's rigid
// geometric algebra (rigidgeometricalgebra.org) names each blade by its
// generators in the order of their product: e23 = +e23, e31 = -e13,
// e12 = +e12, e41 = -e14, e42 = -e24, e43 = -e34, e423 = +e234,
// e431 = -e134, e412 = +e124, and e321 = -e123. Its point lies on e1, e2,
// e3, e4, its line on e41, e42, e43, e23, e31, e12, its plane on e423, e431,
// e412, e321, and its motor on e41, e42, e43, e1234, e23, e31, e12, 1.
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <format>
#include <iterator>
#include <memory>
#include <print>
#include <string>
#include <string_view>
#include <utility>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/format.hpp> // IWYU pragma: keep
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>

#include <doctest/doctest.h>

namespace {

namespace core = galoot::core;

// Each member names below is marked [[maybe_unused]] because only the
// formatter reads it, and a compiler may not count that as a use.
//
// rigid(3) with no names: every blade prints by its canonical name.
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
// rigid(3) with a name for each of its sixteen blades, those of Lengyel's
// rigid geometric algebra.
struct rga3_named {
  static constexpr auto value = core::rigid<3>();
  [[maybe_unused]] static constexpr auto names =
      "1 e1 e2 e3 e4 e23 e31 e12 e41 e42 e43 e423 e431 e412 e321 e1234";
};
// rigid(3) naming the blades of a line only, held as a std::string_view and
// separated by runs of spaces, which blades<Alg, "..."> also reads.
struct rga3_line_names {
  static constexpr auto value = core::rigid<3>();
  [[maybe_unused]] static constexpr std::string_view names =
      "  e41 e42  e43 e23   e31 e12 ";
};
// rigid(3) with an empty list of names, which declares none.
struct rga3_no_names_listed {
  static constexpr auto value = core::rigid<3>();
  [[maybe_unused]] static constexpr auto names = "";
};
// rigid(3) with a member called names that is not text, which is not read.
struct rga3_names_not_text {
  static constexpr auto value = core::rigid<3>();
  [[maybe_unused]] static constexpr int names = 41;
};
template <std::size_t N>
struct euclidean {
  static constexpr auto value = core::R<N, 0, 0>();
};

// blades<Alg, "..."> reads only the number of generators, so these layouts
// serve every rigid(3) tag above.
inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
inline constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
inline constexpr core::blade_set<0> empty_layout{};

template <class T>
using Point = core::mv<rga3_named, point_layout, T>;
template <class T>
using Line = core::mv<rga3_named, line_layout, T>;
template <class T>
using Plane = core::mv<rga3_named, plane_layout, T>;
template <class T>
using Motor = core::mv<rga3_named, motor_layout, T>;

// A ring scalar with no std::formatter: an integer with the operations a
// multivector needs and nothing else.
struct plain_number {
  std::int64_t value = 0;

  constexpr plain_number() = default;
  explicit constexpr plain_number(std::int64_t v) noexcept : value(v) {}

  [[maybe_unused]] friend constexpr plain_number
  operator+(plain_number a, plain_number b) noexcept {
    return plain_number{a.value + b.value};
  }
  [[maybe_unused]] friend constexpr plain_number
  operator-(plain_number a, plain_number b) noexcept {
    return plain_number{a.value - b.value};
  }
  [[maybe_unused]] friend constexpr plain_number
  operator*(plain_number a, plain_number b) noexcept {
    return plain_number{a.value * b.value};
  }
  [[maybe_unused]] friend constexpr plain_number
  operator-(plain_number a) noexcept {
    return plain_number{-a.value};
  }
};

// A ring scalar whose std::formatter, below, is written for
// std::format_context alone, as most formatters are written: a fraction,
// kept unreduced, that prints as 1/2.
struct ratio {
  std::int64_t num = 0;
  std::int64_t den = 1;

  [[maybe_unused]] friend constexpr ratio operator+(ratio a, ratio b) noexcept {
    return {.num = (a.num * b.den) + (b.num * a.den), .den = a.den * b.den};
  }
  [[maybe_unused]] friend constexpr ratio operator-(ratio a, ratio b) noexcept {
    return {.num = (a.num * b.den) - (b.num * a.den), .den = a.den * b.den};
  }
  [[maybe_unused]] friend constexpr ratio operator*(ratio a, ratio b) noexcept {
    return {.num = a.num * b.num, .den = a.den * b.den};
  }
  [[maybe_unused]] friend constexpr ratio operator-(ratio a) noexcept {
    return {.num = -a.num, .den = a.den};
  }
};

// Classes derived from a line, as a model might declare a named type: the
// first is given the formatter of its base, below; the second is not.
struct NamedLine : Line<std::int64_t> {
  using Line<std::int64_t>::Line;
};
struct UnformattedLine : Line<std::int64_t> {
  using Line<std::int64_t>::Line;
};

} // namespace

// A formatter specialization matches one type exactly, so a derived class
// takes its base's formatter by inheriting it.
template <>
struct std::formatter<NamedLine, char>
    : std::formatter<Line<std::int64_t>, char> {};

// The formatter of ratio, with parse and format on the standard library's
// context types rather than templates on them. It takes no specification.
template <>
struct std::formatter<ratio, char> {
  constexpr std::format_parse_context::iterator
  parse(std::format_parse_context& context) {
    if (context.begin() != context.end() && *context.begin() != '}') {
      throw std::format_error{"ratio takes no format specification"};
    }
    return context.begin();
  }

  std::format_context::iterator format(const ratio& r,
                                       std::format_context& context) const {
    return std::format_to(context.out(), "{}/{}", r.num, r.den);
  }
};

namespace {

// The name of the blade of `mask` with orientation `sign`, worked out from
// the definition: 1 for the empty mask, otherwise e followed by i for every
// set bit i - 1, in increasing order, and preceded by - for orientation -1.
std::string expected_name(core::mask_t mask, int sign) {
  std::string name = sign < 0 ? "-" : "";
  if (mask == 0) {
    return name + "1";
  }
  name += 'e';
  for (unsigned bit = 0; bit < 9; ++bit) {
    if (((mask >> bit) & 1U) != 0) {
      name += static_cast<char>('1' + bit);
    }
  }
  return name;
}

// The number of masks of n bits with k bits set, counted one by one.
consteval std::size_t masks_of_grade(std::size_t n, std::size_t k) {
  std::size_t count = 0;
  for (core::mask_t mask = 0; mask < (core::mask_t{1} << n); ++mask) {
    if (std::cmp_equal(std::popcount(mask), k)) {
      ++count;
    }
  }
  return count;
}

// Every blade of grade K in an algebra of N generators, in increasing order
// of mask, with signs alternating from FirstSign. That order is not the
// canonical one from N = 4 on, where e23 (mask 6) comes before e14 (mask 9),
// so a formatter that printed in canonical order would fail.
template <std::size_t N, std::size_t K, int FirstSign>
consteval auto make_grade_layout() {
  core::blade_set<masks_of_grade(N, K)> set{};
  std::size_t at = 0;
  int sign = FirstSign;
  for (core::mask_t mask = 0; mask < (core::mask_t{1} << N); ++mask) {
    if (std::cmp_equal(std::popcount(mask), K)) {
      set.blades[at] = {.mask = mask, .sign = sign};
      ++at;
      sign = -sign;
    }
  }
  return set;
}
template <std::size_t N, std::size_t K, int FirstSign>
inline constexpr auto grade_layout = make_grade_layout<N, K, FirstSign>();

// Formats the blades of grade K of R(N, 0, 0) with the coefficients 1, 2, 3,
// and so on, checks the text against the expected names, and returns the
// number of blades checked.
template <std::size_t N, std::size_t K, int FirstSign>
std::size_t check_grade_layout() {
  constexpr auto layout = grade_layout<N, K, FirstSign>;
  core::mv<euclidean<N>, layout, int> x;
  std::string expected = "{";
  for (std::size_t i = 0; i < layout.size(); ++i) {
    const int value = static_cast<int>(i) + 1;
    x.coefficients[i] = value;
    if (i > 0) {
      expected += ", ";
    }
    expected += expected_name(layout.blades[i].mask, layout.blades[i].sign);
    expected += ": " + std::to_string(value);
  }
  expected += '}';

  const std::size_t n = N;
  const std::size_t k = K;
  const int first_sign = FirstSign;
  CAPTURE(n);
  CAPTURE(k);
  CAPTURE(first_sign);
  CHECK(std::format("{}", x) == expected);
  return layout.size();
}

// Every grade of R(N, 0, 0), each with signs alternating from +1 and from
// -1, so that each blade is checked once with each orientation.
template <std::size_t N, std::size_t... K>
std::size_t check_algebra(std::index_sequence<K...> /*grades*/) {
  return (check_grade_layout<N, K, 1>() + ... + 0) +
         (check_grade_layout<N, K, -1>() + ... + 0);
}

template <std::size_t... M>
std::size_t check_algebras(std::index_sequence<M...> /*n_minus_one*/) {
  return (check_algebra<M + 1>(std::make_index_sequence<M + 2>{}) + ...);
}

// Closes a file that a std::unique_ptr owns.
struct file_closer {
  void operator()(std::FILE* file) const noexcept {
    // The file is owned by the std::unique_ptr that calls this.
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    static_cast<void>(std::fclose(file));
  }
};

// The text of a line of the names below with the given texts of its
// coefficients.
std::string line_text(const std::array<std::string, 6>& values) {
  constexpr std::array<std::string_view, 6> names{
      "e41", "e42", "e43", "e23", "e31", "e12",
  };
  std::string text = "{";
  for (std::size_t i = 0; i < names.size(); ++i) {
    if (i > 0) {
      text += ", ";
    }
    text += std::string{names[i]} + ": " + values[i];
  }
  return text + "}";
}

// Reference: proof: exhaustive over every blade of R(n, 0, 0) for n from 1
//   to 9, each with orientation +1 and -1, 2044 oriented blades in all,
//   against names worked out from the mask and sign alone: a blade stored
//   with orientation -1 and no declared name prints as its canonical name
//   preceded by -; contract: format writes every component in the order of
//   the layout, which here is increasing mask order and not canonical order
TEST_CASE("format names every oriented blade of R(n, 0, 0) for n up to 9 by "
          "its canonical name, with - for orientation -1") {
  CHECK(check_algebras(std::make_index_sequence<9>{}) == 2044);
}

// Reference: independent value: the oriented names of the sixteen blades of
//   rigid(3) in Lengyel's rigid geometric algebra, 1, e1, e2, e3, e4, e23,
//   e31, e12, e41, e42, e43, e423, e431, e412, e321, e1234; contract: a
//   blade the algebra tag names with the layout's orientation prints by
//   that name
TEST_CASE("A blade that the algebra tag names with the layout's orientation "
          "prints by that name") {
  constexpr auto all = core::blades<rga3_named, "1 e1 e2 e3 e4 e23 e31 e12 "
                                                "e41 e42 e43 e423 e431 e412 "
                                                "e321 e1234">;
  core::mv<rga3_named, all, int> x;
  for (std::size_t i = 0; i < all.size(); ++i) {
    x.coefficients[i] = static_cast<int>(i) + 1;
  }
  CHECK(std::format("{}", x) ==
        "{1: 1, e1: 2, e2: 3, e3: 4, e4: 5, e23: 6, e31: 7, e12: 8, e41: 9, "
        "e42: 10, e43: 11, e423: 12, e431: 13, e412: 14, e321: 15, "
        "e1234: 16}");
  CHECK(std::format("{}", Line<int>{1, 2, 3, 4, 5, 6}) ==
        "{e41: 1, e42: 2, e43: 3, e23: 4, e31: 5, e12: 6}");
  CHECK(std::format("{}", Plane<int>{1, 2, 3, 4}) ==
        "{e423: 1, e431: 2, e412: 3, e321: 4}");
}

// Reference: proof: as blades<Alg, "..."> reads them, e13 = +e13,
//   e32 = -e23, e21 = -e12, e14 = +e14, e24 = +e24, e34 = +e34,
//   e324 = -e234, e134 = +e134, e142 = -e124, e123 = +e123, and
//   e2134 = -e1234, each the orientation opposite to the tag's name;
//   contract: a blade stored with orientation -1 and no declared name
//   prints as its canonical name preceded by -
TEST_CASE("A blade that the algebra tag names only with the other "
          "orientation prints by its canonical name") {
  constexpr auto opposite =
      core::blades<rga3_named,
                   "e13 e32 e21 e14 e24 e34 e324 e134 e142 e123 e2134">;
  core::mv<rga3_named, opposite, int> x;
  for (std::size_t i = 0; i < opposite.size(); ++i) {
    x.coefficients[i] = static_cast<int>(i) + 1;
  }
  CHECK(std::format("{}", x) ==
        "{e13: 1, -e23: 2, -e12: 3, e14: 4, e24: 5, e34: 6, -e234: 7, "
        "e134: 8, -e124: 9, e123: 10, -e1234: 11}");

  // The scalar and the vectors have one name each, so their negations,
  // written out by hand, are named by no tag.
  constexpr auto negated_blades = std::to_array<core::oriented_blade>({
      {.mask = 0, .sign = -1},
      {.mask = 0b1, .sign = -1},
      {.mask = 0b10, .sign = -1},
      {.mask = 0b100, .sign = -1},
      {.mask = 0b1000, .sign = -1},
  });
  constexpr core::blade_set<5> negated{negated_blades};
  CHECK(std::format("{}", core::mv<rga3_named, negated, int>{1, 2, 3, 4, 5}) ==
        "{-1: 1, -e1: 2, -e2: 3, -e3: 4, -e4: 5}");
}

// Reference: proof: in rigid(3), e41 = -e14, e42 = -e24, e43 = -e34,
//   e31 = -e13, e423 = +e234, e431 = -e134, e412 = +e124, e321 = -e123;
//   contract: a blade stored with orientation -1 and no declared name
//   prints as its canonical name preceded by -
TEST_CASE("A blade that the algebra tag does not name prints by its "
          "canonical name, with - for orientation -1") {
  CHECK(std::format("{}", core::mv<rga3, line_layout, int>{1, 2, 3, 4, 5, 6}) ==
        "{-e14: 1, -e24: 2, -e34: 3, e23: 4, -e13: 5, e12: 6}");
  CHECK(std::format("{}", core::mv<rga3, plane_layout, int>{1, 2, 3, 4}) ==
        "{e234: 1, -e134: 2, e124: 3, -e123: 4}");
  CHECK(std::format("{}", core::mv<rga3, point_layout, int>{1, 2, 3, 4}) ==
        "{e1: 1, e2: 2, e3: 3, e4: 4}");

  // A tag that names a line's blades only names nothing else.
  CHECK(std::format("{}",
                    core::mv<rga3_line_names, plane_layout, int>{1, 2, 3, 4}) ==
        "{e234: 1, -e134: 2, e124: 3, -e123: 4}");
}

// Reference: contract: the names are read as blades<Alg, "..."> reads
//   them, separated by runs of spaces, from a constant convertible to
//   std::string_view, where an empty list declares no name and a member
//   called names that is not text is not read; contract: a blade the
//   algebra tag names with the layout's orientation prints by that name
TEST_CASE("The algebra tag's names are read from any constant that "
          "converts to std::string_view") {
  CHECK(
      std::format("{}", core::mv<rga3_line_names, line_layout, int>{1, 2, 3, 4,
                                                                    5, 6}) ==
      "{e41: 1, e42: 2, e43: 3, e23: 4, e31: 5, e12: 6}");
  CHECK(std::format("{}",
                    core::mv<rga3_no_names_listed, line_layout, int>{1, 2, 3, 4,
                                                                     5, 6}) ==
        "{-e14: 1, -e24: 2, -e34: 3, e23: 4, -e13: 5, e12: 6}");
  CHECK(std::format("{}",
                    core::mv<rga3_names_not_text, line_layout, int>{1, 2, 3, 4,
                                                                    5, 6}) ==
        "{-e14: 1, -e24: 2, -e34: 3, e23: 4, -e13: 5, e12: 6}");
}

// Reference: contract: format writes every component in the order of the
//   layout, as {name: value, name: value}, each coefficient as
//   std::formatter<std::int64_t> writes it; independent value: the names
//   and the orders of a point, line, plane, and motor of Lengyel's rigid
//   geometric algebra
TEST_CASE("format writes a point, a line, a plane, and a motor of rigid(3) "
          "over std::int64_t") {
  CHECK(std::format("{}", Point<std::int64_t>{1, -2, 3, 1}) ==
        "{e1: 1, e2: -2, e3: 3, e4: 1}");
  CHECK(std::format("{}", Line<std::int64_t>{1, -2, 3, 0, 5, -6}) ==
        "{e41: 1, e42: -2, e43: 3, e23: 0, e31: 5, e12: -6}");
  CHECK(std::format("{}", Plane<std::int64_t>{-2, 3, 0, 7}) ==
        "{e423: -2, e431: 3, e412: 0, e321: 7}");
  CHECK(std::format("{}", Motor<std::int64_t>{0, 0, 1, 2, 0, -3, 0, 4}) ==
        "{e41: 0, e42: 0, e43: 1, e1234: 2, e23: 0, e31: -3, e12: 0, 1: 4}");
}

// Reference: contract: format writes every component in the order of the
//   layout, each coefficient as std::formatter<double> writes it, which for
//   {} is the shortest text that reads back as the value, so 0.5, -2, 3,
//   -0.25, and 0.75, all exact in binary; independent value: the names and
//   the orders of a point, line, plane, and motor of Lengyel's rigid
//   geometric algebra
TEST_CASE("format writes a point, a line, a plane, and a motor of rigid(3) "
          "over double") {
  CHECK(std::format("{}", Point<double>{0.5, -2, 3, 1}) ==
        "{e1: 0.5, e2: -2, e3: 3, e4: 1}");
  CHECK(std::format("{}", Line<double>{0.5, -2, 3, 0, -0.25, 1}) ==
        "{e41: 0.5, e42: -2, e43: 3, e23: 0, e31: -0.25, e12: 1}");
  CHECK(std::format("{}", Plane<double>{0.5, -2, 3, 0}) ==
        "{e423: 0.5, e431: -2, e412: 3, e321: 0}");
  CHECK(std::format("{}", Motor<double>{0.5, 0, 0, -2, 3, 0, 0, 0.75}) ==
        "{e41: 0.5, e42: 0, e43: 0, e1234: -2, e23: 3, e31: 0, e12: 0, "
        "1: 0.75}");
}

// Reference: independent value: the wedge of the points p and q has
//   p_i q_j - p_j q_i on e_ij, worked by hand for p = (1, 2, 0, 1) and
//   q = (3, 0, 5, 1); contract: results are ordered by grade, then by
//   index list, e12, e13, e14, e23, e24, e34, each with orientation +1
TEST_CASE("A product's result prints its blades by grade, then by index "
          "list") {
  const core::mv<rga3, point_layout, int> p{1, 2, 0, 1};
  const core::mv<rga3, point_layout, int> q{3, 0, 5, 1};
  CHECK(std::format("{}", core::wedge(p, q)) ==
        "{e12: -6, e13: 5, e14: -2, e23: 10, e24: 2, e34: -5}");

  // With the names of the rigid algebra the result is the same: the tag
  // names e12 and e23 with orientation +1, and e13, e14, e24, e34 only
  // with orientation -1, as e31, e41, e42, e43.
  const Point<int> pn{1, 2, 0, 1};
  const Point<int> qn{3, 0, 5, 1};
  CHECK(std::format("{}", core::wedge(pn, qn)) ==
        "{e12: -6, e13: 5, e14: -2, e23: 10, e24: 2, e34: -5}");
}

// Reference: contract: format writes every component in the order of the
//   layout, zero or not, and an empty layout as {}
TEST_CASE("Zero coefficients print, and an empty layout prints {}") {
  CHECK(std::format("{}", Line<std::int64_t>{}) ==
        "{e41: 0, e42: 0, e43: 0, e23: 0, e31: 0, e12: 0}");
  CHECK(std::format("{}", Point<double>{0, 0, 0, 1}) ==
        "{e1: 0, e2: 0, e3: 0, e4: 1}");
  CHECK(std::format("{}", core::mv<rga3_named, empty_layout, double>{}) ==
        "{}");
  CHECK(std::format("{:>5}", core::mv<rga3_named, empty_layout, double>{}) ==
        "{}");
}

// Reference: contract: a format specification applies to each coefficient,
//   and to nothing else, checked against the scalar's formatter applied to
//   each coefficient alone and against the texts written out
TEST_CASE("A format specification applies to each coefficient") {
  const Line<double> l{0.5, -2, 3, 0, -0.25, 1};
  std::array<std::string, 6> fixed;
  for (std::size_t i = 0; i < fixed.size(); ++i) {
    fixed[i] = std::format("{:.2f}", l.coefficients[i]);
  }
  CHECK(std::format("{:.2f}", l) == line_text(fixed));
  CHECK(std::format("{:.2f}", l) ==
        "{e41: 0.50, e42: -2.00, e43: 3.00, e23: 0.00, e31: -0.25, "
        "e12: 1.00}");

  const Line<std::int64_t> m{1, -2, 3, 0, 50, -600};
  std::array<std::string, 6> padded;
  for (std::size_t i = 0; i < padded.size(); ++i) {
    padded[i] = std::format("{:>5}", m.coefficients[i]);
  }
  CHECK(std::format("{:>5}", m) == line_text(padded));
  CHECK(std::format("{:>5}", m) ==
        "{e41:     1, e42:    -2, e43:     3, e23:     0, e31:    50, "
        "e12:  -600}");

  // Two arguments, each with its own specification, in surrounding text.
  CHECK(
      std::format("p = {}, l = {:.3f}.", Point<std::int64_t>{1, -2, 3, 1}, l) ==
      "p = {e1: 1, e2: -2, e3: 3, e4: 1}, l = {e41: 0.500, e42: -2.000, "
      "e43: 3.000, e23: 0.000, e31: -0.250, e12: 1.000}.");
}

// Reference: contract: std::print and std::format_to go through the same
//   formatter as std::format, so they write the same text
TEST_CASE("std::print and std::format_to write the text that std::format "
          "writes") {
  const Motor<double> q{0.5, 0, 0, -2, 3, 0, 0, 0.75};
  const std::string expected = std::format("{}", q);
  REQUIRE(expected == "{e41: 0.5, e42: 0, e43: 0, e1234: -2, e23: 3, e31: 0, "
                      "e12: 0, 1: 0.75}");

  std::string written;
  std::format_to(std::back_inserter(written), "{}", q);
  CHECK(written == expected);

  const std::unique_ptr<std::FILE, file_closer> file{std::tmpfile()};
  REQUIRE(file != nullptr);
  std::print(file.get(), "{}", q);
  REQUIRE(std::fseek(file.get(), 0, SEEK_SET) == 0);
  std::array<char, 256> buffer{};
  const std::size_t size =
      std::fread(buffer.data(), 1, buffer.size(), file.get());
  CHECK(std::string_view{buffer.data(), size} == expected);
}

// Reference: contract: format.hpp declares a formatter for every
//   multivector, for char only, and rejects a scalar without a
//   std::formatter inside format, with a message, so std::formattable
//   still answers yes for a multivector over such a scalar
TEST_CASE("std::formattable answers yes for a multivector over char, also "
          "when its scalar has no formatter") {
  static_assert(std::formattable<Line<double>, char>);
  static_assert(std::formattable<Motor<std::int64_t>, char>);
  static_assert(
      std::formattable<core::mv<rga3_named, empty_layout, int>, char>);
  static_assert(!std::formattable<Line<double>, wchar_t>);

  static_assert(!std::formattable<plain_number, char>);
  static_assert(std::formattable<Line<plain_number>, char>);
  CHECK(std::formattable<Line<plain_number>, char>);
}

// Reference: contract: a scalar with an enabled std::formatter prints, also
//   when that formatter's parse and format take std::format_parse_context&
//   and std::format_context& rather than any context; independent value:
//   the texts written out, each coefficient as the scalar's formatter
//   writes it alone
TEST_CASE("A scalar whose formatter takes std::format_context& prints "
          "inside a multivector") {
  // std::formattable tries a formatter with a context whose iterator is
  // left unspecified. libstdc++'s is not std::format_context's, so ratio
  // is not std::formattable there, though std::format prints it.
  static_assert(!std::formattable<ratio, char>);
  REQUIRE(std::format("{}", ratio{.num = -3, .den = 4}) == "-3/4");

  const Point<ratio> p{ratio{.num = 1, .den = 2}, ratio{.num = -3, .den = 4},
                       ratio{}, ratio{.num = 1, .den = 1}};
  CHECK(std::format("{}", p) == "{e1: 1/2, e2: -3/4, e3: 0/1, e4: 1/1}");

  std::string written;
  std::format_to(std::back_inserter(written), "p = {}.", p);
  CHECK(written == "p = {e1: 1/2, e2: -3/4, e3: 0/1, e4: 1/1}.");
}

// Reference: contract: a formatter specialization matches one type exactly,
//   so a class derived from a multivector formats once it inherits its
//   base's formatter, and then writes its base's text
TEST_CASE("A class derived from a multivector formats with its base's "
          "inherited formatter") {
  static_assert(std::formattable<NamedLine, char>);
  static_assert(!std::formattable<UnformattedLine, char>);

  const NamedLine l{1, -2, 3, 0, 5, -6};
  const Line<std::int64_t>& base = l;
  CHECK(std::format("{}", l) == std::format("{}", base));
  CHECK(std::format("{}", l) ==
        "{e41: 1, e42: -2, e43: 3, e23: 0, e31: 5, e12: -6}");
  CHECK(std::format("{:>3}", l) == std::format("{:>3}", base));
}

} // namespace
