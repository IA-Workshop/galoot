// A line over float given a double variable for one coefficient, which
// brace initialization rejects as narrowing.
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
using Line = galoot::core::mv<rga3, line_layout, float>;

[[maybe_unused]] Line from_double(double w) {
  // The narrowing is the mistake this file shows.
  // NOLINTNEXTLINE(*-narrowing-conversions)
  return Line{0.0F, 0.0F, 1.0F, 0.0F, 0.0F, w};
}

} // namespace
