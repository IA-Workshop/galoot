// The empty layout, which the dot of a point and a line produces, through
// the operations a user may apply to it next. Each is expected to compile and
// give an empty value.
#include <cstdio>
#include <format>
#include <galoot/core.hpp>
#include <galoot/core/format.hpp>
namespace core = galoot::core;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
using Point = core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4">, double>;
using Line = core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;
using Motor = core::mv<rga3, core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">, double>;
int main() {
  const auto none = core::dot(Point{1, 2, 3, 1}, Line{0, 0, 1, 0, 0, 0});
  using Empty = std::remove_cvref_t<decltype(none)>;
  static_assert(Empty::component_count == 0);
  const auto sum = none + none;
  const auto scaled = 2.0 * none;
  const auto negated = -none;
  const auto product = none * Point{1, 0, 0, 1};
  const auto complement = !none;
  const auto bulk = core::bulk(none);
  const auto moved = core::transform(none, Motor{0, 0, 0, 1, 0, 0, 0, 0});
  const auto as_point = Point{none};
  std::printf("%s %s %s %s %s %s %s %s %d\n", std::format("{}", none).c_str(),
              std::format("{}", sum).c_str(), std::format("{}", scaled).c_str(),
              std::format("{}", negated).c_str(), std::format("{}", product).c_str(),
              std::format("{}", complement).c_str(), std::format("{}", bulk).c_str(),
              std::format("{}", moved).c_str(), as_point == Point{} ? 1 : 0);
  return none == Empty{} ? 0 : 1;
}
