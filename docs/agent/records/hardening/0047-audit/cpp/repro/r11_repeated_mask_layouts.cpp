// A blade set with one mask listed twice, given to the public layout
// variables, which take a set that no multivector has checked. The plan
// generator does not reject the repeat. This checks what the user gets: the
// same layout as without the repeat, so the repeat is harmless here, and an
// mv on the repeated set is still rejected.
#include <galoot/core.hpp>
namespace core = galoot::core;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
constexpr core::blade_set<2> twice{{{{.mask = 1, .sign = 1}, {.mask = 1, .sign = -1}}}};
constexpr core::blade_set<1> once{{{{.mask = 1, .sign = 1}}}};
constexpr auto vectors = core::blades<rga3, "e1 e2 e3 e4">;
static_assert(core::geometric_product_layout<rga3, twice, vectors> ==
              core::geometric_product_layout<rga3, once, vectors>);
static_assert(core::antiwedge_layout<rga3, twice, vectors> ==
              core::antiwedge_layout<rga3, once, vectors>);
static_assert(core::right_complement_layout<rga3, twice> ==
              core::right_complement_layout<rga3, once>);
static_assert(core::bulk_layout<rga3, twice> == core::bulk_layout<rga3, once>);
int main() { return 0; }
