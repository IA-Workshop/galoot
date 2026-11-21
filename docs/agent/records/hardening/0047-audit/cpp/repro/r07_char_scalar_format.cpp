// A multivector over char, which ring_scalar accepts. Its coefficients print
// as characters, through std::formatter<char, char>, so 65 prints as A and 0
// as a NUL byte.
#include <cstdio>
#include <format>
#include <string>
#include <galoot/core.hpp>
#include <galoot/core/format.hpp>
struct r2 { static constexpr auto value = galoot::core::R<2, 0, 0>(); };
using V = galoot::core::mv<r2, galoot::core::blades<r2, "e1 e2">, char>;
int main() {
  const std::string text = std::format("{}", V{65, 0});
  std::printf("length %zu:", text.size());
  for (const char c : text) {
    std::printf(" %02x", static_cast<unsigned char>(c));
  }
  std::printf("\n");
  return 0;
}
