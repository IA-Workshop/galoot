// Tests of galoot/core/bits.hpp against the definitions it implements.
//
// Expected values come from a reference written here that shares no code with
// the header: a blade is the increasing list of its generator indices, read
// bit by bit, and every sign is the parity of a permutation counted pair by
// pair. Where the inputs are finite the checks are exhaustive, which makes
// them proofs over those ranges rather than samples. A second route compares
// the header with the golden values of reference/golden.hpp, which the
// reference scripts in test/reference/ compute in Python.
#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <span>
#include <string_view>
#include <utility>

#include <galoot/core/bits.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>

namespace {

using galoot::core::mask_t;

// Every generator a mask can name, read from the mask type itself.
constexpr std::size_t mask_width = std::numeric_limits<mask_t>::digits;

static_assert(galoot::core::mask_bits == mask_width);

// A list of generator indices, counting from 1; two blades of the widest mask
// fit end to end.
struct index_list {
  std::array<int, 2 * mask_width> at{};
  int size = 0;
};

constexpr void push(index_list& list, int index) {
  list.at.at(static_cast<std::size_t>(list.size)) = index;
  ++list.size;
}

constexpr int get(const index_list& list, int p) {
  return list.at.at(static_cast<std::size_t>(p));
}

constexpr bool contains(const index_list& list, int index) {
  for (int p = 0; p < list.size; ++p) {
    if (get(list, p) == index) {
      return true;
    }
  }
  return false;
}

// Bit i - 1 of a mask stands for e_i, and the mask's blade lists its indices
// in increasing order.
constexpr index_list indices_of(mask_t mask) {
  index_list list;
  for (std::size_t bit = 0; bit < mask_width; ++bit) {
    if (((mask >> bit) & 1U) == 1U) {
      push(list, static_cast<int>(bit) + 1);
    }
  }
  return list;
}

constexpr mask_t mask_of(const index_list& list) {
  mask_t mask = 0;
  for (int p = 0; p < list.size; ++p) {
    mask |= mask_t{1} << static_cast<unsigned>(get(list, p) - 1);
  }
  return mask;
}

// The indices among 1 to n that `list` lacks, in increasing order.
constexpr index_list missing_from(const index_list& list, std::size_t n) {
  index_list rest;
  for (int i = 1; std::cmp_less_equal(i, n); ++i) {
    if (!contains(list, i)) {
      push(rest, i);
    }
  }
  return rest;
}

constexpr index_list concat(const index_list& a, const index_list& b) {
  index_list both = a;
  for (int p = 0; p < b.size; ++p) {
    push(both, get(b, p));
  }
  return both;
}

constexpr index_list reversed(const index_list& list) {
  index_list back;
  for (int p = list.size - 1; p >= 0; --p) {
    push(back, get(list, p));
  }
  return back;
}

// The sign of the permutation that sorts `list` by swapping neighbors that are
// out of order: the parity of its pairs of positions p < q holding
// list[p] > list[q]. Equal indices are never swapped.
constexpr int sorting_sign(const index_list& list) {
  int inversions = 0;
  for (int p = 0; p < list.size; ++p) {
    for (int q = p + 1; q < list.size; ++q) {
      if (get(list, p) > get(list, q)) {
        ++inversions;
      }
    }
  }
  return inversions % 2 == 0 ? 1 : -1;
}

// The exterior product e_A ∧ e_B as a sign and a mask: zero when A and B share
// a generator, otherwise the sign of sorting A followed by B times the blade of
// A ∪ B.
struct signed_blade {
  int sign;
  mask_t mask;
};

constexpr signed_blade wedge(mask_t a, mask_t b) {
  const index_list la = indices_of(a);
  const index_list lb = indices_of(b);
  for (int p = 0; p < la.size; ++p) {
    if (contains(lb, get(la, p))) {
      return {.sign = 0, .mask = 0};
    }
  }
  const index_list both = concat(la, lb);
  return {.sign = sorting_sign(both), .mask = mask_of(both)};
}

// The antiscalar is +e_12...n, the generators in increasing order with sign
// +1.
constexpr signed_blade antiscalar(std::size_t n) {
  index_list all;
  for (int i = 1; std::cmp_less_equal(i, n); ++i) {
    push(all, i);
  }
  return {.sign = 1, .mask = mask_of(all)};
}

constexpr bool operator==(signed_blade x, signed_blade y) {
  return x.sign == y.sign && x.mask == y.mask;
}

constexpr int expected_grade(mask_t a) { return indices_of(a).size; }

constexpr int expected_antigrade(mask_t a, std::size_t n) {
  return missing_from(indices_of(a), n).size;
}

constexpr int expected_reorder_sign(mask_t a, mask_t b) {
  return sorting_sign(concat(indices_of(a), indices_of(b)));
}

constexpr mask_t expected_complement_mask(mask_t a, std::size_t n) {
  return mask_of(missing_from(indices_of(a), n));
}

// u ∧ (s e_C) = +e_12...n holds exactly when s is the sign of sorting A
// followed by C, because a sign is its own inverse.
constexpr int expected_right_sign(mask_t a, std::size_t n) {
  const index_list la = indices_of(a);
  return sorting_sign(concat(la, missing_from(la, n)));
}

// (s e_C) ∧ u = +e_12...n: the sign of sorting C followed by A.
constexpr int expected_left_sign(mask_t a, std::size_t n) {
  const index_list la = indices_of(a);
  return sorting_sign(concat(missing_from(la, n), la));
}

// The reverse writes the generators of a blade in the opposite order, so its
// sign is that of sorting the reversed list.
constexpr int expected_reverse_sign(mask_t a) {
  return sorting_sign(reversed(indices_of(a)));
}

// The antireverse is the reverse with the antigrade in place of the grade:
// the sign of reversing a list as long as the blade's complement.
constexpr int expected_antireverse_sign(mask_t a, std::size_t n) {
  return sorting_sign(reversed(missing_from(indices_of(a), n)));
}

// The grade involution negates each generator.
constexpr int expected_involution_sign(mask_t a) {
  int sign = 1;
  for (int p = 0; p < indices_of(a).size; ++p) {
    sign = -sign;
  }
  return sign;
}

// The geometric product e_A e_B of basis blades in a diagonal metric whose
// generator e_i squares to squares[i - 1]: write A then B, sort by swapping
// out-of-order neighbors (each swap e_j e_i = -e_i e_j for i != j), then
// replace each adjacent pair e_i e_i by its square. The result is a
// coefficient times the blade of what remains.
struct scaled_blade {
  int coefficient;
  mask_t mask;
};

// The diagonal-product test uses every pair of blades of this many generators.
constexpr std::size_t diagonal_n = 6;

constexpr scaled_blade
diagonal_product(mask_t a, mask_t b,
                 const std::array<int, diagonal_n>& squares) {
  index_list word = concat(indices_of(a), indices_of(b));
  int coefficient = 1;
  for (bool swapped = true; swapped;) {
    swapped = false;
    for (int p = 0; p + 1 < word.size; ++p) {
      const auto here = static_cast<std::size_t>(p);
      if (word.at.at(here) > word.at.at(here + 1)) {
        std::swap(word.at.at(here), word.at.at(here + 1));
        coefficient = -coefficient;
        swapped = true;
      }
    }
  }

  index_list rest;
  for (int p = 0; p < word.size; ++p) {
    if (p + 1 < word.size && get(word, p) == get(word, p + 1)) {
      coefficient *= squares.at(static_cast<std::size_t>(get(word, p) - 1));
      ++p;
    } else {
      push(rest, get(word, p));
    }
  }
  return {.coefficient = coefficient, .mask = mask_of(rest)};
}

// The masks of an algebra of n generators, all of them for n up to 8; the
// exhaustive ranges below are these.
constexpr std::size_t exhaustive_n = 8;

constexpr mask_t blade_count(std::size_t n) { return mask_t{1} << n; }

// Every function agrees with its definition on every blade, and the reorder
// sign on every pair of blades, of an algebra of n generators. Run in a
// constant evaluation, so that the compile-time path of the header is proved
// as well as the run-time one.
constexpr bool all_agree(std::size_t n) {
  for (mask_t a = 0; a < blade_count(n); ++a) {
    using namespace galoot::core;
    if (grade(a) != expected_grade(a) ||
        antigrade(a, n) != expected_antigrade(a, n) ||
        complement_mask(a, n) != expected_complement_mask(a, n) ||
        right_complement_sign(a, n) != expected_right_sign(a, n) ||
        left_complement_sign(a, n) != expected_left_sign(a, n) ||
        reverse_sign(a) != expected_reverse_sign(a) ||
        antireverse_sign(a, n) != expected_antireverse_sign(a, n) ||
        grade_involution_sign(a) != expected_involution_sign(a)) {
      return false;
    }
    for (mask_t b = 0; b < blade_count(n); ++b) {
      if (reorder_sign(a, b) != expected_reorder_sign(a, b)) {
        return false;
      }
    }
  }
  return true;
}

static_assert(all_agree(1));
static_assert(all_agree(2));
static_assert(all_agree(3));
static_assert(all_agree(4));

// A sample of the masks of an algebra of n generators, for n too large to
// check exhaustively: the scalar, the antiscalar, each generator, each blade
// of grade n - 1, and three alternating patterns of generators. That is
// 2n + 5 masks, so the widest mask sets the capacity.
struct mask_sample {
  std::array<mask_t, (2 * mask_width) + 5> at{};
  int size = 0;
};

constexpr mask_sample sample_of(std::size_t n) {
  mask_sample s;
  const auto add = [&s](mask_t m) {
    s.at.at(static_cast<std::size_t>(s.size)) = m;
    ++s.size;
  };
  const mask_t all = antiscalar(n).mask;
  add(0);
  add(all);
  for (std::size_t i = 0; i < n; ++i) {
    const mask_t generator = mask_t{1} << i;
    add(generator);
    add(all & ~generator);
  }
  add(all & 0x55555555U);
  add(all & 0xAAAAAAAAU);
  add(all & 0x0F0F0F0FU);
  return s;
}

// The indices of the generators in a canonical blade name: "1" for the
// scalar, or "e" followed by one digit per generator in increasing order, as
// in "e124". Any other text, such as "e21", gives no value.
constexpr std::optional<index_list> indices_named(std::string_view name) {
  index_list list;
  if (name == "1") {
    return list;
  }
  if (name.size() < 2 || name.front() != 'e') {
    return std::nullopt;
  }
  int previous = 0;
  for (const char digit : name.substr(1)) {
    const int index = digit - '0';
    if (index <= previous || index > 9) {
      return std::nullopt;
    }
    push(list, index);
    previous = index;
  }
  return list;
}

// Whether bits gives the complement masks and signs of `rows`, the
// complements of every blade of n generators.
template <std::size_t Size>
constexpr bool
complements_match(const std::array<golden::complement_pair, Size>& rows,
                  std::size_t n) {
  using namespace galoot::core;
  return std::ranges::all_of(rows, [n](const golden::complement_pair& row) {
    return complement_mask(row.blade, n) == row.right.blade &&
           right_complement_sign(row.blade, n) == row.right.sign &&
           complement_mask(row.blade, n) == row.left.blade &&
           left_complement_sign(row.blade, n) == row.left.sign;
  });
}

// Whether bits gives the grade signs of `rows`, the signs of every blade of n
// generators.
template <std::size_t Size>
constexpr bool
grade_signs_match(const std::array<golden::blade_signs, Size>& rows,
                  std::size_t n) {
  using namespace galoot::core;
  return std::ranges::all_of(rows, [n](const golden::blade_signs& row) {
    return reverse_sign(row.blade) == row.reverse &&
           antireverse_sign(row.blade, n) == row.antireverse &&
           grade_involution_sign(row.blade) == row.involution;
  });
}

// Reference: independent value: u ^ right(u) = antiscalar and left(u) ^ u =
//   antiscalar, by the golden complements of every blade of four generators,
//   in a constant evaluation
static_assert(complements_match(golden::complements::n4, 4));
// Reference: independent value: u ^ right(u) = antiscalar and left(u) ^ u =
//   antiscalar, by the golden complements of every blade of five generators,
//   in a constant evaluation
static_assert(complements_match(golden::complements::n5, 5));
// Reference: independent value: the reverse of a grade-k blade is
//   (-1)^(k(k-1)/2) times it, by the golden grade signs of every blade of four
//   generators, in a constant evaluation
static_assert(grade_signs_match(golden::grade_signs::n4, 4));
// Reference: independent value: the reverse of a grade-k blade is
//   (-1)^(k(k-1)/2) times it, by the golden grade signs of every blade of five
//   generators, in a constant evaluation
static_assert(grade_signs_match(golden::grade_signs::n5, 5));

} // namespace

