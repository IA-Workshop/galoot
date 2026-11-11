#include <galoot/core.hpp>
struct rn {
  static constexpr auto value = galoot::core::R<GENERATORS, 0, 0>();
};
using Full = galoot::core::mv<rn, galoot::core::full<GENERATORS>, double>;
auto multiply(const Full& a, const Full& b) { return a * b; }
auto antimultiply(const Full& a, const Full& b) { return a % b; }
