#ifndef GALOOT_CORE_BLADE_SET_HPP
#define GALOOT_CORE_BLADE_SET_HPP

// Blade sets: the storage layout of a named type as an ordered list of
// oriented blades. An oriented blade is a mask and a sign of +1 or -1, and a
// coefficient stored on it multiplies sign times the canonical blade of the
// mask. So a type can hold its coefficients on the blades of a published
// formula, such as e31 = -e13 for the moment of a line, with no hidden
// negation.
//
// Generators are e1 to en, counting from 1, and bit i - 1 of a mask stands
// for e_i. The canonical blade of a mask lists its indices in increasing
// order: the mask 0b1011 is e124.
//
// The sets are values, so that a set can be a template argument. Each one is
// built by ordinary constant evaluation and held in a variable template, so
// it is computed once for each argument list a program uses. A misuse that
// the arguments reveal fails to compile with a static_assert whose message
// names it.
//
// A set produced here for a result with no declared type is in canonical
// order: by grade and, within a grade, lexicographically by index list, so
// that the blades of grade 2 in four generators come as e12, e13, e14, e23,
// e24, e34. That is not numeric order of the masks, in which e23 (0b0110)
// precedes e14 (0b1001).

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>

#include <galoot/core/assert.hpp>
#include <galoot/core/bits.hpp>

namespace galoot::core {

/// A basis blade with an orientation: sign times the canonical blade of mask.
///
/// Requires: sign is +1 or -1.
/// Guarantees: a structural aggregate, so it can be part of a template
/// argument; two are equal when mask and sign are.
struct oriented_blade {
  /// The generators of the blade: bit i - 1 stands for e_i.
  mask_t mask;
  /// +1 or -1, the factor by which this blade differs from the canonical
  /// blade of mask.
  int sign;

  /// Equal when mask and sign are equal.
  [[nodiscard]] constexpr bool
  operator==(const oriented_blade&) const = default;
};

/// An ordered list of K oriented blades: the components of a named type, in
/// the order in which it stores its coefficients.
///
/// Requires: every sign is +1 or -1 and no mask appears twice. The
/// constructors below guarantee it; a set written out by hand must keep it,
/// and merge and index_of check it.
/// Guarantees: a structural, trivially copyable aggregate, so that a set can
/// be a class-type template argument, and two sets with equal blades in the
/// same order are the same template argument.
template <std::size_t K>
struct blade_set {
  /// The blades, in the order in which a type stores its coefficients.
  std::array<oriented_blade, K> blades;

  /// The number of blades, K.
  [[nodiscard]] constexpr std::size_t size() const noexcept { return K; }

  /// Equal when the same blades are in the same order.
  [[nodiscard]] constexpr bool operator==(const blade_set&) const = default;
};

/// The largest number of generators whose blades have compact names: one
/// decimal digit per index, so e1 to e9. In a larger algebra a name such as
/// e123 could mean e1 e23 or e12 e3, so blades there are given by masks.
inline constexpr std::size_t max_compact_generators = 9;

static_assert(max_compact_generators <= mask_bits);
// The messages below are literals that name this bound.
static_assert(max_compact_generators == 9, "update the messages that say 9");

/// Whether the blade of `a` comes before the blade of `b` in canonical order:
/// lower grade first, and within a grade, the lexicographically smaller list
/// of increasing indices first, so e14 before e23.
///
/// Requires: nothing.
/// Guarantees: a strict total order on masks.
[[nodiscard]] constexpr bool canonical_less(mask_t a, mask_t b) noexcept {
  if (grade(a) != grade(b)) {
    return grade(a) < grade(b);
  }
  if (a == b) {
    return false;
  }
  // Two increasing index lists of one length agree up to the lowest index
  // that only one of them holds, and the list holding it is the smaller.
  const auto first_difference = static_cast<unsigned>(std::countr_zero(a ^ b));
  return ((a >> first_difference) & 1U) == 1U;
}

/// The canonical name of a blade, as text held by value.
///
/// Guarantees: text() is "1" for the scalar, otherwise "e" followed by the
/// blade's indices in increasing order, as in "e124".
struct blade_name {
  /// The characters of the name, followed by unused places: "e" and at most
  /// nine digits.
  std::array<char, 1 + max_compact_generators> chars;
  /// The number of characters of the name.
  std::size_t length;

