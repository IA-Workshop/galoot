// One unit whose constant-evaluation cost core.plan_cost_model pins: the
// reverse of every blade on seven generators. Its largest evaluation
// collects the result blades. A map has one term for each blade.
// The unit is compiled and not run.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using core::detail::unary_map;
using core::detail::UnaryOperation;

namespace {

struct r7 {
  static constexpr auto value = core::R<7, 0, 0>();
};

constexpr const auto& plan =
    unary_map<UnaryOperation::reverse, r7, core::full<7>>;

} // namespace

static_assert(plan.terms.size() == 128);
