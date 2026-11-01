// The reverse of the scalar in an algebra of no generators, taken directly.
// This file must not compile, and the diagnostic must show the check of the
// number of generators (core.plan_map_term_zero_n).
#include <galoot/core/plan.hpp>

namespace {

using galoot::core::detail::unary_term;
using galoot::core::detail::UnaryOperation;

static_assert(unary_term(UnaryOperation::reverse, 0, 0).coefficient == 1);

} // namespace