// Reference: independent value: bit i - 1 of a blade mask stands for e_i,
//   worked by hand: the mask 0b1011 is e124, of grade 3, with complement e3
//   in four generators, and bit 1 is e2 and bit 0 is e1, so e2 e1 = -e12
//   gives reorder_sign(0b010, 0b001) = -1 and reorder_sign(0b001, 0b010) = +1
TEST_CASE("Bit i - 1 of a mask stands for e_i, so e124 is the mask 0b1011") {
  // The reference reads masks this way; this case pins the reading itself.
  CHECK(mask_of(indices_of(0b1011U)) == 0b1011U);
  CHECK(indices_of(0b1011U).size == 3);
  CHECK(get(indices_of(0b1011U), 0) == 1);
  CHECK(get(indices_of(0b1011U), 1) == 2);
  CHECK(get(indices_of(0b1011U), 2) == 4);
  CHECK(galoot::core::grade(0b1011U) == 3);
  CHECK(galoot::core::complement_mask(0b1011U, 4) == 0b0100U);
  CHECK(galoot::core::reorder_sign(0b010U, 0b001U) == -1);
  CHECK(galoot::core::reorder_sign(0b001U, 0b010U) == 1);
}

// Reference: independent value: worked by hand, sorting (3, 2, 1) takes three
//   swaps, e2 ^ e1 = -e12, e1 ^ e23 = e123, e12 ^ e2 = 0 because the two
//   share e2, and the antiscalar of four generators is +e1234
TEST_CASE("The reference gives hand-worked sorting signs and exterior "
          "products") {
  // Values worked by hand, so that a defect in the reference cannot pass
  // every case below unnoticed.
  index_list three_two_one;
  push(three_two_one, 3);
  push(three_two_one, 2);
  push(three_two_one, 1);
  CHECK(sorting_sign(three_two_one) == -1); // three inversions
  CHECK(wedge(0b010U, 0b001U) == signed_blade{.sign = -1, .mask = 0b011U});
  CHECK(wedge(0b001U, 0b110U) == signed_blade{.sign = 1, .mask = 0b111U});
  CHECK(wedge(0b011U, 0b010U).sign == 0);
  CHECK(antiscalar(4) == signed_blade{.sign = 1, .mask = 0b1111U});
}