  /// The name, as a view of chars.
  [[nodiscard]] constexpr std::string_view text() const noexcept {
    return {chars.data(), length};
  }
};

/// The canonical name of the blade of `a`: the mask 0b1011 is "e124", and the
/// mask 0 is "1".
///
/// Requires: `a` names only e1 to e9, checked by GALOOT_ASSERT, because a
/// larger index has no single-digit name.
[[nodiscard]] constexpr blade_name canonical_name(mask_t a) noexcept {
  const bool compact = (a >> max_compact_generators) == 0;
  GALOOT_ASSERT(compact, "canonical_name: a generator beyond e9");

  blade_name name{};
  if (a == 0) {
    name.chars[0] = '1';
    name.length = 1;
    return name;
  }
  name.chars[0] = 'e';
  name.length = 1;
  for (std::size_t bit = 0; bit < max_compact_generators; ++bit) {
    if (((a >> bit) & 1U) == 1U) {
      name.chars[name.length] = static_cast<char>('1' + bit);
      ++name.length;
    }
  }
  return name;
}

namespace detail {

// Blades of n generators exist only for 1 <= n <= mask_bits. The messages
// name the misuse; the call trace that the compiler prints with them names
// the variable template that was used.
template <std::size_t N>
consteval bool valid_generator_count() {
  static_assert(N >= 1, "blade_set: an algebra has at least one generator, "
                        "so n = 0 is rejected");
  static_assert(N <= mask_bits, "blade_set: n is larger than the number of "
                                "bits in a blade mask");
  return N >= 1 && N <= mask_bits;
}

// The number of ways to choose k of n items. After step i, count is
// C(n, i + 1): C(n, i) (n - i) = C(n, i + 1) (i + 1), so each division is
// exact.
constexpr std::size_t binomial(std::size_t n, std::size_t k) {
  std::size_t count = 1;
  for (std::size_t i = 0; i < k; ++i) {
    count = count * (n - i) / (i + 1);
  }
  return count;
}

// Writes the blades of grade k in n generators, with sign +1, into `out` from
// position `at`, in lexicographic order of their index lists, and moves `at`
// past them. The index list starts at 1, 2, ..., k; each step raises the last
// index that can still rise and restarts the ones after it just above it,
// which is the next list in lexicographic order.
template <std::size_t K>
constexpr void append_grade(std::array<oriented_blade, K>& out, std::size_t& at,
                            std::size_t n, std::size_t k) {
  // Bit positions, counting from 0, of the generators of the current blade.
  std::array<std::size_t, mask_bits> bit{};
  for (std::size_t i = 0; i < k; ++i) {
    bit[i] = i;
  }

  while (true) {
    mask_t mask = 0;
    for (std::size_t i = 0; i < k; ++i) {
      mask |= mask_t{1} << bit[i];
    }
    out[at] = {.mask = mask, .sign = 1};
    ++at;

    // The entry at position p is at its largest when it is n - k + p, which
    // leaves room for the k - 1 - p entries after it. Find the last entry
    // below its largest; when there is none, this was the last list.
    std::size_t i = k;
    while (i > 0 && bit[i - 1] == n - k + (i - 1)) {
      --i;
    }
    if (i == 0) {
      return;
    }
    ++bit[i - 1];
    for (std::size_t j = i; j < k; ++j) {
      bit[j] = bit[j - 1] + 1;
    }
  }
}

template <std::size_t N>
consteval auto make_full() {
  if constexpr (valid_generator_count<N>()) {
    blade_set<std::size_t{1} << N> set{};
    std::size_t at = 0;
    for (std::size_t k = 0; k <= N; ++k) {
      append_grade(set.blades, at, N, k);
    }
    return set;
  } else {
    return blade_set<0>{};
  }
}

template <std::size_t N, int K>
consteval auto make_of_grade() {
  static_assert(K >= 0 && std::cmp_less_equal(K, N),
                "of_grade: the grade must be from 0 to n");
  if constexpr (valid_generator_count<N>() && K >= 0 &&
                std::cmp_less_equal(K, N)) {
    constexpr auto k = static_cast<std::size_t>(K);
    blade_set<binomial(N, k)> set{};
    std::size_t at = 0;
    append_grade(set.blades, at, N, k);
    return set;
  } else {
    return blade_set<0>{};
  }
}

constexpr bool is_sign(int sign) { return sign == 1 || sign == -1; }

// The union of two lists of blades in canonical order, in the first `size`
// places of `blades`, with which input each blade came from, and what was
// wrong with the inputs: a sign other than +1 or -1, a mask listed twice in
// one input, or a mask in both inputs with opposite signs.
template <std::size_t Capacity>
struct union_of {
  std::array<oriented_blade, Capacity> blades;
  std::array<bool, Capacity> from_second;
  std::size_t size;
  bool bad_sign;
  bool repeated_mask;
  bool opposite_signs;
};

// Inserts each blade of `a` and then of `b` into a list kept in canonical
// order, skipping a mask already in the list. A mask found again in the same
// input is a repeated mask; found again from the other input, it is the same
// blade, unless the signs differ.
template <std::size_t KA, std::size_t KB>
constexpr union_of<KA + KB> unite(const blade_set<KA>& a,
                                  const blade_set<KB>& b) {
  union_of<KA + KB> u{};
  const auto insert = [&u](oriented_blade blade, bool from_second) {
    u.bad_sign = u.bad_sign || !is_sign(blade.sign);
    std::size_t at = 0;
    while (at < u.size && canonical_less(u.blades[at].mask, blade.mask)) {
      ++at;
    }
    if (at < u.size && u.blades[at].mask == blade.mask) {
      if (u.from_second[at] == from_second) {
        u.repeated_mask = true;
      } else {
        u.opposite_signs = u.opposite_signs || u.blades[at].sign != blade.sign;
      }
      return;
    }
    for (std::size_t j = u.size; j > at; --j) {
      u.blades[j] = u.blades[j - 1];
      u.from_second[j] = u.from_second[j - 1];
    }
    u.blades[at] = blade;
    u.from_second[at] = from_second;
    ++u.size;
  };
  for (const oriented_blade& blade : a.blades) {
    insert(blade, false);
  }
  for (const oriented_blade& blade : b.blades) {
    insert(blade, true);
  }
  return u;
}

template <blade_set A, blade_set B>
consteval auto make_merge() {
  constexpr auto u = unite(A, B);
  static_assert(!u.bad_sign, "merge: a set holds a sign other than +1 or -1");
  static_assert(!u.repeated_mask, "merge: a set holds one mask twice");
  // Only when each input is valid is a mask found twice in both inputs.
  if constexpr (!u.bad_sign && !u.repeated_mask) {
    static_assert(!u.opposite_signs, "merge: the two sets hold one blade with "
                                     "opposite signs");
  }
  blade_set<u.size> set{};
  for (std::size_t i = 0; i < u.size; ++i) {
    set.blades[i] = u.blades[i];
  }
  return set;
}

// The position of `mask` in `set`, or the size of the set when it is absent.
template <std::size_t K>
constexpr std::size_t position_of(const blade_set<K>& set, mask_t mask) {
  std::size_t at = 0;
  while (at < K && set.blades[at].mask != mask) {
    ++at;
  }
  return at;
}

struct lookup {
  bool bad_sign;
  std::size_t count;
};

// Whether a sign of `set` is other than +1 or -1, and how many of its blades
// have the mask `mask`. A check that no mask at all repeats would cost O(K^2)
// steps, too many for a large set within the compilers' limits on constant
// evaluation; a repeat of the mask looked up is what makes the answer wrong.
template <std::size_t K>
constexpr lookup look_up(const blade_set<K>& set, mask_t mask) {
  lookup found{};
  for (const oriented_blade& blade : set.blades) {
    found.bad_sign = found.bad_sign || !is_sign(blade.sign);
    found.count += blade.mask == mask ? 1U : 0U;
  }
  return found;
}

template <blade_set S, mask_t Mask>
consteval std::size_t make_index_of() {
  constexpr auto found = look_up(S, Mask);
  static_assert(!found.bad_sign,
                "index_of: the set holds a sign other than +1 or -1");
  static_assert(found.count <= 1, "index_of: the set holds this mask twice");
  constexpr std::size_t at = position_of(S, Mask);
  static_assert(at < S.size(), "index_of: the set holds no blade with this "
                               "mask");
  return at;
}

// A list of blade names given as a template argument: the characters of a
// string literal without its terminating null.
template <std::size_t L>
struct name_list {
  std::array<char, L> chars{};

