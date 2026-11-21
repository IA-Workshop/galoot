// get on a class derived from mv. Products, sums, and transforms accept such
// a class as its mv base; get does not, and the error is the compiler's.
#include <galoot/core.hpp>
struct rga3 { static constexpr auto value = galoot::core::rigid<3>(); };
using Point = galoot::core::mv<rga3, galoot::core::blades<rga3, "e1 e2 e3 e4">, double>;
struct NamedPoint : Point { using Point::Point; };
double first(const NamedPoint& p) { return get<"e1">(p); }
