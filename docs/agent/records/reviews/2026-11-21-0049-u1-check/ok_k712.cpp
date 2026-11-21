#include <galoot/core.hpp>
#include <galoot/presets/rga3.hpp>
namespace core = galoot::core; namespace rga3 = galoot::presets::rga3;
struct e3 { static constexpr auto value = core::R<3, 0, 0>(); };
using V = core::mv<e3, core::blades<e3, "e1 e2 e3">, int>;
using D = core::mv<e3, core::blades<e3, "e1 e2 e3">, double>;
constexpr rga3::Point<int> p{0,0,0,1}, q{0,1,0,1}; constexpr auto j = p ^ q; static_assert(core::get<"e24">(j) == -1); static_assert(core::get<"e42">(rga3::Line<int>{j}) == 1); static_assert(core::get<"1">(core::dot(V{1,2,0}, V{3,1,0})) == 5);
int main(){}
