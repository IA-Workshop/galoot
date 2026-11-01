// The reverse of e4 in an algebra of three generators, taken directly: the
// blade names a generator beyond the algebra. The reverse's sign does not
// depend on n, so only the map term's own range check can reject it. This
// file must not compile, and the diagnostic must show that check
// (core.plan_map_term_beyond_n).
#include <galoot/core/plan.hpp>

namespace {

using galoot::core::detail::unary_term;
using galoot::core::detail::UnaryOperation;

static_assert(unary_term(UnaryOperation::reverse, 0b1000, 3).coefficient == 1);

} // namespace
