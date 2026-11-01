// A name with the index 0: generators count from e1.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = galoot::core::mv<rga3, line_layout, double>;

[[maybe_unused]] double read(const Line& line) { return get<"e0">(line); }

} // namespace
