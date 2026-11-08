// A number minus a value of a class derived from a line. This file must not
// compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

using Line =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;

// A class derived from a line, as a user might declare a named type.
struct DerivedLine : Line {
  using Line::Line;
};

[[maybe_unused]] Line use(const DerivedLine& x) { return 1.0 - x; }

} // namespace
