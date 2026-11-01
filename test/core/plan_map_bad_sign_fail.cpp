// A blade set written by hand with a sign of 0, under the right complement.
// This file must not compile, and the diagnostic must show the sign check's
// message and no other (core.plan_map_bad_sign).
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace {

namespace core = galoot::core;
using core::detail::unary_map;
using core::detail::UnaryOperation;

struct r3 {
  static constexpr auto value = core::R<3, 0, 0>();
};

constexpr core::blade_set<1> unsigned_blade{{{{.mask = 0b0001, .sign = 0}}}};

constexpr const auto& map =
    unary_map<UnaryOperation::right_complement, r3, unsigned_blade>;
static_assert(map.terms.size() == 1);

} // namespace
