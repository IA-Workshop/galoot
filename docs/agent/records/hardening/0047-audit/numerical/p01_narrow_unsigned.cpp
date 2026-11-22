// A multivector over std::uint16_t: its coefficients promote to int before
// they are multiplied, so 65535 * 65535 overflows int. The header describes an
// unsigned scalar as one "whose arithmetic is modular"; the kernel's product
// is not. Built twice: with -DCONSTANT_CASE the product runs in a constant
// evaluation, otherwise at run time under -fsanitize=undefined.
#include <cstdint>
#include <cstdio>

#include <galoot/core.hpp>

namespace core = galoot::core;

struct r200 {
  static constexpr auto value = core::R<2, 0, 0>();
};

using U16 = std::uint16_t;
using Vector = core::mv<r200, core::blades<r200, "e1 e2">, U16>;

// The exact product e1 e1 of 65535 e1 with itself is 65535^2 = 4294836225,
// which is 1 modulo 2^16: a modular scalar gives 1 on the scalar blade.
#ifdef CONSTANT_CASE
constexpr auto constant_square = core::geometric_product(
    Vector{U16{65535}, U16{0}}, Vector{U16{65535}, U16{0}});
#endif

int main(int argc, char** /*argv*/) {
  // From argc, so that the compiler cannot fold the product.
  const auto big = static_cast<U16>(65534 + argc);
  const Vector a{big, U16{0}};
  const auto square = core::geometric_product(a, a);
  std::printf("uint16 e1 * e1 on the scalar: %u (2^16-modular answer 1)\n",
              static_cast<unsigned>(core::get<"1">(square)));
  const Vector scaled = big * a;
  std::printf("uint16 s * x on e1: %u (2^16-modular answer 1)\n",
              static_cast<unsigned>(core::get<"e1">(scaled)));

  // The same products over std::int16_t wrap, by the defined conversion of
  // an int back to a narrower type, and over int they are the scalar's own
  // overflow, undefined as for any int.
  using I16 = core::mv<r200, core::blades<r200, "e1 e2">, std::int16_t>;
  const auto i16 = static_cast<std::int16_t>(32766 + argc);
  const auto s16 = core::geometric_product(I16{i16, std::int16_t{0}},
                                           I16{i16, std::int16_t{0}});
  std::printf("int16 e1 * e1 on the scalar: %d (2^16-modular answer 1)\n",
              static_cast<int>(core::get<"1">(s16)));
  return 0;
}
