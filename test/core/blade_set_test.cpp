// Tests of galoot/core/blade_set.hpp against definitions written here.
//
// The reference in this file shares no code with the header. A blade is the
// increasing list of its generator indices, read bit by bit. Canonical order
// is a sort of those lists, shorter first and then lexicographically. The
// sign of an oriented name is the parity of its inversions, counted pair by
// pair, and the number of blades of one grade comes from Pascal's rule. The
// inputs are finite, so each check covers every case in its range, which
// makes it a proof over that range. A second route compares the header with
// the golden values of reference/golden.hpp, which the reference scripts in
// test/reference/ compute in Python.
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>

namespace {

namespace core = galoot::core;
using core::mask_t;

// Names are read in an algebra, and the parser reads only its number of
// generators, so R(n, 0, 0) serves every n, and rigid(3) and conformal(3)
// serve their own objects.
template <std::size_t N>
struct euclidean {
  static constexpr auto value = core::R<N, 0, 0>();
};
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct conformal3 {
  static constexpr auto value = core::conformal<3>();
};

constexpr std::size_t mask_width = std::numeric_limits<mask_t>::digits;

// The exhaustive checks cover every algebra of up to this many generators.
constexpr std::size_t exhaustive_n = 6;
constexpr std::size_t exhaustive_blades = std::size_t{1} << exhaustive_n;

constexpr mask_t blade_count(std::size_t n) { return mask_t{1} << n; }

// The generator indices of a blade, counting from 1, in increasing order.
struct index_list {
  std::array<int, mask_width> at{};
  std::size_t size = 0;
};

// Bit i - 1 of a mask stands for e_i.
constexpr index_list indices_of(mask_t mask) {
  index_list list;
  for (std::size_t bit = 0; bit < mask_width; ++bit) {
    if (((mask >> bit) & 1U) == 1U) {
      list.at.at(list.size) = static_cast<int>(bit) + 1;
      ++list.size;
    }
  }
  return list;
}

constexpr int grade_of(mask_t mask) {
  return static_cast<int>(indices_of(mask).size);
}

// Canonical order: shorter index lists first, and lists of one length by the
// first index at which they differ.
constexpr bool by_grade_then_indices(mask_t a, mask_t b) {
  const index_list la = indices_of(a);
  const index_list lb = indices_of(b);
  if (la.size != lb.size) {
    return la.size < lb.size;
  }
  for (std::size_t p = 0; p < la.size; ++p) {
    if (la.at.at(p) != lb.at.at(p)) {
      return la.at.at(p) < lb.at.at(p);
    }
  }
  return false;
}

// The order that canonical order is not: by grade, then by the mask read as
// a number, which puts e23 (0b0110) before e14 (0b1001).
constexpr bool by_grade_then_mask(mask_t a, mask_t b) {
  return grade_of(a) != grade_of(b) ? grade_of(a) < grade_of(b) : a < b;
}

// A list of masks, as long as the blades of exhaustive_n generators.
struct mask_list {
  std::array<mask_t, exhaustive_blades> at{};
  std::size_t size = 0;

