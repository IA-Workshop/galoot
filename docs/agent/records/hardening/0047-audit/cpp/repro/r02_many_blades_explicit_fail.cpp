// The same scalar, a point given four ints: the count is right, and each value
// converts to the scalar only explicitly.
#include <galoot/core.hpp>
#include "exact.hpp"
struct rga3 { static constexpr auto value = galoot::core::rigid<3>(); };
using Point = galoot::core::mv<rga3, galoot::core::blades<rga3, "e1 e2 e3 e4">, Exact>;
[[maybe_unused]] constexpr Point p{1, 2, 3, 4};
