// A motor over the test rational with a nonzero scalar truncated to a line in a
// constant evaluation, which checks that what it drops is zero. This file
// must not compile.
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
inline constexpr auto motor_layout =
    galoot::core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using T = test_support::rational;
using Motor = galoot::core::mv<rga3, motor_layout, T>;

constexpr Motor motor{T{7}, T{1}, T{2}, T{3}, T{4}, T{5}, T{6}, T{0}};
[[maybe_unused]] constexpr auto kept =
    galoot::core::truncate<line_layout>(motor);

} // namespace
