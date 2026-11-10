// The geometric product of a scalar and a full<GENERATORS> value of
// R(GENERATORS,0,0), as the kernel-init cost unit wrote it. GENERATORS comes
// from -DGENERATORS=...; default 10.
#include <galoot/core.hpp>

#ifndef GENERATORS
#define GENERATORS 10
#endif

struct rn {
  static constexpr auto value = galoot::core::R<GENERATORS, 0, 0>();
};
inline constexpr galoot::core::blade_set<1> scalar_layout{
    std::array<galoot::core::oriented_blade, 1>{{{.mask = 0, .sign = 1}}}};
using Full = galoot::core::mv<rn, galoot::core::full<GENERATORS>, double>;
using Scalar = galoot::core::mv<rn, scalar_layout, double>;

auto multiply(const Scalar& s, const Full& f) {
  return galoot::core::geometric_product(s, f);
}