  friend constexpr bool operator==(const mask_list&,
                                   const mask_list&) = default;
};

constexpr void push(mask_list& list, mask_t mask) {
  list.at.at(list.size) = mask;
  ++list.size;
}

// Every blade of n generators, sorted by `less`.
constexpr mask_list sorted_blades(std::size_t n, auto less) {
  mask_list list;
  for (mask_t mask = 0; mask < blade_count(n); ++mask) {
    push(list, mask);
  }
  std::ranges::sort(std::span(list.at).first(list.size), less);
  return list;
}

constexpr mask_list canonical_blades(std::size_t n) {
  return sorted_blades(n, by_grade_then_indices);
}

// Each n's canonical list, sorted once, because the checks below read it for
// many sets and a constant evaluation has a limit on its steps.
template <std::size_t N>
constexpr mask_list canonical = canonical_blades(N);

// The masks of `list` whose grade is j or k, in the order of `list`.
constexpr mask_list of_grades(const mask_list& list, int j, int k) {
  mask_list kept;
  for (std::size_t i = 0; i < list.size; ++i) {
    const int g = grade_of(list.at.at(i));
    if (g == j || g == k) {
      push(kept, list.at.at(i));
    }
  }
  return kept;
}

// C(n, k) by Pascal's rule, C(m, j) = C(m - 1, j - 1) + C(m - 1, j), applied
// row by row from C(0, 0) = 1. Each row is updated in place from its right
// end, so that C(m - 1, j - 1) is still the old row's value when it is read.
constexpr std::size_t pascal(std::size_t n, std::size_t k) {
  std::array<std::size_t, exhaustive_n + 1> row{1};
  for (std::size_t m = 1; m <= n; ++m) {
    for (std::size_t j = m; j > 0; --j) {
      row.at(j) += row.at(j - 1);
    }
  }
  return row.at(k);
}

constexpr int plus_one(mask_t /*mask*/) { return 1; }

// An orientation that two sets can share without a blade having opposite
// signs, so that the signs after a merge can be predicted: -1 for a blade
// that holds e1, +1 otherwise.
constexpr int minus_with_e1(mask_t mask) { return (mask & 1U) == 1U ? -1 : 1; }

// The number of places where `set` differs from the masks of `expected`
// with the signs `sign_of` gives them; a difference in size counts as one.
template <std::size_t K>
constexpr int mismatches(const core::blade_set<K>& set,
                         const mask_list& expected, auto sign_of) {
  int count = K == expected.size ? 0 : 1;
  for (std::size_t i = 0; i < std::min(K, expected.size); ++i) {
    const mask_t mask = expected.at.at(i);
    const bool same =
        set.blades.at(i).mask == mask && set.blades.at(i).sign == sign_of(mask);
    count += same ? 0 : 1;
  }
  return count;
}

template <std::size_t N>
consteval int full_mismatches() {
  return mismatches(core::full<N>, canonical<N>, plus_one);
}

template <std::size_t N, int K>
consteval int of_grade_mismatches() {
  const mask_list expected = of_grades(canonical<N>, K, K);
  const bool counted =
      core::of_grade<N, K>.size() == pascal(N, static_cast<std::size_t>(K));
  return mismatches(core::of_grade<N, K>, expected, plus_one) +
         (counted ? 0 : 1);
}

template <std::size_t N, std::size_t... K>
consteval int every_grade_mismatches(std::index_sequence<K...> /*grades*/) {
  return (of_grade_mismatches<N, static_cast<int>(K)>() + ...);
}

// of_grade<N, K> for every K from 0 to N.
template <std::size_t N>
consteval int every_grade_mismatches() {
  return every_grade_mismatches<N>(std::make_index_sequence<N + 1>());
}

template <std::size_t N, int J, int K>
consteval int merged_grades_mismatches() {
  const mask_list expected = of_grades(canonical<N>, J, K);
  return mismatches(core::merge<core::of_grade<N, J>, core::of_grade<N, K>>,
                    expected, plus_one);
}

template <std::size_t N, std::size_t... P>
consteval int every_grade_pair_mismatches(std::index_sequence<P...> /*pairs*/) {
  constexpr std::size_t grades = N + 1;
  return (merged_grades_mismatches<N, static_cast<int>(P / grades),
                                   static_cast<int>(P % grades)>() +
          ...);
}

// merge<of_grade<N, J>, of_grade<N, K>> for every J and K from 0 to N.
template <std::size_t N>
consteval int every_grade_pair_mismatches() {
  return every_grade_pair_mismatches<N>(
      std::make_index_sequence<(N + 1) * (N + 1)>());
}

// The blades of n generators chosen by the bits of Subset, bit m for the
// mask m, with the signs of minus_with_e1. They are listed from the largest
// mask down, which is not canonical order, so a merge has to reorder them.
template <std::size_t N, unsigned Subset>
consteval auto chosen_blades() {
  core::blade_set<static_cast<std::size_t>(std::popcount(Subset))> set{};
  std::size_t at = 0;
  for (mask_t mask = blade_count(N); mask-- > 0;) {
    if (((Subset >> mask) & 1U) == 1U) {
      set.blades.at(at) = {.mask = mask, .sign = minus_with_e1(mask)};
      ++at;
    }
  }
  return set;
}

template <std::size_t N, unsigned S, unsigned T>
consteval int merged_subsets_mismatches() {
  const mask_list& all = canonical<N>;
  mask_list expected;
  for (std::size_t i = 0; i < all.size; ++i) {
    if ((((S | T) >> all.at.at(i)) & 1U) == 1U) {
      push(expected, all.at.at(i));
    }
  }
  constexpr auto a = chosen_blades<N, S>();
  constexpr auto b = chosen_blades<N, T>();
  return mismatches(core::merge<a, b>, expected, minus_with_e1);
}

template <std::size_t N, std::size_t... P>
consteval int every_subset_pair_mismatches(std::index_sequence<P...> /*p*/) {
  constexpr std::size_t subsets = std::size_t{1} << blade_count(N);
  return (merged_subsets_mismatches<N, static_cast<unsigned>(P / subsets),
                                    static_cast<unsigned>(P % subsets)>() +
          ...);
}

// merge<A, B> for every pair of sets of blades of N generators.
template <std::size_t N>
consteval int every_subset_pair_mismatches() {
  constexpr std::size_t subsets = std::size_t{1} << blade_count(N);
  return every_subset_pair_mismatches<N>(
      std::make_index_sequence<subsets * subsets>());
}

// The places where index_of<Set, mask> is not the position of the mask in
// Expected, for every mask of Expected.
template <core::blade_set Set, mask_list Expected, std::size_t... I>
consteval int index_mismatches(std::index_sequence<I...> /*positions*/) {
  return ((core::index_of<Set, Expected.at.at(I)> == I ? 0 : 1) + ... + 0);
}

template <std::size_t N>
consteval int full_index_mismatches() {
  constexpr mask_list expected = canonical<N>;
  return index_mismatches<core::full<N>, expected>(
      std::make_index_sequence<expected.size>());
}

template <std::size_t N, int K>
consteval int one_grade_index_mismatches() {
  constexpr mask_list expected = of_grades(canonical<N>, K, K);
  return index_mismatches<core::of_grade<N, K>, expected>(
      std::make_index_sequence<expected.size>());
}

template <std::size_t N, std::size_t... K>
consteval int grade_index_mismatches(std::index_sequence<K...> /*grades*/) {
  return (one_grade_index_mismatches<N, static_cast<int>(K)>() + ...);
}

// index_of<of_grade<N, K>, mask> for every K and every mask of grade K.
template <std::size_t N>
consteval int grade_index_mismatches() {
  return grade_index_mismatches<N>(std::make_index_sequence<N + 1>());
}

// Random pairs of sets for merge and index_of. The seed is fixed, so every
// build draws the same cases.
constexpr std::uint64_t random_seed = 0x5eed'b1ad'e5e7'0001;
constexpr std::size_t random_cases = 30;
constexpr std::size_t most_random_blades = 8;

// The widths the cases cycle through, from small algebras to the widest.
constexpr std::array<std::size_t, 7> random_widths{
    3, 4, 5, 6, 9, 12, mask_width,
};

// A fixed-seed integer hash that turns nearby inputs into unrelated outputs.
// Its statistical quality does not matter here, only that every build draws
// the same numbers.
constexpr std::uint64_t mix(std::uint64_t x) {
  x += 0x9e37'79b9'7f4a'7c15U;
  x = (x ^ (x >> 30U)) * 0xbf58'476d'1ce4'e5b9U;
  x = (x ^ (x >> 27U)) * 0x94d0'49bb'1331'11ebU;
  return x ^ (x >> 31U);
}

// The k-th random number of case c.
constexpr std::uint64_t draw(std::size_t c, std::size_t k) {
  return mix(random_seed + (c * 10'000) + k);
}

constexpr std::size_t random_n(std::size_t c) {
  return random_widths.at(c % random_widths.size());
}

// A sign for each mask that both sets of a case share, so that no mask is in
// both with opposite signs.
constexpr int random_sign(mask_t mask) {
  return (mix(mask) & 1U) == 1U ? -1 : 1;
}

// The masks of set `which` (0 or 1) of case c: 1 to most_random_blades
// distinct masks of the case's algebra, in the order drawn.
constexpr mask_list random_masks(std::size_t c, std::size_t which) {
  const std::size_t n = random_n(c);
  const mask_t all = n == mask_width ? ~mask_t{0} : blade_count(n) - 1;
  const std::size_t size = 1 + (draw(c, 1 + which) % most_random_blades);
  mask_list list;
  for (std::size_t d = 0; list.size < size; ++d) {
    const auto mask =
        static_cast<mask_t>(draw(c, 100 + (which * 1000) + d)) & all;
    bool drawn = false;
    for (std::size_t i = 0; i < list.size; ++i) {
      drawn = drawn || list.at.at(i) == mask;
    }
    if (!drawn) {
      push(list, mask);
    }
  }
  return list;
}

template <std::size_t C, std::size_t Which>
consteval auto random_set() {
  constexpr mask_list list = random_masks(C, Which);
  core::blade_set<list.size> set{};
  for (std::size_t i = 0; i < list.size; ++i) {
    const mask_t mask = list.at.at(i);
    set.blades.at(i) = {.mask = mask, .sign = random_sign(mask)};
  }
  return set;
}

// The masks of both sets of case c, each once, sorted by grade and then by
// index list.
constexpr mask_list random_union(std::size_t c) {
  mask_list all;
  for (std::size_t which = 0; which < 2; ++which) {
    const mask_list list = random_masks(c, which);
    for (std::size_t i = 0; i < list.size; ++i) {
      bool listed = false;
      for (std::size_t j = 0; j < all.size; ++j) {
        listed = listed || all.at.at(j) == list.at.at(i);
      }
      if (!listed) {
        push(all, list.at.at(i));
      }
    }
  }
  std::ranges::sort(std::span(all.at).first(all.size), by_grade_then_indices);
  return all;
}

// The places where merge of the two sets of case C differs from their union
// with the shared signs, and where index_of misses a blade of the result.
template <std::size_t C>
consteval int random_case_mismatches() {
  constexpr auto a = random_set<C, 0>();
  constexpr auto b = random_set<C, 1>();
  constexpr auto merged = core::merge<a, b>;
  constexpr mask_list expected = random_union(C);
  return mismatches(merged, expected, random_sign) +
         index_mismatches<merged, expected>(
             std::make_index_sequence<expected.size>());
}

// Each case is its own constant evaluation, because the compilers limit the
// steps of one.
template <std::size_t C>
constexpr int random_case = random_case_mismatches<C>();

// The mismatches of every case, by case, so that a failure names its case.
template <std::size_t... C>
constexpr std::array<int, sizeof...(C)>
random_case_mismatch_counts(std::index_sequence<C...> /*cases*/) {
  return {random_case<C>...};
}

// The characters of a name, held by value.
struct name_text {
  std::array<char, 1 + mask_width> at{};
  std::size_t size = 0;

  [[nodiscard]] constexpr std::string_view view() const {
    return {at.data(), size};
  }
};

// "0x" and the sixteen hexadecimal digits of x, as the seed is written.
constexpr name_text hex_text(std::uint64_t x) {
  name_text text;
  text.at.at(0) = '0';
  text.at.at(1) = 'x';
  text.size = 2;
  for (int shift = 60; shift >= 0; shift -= 4) {
    const auto digit =
        static_cast<int>((x >> static_cast<unsigned>(shift)) & 0xFU);
    text.at.at(text.size) =
        static_cast<char>(digit < 10 ? '0' + digit : 'a' + digit - 10);
    ++text.size;
  }
  return text;
}

// "1" for the scalar, otherwise e and the indices in increasing order.
constexpr name_text name_of(mask_t mask) {
  name_text name;
  const index_list list = indices_of(mask);
  if (list.size == 0) {
    name.at.at(0) = '1';
    name.size = 1;
    return name;
  }
  name.at.at(0) = 'e';
  name.size = 1;
  for (std::size_t p = 0; p < list.size; ++p) {
    name.at.at(name.size) = static_cast<char>('0' + list.at.at(p));
    ++name.size;
  }
  return name;
}

// The characters of a string literal, built at compile time, so that the
// list of names can be a template argument.
template <std::size_t Length>
struct literal {
  // NOLINTNEXTLINE(*-avoid-c-arrays): it stands for a string literal.
  char text[Length];
};

// Every canonical name of n generators, in numeric order of the masks,
// separated by spaces.
template <std::size_t N>
consteval std::size_t canonical_list_length() {
  std::size_t length = 0;
  for (mask_t mask = 0; mask < blade_count(N); ++mask) {
    length += name_of(mask).size + 1; // a space, or the final null
  }
  return length;
}

template <std::size_t N>
consteval literal<canonical_list_length<N>()> canonical_list() {
  literal<canonical_list_length<N>()> list{};
  std::size_t at = 0;
  for (mask_t mask = 0; mask < blade_count(N); ++mask) {
    for (const char c : name_of(mask).view()) {
      list.text[at] = c;
      ++at;
    }
    list.text[at] = ' ';
    ++at;
  }
  list.text[at - 1] = '\0';
  return list;
}

template <std::size_t N>
constexpr auto canonical_names = canonical_list<N>();

// The places where the blades parsed from every canonical name of n
// generators, listed in numeric order of the masks, are not those masks in
// that order with sign +1.
template <std::size_t N>
consteval int parsed_canonical_mismatches() {
  constexpr auto set = core::blades<euclidean<N>, canonical_names<N>.text>;
  int count = set.size() == blade_count(N) ? 0 : 1;
  for (std::size_t i = 0; i < std::min<std::size_t>(set.size(), blade_count(N));
       ++i) {
    const auto mask = static_cast<mask_t>(i);
    const core::oriented_blade expected{.mask = mask, .sign = 1};
    count += set.blades.at(i) == expected ? 0 : 1;
  }
  return count;
}

// The mask of an oriented name such as e31: the generators it names.
constexpr mask_t mask_named(std::string_view name) {
  mask_t mask = 0;
  for (const char digit : name.substr(1)) {
    mask |= mask_t{1} << static_cast<unsigned>(digit - '1');
  }
  return mask;
}

// The sign of an oriented name: the parity of its pairs of positions p < q
// whose indices are out of order, each pair one transposition of the sort.
constexpr int inversion_sign(std::string_view name) {
  int inversions = 0;
  for (std::size_t p = 1; p < name.size(); ++p) {
    for (std::size_t q = p + 1; q < name.size(); ++q) {
      inversions += name[p] > name[q] ? 1 : 0;
    }
  }
  return inversions % 2 == 0 ? 1 : -1;
}

// The rigid(3) list written out from oriented names, for the golden values:
// the names separated by spaces.
template <std::size_t Size>
constexpr std::size_t
joined_length(const std::array<golden::oriented_name, Size>& names) {
  std::size_t length = 0;
  for (const golden::oriented_name& row : names) {
    length += row.name.size() + 1;
  }
  return length;
}

template <std::size_t Length, std::size_t Size>
constexpr literal<Length>
joined(const std::array<golden::oriented_name, Size>& names) {
  literal<Length> list{};
  std::size_t at = 0;
  for (const golden::oriented_name& row : names) {
    for (const char c : row.name) {
      list.text[at] = c;
      ++at;
    }
    list.text[at] = ' ';
    ++at;
  }
  list.text[at - 1] = '\0';
  return list;
}

// A set is a class-type template argument, and equal sets are the same
// argument: the same blades in the same order give the same specialization,
// however they were built, while another order or another sign gives a
// different one.
template <core::blade_set Set>
struct stored_on {};

constexpr auto e12_e13 = std::to_array<core::oriented_blade>({
    {.mask = 0b0011, .sign = 1},
    {.mask = 0b0101, .sign = 1},
});
static_assert(std::is_same_v<stored_on<core::blades<euclidean<4>, "e12 e13">>,
                             stored_on<core::blade_set<2>{e12_e13}>>);
static_assert(
    std::is_same_v<stored_on<core::blades<euclidean<4>, "e12 e13">>,
                   stored_on<core::blades<euclidean<4>, "  e12   e13 ">>>);
static_assert(std::is_same_v<stored_on<core::blades<euclidean<3>, "e1 e2 e3">>,
                             stored_on<core::of_grade<3, 1>>>);
static_assert(
    std::is_same_v<stored_on<core::blades<euclidean<2>, "1 e1 e2 e12">>,
                   stored_on<core::full<2>>>);
static_assert(std::is_same_v<
              stored_on<core::merge<core::blades<euclidean<4>, "e23 e12">,
                                    core::blades<euclidean<4>, "e31">>>,
              stored_on<core::merge<core::blades<euclidean<4>, "e31">,
                                    core::blades<euclidean<4>, "e12 e23">>>>);
static_assert(!std::is_same_v<stored_on<core::blades<euclidean<4>, "e13">>,
                              stored_on<core::blades<euclidean<4>, "e31">>>);
static_assert(
    !std::is_same_v<stored_on<core::blades<euclidean<4>, "e12 e13">>,
                    stored_on<core::blades<euclidean<4>, "e13 e12">>>);
static_assert(std::is_trivially_copyable_v<core::blade_set<3>>);
static_assert(std::is_aggregate_v<core::blade_set<3>>);
static_assert(std::is_aggregate_v<core::oriented_blade>);

// The widest algebra a blade mask can hold is accepted: its mask_bits
// generators, and its antiscalar, the one blade of the top grade, whose mask
// has every bit set.
static_assert(core::of_grade<mask_width, 1>.size() == mask_width);
static_assert(core::of_grade<mask_width, static_cast<int>(mask_width)>.size() ==
              1);
static_assert(
    core::of_grade<mask_width, static_cast<int>(mask_width)>.blades[0].mask ==
    ~mask_t{0});

} // namespace

// Reference: independent value: an oriented name lists generators in the
//   order of their product, by the golden oriented names of rigid(3), which
//   follow Lengyel's rigid geometric algebra; proof: the sign of each name is
//   the sign of the permutation that sorts its indices, counted here pair by
//   pair, and the mask holds the indices it names
TEST_CASE("The oriented names of rigid(3) parse to their masks and signs") {
  constexpr std::size_t length = joined_length(golden::oriented_names::rigid_3);
  static constexpr literal<length> names =
      joined<length>(golden::oriented_names::rigid_3);
  constexpr auto set = core::blades<rga3, names.text>;

  REQUIRE(set.size() == golden::oriented_names::rigid_3.size());
  for (std::size_t i = 0; i < set.size(); ++i) {
    const golden::oriented_name& row = golden::oriented_names::rigid_3.at(i);
    CAPTURE(i);
    CHECK(set.blades.at(i).mask == row.blade.blade);
    CHECK(set.blades.at(i).sign == row.blade.sign);
    if (row.name != "1") {
      CHECK(mask_named(row.name) == row.blade.blade);
      CHECK(inversion_sign(row.name) == row.blade.sign);
    }
  }
}

// Reference: independent value: an oriented name lists generators in the
//   order of their product, written out for the line of rigid(3), e41 e42
//   e43 e23 e31 e12, whose direction blades e41 = -e14, e42 = -e24, and
//   e43 = -e34 and moment blade e31 = -e13 carry sign -1
TEST_CASE("The line of rigid(3) parses to -e14, -e24, -e34, +e23, -e13, "
          "+e12") {
  constexpr auto line = core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
  constexpr auto expected = std::to_array<core::oriented_blade>({
      {.mask = 0b1001, .sign = -1},
      {.mask = 0b1010, .sign = -1},
      {.mask = 0b1100, .sign = -1},
      {.mask = 0b0110, .sign = 1},
      {.mask = 0b0101, .sign = -1},
      {.mask = 0b0011, .sign = 1},
  });
  CHECK(line.blades == expected);
}

// Reference: independent value: an oriented name lists generators in the
//   order of their product, written out by hand: e41 = -e14, e23 = +e23,
//   e31 = -e13 in rigid(3), e45 = +e45 and e54 = -e45 in conformal(3), and e9
//   = +e9 in R(9, 0, 0); contract: blades reads only the number of generators
//   of the algebra, so a name may use every index up to it, whatever the
//   metric
TEST_CASE("blades reads names in the algebra its tag names") {
  constexpr auto rigid_set = std::to_array<core::oriented_blade>({
      {.mask = 0b1001, .sign = -1},
      {.mask = 0b0110, .sign = 1},
      {.mask = 0b0101, .sign = -1},
  });
  CHECK(core::blades<rga3, "e41 e23 e31">.blades == rigid_set);

  constexpr auto conformal_set = std::to_array<core::oriented_blade>({
      {.mask = 0b11000, .sign = 1},
      {.mask = 0b11000, .sign = -1},
  });
  CHECK(core::blades<conformal3, "e45">.blades[0] == conformal_set[0]);
  CHECK(core::blades<conformal3, "e54">.blades[0] == conformal_set[1]);

  CHECK(core::blades<euclidean<9>, "e9">.blades[0] ==
        core::oriented_blade{.mask = mask_t{1} << 8U, .sign = 1});
}

// Reference: proof: a canonical name lists its indices in increasing order,
//   so the permutation that sorts them is the identity and its sign is +1,
//   for every canonical name of n generators, for n from 1 to 9, the largest n
//   whose indices are single digits
TEST_CASE("Every canonical name parses to its mask with sign +1, for n from "
          "1 to 9") {
  CHECK(parsed_canonical_mismatches<1>() == 0);
  CHECK(parsed_canonical_mismatches<2>() == 0);
  CHECK(parsed_canonical_mismatches<3>() == 0);
  CHECK(parsed_canonical_mismatches<4>() == 0);
  CHECK(parsed_canonical_mismatches<5>() == 0);
  CHECK(parsed_canonical_mismatches<6>() == 0);
  CHECK(parsed_canonical_mismatches<7>() == 0);
  CHECK(parsed_canonical_mismatches<8>() == 0);
  CHECK(parsed_canonical_mismatches<9>() == 0);
}

// Reference: independent value: bit i - 1 of a blade mask stands for e_i, by
//   the golden canonical names of every blade of four and of five generators;
//   proof: every mask of nine generators against a name built here from its
//   indices
TEST_CASE("canonical_name lists the indices of a mask in increasing order") {
  for (const golden::named_blade& row : golden::blade_names::n4) {
    CAPTURE(row.blade);
    CHECK(core::canonical_name(row.blade).text() == row.name);
  }
  for (const golden::named_blade& row : golden::blade_names::n5) {
    CAPTURE(row.blade);
    CHECK(core::canonical_name(row.blade).text() == row.name);
  }
  for (mask_t mask = 0; mask < blade_count(core::max_compact_generators);
       ++mask) {
    CAPTURE(mask);
    CHECK(core::canonical_name(mask).text() == name_of(mask).view());
  }
}

// Reference: independent value: results are ordered by grade, then by index
//   list, by the golden order of every blade of four and of five generators
TEST_CASE("full<4> and full<5> list their blades in the golden result order") {
  REQUIRE(core::full<4>.size() == golden::result_order::n4.size());
  for (std::size_t i = 0; i < core::full<4>.size(); ++i) {
    CAPTURE(i);
    CHECK(core::full<4>.blades.at(i).mask == golden::result_order::n4.at(i));
    CHECK(core::full<4>.blades.at(i).sign == 1);
  }
  REQUIRE(core::full<5>.size() == golden::result_order::n5.size());
  for (std::size_t i = 0; i < core::full<5>.size(); ++i) {
    CAPTURE(i);
    CHECK(core::full<5>.blades.at(i).mask == golden::result_order::n5.at(i));
    CHECK(core::full<5>.blades.at(i).sign == 1);
  }
}

// Reference: proof: results are ordered by grade, then by index list, for
//   every blade of n generators with n from 1 to 6, against a sort of the
//   index lists written here; proof: that order differs from ordering by
//   grade and then by the mask as a number from n = 4, where e14 = 0b1001
//   comes before e23 = 0b0110, and agrees with it below
TEST_CASE("full<N> lists every blade by grade and then by index list, for n "
          "from 1 to 6, which is not numeric order within a grade") {
  CHECK(full_mismatches<1>() == 0);
  CHECK(full_mismatches<2>() == 0);
  CHECK(full_mismatches<3>() == 0);
  CHECK(full_mismatches<4>() == 0);
  CHECK(full_mismatches<5>() == 0);
  CHECK(full_mismatches<6>() == 0);

  for (std::size_t n = 1; n <= exhaustive_n; ++n) {
    CAPTURE(n);
    const bool same =
        canonical_blades(n) == sorted_blades(n, by_grade_then_mask);
    CHECK(same == (n <= 3));
  }
  CHECK(mismatches(core::full<4>, sorted_blades(4, by_grade_then_mask),
                   plus_one) > 0);
}

// Reference: proof: the blades of grade k of n generators, for every n from
//   1 to 6 and k from 0 to n, against the canonical list written here kept
//   to grade k; proof: their number is C(n, k), by Pascal's rule
TEST_CASE("of_grade<N, K> holds the C(n, k) blades of grade k in canonical "
          "order, for n from 1 to 6") {
  CHECK(every_grade_mismatches<1>() == 0);
  CHECK(every_grade_mismatches<2>() == 0);
  CHECK(every_grade_mismatches<3>() == 0);
  CHECK(every_grade_mismatches<4>() == 0);
  CHECK(every_grade_mismatches<5>() == 0);
  CHECK(every_grade_mismatches<6>() == 0);
}

// Reference: proof: the union of two sets of blades, each blade once, in
//   canonical order, for the sets of grades j and k of n generators with
//   every j and k from 0 to n and n from 1 to 6, against the canonical list
//   written here kept to grades j and k
TEST_CASE("merge of the blades of grades j and k is their union in canonical "
          "order, for n from 1 to 6") {
  CHECK(every_grade_pair_mismatches<1>() == 0);
  CHECK(every_grade_pair_mismatches<2>() == 0);
  CHECK(every_grade_pair_mismatches<3>() == 0);
  CHECK(every_grade_pair_mismatches<4>() == 0);
  CHECK(every_grade_pair_mismatches<5>() == 0);
  CHECK(every_grade_pair_mismatches<6>() == 0);
}

// Reference: proof: the union of two sets of blades in canonical order, each
//   blade with the sign it has in its set, for every pair of sets of blades
//   of one and of two generators, listed out of canonical order
TEST_CASE("merge of every pair of sets of blades of up to two generators "
          "reorders their union and keeps each sign") {
  CHECK(every_subset_pair_mismatches<1>() == 0);
  CHECK(every_subset_pair_mismatches<2>() == 0);
}

// Reference: independent value: an oriented name lists generators in the
//   order of their product, so the line e41 e42 e43 e23 e31 e12 and the plane
//   e423 e431 e412 e321 of rigid(3) hold -e14, -e24, -e34, +e23, -e13, +e12
//   and +e234, -e134, +e124, -e123, written out here in canonical order
TEST_CASE("merge of the line and the plane of rigid(3) keeps the sign of "
          "each oriented blade") {
  constexpr auto line = core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
  constexpr auto plane = core::blades<rga3, "e423 e431 e412 e321">;
  constexpr auto expected = std::to_array<core::oriented_blade>({
      {.mask = 0b0011, .sign = 1},
      {.mask = 0b0101, .sign = -1},
      {.mask = 0b1001, .sign = -1},
      {.mask = 0b0110, .sign = 1},
      {.mask = 0b1010, .sign = -1},
      {.mask = 0b1100, .sign = -1},
      {.mask = 0b0111, .sign = -1},
      {.mask = 0b1011, .sign = 1},
      {.mask = 0b1101, .sign = -1},
      {.mask = 0b1110, .sign = 1},
  });
  CHECK(core::merge<line, plane>.blades == expected);
  CHECK(core::merge<plane, line>.blades == expected);

  constexpr auto line_in_canonical_order = std::to_array<core::oriented_blade>({
      {.mask = 0b0011, .sign = 1},
      {.mask = 0b0101, .sign = -1},
      {.mask = 0b1001, .sign = -1},
      {.mask = 0b0110, .sign = 1},
      {.mask = 0b1010, .sign = -1},
      {.mask = 0b1100, .sign = -1},
  });
  CHECK(core::merge<line, line>.blades == line_in_canonical_order);
}

// Reference: independent value: the union of e2 with e3 e1 e2, each blade
//   once in canonical order, is e1 e2 e3 with sign +1, written out here, where
//   the second set is out of canonical order and shares e2 with the first, so
//   the merge must keep track of which set each listed blade came from as it
//   reorders them
TEST_CASE("merge of e2 with e3 e1 e2 is e1 e2 e3") {
  constexpr auto expected = std::to_array<core::oriented_blade>({
      {.mask = 0b001, .sign = 1},
      {.mask = 0b010, .sign = 1},
      {.mask = 0b100, .sign = 1},
  });
  constexpr auto e2 = core::blades<euclidean<3>, "e2">;
  constexpr auto e3_e1_e2 = core::blades<euclidean<3>, "e3 e1 e2">;
  CHECK(core::merge<e2, e3_e1_e2>.blades == expected);
}

// Reference: identity: merge of two sets is their union in canonical order,
//   each blade with its sign, and index_of finds each blade of the result at
//   its position, for 30 random pairs of sets of 1 to 8 blades with n
//   cycling through 3, 4, 5, 6, 9, 12, and 32, listed in the order drawn,
//   against a union sorted by grade and index list in the test
TEST_CASE("merge and index_of agree with the union of 30 random pairs of "
          "sets") {
  constexpr auto counts =
      random_case_mismatch_counts(std::make_index_sequence<random_cases>());
  constexpr name_text seed = hex_text(random_seed);
  INFO("random_seed = " << doctest::String(seed.at.data()));
  for (std::size_t c = 0; c < counts.size(); ++c) {
    CAPTURE(c);
    CHECK(counts.at(c) == 0);
  }
}

// Reference: proof: index_of gives the position of a mask in its set, for
//   every blade of full<N> and of each of_grade<N, K> with n from 1 to 6,
//   against the positions in the canonical lists written here
TEST_CASE("index_of finds every blade of full<N> and of_grade<N, K> at its "
          "canonical position, for n from 1 to 6") {
  CHECK(full_index_mismatches<1>() == 0);
  CHECK(full_index_mismatches<2>() == 0);
  CHECK(full_index_mismatches<3>() == 0);
  CHECK(full_index_mismatches<4>() == 0);
  CHECK(full_index_mismatches<5>() == 0);
  CHECK(full_index_mismatches<6>() == 0);
  CHECK(grade_index_mismatches<1>() == 0);
  CHECK(grade_index_mismatches<2>() == 0);
  CHECK(grade_index_mismatches<3>() == 0);
  CHECK(grade_index_mismatches<4>() == 0);
  CHECK(grade_index_mismatches<5>() == 0);
  CHECK(grade_index_mismatches<6>() == 0);
}

// Reference: independent value: an oriented name lists generators in the
//   order of their product, so in the line e41 e42 e43 e23 e31 e12 of
//   rigid(3) the mask of e13 is at position 4 and that of e14 at position 0,
//   whatever their signs
TEST_CASE("index_of finds a blade of an oriented set by its mask") {
  constexpr auto line = core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
  CHECK(core::index_of<line, 0b1001> == 0);
  CHECK(core::index_of<line, 0b1010> == 1);
  CHECK(core::index_of<line, 0b1100> == 2);
  CHECK(core::index_of<line, 0b0110> == 3);
  CHECK(core::index_of<line, 0b0101> == 4);
  CHECK(core::index_of<line, 0b0011> == 5);
}

// Reference: proof: results are ordered by grade, then by index list, for
//   every pair of masks of six generators, against the comparison of index
//   lists written here, evaluated at run time
TEST_CASE("canonical_less orders masks by grade and then by index list") {
  for (mask_t a = 0; a < blade_count(exhaustive_n); ++a) {
    for (mask_t b = 0; b < blade_count(exhaustive_n); ++b) {
      CAPTURE(a);
      CAPTURE(b);
      CHECK(core::canonical_less(a, b) == by_grade_then_indices(a, b));
    }
  }
}
