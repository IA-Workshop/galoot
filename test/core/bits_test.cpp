// Tests of galoot/core/bits.hpp against the definitions it implements.
//
// Expected values come from a reference written here that shares no code with
// the header: a blade is the increasing list of its generator indices, read
// bit by bit, and every sign is the parity of a permutation counted pair by
// pair. Where the inputs are finite the checks are exhaustive, which makes
// them proofs over those ranges rather than samples.
#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <utility>

#include <galoot/core/bits.hpp>

#include <doctest/doctest.h>

namespace {

using galoot::core::mask_t;

// Every generator a mask can name, read from the mask type itself.
constexpr int mask_width = std::numeric_limits<mask_t>::digits;
constexpr auto widest_n = static_cast<std::size_t>(mask_width);

static_assert(galoot::core::mask_bits == widest_n);

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
  for (int i = 1; i <= mask_width; ++i) {
    if (((mask >> static_cast<unsigned>(i - 1)) & 1U) == 1U) {
      push(list, i);
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
  for (int i = 1; i <= static_cast<int>(n); ++i) {
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
  for (int i = 1; i <= static_cast<int>(n); ++i) {
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

constexpr scaled_blade diagonal_product(mask_t a, mask_t b,
                                        const std::array<int, 6>& squares) {
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

// The 16 complements of rigid(3), the algebra R(3,0,1) of four generators,
// written with masks in increasing order rather than Lengyel's oriented names
// (e431 = e4 ∧ e3 ∧ e1 = -e134). Each row was worked by hand from
// u ∧ right(u) = +e1234 and left(u) ∧ u = +e1234: the right sign is the sign
// of sorting A then C, and the left sign that of sorting C then A. For e2,
// C = {1, 3, 4}; (2, 1, 3, 4) has one inversion, so right(e2) = -e134 = e431,
// and (1, 3, 4, 2) has two, so left(e2) = +e134 = -e431.
struct complement_row {
  mask_t blade;
  mask_t complement;
  int right;
  int left;
};

constexpr std::array<complement_row, 16> rigid3_complements{
    {
        {.blade = 0b0000, .complement = 0b1111, .right = 1, .left = 1},   // 1
        {.blade = 0b0001, .complement = 0b1110, .right = 1, .left = -1},  // e1
        {.blade = 0b0010, .complement = 0b1101, .right = -1, .left = 1},  // e2
        {.blade = 0b0100, .complement = 0b1011, .right = 1, .left = -1},  // e3
        {.blade = 0b1000, .complement = 0b0111, .right = -1, .left = 1},  // e4
        {.blade = 0b0011, .complement = 0b1100, .right = 1, .left = 1},   // e12
        {.blade = 0b0101, .complement = 0b1010, .right = -1, .left = -1}, // e13
        {.blade = 0b0110, .complement = 0b1001, .right = 1, .left = 1},   // e23
        {.blade = 0b1001, .complement = 0b0110, .right = 1, .left = 1},   // e14
        {.blade = 0b1010, .complement = 0b0101, .right = -1, .left = -1}, // e24
        {.blade = 0b1100, .complement = 0b0011, .right = 1, .left = 1},   // e34
        {.blade = 0b0111, .complement = 0b1000, .right = 1, .left = -1}, // e123
        {.blade = 0b1011, .complement = 0b0100, .right = -1, .left = 1}, // e124
        {.blade = 0b1101, .complement = 0b0010, .right = 1, .left = -1}, // e134
        {.blade = 0b1110, .complement = 0b0001, .right = -1, .left = 1}, // e234
        {.blade = 0b1111, .complement = 0b0000, .right = 1, .left = 1}, // e1234
    },
};

constexpr bool rigid3_table_holds() {
  using namespace galoot::core;
  return std::ranges::all_of(rigid3_complements, [](complement_row row) {
    return complement_mask(row.blade, 4) == row.complement &&
           right_complement_sign(row.blade, 4) == row.right &&
           left_complement_sign(row.blade, 4) == row.left;
  });
}

static_assert(rigid3_table_holds());

} // namespace

TEST_CASE("Bit i - 1 of a mask stands for e_i, so e124 is the mask 0b1011") {
  // The reference reads masks this way; this case pins the reading itself.
  CHECK(mask_of(indices_of(0b1011U)) == 0b1011U);
  CHECK(indices_of(0b1011U).size == 3);
  CHECK(get(indices_of(0b1011U), 0) == 1);
  CHECK(get(indices_of(0b1011U), 1) == 2);
  CHECK(get(indices_of(0b1011U), 2) == 4);
  CHECK(galoot::core::grade(0b1011U) == 3);
  CHECK(galoot::core::complement_mask(0b1011U, 4) == 0b0100U);
}

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

TEST_CASE("The 16 complements of rigid(3) hold at run time") {
  // The same table is checked in a constant evaluation above.
  for (const complement_row& row : rigid3_complements) {
    CAPTURE(row.blade);
    CHECK(galoot::core::complement_mask(row.blade, 4) == row.complement);
    CHECK(galoot::core::right_complement_sign(row.blade, 4) == row.right);
    CHECK(galoot::core::left_complement_sign(row.blade, 4) == row.left);
  }
}

TEST_CASE("The right complement of the scalar is +e_12...n and that of the "
          "antiscalar is +1, for every n up to the widest mask") {
  using namespace galoot::core;
  for (std::size_t n = 1; n <= widest_n; ++n) {
    CAPTURE(n);
    CHECK(complement_mask(0, n) == antiscalar(n).mask);
    CHECK(right_complement_sign(0, n) == 1);
    CHECK(complement_mask(antiscalar(n).mask, n) == 0);
    CHECK(right_complement_sign(antiscalar(n).mask, n) == 1);
  }
}

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

TEST_CASE("Every function matches its definition on a sample of blades at "
          "n = 12, 16, and the widest mask") {
  using namespace galoot::core;
  for (const std::size_t n : std::array<std::size_t, 3>{12, 16, widest_n}) {
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

TEST_CASE("In a diagonal metric, e_A e_B is reorder_sign times the squares of "
          "the common generators times e_(A xor B), for every pair of blades "
          "of six generators") {
  // Euclidean, and one with every kind of square, so that a zero or negative
  // square of a common generator shows in the coefficient.
  constexpr std::array<std::array<int, 6>, 2> metrics{{
      {1, 1, 1, 1, 1, 1},
      {1, -1, 0, -1, 1, 0},
  }};
  constexpr mask_t count = mask_t{1} << 6U;
  for (const std::array<int, 6>& squares : metrics) {
    for (mask_t a = 0; a < count; ++a) {
      for (mask_t b = 0; b < count; ++b) {
        CAPTURE(squares.at(1));
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
