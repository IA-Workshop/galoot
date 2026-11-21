// One unit of a program whose units disagree on GALOOT_FORCE_INLINE. Built
// twice, with -DGALOOT_FORCE_INLINE=1 -DUNIT=forced_unit and with
// -DGALOOT_FORCE_INLINE=0 -DUNIT=free_unit. Each defines a function of its own
// name that applies the same rotor of R(3, 0, 0) to the same vector with
// transform, and so instantiates the same inline functions of the core with
// two different definitions: one carries [[gnu::always_inline]], the other
// does not.
#include <galoot/core.hpp>

namespace core = galoot::core;

struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Rotor = core::mv<r300, core::blades<r300, "1 e12 e13 e23">, double>;
using Vector = core::mv<r300, core::blades<r300, "e1 e2 e3">, double>;

Vector UNIT(const Vector& x, const Rotor& v) { return core::transform(x, v); }
