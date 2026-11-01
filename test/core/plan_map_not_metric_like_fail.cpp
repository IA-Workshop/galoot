// A map over a type that is not an algebra tag: int carries no number of
// generators. This file must not compile, and the diagnostic must name the
// constraint (core.plan_map_not_metric_like).
#include <galoot/core/blade_set.hpp>
#include <galoot/core/plan.hpp>

namespace {

namespace core = galoot::core;
using core::detail::unary_map;
using core::detail::UnaryOperation;

constexpr const auto& map =
    unary_map<UnaryOperation::reverse, int, core::full<2>>;
static_assert(map.terms.size() == 4);

} // namespace
