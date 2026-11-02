// One unit whose constant-evaluation cost core.plan_cost_model pins: the
// geometric product of every blade with every blade of R(5,0,0). Its largest
// evaluation collects the result blades on Clang and generates the terms on
// GCC. In R(5,0,0) the product of two blades is one blade, so there is one
// term for each of the 32 x 32 pairs.
// The unit is compiled and not run.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using core::detail::BinaryOperation;
using core::detail::product_plan;

namespace {

struct r5 {
  static constexpr auto value = core::R<5, 0, 0>();
};

constexpr const auto& plan =
    product_plan<BinaryOperation::gp, r5, core::full<5>, core::full<5>>;

} // namespace

static_assert(plan.terms.size() == 1024);
