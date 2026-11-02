// A line over a fixed-point type that has no default constructor, so it is
// not semiregular: mv's template head rejects it, with the compiler's account
// of the constraint. This file must not compile.
#include <cstdint>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
// mv is used below; its failed constraint hides that use from the check.
// NOLINTNEXTLINE(misc-include-cleaner)
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;

// Sixteenths, which must be given a value when made.
struct fixed {
  explicit fixed(std::int64_t value) : sixteenths(value * 16) {}
  std::int64_t sixteenths;
};

[[maybe_unused]] constexpr galoot::core::mv<rga3, line_layout, fixed> wrong{};

} // namespace
