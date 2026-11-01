// A blade set written by hand with a sign of 2 in the last blade of the
// second set. This file must not compile, and the diagnostic must show the
// sign check's message and no other (core.plan_bad_sign_second).
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace {

namespace core = galoot::core;

struct r3 {
  static constexpr auto value = core::R<3, 0, 0>();
};

constexpr core::blade_set<2> doubled{{{
    {.mask = 0b0001, .sign = -1},
    {.mask = 0b0010, .sign = 2},
}}};

static_assert(core::detail::gp_plan<r3, core::full<3>, doubled>.empty());

} // namespace
