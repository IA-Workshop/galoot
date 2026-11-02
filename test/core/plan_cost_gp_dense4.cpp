// One unit whose constant-evaluation cost core.plan_cost_model pins: the
// geometric product of every blade with every blade on four generators, in
// the form with 2 on the diagonal and 1 elsewhere. With every entry nonzero,
// the product of two blades has many terms, so the largest evaluation
// generates them.
// The unit is compiled and not run.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using core::detail::BinaryOperation;
using core::detail::product_plan;

namespace {

struct dense4 {
  static constexpr auto value =
      core::from_gram({{2, 1, 1, 1}, {1, 2, 1, 1}, {1, 1, 2, 1}, {1, 1, 1, 2}});
};

constexpr const auto& plan = product_plan<BinaryOperation::geometric_product,
                                          dense4, core::full<4>, core::full<4>>;

} // namespace

static_assert(!plan.terms.empty());
