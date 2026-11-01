// A sketch of a metric that only its factories can build: a private
// constructor, so that it is not an aggregate, and a const matrix, so that a
// copy cannot be changed. The matrix is a C array because Clang does not
// count a class with a const member of class type, such as std::array, as
// trivially copyable (const_class_member.cpp). The static_asserts state what it keeps of the
// library's metric; the blocks after them are uses that must not compile,
// each enabled by one macro, and one that does compile and shows what the
// sketch cannot stop.
#include <array>
#include <bit>
#include <cstddef>
#include <type_traits>
#include <utility>

using entry_type = int;

template <std::size_t N>
class sealed_metric;

template <std::size_t N>
consteval sealed_metric<N> euclidean();

template <std::size_t N>
class sealed_metric {
public:
  // Public, as a template argument's members must be; const, so that it
  // cannot be assigned.
  // NOLINTNEXTLINE(*-avoid-c-arrays): see const_class_member.cpp.
  const entry_type gram[N][N];
  static constexpr std::size_t generator_count = N;

private:
  template <std::size_t... I>
  consteval sealed_metric(const std::array<std::array<entry_type, N>, N>& g,
                          std::index_sequence<I...> /*entries*/)
      // One flat list for the two-dimensional array: braces are elided.
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif
      : gram{g[I / N][I % N]...} {
  }
#ifdef __clang__
#pragma clang diagnostic pop
#endif
  friend consteval sealed_metric euclidean<N>();
};

template <std::size_t N>
consteval sealed_metric<N> euclidean() {
  std::array<std::array<entry_type, N>, N> g{};
  for (std::size_t i = 0; i < N; ++i) {
    g[i][i] = 1;
  }
  return sealed_metric<N>(g, std::make_index_sequence<N * N>());
}

// Still a template argument, still trivially copyable; no longer an
// aggregate, and no longer assignable.
template <sealed_metric<2> M>
constexpr entry_type first_square = M.gram[0][0];
static_assert(first_square<euclidean<2>()> == 1);
static_assert(std::is_trivially_copyable_v<sealed_metric<2>>);
static_assert(!std::is_aggregate_v<sealed_metric<2>>);
static_assert(!std::is_copy_assignable_v<sealed_metric<2>>);

// A copy of a factory's metric is the factory's metric.
constexpr sealed_metric<2> copied = euclidean<2>();
static_assert(copied.gram[1][1] == 1);

#ifdef TRY_AGGREGATE
constexpr sealed_metric<2> by_hand{{{{0, 1}, {2, 0}}}};
#endif

#ifdef TRY_CONSTRUCTOR
constexpr sealed_metric<2> by_constructor(
    std::array<std::array<entry_type, 2>, 2>{{{0, 1}, {2, 0}}},
    std::make_index_sequence<4>());
#endif

#ifdef TRY_MUTATION
constexpr sealed_metric<2> changed() {
  sealed_metric<2> m = euclidean<2>();
  m.gram[0][1] = 7;
  return m;
}
#endif

#ifdef TRY_ASSIGNMENT
constexpr sealed_metric<2> reassigned() {
  sealed_metric<2> m = euclidean<2>();
  m = copied;
  return m;
}
#endif

// What a private constructor cannot stop: std::bit_cast builds the object
// from bytes without calling a constructor, in a constant expression.
constexpr auto forged = std::bit_cast<sealed_metric<2>>(
    std::array<entry_type, 4>{0, 1, 2, 0});
static_assert(forged.gram[0][1] != forged.gram[1][0]);

int main() { return 0; }
