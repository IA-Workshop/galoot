// A line of an algebra tag whose names hold e434, which lists e4 twice,
// formatted. This file must not compile.
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
  [[maybe_unused]] static constexpr auto names = "e41 e434";
};

using Line =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;

[[maybe_unused]] std::string text(const Line& x) {
  return std::format("{}", x);
}

} // namespace
