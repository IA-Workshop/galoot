// A line of an algebra tag whose names hold x43, which is neither 1 nor e and
// indices, formatted into auto s. This file must not compile.
#include <format>
#include <string>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/format.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
  [[maybe_unused]] static constexpr auto names = "e41 e42 x43";
};

using Line =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;

[[maybe_unused]] void use(const Line& x) { auto s = std::format("{}", x); }

} // namespace
