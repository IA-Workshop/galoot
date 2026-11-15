#include <cstddef>
#include <utility>

#include <galoot/core/blade_set.hpp>

namespace core = galoot::core;

template <std::size_t... I>
consteval std::size_t positions(std::index_sequence<I...>) {
  return (core::index_of<core::full<10>, core::full<10>.blades[I].mask> + ...);
}

// Every blade is found at its own position: the sum of 0 to 2^n - 1.
static_assert(positions(std::make_index_sequence<std::size_t{1} << 10>()) ==
              (std::size_t{1} << 10) * ((std::size_t{1} << 10) - 1) / 2);
