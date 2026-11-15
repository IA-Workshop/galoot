#ifndef SPIKE_E_BENCH_WRAPPERS_HPP
#define SPIKE_E_BENCH_WRAPPERS_HPP

// The library's transform and the collection pass's result as forms of the
// harness: a struct with a name and the always-inlined setup(q) and
// apply(m, q, x) over coefficient arrays.

#include <array>
#include <cstddef>

#include <galoot/presets/rga3.hpp>

#include "../proto/collect.hpp"

namespace spike_bench {

namespace rga3 = galoot::presets::rga3;

// galoot::core::transform itself; setup does nothing.
template <class Object>
struct library {
  static constexpr char name[] = "galoot";
  using T = typename Object::scalar_type;
  static constexpr std::size_t size = Object::component_count;

  [[gnu::always_inline]] static constexpr std::array<T, 0>
  setup(const std::array<T, 8>& /*q*/) {
    return {};
  }

  [[gnu::always_inline]] static constexpr std::array<T, size>
  apply(const std::array<T, 0>& /*m*/, const std::array<T, 8>& q,
        const std::array<T, size>& x) {
    Object y;
    y.coefficients = x;
    rga3::Motor<T> v;
    v.coefficients = q;
    return galoot::core::transform(y, v).coefficients;
  }
};

// The collected form that the collection pass derives from the library's
// two kernels.
template <class Object>
struct collected {
  static constexpr char name[] = "bp";
  using T = typename Object::scalar_type;
  static constexpr auto layout = Object::layout;
  static constexpr std::size_t size = Object::component_count;

  [[gnu::always_inline]] static constexpr auto
  setup(const std::array<T, 8>& q) {
    return collect::setup<rga3::algebra, rga3::motor_layout, layout>(q);
  }

  template <std::size_t E>
  [[gnu::always_inline]] static constexpr std::array<T, size>
  apply(const std::array<T, E>& m, const std::array<T, 8>& /*q*/,
        const std::array<T, size>& x) {
    return collect::apply<rga3::algebra, rga3::motor_layout, layout>(m, x);
  }
};

} // namespace spike_bench

#endif // SPIKE_E_BENCH_WRAPPERS_HPP
