#include <format>
#include <string>

#include <galoot/core.hpp>
#include <galoot/core/format.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

}  // namespace

std::string point_text() {
  const galoot::core::mv<rga3, galoot::core::blades<rga3, "e1 e2 e3 e4">, double> p{1, 2, 3, 1};
  return std::format("{}", p);
}
