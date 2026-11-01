// A blade set that names a generator beyond the algebra: e4, the last blade
// of the first set, in R(3,0,0). This file must not compile, and the
// diagnostic must show the set check's message and no other
// (core.plan_beyond_n).
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace {

namespace core = galoot::core;

struct r3 {
  static constexpr auto value = core::R<3, 0, 0>();
};

constexpr core::blade_set<2> beyond{{{
    {.mask = 0b0001, .sign = 1},
    {.mask = 0b1000, .sign = 1},
}}};

static_assert(core::detail::gp_plan<r3, beyond, core::full<3>>.empty());

} // namespace
