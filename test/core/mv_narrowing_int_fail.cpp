// A line over double given an int variable for one coefficient, which brace
// initialization rejects as narrowing, as it would for an aggregate.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = galoot::core::mv<rga3, line_layout, double>;

[[maybe_unused]] Line from_int(int w) {
  return Line{0.0, 0.0, 1.0, 0.0, 0.0, w};
}

} // namespace
