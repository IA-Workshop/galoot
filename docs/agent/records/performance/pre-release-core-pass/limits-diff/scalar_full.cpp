#include <galoot/core.hpp>
struct rn {
  static constexpr auto value = galoot::core::R<GENERATORS, 0, 0>();
};
inline constexpr galoot::core::blade_set<1> scalar_layout{
    std::array<galoot::core::oriented_blade, 1>{{{.mask = 0, .sign = 1}}}};
using Full = galoot::core::mv<rn, galoot::core::full<GENERATORS>, double>;
using Scalar = galoot::core::mv<rn, scalar_layout, double>;
Full multiply(const Scalar& s, const Full& f) {
  return galoot::core::geometric_product(s, f);
}
