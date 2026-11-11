// A user of the core with no build system who prints a value, which needs
// the printing header that the umbrella header leaves out:
// core.bare_compile_format_no_exceptions compiles this file with only
// -std=c++23 -pedantic-errors -fno-exceptions -I include. It prints a line of
// rigid(3).
#include <print>

#include <galoot/core.hpp>
#include <galoot/core/format.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

using Line = galoot::core::mv<
    rga3, galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;

} // namespace

int main() {
  const Line axis{0, 0, 1, 0, 0, 0};
  std::println("{}", axis);
}
