// One unit whose constant-evaluation cost core.plan_cost_model pins: the
// geometric antiproduct of {1, e1, e16, e1 e16} with {e1, e2, e16} in
// R(15,0,1). An antiproduct multiplies the complements, blades of 13 to 16
// generators here, so the largest evaluation generates the terms.
// The unit is compiled and not run.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using core::detail::BinaryOperation;
using core::detail::product_plan;

namespace {

struct rigid15 {
  static constexpr auto value = core::R<15, 0, 1>();
};

// Masks: bit i - 1 stands for e_i, so 0x8000 is e16.
constexpr core::blade_set<4> sparse_a{
    {{{0x0U, 1}, {0x1U, 1}, {0x8000U, 1}, {0x8001U, 1}}}};
constexpr core::blade_set<3> sparse_b{{{{0x1U, 1}, {0x2U, 1}, {0x8000U, 1}}}};

constexpr const auto& plan =
    product_plan<BinaryOperation::antigp, rigid15, sparse_a, sparse_b>;

} // namespace

static_assert(plan.terms.size() > 0);
