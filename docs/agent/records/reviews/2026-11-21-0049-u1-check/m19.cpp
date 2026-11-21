#include <galoot/core.hpp>
#include <galoot/presets/rga3.hpp>
namespace core = galoot::core; namespace rga3 = galoot::presets::rga3;
struct e3 { static constexpr auto value = core::R<3, 0, 0>(); };
using V = core::mv<e3, core::blades<e3, "e1 e2 e3">, int>;
using D = core::mv<e3, core::blades<e3, "e1 e2 e3">, double>;
D f(){ D v{1,2,3}; return 2 * v; }
int main(){}