// Reference: independent value: worked by hand, a canonical name is 1 or an e
//   followed by its generators in increasing order, so e124 reads as
//   (1, 2, 4), and e21, e11, 124, e, and ex are not canonical names
TEST_CASE("The blade-name reader accepts only canonical names") {
  CHECK(indices_named("1").value().size == 0);
  const index_list e124 = indices_named("e124").value();
  CHECK(e124.size == 3);
  CHECK(get(e124, 0) == 1);
  CHECK(get(e124, 1) == 2);
  CHECK(get(e124, 2) == 4);
  CHECK_FALSE(indices_named("e21").has_value());
  CHECK_FALSE(indices_named("e11").has_value());
  CHECK_FALSE(indices_named("124").has_value());
  CHECK_FALSE(indices_named("e").has_value());
  CHECK_FALSE(indices_named("ex").has_value());
}

// Reference: proof: the grade of a blade is its number of generators and its
//   antigrade is n minus that. Exhaustive for n up to 8
TEST_CASE("grade and antigrade count the generators in and out of a blade, "
          "for every blade with n up to 8") {
  for (std::size_t n = 1; n <= exhaustive_n; ++n) {
    for (mask_t a = 0; a < blade_count(n); ++a) {
      CAPTURE(n);
      CAPTURE(a);
      CHECK(galoot::core::grade(a) == expected_grade(a));
      CHECK(galoot::core::antigrade(a, n) == expected_antigrade(a, n));
    }
  }
}

