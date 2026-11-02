// C1 probe: code that passes a C array where an overload set holds both a
// multivector and a span or array parameter, before and after the braced-
// list catch-all lost explicit.
#include <array>
#include <cstdio>
#include <span>
#include <variant>
#include <vector>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace core = galoot::core;
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
using Point = core::mv<rga3, point_layout, double>;

[[maybe_unused]] static const char* g(const Point&) { return "Point"; }
[[maybe_unused]] static const char* g(std::span<const double>) { return "span"; }
[[maybe_unused]] static const char* h(const Point&) { return "Point"; }
[[maybe_unused]] static const char* h(const std::vector<double>&) { return "vector"; }
template <class X> const char* generic(const X& x) {
  if constexpr (std::is_convertible_v<const X&, Point>) {
    return "converts to Point";
  } else {
    (void)x;
    return "kept as is";
  }
}

int main() {
  double raw[4] = {1, 2, 3, 1};  // NOLINT
#if CASE == 1
  std::puts(g(raw));
#elif CASE == 2
  std::variant<Point, std::span<const double>> v = raw;
  std::puts(v.index() == 0 ? "variant Point" : "variant span");
#elif CASE == 3
  std::puts(generic(raw));
#elif CASE == 4
  std::puts(h({1.0, 2.0, 3.0, 1.0}));
#elif CASE == 5
  const Point p = raw;
  (void)p;
#elif CASE == 6
  std::puts(h(raw));
#endif
  (void)raw;
}
