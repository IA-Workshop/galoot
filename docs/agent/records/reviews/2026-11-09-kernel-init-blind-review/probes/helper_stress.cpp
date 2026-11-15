// The helper's form under stress: several kernels share one instantiation
// of zero_places; constant evaluations write first at places 2, 5, and
// R - 1 of results made from it, and also where every place is written;
// then each kernel runs at run time, writing one place, and every place it
// does not write must be the value-initialized element (d == 1).
#include <array>
#include <cstddef>
#include <cstdio>
#include <utility>

struct q { long n = 0; long d = 1; };

template <class T, std::size_t... P>
constexpr std::array<T, sizeof...(P)> zero_places(std::index_sequence<P...>) {
  return {(static_cast<void>(P), T{})...};
}

template <std::size_t R, std::size_t W>
constexpr std::array<q, R> kernel(long v) {
  std::array<q, R> out = zero_places<q>(std::make_index_sequence<R>{});
  out[W].n = v;
  return out;
}
template <std::size_t R>
constexpr std::array<q, R> all(long v) {
  std::array<q, R> out = zero_places<q>(std::make_index_sequence<R>{});
  for (std::size_t i = 0; i < R; ++i) { out[i] = q{v, 0}; }
  return out;
}

constexpr std::size_t R = 16;
[[maybe_unused]] constexpr auto c2 = kernel<R, 2>(1);
[[maybe_unused]] constexpr auto c5 = kernel<R, 5>(1);
[[maybe_unused]] constexpr auto c15 = kernel<R, R - 1>(1);
[[maybe_unused]] constexpr auto call = all<R>(1);
static_assert(kernel<R, 9>(3)[9].n == 3);

template <std::size_t W>
int check(long v) {
  const auto a = kernel<R, W>(v);
  int bad = 0;
  for (std::size_t i = 0; i < R; ++i) {
    if (i != W && a[i].d != 1) { ++bad; }
  }
  return bad;
}

int main() {
  volatile long v = 1;
  const int bad = check<2>(v) + check<5>(v) + check<R - 1>(v) + check<9>(v) + check<0>(v);
  const auto z = zero_places<q>(std::make_index_sequence<R>{});
  int badz = 0;
  for (const auto& e : z) { badz += e.d != 1; }
  std::printf("bad places %d, bad helper places %d\n", bad, badz);
  return bad + badz == 0 ? 0 : 1;
}