// Reference: proof: reorder_sign(A, B) is (-1)^s, where s is the number of
//   swaps of out-of-order neighbors that sort the indices of A followed by
//   those of B, equal indices never swapped. Exhaustive over every pair of
//   masks below 2^8
TEST_CASE("reorder_sign is the sign of sorting A followed by B, for every pair "
          "of masks below 2^8") {
  for (mask_t a = 0; a < blade_count(exhaustive_n); ++a) {
    for (mask_t b = 0; b < blade_count(exhaustive_n); ++b) {
      CAPTURE(a);
      CAPTURE(b);
      CHECK(galoot::core::reorder_sign(a, b) == expected_reorder_sign(a, b));
    }
  }
}

// Reference: proof: u ^ right(u) = antiscalar and left(u) ^ u = antiscalar,
//   so both complements of a blade hold exactly the generators it lacks.
//   Exhaustive for n up to 8
TEST_CASE("complement_mask lists the generators a blade lacks, for every "
          "blade with n up to 8") {
  for (std::size_t n = 1; n <= exhaustive_n; ++n) {
    for (mask_t a = 0; a < blade_count(n); ++a) {
      CAPTURE(n);
      CAPTURE(a);
      CHECK(galoot::core::complement_mask(a, n) ==
            expected_complement_mask(a, n));
    }
  }
}

