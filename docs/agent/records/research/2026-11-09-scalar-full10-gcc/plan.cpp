// The product plan alone, with no mv: the plan of the scalar times
// full<GENERATORS> of R(GENERATORS,0,0). GENERATORS comes from
// -DGENERATORS=...; default 10.
#include <galoot/core/plan.hpp>

#ifndef GENERATORS
#define GENERATORS 10
#endif

struct rn {
  static constexpr auto value = galoot::core::R<GENERATORS, 0, 0>();
};
inline constexpr galoot::core::blade_set<1> scalar_layout{
    std::array<galoot::core::oriented_blade, 1>{{{.mask = 0, .sign = 1}}}};

constexpr auto plan = galoot::core::detail::product_plan<
    galoot::core::detail::BinaryOperation::geometric_product, rn, scalar_layout,
    galoot::core::full<GENERATORS>>;
static_assert(plan.result.size() == std::size_t{1} << GENERATORS);
