// A line over the test rational given one integer, which the rational's
// explicit constructor takes: a wrong number of values. This file must not
// compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

// The compile-fail tests compile with the library's include paths only, so
// the header is found from this file.
#include "../support/rational.hpp"

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = galoot::core::mv<rga3, line_layout, test_support::rational>;

[[maybe_unused]] constexpr Line wrong{1};

} // namespace