// Reference: proof: u ^ right(u) = antiscalar and left(u) ^ u = antiscalar
//   make the right sign that of sorting A then its complement C, and the left
//   sign that of sorting C then A. Exhaustive for n up to 8
TEST_CASE("The complement signs match their definitions, for every blade "
          "with n up to 8") {
  for (std::size_t n = 1; n <= exhaustive_n; ++n) {
    for (mask_t a = 0; a < blade_count(n); ++a) {
      CAPTURE(n);
      CAPTURE(a);
      CHECK(galoot::core::right_complement_sign(a, n) ==
            expected_right_sign(a, n));
      CHECK(galoot::core::left_complement_sign(a, n) ==
            expected_left_sign(a, n));
    }
  }
}

// Reference: identity: u ^ right(u) = antiscalar and left(u) ^ u = antiscalar,
//   with the exterior product computed here by sorting. Exhaustive for n up
//   to 8
TEST_CASE("u ∧ right(u) and left(u) ∧ u are the antiscalar, for every blade "
          "with n up to 8") {
  using namespace galoot::core;
  for (std::size_t n = 1; n <= exhaustive_n; ++n) {
    for (mask_t a = 0; a < blade_count(n); ++a) {
      CAPTURE(n);
      CAPTURE(a);
      const mask_t c = complement_mask(a, n);
      const signed_blade u_right = wedge(a, c);
      const signed_blade left_u = wedge(c, a);
      CHECK(signed_blade{.sign = u_right.sign * right_complement_sign(a, n),
                         .mask = u_right.mask} == antiscalar(n));
      CHECK(signed_blade{.sign = left_u.sign * left_complement_sign(a, n),
                         .mask = left_u.mask} == antiscalar(n));
    }
  }
}

// Reference: identity: the left complement undoes the right complement, and
//   left(u) = (-1)^(gr ag) right(u). Exhaustive for n up to 8
TEST_CASE("left(right(u)) = u, and left(u) = (-1)^(gr ag) right(u), for every "
          "blade with n up to 8") {
  using namespace galoot::core;
  for (std::size_t n = 1; n <= exhaustive_n; ++n) {
    for (mask_t a = 0; a < blade_count(n); ++a) {
      CAPTURE(n);
      CAPTURE(a);
      // right(u) = s e_C, and left(s e_C) = s left_sign(C) e_A.
      const mask_t c = complement_mask(a, n);
      CHECK(complement_mask(c, n) == a);
      CHECK(right_complement_sign(a, n) * left_complement_sign(c, n) == 1);

      const int gr_ag = expected_grade(a) * expected_antigrade(a, n);
      const int parity = gr_ag % 2 == 0 ? 1 : -1;
      CHECK(left_complement_sign(a, n) == parity * right_complement_sign(a, n));
    }
  }
}

