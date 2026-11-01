// The verdict of metric_like on every Gram matrix of one and two generators
// whose entries come from a seven-value set, against a predicate written here
// from the concept's stated requirement, and on tags whose value is reached
// in ways the library's own tests do not use.
//
// The seven values hold two of each class an entry can fall in (below
// -max_form_entry, within range, above max_form_entry), plus 0. The
// predicates read an entry only through its class and its equality with the
// mirrored entry, so two values per class reach every pattern of classes and
// equalities, and this check covers every int matrix of these sizes.
#include <array>
#include <climits>
#include <cstddef>
#include <utility>

#include <galoot/core/metric.hpp>

namespace {

namespace core = galoot::core;

constexpr core::entry_type big = core::max_form_entry;
constexpr std::array<core::entry_type, 7> values{
    INT_MIN, -big - 1, -big, 0, big, big + 1, INT_MAX};

template <std::size_t N>
constexpr core::metric<N> decode(std::size_t k) {
  core::metric<N> m{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      m.gram[i][j] = values[k % values.size()];
      k /= values.size();
    }
  }
  return m;
}

// The requirement, written out: symmetric, and every entry within range.
template <std::size_t N>
constexpr bool required(const core::metric<N>& m) {
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      const core::entry_type g = m.gram[i][j];
      if (g != m.gram[j][i] || g < -big || g > big) {
        return false;
      }
    }
  }
  return true;
}

template <std::size_t N, std::size_t K>
struct tag {
  static constexpr core::metric<N> value = decode<N>(K);
};

// The matrices are taken in blocks of 343, because Clang limits a fold
// expression to 2048 operands.
constexpr std::size_t block = 7 * 7 * 7;

template <std::size_t N, std::size_t First, std::size_t... K>
consteval int disagreements(std::index_sequence<K...> /*matrices*/) {
  return ((core::metric_like<tag<N, First + K>> ==
                   required(decode<N>(First + K))
               ? 0
               : 1) +
          ... + 0);
}

template <std::size_t N, std::size_t... B>
consteval int disagreements_in_blocks(std::index_sequence<B...> /*blocks*/) {
  return (disagreements<N, B * block>(std::make_index_sequence<block>()) +
          ... + 0);
}

static_assert(disagreements<1, 0>(std::make_index_sequence<7>()) == 0);
static_assert(disagreements_in_blocks<2>(std::make_index_sequence<7>()) == 0);

// How many of the matrices are valid: 3 of 7 at n = 1 (the three values in
// range) and 3 * 3 * 3 = 27 of 2401 at n = 2 (a range value on each diagonal
// entry and one shared by the pair off it), so both outcomes are exercised.
template <std::size_t N, std::size_t First, std::size_t... K>
consteval int accepted(std::index_sequence<K...> /*matrices*/) {
  return ((core::metric_like<tag<N, First + K>> ? 1 : 0) + ... + 0);
}

template <std::size_t N, std::size_t... B>
consteval int accepted_in_blocks(std::index_sequence<B...> /*blocks*/) {
  return (accepted<N, B * block>(std::make_index_sequence<block>()) + ... +
          0);
}

static_assert(accepted<1, 0>(std::make_index_sequence<7>()) == 3);
static_assert(accepted_in_blocks<2>(std::make_index_sequence<7>()) == 27);

// A value reached through a reference is read like any other.
constexpr core::metric<2> plane = core::R<2, 0, 0>();
struct by_reference {
  static constexpr const core::metric<2>& value = plane;
};
static_assert(core::metric_like<by_reference>);

// A reference to an object that is not constexpr cannot be read at compile
// time, even though the reference itself is a constant.
// NOLINTNEXTLINE(*-avoid-non-const-global-variables)
core::metric<2> mutable_plane = core::R<2, 0, 0>();
struct by_reference_to_runtime {
  static constexpr const core::metric<2>& value = mutable_plane;
};
static_assert(!core::metric_like<by_reference_to_runtime>);

// The same at n = 1, where only the diagonal entry is read.
// NOLINTNEXTLINE(*-avoid-non-const-global-variables)
core::metric<1> mutable_line = core::R<1, 0, 0>();
struct by_reference_to_runtime_1 {
  static constexpr const core::metric<1>& value = mutable_line;
};
static_assert(!core::metric_like<by_reference_to_runtime_1>);

// A template parameter object.
template <core::metric<3> M>
struct from_parameter {
  static constexpr const auto& value = M;
};
static_assert(core::metric_like<from_parameter<core::conformal<1>()>>);

// The widest metric a blade mask allows, with one entry off the diagonal
// broken at the far corner, so that the pass must reach the last pair.
constexpr core::metric<core::mask_bits> broken_corner() {
  auto m = core::R<core::mask_bits, 0, 0, core::mask_bits>();
  m.gram[core::mask_bits - 1][0] = 1;
  return m;
}
struct widest_broken {
  static constexpr auto value = broken_corner();
};
static_assert(!core::metric_like<widest_broken>);

} // namespace

int main() { return 0; }
