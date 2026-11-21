// The example of the GALOOT_ALWAYS_INLINE comment: in a file built with
// -march=x86-64-v3, a fallback declared [[gnu::target("arch=x86-64")]] that
// calls transform. With GALOOT_FORCE_INLINE 1 both compilers should stop;
// with 0 the file should build.
#include <galoot/core.hpp>

namespace core = galoot::core;

struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Rotor = core::mv<r300, core::blades<r300, "1 e12 e13 e23">, double>;
using Vector = core::mv<r300, core::blades<r300, "e1 e2 e3">, double>;

[[gnu::target("arch=x86-64")]] Vector fallback(const Vector& x,
                                               const Rotor& v) {
  return core::transform(x, v);
}

int main() { return 0; }
