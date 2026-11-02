// One unit whose constant-evaluation cost core.plan_cost_model pins: the
// geometric product of the vectors with every blade of R(6,0,0). Its largest
// evaluation collects the result blades, and since the first factor is not
// the scalar, blades of lower grade come after higher ones and are inserted
// before them. In R(6,0,0) the product of two blades is one blade, so there
// is one term for each of the 6 x 64 pairs.
// The unit is compiled and not run.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using core::detail::BinaryOperation;
using core::detail::product_plan;

namespace {

struct r6 {
  static constexpr auto value = core::R<6, 0, 0>();
};

constexpr const auto& plan =
    product_plan<BinaryOperation::gp, r6, core::of_grade<6, 1>, core::full<6>>;

} // namespace

static_assert(plan.terms.size() == 384);
