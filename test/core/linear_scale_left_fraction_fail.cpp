// A line over std::int64_t times the double 2.5, which no conversion to the
// scalar type could keep. This file must not compile.
#include <cstdint>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = galoot::core::mv<rga3, line_layout, std::int64_t>;

[[maybe_unused]] void use(const Line& a) {
  [[maybe_unused]] const auto r = 2.5 * a;
}

} // namespace
