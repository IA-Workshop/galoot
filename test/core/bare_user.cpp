// A user of the core with no build system, who adds the headers to a
// project and nothing else: core.bare_compile compiles this file with only
// -std=c++23 -pedantic-errors -I include. It declares a line of rigid(3),
// makes one, reads and writes it by name, and compares it.
#include <galoot/core.hpp>
#include <galoot/core/assert.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = galoot::core::mv<rga3, line_layout, double>;

} // namespace

int main() {
  GALOOT_ASSERT(true, "bare_user: a true condition");

  Line axis{0, 0, 1, 0, 0, 0};
  get<"e23">(axis) = 2.0;
  const bool moved = axis != Line{0, 0, 1, 0, 0, 0};
  return moved && get<"e43">(axis) == 1.0 ? 0 : 1;
}
