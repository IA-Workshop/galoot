// A line over a scalar with + and - and a constructor from an integer, but
// neither * nor unary -: the multivector reports multiplication, the
// earlier of the two. This file must not compile.
#include <cstdint>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;

struct count {
  std::int64_t value = 0;

  count() = default;
  explicit count(std::int64_t v) : value(v) {}

  [[maybe_unused]] friend count operator+(count a, count b) {
    return count{a.value + b.value};
  }
  [[maybe_unused]] friend count operator-(count a, count b) {
    return count{a.value - b.value};
  }
};

using Line = galoot::core::mv<rga3, line_layout, count>;

[[maybe_unused]] const Line wrong;

} // namespace
