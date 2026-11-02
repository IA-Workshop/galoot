// A line over a scalar with every operation of a ring scalar except
// a * b. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

// The compile-fail tests compile with the library's include paths only, so
// the header is found from this file.
#include "../support/scalar_faults.hpp"

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line =
    galoot::core::mv<rga3, line_layout, test_support::lacks_multiplication>;

[[maybe_unused]] const Line wrong;

} // namespace
