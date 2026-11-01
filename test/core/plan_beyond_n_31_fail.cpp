// A blade set that names e32, the top bit of the mask, in R(31,0,0), one
// generator below the widest algebra. This file must not compile, and the
// diagnostic must show the set check's message and no other
// (core.plan_beyond_n_31).
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace {

namespace core = galoot::core;

struct r31 {
  static constexpr auto value =
      core::R<core::mask_bits - 1, 0, 0, core::mask_bits>();
};

constexpr core::blade_set<1> top{{{
    {.mask = core::mask_t{1} << (core::mask_bits - 1), .sign = 1},
}}};

static_assert(core::detail::gp_plan<r31, top, top>.empty());

} // namespace
