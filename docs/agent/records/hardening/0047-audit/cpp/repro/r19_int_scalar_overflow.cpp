// A product over int, an exact scalar, whose values overflow int at run time:
// undefined behavior, which no check of the library reports in any mode. The
// guide calls the integer types exact without stating their range. Build with
// -fsanitize=undefined.
#include <cstdio>
#include <galoot/core.hpp>
namespace core = galoot::core;
struct r3 { static constexpr auto value = core::R<3, 0, 0>(); };
using V = core::mv<r3, core::blades<r3, "e1 e2 e3">, int>;
int main(int argc, char**) {
  const int big = 65536 * argc;  // 2^16
  const auto b = core::wedge(V{big, 0, 0}, V{0, big, 0});
  std::printf("e1 ^ e2 scaled by 2^16 each: %d on e12\n", b.coefficients[0]);
  return 0;
}
