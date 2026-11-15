// A line over std::string, which has + but no - or *, so it is not a ring
// scalar. This file must not compile.
#include <string>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = galoot::core::mv<rga3, line_layout, std::string>;

[[maybe_unused]] const Line wrong;

} // namespace
