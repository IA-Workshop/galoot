// The weight of a line compared with the line: the result is on the canonical
// blades the weight reaches, a layout other than the line's, so there is no
// operator== for the two. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = core::mv<rga3, line_layout, double>;

[[maybe_unused]] bool use(const Line& l) { return core::weight(l) == l; }

} // namespace
