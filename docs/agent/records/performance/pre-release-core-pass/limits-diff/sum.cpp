#include <galoot/core.hpp>
struct rn {
  static constexpr auto value = galoot::core::R<GENERATORS, 0, 0>();
};
using Full = galoot::core::mv<rn, galoot::core::full<GENERATORS>, double>;
using Biv = galoot::core::mv<rn, galoot::core::of_grade<GENERATORS, 2>, double>;
auto add(const Full& a, const Biv& b) { return a + b; }