// Reference: independent value: u ^ right(u) = antiscalar and left(u) ^ u =
//   antiscalar, by the golden complements of every blade of four and of five
//   generators. Complements do not depend on the metric, so the four-generator
//   values serve rigid(3) and every other algebra of four generators
TEST_CASE("The complements of every blade of four and five generators match "
          "the golden values") {
  const auto check = [](std::span<const golden::complement_pair> rows,
                        std::size_t n) {
    for (const golden::complement_pair& row : rows) {
      CAPTURE(n);
      CAPTURE(row.blade);
      CHECK(galoot::core::complement_mask(row.blade, n) == row.right.blade);
      CHECK(galoot::core::right_complement_sign(row.blade, n) ==
            row.right.sign);
      CHECK(galoot::core::complement_mask(row.blade, n) == row.left.blade);
      CHECK(galoot::core::left_complement_sign(row.blade, n) == row.left.sign);
    }
  };
  check(golden::complements::n4, 4);
  check(golden::complements::n5, 5);
}

// Reference: independent value: bit i - 1 of a blade mask stands for e_i, read
//   from the golden canonical names of every blade of four and five generators
TEST_CASE("grade and complement_mask agree with the golden blade names, for "
          "four and five generators") {
  const auto check = [](std::span<const golden::named_blade> names,
                        std::size_t n) {
    for (const golden::named_blade& entry : names) {
      CAPTURE(n);
      CAPTURE(entry.blade);
      const std::optional<index_list> read = indices_named(entry.name);
      REQUIRE(read.has_value());
      const index_list& named = read.value();
      CHECK(mask_of(named) == entry.blade);
      CHECK(galoot::core::grade(entry.blade) == named.size);
      CHECK(galoot::core::complement_mask(entry.blade, n) ==
            mask_of(missing_from(named, n)));
    }
  };
  check(golden::blade_names::n4, 4);
  check(golden::blade_names::n5, 5);
}

// Reference: independent value: the antiscalar is e12...n with sign +1, by the
//   golden antiscalars of one to six generators
TEST_CASE("The right complement of the scalar is the golden antiscalar with "
          "sign +1, for one to six generators") {
  for (std::size_t n = 1; n <= golden::units::antiscalars.size(); ++n) {
    const golden::named_blade& expected = golden::units::antiscalars.at(n - 1);
    CAPTURE(n);
    CHECK(galoot::core::complement_mask(golden::units::scalar.blade, n) ==
          expected.blade);
    CHECK(galoot::core::right_complement_sign(golden::units::scalar.blade, n) ==
          1);
    CHECK(galoot::core::grade(expected.blade) == static_cast<int>(n));
  }
}

// Reference: independent value: the reverse of a grade-k blade is
//   (-1)^(k(k-1)/2) times it, with the antireverse and grade involution signs,
//   by the golden grade signs of every blade of four and five generators
TEST_CASE("The reverse, antireverse, and grade involution signs of every "
          "blade of four and five generators match the golden values") {
  const auto check = [](std::span<const golden::blade_signs> rows,
                        std::size_t n) {
    for (const golden::blade_signs& row : rows) {
      CAPTURE(n);
      CAPTURE(row.blade);
      CHECK(galoot::core::reverse_sign(row.blade) == row.reverse);
      CHECK(galoot::core::antireverse_sign(row.blade, n) == row.antireverse);
      CHECK(galoot::core::grade_involution_sign(row.blade) == row.involution);
    }
  };
  check(golden::grade_signs::n4, 4);
  check(golden::grade_signs::n5, 5);
}

// Reference: proof: the antiscalar is e12...n with sign +1, so the right
//   complement of the scalar is +e12...n and that of the antiscalar is +1,
//   for every n up to the widest mask
TEST_CASE("The right complement of the scalar is +e_12...n and that of the "
          "antiscalar is +1, for every n up to the widest mask") {
  using namespace galoot::core;
  for (std::size_t n = 1; n <= mask_width; ++n) {
    CAPTURE(n);
    CHECK(complement_mask(0, n) == antiscalar(n).mask);
    CHECK(right_complement_sign(0, n) == 1);
    CHECK(complement_mask(antiscalar(n).mask, n) == 0);
    CHECK(right_complement_sign(antiscalar(n).mask, n) == 1);
  }
}

