// A rotation of R(3, 0, 0) with rational entries, from the quaternion
// (w, x, y, z) = (W, 2, 1, 1): P = M / N with N = w^2 + x^2 + y^2 + z^2 and M
// the integer rotation matrix. With W = 79, N = 6247: P keeps the inner
// products (M M^T = N^2 I, about 3.9e7), its inverse is P^T, and every
// minor of P, the coefficient of a blade image, is a ratio of integers below
// N^3, about 2.4e11. The change is still rejected as overflowing: the
// fraction-free elimination that finds the inverse forms products of 3 x 3
// minors, about N^5, above 2^63. With -DW=78 (N = 6090) it is accepted.
//
// Built with -DPROBE, the file prints the verdicts of the elimination and of
// the inner-product check instead.
#include <galoot/core.hpp>
#ifndef W
#define W 79
#endif
namespace core = galoot::core;
struct r3 { static constexpr auto value = core::R<3, 0, 0>(); };
constexpr int w = W, x = 2, y = 1, z = 1;
constexpr int n = w * w + x * x + y * y + z * z;
struct turn {
  using source = r3;
  using target = r3;
  static constexpr auto value = core::from_rows(
      {{w * w + x * x - y * y - z * z, 2 * (x * y - w * z), 2 * (x * z + w * y)},
       {2 * (x * y + w * z), w * w - x * x + y * y - z * z, 2 * (y * z - w * x)},
       {2 * (x * z - w * y), 2 * (y * z + w * x), w * w - x * x - y * y + z * z}},
      {n, n, n});
};
#ifdef PROBE
#include <cstdio>
int main() {
  constexpr auto e = core::detail::eliminate(core::detail::reduced(turn::value));
  constexpr auto ip = core::detail::keeps_inner_products(
      core::detail::reduced(turn::value), r3::value, r3::value);
  std::printf("W=%d N=%d: elimination fits %d, inner products fit %d hold %d\n",
              w, n, e.fits, ip.fits, ip.holds);
  return 0;
}
#else
using Vector = core::mv<r3, core::blades<r3, "e1 e2 e3">, double>;
Vector rotate(const Vector& v) { return Vector{core::change_basis<turn>(v)}; }
#endif
