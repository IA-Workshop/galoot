// A line over unsigned divided by zero in a constant evaluation, which
// checks that an integer divisor of every integer type is not zero, not only
// of std::int64_t. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = galoot::core::mv<rga3, line_layout, unsigned>;

constexpr Line line{1U, 2U, 3U, 4U, 5U, 6U};
[[maybe_unused]] constexpr Line quotient = line / 0U;

} // namespace