// Reference: proof: the reverse of a grade-k blade is (-1)^(k(k-1)/2) times
//   it, the sign of writing its generators backward. The antireverse does
//   the same with the antigrade, and the grade involution negates each
//   generator. Exhaustive for n up to 8
TEST_CASE("The reverse, antireverse, and grade involution signs match their "
          "definitions, for every blade with n up to 8") {
  for (std::size_t n = 1; n <= exhaustive_n; ++n) {
    for (mask_t a = 0; a < blade_count(n); ++a) {
      CAPTURE(n);
      CAPTURE(a);
      CHECK(galoot::core::reverse_sign(a) == expected_reverse_sign(a));
      CHECK(galoot::core::antireverse_sign(a, n) ==
            expected_antireverse_sign(a, n));
      CHECK(galoot::core::grade_involution_sign(a) ==
            expected_involution_sign(a));
    }
  }
}

// Reference: independent value: each function's value from its definition,
//   by the index-list reference, on a sample of blades (not every blade) at
//   n = 12, 16, and the widest mask
TEST_CASE("Every function matches its definition on a sample of blades at "
          "n = 12, 16, and the widest mask") {
  using namespace galoot::core;
  for (const std::size_t n : std::array<std::size_t, 3>{12, 16, mask_width}) {
    const mask_sample s = sample_of(n);
    for (int p = 0; p < s.size; ++p) {
      const mask_t a = s.at.at(static_cast<std::size_t>(p));
      CAPTURE(n);
      CAPTURE(a);
      CHECK(grade(a) == expected_grade(a));
      CHECK(antigrade(a, n) == expected_antigrade(a, n));
      CHECK(complement_mask(a, n) == expected_complement_mask(a, n));
      CHECK(right_complement_sign(a, n) == expected_right_sign(a, n));
      CHECK(left_complement_sign(a, n) == expected_left_sign(a, n));
      CHECK(reverse_sign(a) == expected_reverse_sign(a));
      CHECK(antireverse_sign(a, n) == expected_antireverse_sign(a, n));
      CHECK(grade_involution_sign(a) == expected_involution_sign(a));
      for (int q = 0; q < s.size; ++q) {
        const mask_t b = s.at.at(static_cast<std::size_t>(q));
        CAPTURE(b);
        CHECK(reorder_sign(a, b) == expected_reorder_sign(a, b));
      }
    }
  }
}

// Reference: proof: in a diagonal metric, sorting e_A e_B by neighbor swaps
//   and replacing each e_i e_i by its square gives reorder_sign(A, B) times
//   the squares of the common generators times e_(A xor B). Exhaustive over
//   the pairs of six generators in two metrics
TEST_CASE("In a diagonal metric, e_A e_B is reorder_sign times the squares of "
          "the common generators times e_(A xor B), for every pair of blades "
          "of six generators") {
  // Euclidean, and one with every kind of square, so that a zero or negative
  // square of a common generator shows in the coefficient.
  constexpr std::array<std::array<int, diagonal_n>, 2> metrics{
      {
          {1, 1, 1, 1, 1, 1},
          {1, -1, 0, -1, 1, 0},
      },
  };
  constexpr mask_t count = mask_t{1} << diagonal_n;
  for (std::size_t metric = 0; metric < metrics.size(); ++metric) {
    const std::array<int, diagonal_n>& squares = metrics.at(metric);
    for (mask_t a = 0; a < count; ++a) {
      for (mask_t b = 0; b < count; ++b) {
        CAPTURE(metric);
        CAPTURE(a);
        CAPTURE(b);
        int common_squares = 1;
        const index_list la = indices_of(a);
        const index_list lb = indices_of(b);
        for (int p = 0; p < la.size; ++p) {
          if (contains(lb, get(la, p))) {
            common_squares *=
                squares.at(static_cast<std::size_t>(get(la, p) - 1));
          }
        }
        const scaled_blade product = diagonal_product(a, b, squares);
        CHECK(product.coefficient ==
              galoot::core::reorder_sign(a, b) * common_squares);
        CHECK(product.mask == (a ^ b));
      }
    }
  }
}
