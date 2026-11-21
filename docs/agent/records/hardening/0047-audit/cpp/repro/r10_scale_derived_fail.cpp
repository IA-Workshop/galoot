// 2 * x for x of a class derived from an mv over double. For an mv, the
// library rejects it with "s * a: the scalar is not of the multivector's
// scalar type ..."; for the derived class the error is the compiler's.
#include <galoot/core.hpp>
struct rga3 { static constexpr auto value = galoot::core::rigid<3>(); };
using Point = galoot::core::mv<rga3, galoot::core::blades<rga3, "e1 e2 e3 e4">, double>;
struct NamedPoint : Point { using Point::Point; };
Point twice(const NamedPoint& p) { return 2 * p; }
