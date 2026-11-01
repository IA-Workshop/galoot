// One unit whose constant-evaluation cost core.plan_cost_model pins: the
// geometric antiproduct of {1, e1, e16, e1 e16} with {e1, e2, e16} in
// R(15,0,1). An antiproduct multiplies the complements, blades of 13 to 16
// generators here, so the largest evaluation generates the terms.
// The unit is compiled and not run.
#include <galoot/core/bits.hpp>
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

// Bit i - 1 of a mask stands for e_i.
constexpr core::mask_t e1 = 0x1;
constexpr core::mask_t e2 = 0x2;
constexpr core::mask_t e16 = 0x8000;

constexpr core::blade_set<4> sparse_a{
    {
        {
            {.mask = 0, .sign = 1},
            {.mask = e1, .sign = 1},
            {.mask = e16, .sign = 1},
            {.mask = e1 | e16, .sign = 1},
        },
    },
};
constexpr core::blade_set<3> sparse_b{
    {
        {
            {.mask = e1, .sign = 1},
            {.mask = e2, .sign = 1},
            {.mask = e16, .sign = 1},
        },
    },
};

constexpr const auto& plan =
    product_plan<BinaryOperation::antigp, rigid15, sparse_a, sparse_b>;

} // namespace

static_assert(!plan.terms.empty());
