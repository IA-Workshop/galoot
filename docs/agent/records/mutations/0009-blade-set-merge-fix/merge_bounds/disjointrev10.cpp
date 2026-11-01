#include <bit>
#include <cstddef>

#include <galoot/core/blade_set.hpp>

namespace core = galoot::core;
constexpr std::size_t n = 10;
constexpr std::size_t half = (std::size_t{1} << n) / 2;

// The blades of full<n> of even (Parity 0) or odd grade, in canonical order.
template <int Parity>
consteval core::blade_set<half> grade_parity() {
  core::blade_set<half> set{};
  std::size_t at = 0;
  for (const auto& blade : core::full<n>.blades) {
    if (std::popcount(blade.mask) % 2 == Parity) {
      set.blades[at++] = blade;
    }
  }
  return set;
}

consteval auto reversed_full() {
  auto set = core::full<n>;
  for (std::size_t i = 0; i < set.size() / 2; ++i) {
    const auto t = set.blades[i];
    set.blades[i] = set.blades[set.size() - 1 - i];
    set.blades[set.size() - 1 - i] = t;
  }
  return set;
}

constexpr auto even = grade_parity<0>();
constexpr auto odd = grade_parity<1>();
constexpr auto reversed = reversed_full();
static_assert(core::merge<odd, even>.size() == 2 * half);
