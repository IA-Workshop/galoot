// A plan over a type that is not an algebra tag: int carries no metric. This
// file must not compile, and the diagnostic must name the constraint
// (core.plan_not_metric_like).
#include <galoot/core/blade_set.hpp>
#include <galoot/core/plan.hpp>

namespace {

namespace core = galoot::core;
using core::detail::BinaryOperation;
using core::detail::product_plan;

constexpr const auto& plan =
    product_plan<BinaryOperation::gp, int, core::full<2>, core::full<2>>;
static_assert(plan.terms.size() == 16);

} // namespace
