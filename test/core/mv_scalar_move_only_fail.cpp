// A point over std::unique_ptr<double>, which has a default constructor and
// cannot be copied. This file must not compile.
#include <memory>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;

[[maybe_unused]] const galoot::core::mv<rga3, point_layout,
                                        std::unique_ptr<double>> wrong{};

} // namespace
