// The wedge of e1 and e4 in R(3,0,0), generated directly: the second blade
// names a generator beyond the algebra. This file must not compile, and the
// diagnostic must show the wedge's own range check
// (core.plan_wedge_pair_beyond_n_second).
#include <array>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace {

namespace core = galoot::core;
using core::detail::BinaryOperation;

constexpr std::array<core::oriented_blade, 1> first{
    {{.mask = 0b0001, .sign = 1}}};
constexpr std::array<core::oriented_blade, 1> second{
    {{.mask = 0b1000, .sign = 1}}};

static_assert(core::detail::generate_plan<BinaryOperation::wedge, 1>(
                  core::R<3, 0, 0>(), first, second, {}) == 1);

} // namespace
