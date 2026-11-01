// One unit whose constant-evaluation cost core.plan_cost_model pins: the
// antiwedge of every blade with every blade on five generators. The
// antiwedge is the wedge of the complements, so the largest evaluation
// generates the terms through the wedge's rule. The wedge of two
// complements is one term when they share no generator, which is 3^5 = 243
// of the 32 x 32 pairs: each generator lies in the first complement, the
// second, or neither.
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
    product_plan<BinaryOperation::antiwedge, r5, core::full<5>, core::full<5>>;

} // namespace

static_assert(plan.terms.size() == 243);
