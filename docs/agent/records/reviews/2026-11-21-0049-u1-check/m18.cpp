#include <galoot/core.hpp>
#include <galoot/presets/rga3.hpp>
namespace core = galoot::core; namespace rga3 = galoot::presets::rga3;
struct e3 { static constexpr auto value = core::R<3, 0, 0>(); };
using V = core::mv<e3, core::blades<e3, "e1 e2 e3">, int>;
using D = core::mv<e3, core::blades<e3, "e1 e2 e3">, double>;
int f(){ V a{1,2,0}, b{3,1,0}; int d = core::dot(a, b); return d; }
int main(){}
