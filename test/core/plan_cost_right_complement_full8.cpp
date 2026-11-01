// One unit whose constant-evaluation cost core.plan_cost_model pins: the
// right complement of every blade on eight generators. Its largest
// evaluation places each term on its result blade. A map has one term for
// each blade.
// The unit is compiled and not run.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using core::detail::unary_map;
using core::detail::UnaryOperation;

namespace {

struct r8 {
  static constexpr auto value = core::R<8, 0, 0>();
};

constexpr const auto& plan =
    unary_map<UnaryOperation::right_complement, r8, core::full<8>>;

} // namespace

static_assert(plan.terms.size() == 256);
