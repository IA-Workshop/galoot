// Two names for one blade, e12 and e21 = -e12: a set holds each blade once.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r400 {
  static constexpr auto value = galoot::core::R<4, 0, 0>();
};

[[maybe_unused]] constexpr auto twice = galoot::core::blades<r400, "e12 e21">;

} // namespace
