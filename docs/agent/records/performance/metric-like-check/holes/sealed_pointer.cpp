// The sketch of sealed.cpp with one more member, a null pointer. std::bit_cast
// is not a constant expression for a type that holds a pointer, so a forged
// metric can no longer be a constant. The static_asserts show what the extra
// member keeps and costs; TRY_BIT_CAST enables the use that must not compile.
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
  // NOLINTNEXTLINE(*-avoid-c-arrays): see const_class_member.cpp.
  const entry_type gram[N][N];
  // Never read: its type alone keeps std::bit_cast out of constant
  // evaluation.
  const void* const seal = nullptr;
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

template <sealed_metric<2> M>
constexpr entry_type first_square = M.gram[0][0];
static_assert(first_square<euclidean<2>()> == 1);
static_assert(std::is_trivially_copyable_v<sealed_metric<2>>);
static_assert(sizeof(sealed_metric<2>) > sizeof(std::array<entry_type, 4>));

#ifdef TRY_BIT_CAST
struct raw {
  std::array<entry_type, 4> gram;
  const void* seal;
};
constexpr auto forged =
    std::bit_cast<sealed_metric<2>>(raw{{0, 1, 2, 0}, nullptr});
#endif

int main() { return 0; }
