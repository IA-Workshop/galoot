// A one-blade multivector over a scalar whose constructor from an integer is
// explicit, given an int. Expected by the library's design: a message in the
// terms of the algebra. Seen: the compiler's own overload-resolution error.
#include <galoot/core.hpp>
#include "exact.hpp"
struct rga3 { static constexpr auto value = galoot::core::rigid<3>(); };
using Scalar = galoot::core::mv<rga3, galoot::core::blades<rga3, "1">, Exact>;
[[maybe_unused]] constexpr Scalar s{1};
