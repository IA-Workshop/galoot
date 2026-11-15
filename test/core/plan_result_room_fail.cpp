// The result blades of two terms on different blades, collected into one
// place. This file must not compile, and the diagnostic must show the room
// check's message (core.plan_result_room).
#include <array>
#include <cstddef>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/plan.hpp>

namespace {

namespace core = galoot::core;
using core::detail::plan_term;

constexpr std::size_t collected() {
  const std::array<plan_term, 2> plan{{
      {.a = 0, .b = 0, .out = 0b01, .coefficient = 1},
      {.a = 0, .b = 1, .out = 0b10, .coefficient = 1},
  }};
  std::array<core::oriented_blade, 1> result{};
  return core::detail::collect_result(plan, result);
}

static_assert(collected() == 2);

} // namespace
