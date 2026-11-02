// One unit whose constant-evaluation cost core.plan_cost_model pins: the
// geometric product of the trivectors with the 7-vectors of R(8,0,0), 448
// terms on 98 result blades. Its largest evaluation places each term on its
// result blade. Each pair is one term, so there are 56 x 8 of them.
// The unit is compiled and not run.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using core::detail::BinaryOperation;
using core::detail::product_plan;

namespace {

struct r8 {
  static constexpr auto value = core::R<8, 0, 0>();
};

constexpr const auto& plan =
    product_plan<BinaryOperation::geometric_product, r8, core::of_grade<8, 3>,
                 core::of_grade<8, 7>>;

} // namespace

static_assert(plan.terms.size() == 448);
