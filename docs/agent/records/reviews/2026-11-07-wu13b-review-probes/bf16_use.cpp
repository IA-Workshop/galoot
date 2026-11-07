#include <stdfloat>
#include <galoot/core.hpp>
namespace core = galoot::core;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
using B = std::bfloat16_t;
using Point = core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4">, B>;
using Motor = core::mv<rga3, core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">, B>;
Point f(const Point& p, const Motor& q) { return core::transform(p, q); }
