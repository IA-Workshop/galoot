// The coefficient path of the kernels over std::uint16_t. In the form
// diag(181, 181), e12 e12 = -181 * 181 = -32761. That coefficient converts to
// std::uint16_t as 32775, and the kernel then multiplies 32775 by the product
// of the two factors. Both promote to int, so 32775 * 65535 overflows int,
// though the product of the factors (65535 * 1) does not. Modulo 2^16 the
// exact answer is -32761 * 65535 = 32761.
//
// Built twice: with -DCONSTANT_CASE the product runs in a constant
// evaluation; otherwise at run time, under -fsanitize=undefined.
#include <cstdint>
#include <cstdio>

#include <galoot/core.hpp>

namespace core = galoot::core;

struct g181 {
  static constexpr auto value = core::from_gram({{181, 0}, {0, 181}});
};

using U16 = std::uint16_t;
using Bivector = core::mv<g181, core::blades<g181, "e12">, U16>;

#ifdef CONSTANT_CASE
constexpr auto constant_square =
    core::geometric_product(Bivector{U16{65535}}, Bivector{U16{1}});
#endif

int main(int argc, char** /*argv*/) {
  // From argc, so that the compiler cannot fold the product.
  const auto big = static_cast<U16>(65534 + argc);
  const auto square =
      core::geometric_product(Bivector{big}, Bivector{U16{1}});
  std::printf("uint16 (65535 e12)(e12) on the scalar: %u (2^16-modular "
              "answer 32761)\n",
              static_cast<unsigned>(core::get<"1">(square)));
  return 0;
}
