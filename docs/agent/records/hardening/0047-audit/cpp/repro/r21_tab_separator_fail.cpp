// Names separated by a tab: only spaces separate names, so "e1<tab>e2" is one
// name, and the message speaks of generator indices, not of the separator.
#include <galoot/core.hpp>
struct rga3 { static constexpr auto value = galoot::core::rigid<3>(); };
[[maybe_unused]] constexpr auto set = galoot::core::blades<rga3, "e1\te2">;
