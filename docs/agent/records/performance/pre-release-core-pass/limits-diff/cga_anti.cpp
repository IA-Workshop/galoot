#include <galoot/core.hpp>
struct cga3 {
  static constexpr auto value = galoot::core::conformal<3>();
};
using Full = galoot::core::mv<cga3, galoot::core::full<5>, double>;
auto antimultiply(const Full& a, const Full& b) { return a % b; }
