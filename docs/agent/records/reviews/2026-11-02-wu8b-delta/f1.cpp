// F1 probe: products and coefficient checks over long double and over
// scalars of 64 or more digits must compile under the test flags.
#include <cstdint>
#include <limits>
#if defined(PROBE_STDFLOAT)
#include <stdfloat>
#endif

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using galoot::core::detail::BinaryOperation;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;

// A binary float wrapper that claims 200 digits.
struct wide {
  long double v = 0;
  wide() = default;
  explicit constexpr wide(std::int64_t x) : v(static_cast<long double>(x)) {}
  friend constexpr wide operator+(wide a, wide b) { return mk(a.v + b.v); }
  friend constexpr wide operator-(wide a, wide b) { return mk(a.v - b.v); }
  friend constexpr wide operator*(wide a, wide b) { return mk(a.v * b.v); }
  friend constexpr wide operator-(wide a) { return mk(-a.v); }
  friend constexpr bool operator==(wide, wide) = default;
  static constexpr wide mk(long double x) { wide w; w.v = x; return w; }
};
template <> struct std::numeric_limits<wide> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_exact = false;
  static constexpr int radix = 2;
  static constexpr int digits = 200;
  static constexpr int max_exponent = 16384;
};

template <class T> constexpr auto product() {
  using Point = core::mv<rga3, point_layout, T>;
  return core::detail::apply<BinaryOperation::gp>(Point{T(1), T(2), T(3), T(1)},
                                                  Point{T(4), T(5), T(6), T(1)});
}

int main() {
  constexpr auto a = product<long double>();
  static_assert(a.coefficients[0] == 32.0L);
  volatile long double sink = product<long double>().coefficients[0];
  constexpr auto b = product<wide>();
  static_assert(b.coefficients[0].v == 32.0L);
  static_assert(core::detail::holds_exactly<long double>(INT64_MIN));
  static_assert(core::detail::holds_exactly<long double>(INT64_MAX));
  static_assert(core::detail::holds_exactly<wide>(INT64_MAX));
#if defined(PROBE_STDFLOAT)
  constexpr auto c = product<std::float128_t>();
  static_assert(c.coefficients[0] == 32);
  static_assert(core::detail::holds_exactly<std::float128_t>(INT64_MIN));
  constexpr auto d = product<std::float16_t>();
  static_assert(d.coefficients[0] == 32);
  static_assert(core::detail::holds_exactly<std::float16_t>(2048));
  static_assert(!core::detail::holds_exactly<std::float16_t>(2049));
  static_assert(core::detail::holds_exactly<std::float16_t>(65504));
  static_assert(!core::detail::holds_exactly<std::float16_t>(65536));
  static_assert(!core::detail::holds_exactly<std::float16_t>(-65536));
  static_assert(core::detail::holds_exactly<std::bfloat16_t>(INT64_MIN));
  static_assert(!core::detail::holds_exactly<std::bfloat16_t>(257));
#endif
  (void)sink;
  return 0;
}
