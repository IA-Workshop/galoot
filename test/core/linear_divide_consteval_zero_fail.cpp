// A line over std::int64_t divided by zero in a constant evaluation, which
// checks that an integer divisor is not zero. This file must not compile.
#include <cstdint>

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
using Line = galoot::core::mv<rga3, line_layout, std::int64_t>;

constexpr Line line{1, 2, 3, 4, 5, 6};
[[maybe_unused]] constexpr Line quotient = line / std::int64_t{0};

} // namespace
