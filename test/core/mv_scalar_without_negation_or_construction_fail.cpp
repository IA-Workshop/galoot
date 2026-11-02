// A line over a scalar with +, -, and *, but neither unary - nor a
// constructor from an integer: the multivector reports unary minus, the
// earlier of the two. This file must not compile.
#include <cstdint>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;

// A declared default constructor, so that it is no aggregate that a
// parenthesized integer could initialize.
struct tally {
  std::int64_t value = 0;

  tally() = default;

  [[maybe_unused]] friend tally operator+(tally a, tally /*b*/) { return a; }
  [[maybe_unused]] friend tally operator-(tally a, tally /*b*/) { return a; }
  [[maybe_unused]] friend tally operator*(tally a, tally /*b*/) { return a; }
};

using Line = galoot::core::mv<rga3, line_layout, tally>;

[[maybe_unused]] const Line wrong;

} // namespace
