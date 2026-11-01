// A blade set written by hand with a sign of 0 in the first set. This file
// must not compile, and the diagnostic must show the sign check's message
// and no other (core.plan_bad_sign).
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace {

namespace core = galoot::core;
using core::detail::BinaryOperation;
using core::detail::product_plan;

struct r3 {
  static constexpr auto value = core::R<3, 0, 0>();
};

constexpr core::blade_set<1> unsigned_blade{{{{.mask = 0b0001, .sign = 0}}}};

constexpr const auto& plan =
    product_plan<BinaryOperation::gp, r3, unsigned_blade, core::full<3>>;
static_assert(plan.terms.empty());

} // namespace
