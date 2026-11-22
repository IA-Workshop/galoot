#include <galoot/core.hpp>
#include <galoot/presets/rga3.hpp>
namespace core = galoot::core; namespace rga3 = galoot::presets::rga3;
struct e3 { static constexpr auto value = core::R<3, 0, 0>(); };
using V = core::mv<e3, core::blades<e3, "e1 e2 e3">, int>;
using D = core::mv<e3, core::blades<e3, "e1 e2 e3">, double>;
int f(){ rga3::Line<int> l{0,1,0,0,0,0}; return core::get<"e14">(l); }
int main(){}
