#include <array>
#include <cstddef>
struct q {
  long n = 0;
  long d = 1;
  constexpr q() = default;
  constexpr explicit q(long v) : n(v) {}
};
#ifndef FORM
#define FORM 0
#endif
template <std::size_t N>
constexpr std::array<q, N> make(std::size_t i) {
#if FORM == 0
  std::array<q, N> a{};
#else
  std::array<q, N> a = std::array<q, N>();
#endif
  a[i].n = 1;
  return a;
}
[[maybe_unused]] constexpr auto once = make<5>(3);
int main() {
  volatile std::size_t i = 0;
  auto r = make<5>(i);
  for (auto& e : r) if (e.d != 1) return 1;
  return 0;
}
