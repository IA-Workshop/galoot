// A line over a colour code, which copies and compares but has no
// arithmetic at all: it lacks every operation of a ring scalar, so the
// multivector reports the first, addition. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;

struct colour {
  int code = 0;
  [[maybe_unused]] friend bool operator==(colour, colour) = default;
};

using Line = galoot::core::mv<rga3, line_layout, colour>;

[[maybe_unused]] const Line wrong;

} // namespace
