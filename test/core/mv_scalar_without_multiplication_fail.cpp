// A line over a length type that can be added and subtracted but neither
// multiplied nor made from an integer coefficient, so it is not a ring
// scalar. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;

// Metres, which add and subtract; their product would be an area.
struct length {
  double metres = 0;
};

[[maybe_unused]] length operator+(length a, length b) {
  return {.metres = a.metres + b.metres};
}
[[maybe_unused]] length operator-(length a, length b) {
  return {.metres = a.metres - b.metres};
}
[[maybe_unused]] length operator-(length a) { return {.metres = -a.metres}; }

using Line = galoot::core::mv<rga3, line_layout, length>;

[[maybe_unused]] const Line wrong;

} // namespace
