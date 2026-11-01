// A multivector that is not an aggregate: it keeps public storage but takes
// its values through a constructor that requires one value per component.
// Checks that it stays trivially copyable and structural (usable as a
// template argument), and shows what too few values report. PROBE_CASE 0 is
// the correct use; 1 gives too few values; 2 gives an int variable.
#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>

#ifndef PROBE_CASE
#define PROBE_CASE 0
#endif

template <std::size_t K, class T = double>
struct counted {
  std::array<T, K> c{};

  constexpr counted() = default;

  template <class... A>
    requires(sizeof...(A) == K && (std::convertible_to<A, T> && ...))
  constexpr explicit(K == 1) counted(A... a) : c{static_cast<T>(a)...} {}

  template <class... A>
    requires(sizeof...(A) != K && sizeof...(A) != 0)
  constexpr counted(A...) {
    static_assert(sizeof...(A) == K,
                  "give one value for each component of the type, in the "
                  "order the type lists them");
  }

  friend constexpr bool operator==(const counted&, const counted&) = default;
};

static_assert(std::is_trivially_copyable_v<counted<4>>);
static_assert(!std::is_aggregate_v<counted<4>>);
static_assert(std::is_trivially_default_constructible_v<counted<4>> ==
              false); // c{} gives a default member initializer
template <counted<2> V> constexpr double first = V.c[0];
static_assert(first<counted<2>{3.0, 4.0}> == 3.0);
static_assert(!std::is_convertible_v<double, counted<1>>);

double use(int w) {
#if PROBE_CASE == 0
  constexpr counted<4> p{1, 2, 3, 1};
  const counted<4> q{1.0, 2.0, 3.0, w};
  return p.c[3] + q.c[3];
#elif PROBE_CASE == 1
  const counted<4> p{1, 2, 3};
  return p.c[3] + w;
#elif PROBE_CASE == 2
  const counted<4> p = {1, 2, 3, w};
  return p.c[3];
#endif
}

int main() { return static_cast<int>(use(1)); }