  // Not explicit, so that a string literal can be written directly as the
  // template argument, as in blades<4, "e41 e42 e43">.
  // A C array, so that L is deduced from the literal.
  // NOLINTNEXTLINE(*-avoid-c-arrays,*-explicit-constructor)
  consteval name_list(const char (&text)[L]) noexcept {
    for (std::size_t i = 0; i < L; ++i) {
      chars[i] = text[i];
    }
  }

  [[nodiscard]] constexpr std::string_view text() const noexcept {
    return {chars.data(), L - 1};
  }
};

// Why a list of names was rejected, for the first name that is wrong.
enum class NameError : std::uint8_t {
  none,
  no_names,
  not_a_name,
  bad_index,
  index_above_n,
  repeated_generator,
  repeated_blade,
};

struct parsed_name {
  oriented_blade blade;
  NameError error;
};

// The blade an oriented name stands for. "1" is the scalar. Otherwise the
// name is e followed by indices, and it stands for the product of those
// generators in the order written: the canonical blade of their mask times
// the sign of the permutation that sorts them. `name` is not empty, because
// next_name returns an empty view only after the last name.
constexpr parsed_name parse_name(std::string_view name, std::size_t n) {
  if (name == "1") {
    return {.blade = {.mask = 0, .sign = 1}, .error = NameError::none};
  }
  if (name.front() != 'e') {
    return {.blade = {}, .error = NameError::not_a_name};
  }
  if (name.size() == 1) {
    return {.blade = {}, .error = NameError::bad_index};
  }

  oriented_blade blade{.mask = 0, .sign = 1};
  for (const char digit : name.substr(1)) {
    if (digit < '1' || digit > '9') {
      return {.blade = {}, .error = NameError::bad_index};
    }
    const auto index = static_cast<std::size_t>(digit - '0');
    if (index > n) {
      return {.blade = {}, .error = NameError::index_above_n};
    }
    const mask_t generator = mask_t{1} << (index - 1);
    if ((blade.mask & generator) != 0) {
      return {.blade = {}, .error = NameError::repeated_generator};
    }
    // e_A e_i = reorder_sign(A, {i}) e_(A with i) when e_i is not in A: the
    // sign counts the generators of A above e_i, each one transposition.
    blade.sign *= reorder_sign(blade.mask, generator);
    blade.mask |= generator;
  }
  return {.blade = blade, .error = NameError::none};
}

// The name that starts at or after `at` in `text`, which separates names by
// runs of spaces, with `at` moved past it; empty when no name is left.
constexpr std::string_view next_name(std::string_view text, std::size_t& at) {
  while (at < text.size() && text[at] == ' ') {
    ++at;
  }
  const std::size_t start = at;
  while (at < text.size() && text[at] != ' ') {
    ++at;
  }
  return text.substr(start, at - start);
}

constexpr std::size_t name_count(std::string_view text) {
  std::size_t count = 0;
  for (std::size_t at = 0; !next_name(text, at).empty();) {
    ++count;
  }
  return count;
}

// The error of the first wrong name in `text`, or NameError::none. A blade
// named twice is found by marking each mask as it is read; n is at most
// max_compact_generators, so every mask fits the table.
constexpr NameError first_error(std::string_view text, std::size_t n) {
  std::array<bool, std::size_t{1} << max_compact_generators> named{};
  std::size_t at = 0;
  std::size_t count = 0;
  for (std::string_view name = next_name(text, at); !name.empty();
       name = next_name(text, at)) {
    const parsed_name parsed = parse_name(name, n);
    if (parsed.error != NameError::none) {
      return parsed.error;
    }
    if (named[parsed.blade.mask]) {
      return NameError::repeated_blade;
    }
    named[parsed.blade.mask] = true;
    ++count;
  }
  return count == 0 ? NameError::no_names : NameError::none;
}

template <std::size_t N, name_list Names>
consteval auto make_blades() {
  static_assert(N <= max_compact_generators,
                "blades: names such as e123 have one digit per index, so they "
                "are read only for n up to 9, and a larger algebra takes "
                "masks");
  if constexpr (valid_generator_count<N>() && N <= max_compact_generators) {
    constexpr NameError error = first_error(Names.text(), N);
    static_assert(error != NameError::no_names,
                  "blades: the list names no blade");
    static_assert(error != NameError::not_a_name,
                  "blades: a name must be 1 or start with e, as in e12");
    static_assert(error != NameError::bad_index,
                  "blades: after the e, a name lists generator indices, each "
                  "a digit from 1 to 9");
    static_assert(error != NameError::index_above_n,
                  "blades: a name has an index above n, the number of "
                  "generators");
    static_assert(error != NameError::repeated_generator,
                  "blades: a name lists one generator twice");
    static_assert(error != NameError::repeated_blade,
                  "blades: two names stand for the same blade");

    if constexpr (error == NameError::none) {
      blade_set<name_count(Names.text())> set{};
      std::size_t at = 0;
      for (oriented_blade& blade : set.blades) {
        blade = parse_name(next_name(Names.text(), at), N).blade;
      }
      return set;
    } else {
      return blade_set<0>{};
    }
  } else {
    return blade_set<0>{};
  }
}

} // namespace detail

/// Every blade of an algebra of N generators, each with sign +1, in canonical
/// order: by grade, then lexicographically by index list.
///
/// Requires: 1 <= N <= mask_bits, checked at compile time. The set holds 2^N
/// blades, so the compiler's limits on constant evaluation bound N in
/// practice well below mask_bits: at their default limits, Clang 23 builds
/// full<13> but not full<14>, and GCC 16 builds full<15> but not full<16>.
/// Guarantees: full<N>.size() is 2^N.
template <std::size_t N>
inline constexpr auto full = detail::make_full<N>();

/// The blades of grade K in an algebra of N generators, each with sign +1, in
/// lexicographic order of their index lists.
///
/// Requires: 1 <= N <= mask_bits and 0 <= K <= N, checked at compile time.
/// The limits on constant evaluation bound the size as for full: at its
/// default limits, Clang 23 builds of_grade<32, 3>, with 4,960 blades, but
/// not of_grade<16, 8> or of_grade<32, 4>, with 12,870 and 35,960.
/// Guarantees: of_grade<N, K>.size() is the binomial coefficient C(N, K).
template <std::size_t N, int K>
inline constexpr auto of_grade = detail::make_of_grade<N, K>();

/// The blades of A and of B, each once, in canonical order. A blade keeps the
/// sign it has in the set that holds it, so merging a set with itself
/// reorders it but changes no sign.
///
/// Requires: every sign is +1 or -1, no mask appears twice in one set, and no
/// mask is in both sets with opposite signs, because the merged set could
/// keep only one of the two orientations. Each is checked at compile time.
/// Guarantees: the size is the number of distinct masks in A and B.
template <blade_set A, blade_set B>
inline constexpr auto merge = detail::make_merge<A, B>();

/// The position in S of the blade whose mask is Mask, counting from 0.
///
/// Requires: every sign of S is +1 or -1, and S holds exactly one blade with
/// mask Mask, checked at compile time. A repeat of another mask is not
/// checked here.
/// Guarantees: S.blades[index_of<S, Mask>].mask == Mask.
template <blade_set S, mask_t Mask>
inline constexpr std::size_t index_of = detail::make_index_of<S, Mask>();

/// The oriented blades named in Names, in the order written. Names are
/// separated by spaces. "1" is the scalar; any other name is e followed by
/// generator indices, one digit each, and stands for the product of those
/// generators in the order written: the canonical blade of their mask times
/// the sign of the permutation that sorts them. So in an algebra of four
/// generators, blades<4, "e41 e23 e31"> holds -e14, +e23, and -e13.
///
/// Requires: 1 <= N <= 9, because each index is one digit; at least one
/// name; each name 1 or e followed by digits from 1 to N, none repeated; and
/// no blade named twice, in either order. Each is checked at compile time,
/// with a message for the first name that breaks it.
/// Guarantees: blades<N, Names>.size() is the number of names.
template <std::size_t N, detail::name_list Names>
inline constexpr auto blades = detail::make_blades<N, Names>();

} // namespace galoot::core

#endif // GALOOT_CORE_BLADE_SET_HPP
