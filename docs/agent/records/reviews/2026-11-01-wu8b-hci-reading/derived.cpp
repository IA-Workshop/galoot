// A user's named classes derived from mv: do conversions and truncate see
// through them?
#include <type_traits>

#include <galoot/core.hpp>

namespace gc = galoot::core;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
struct Line : gc::mv<rga3, line_layout, double> {
  using mv::mv;
};
struct Motor : gc::mv<rga3, motor_layout, double> {
  using mv::mv;
};

int main() {
  const Line l{1, 2, 3, 4, 5, 6};
  const Motor m{l};                          // derived to derived, lossless
  const Line copy{l};                        // copy
#ifdef DERIVED_TARGET
  const auto back = gc::truncate<Line>(m);
#else
  const Line back{gc::truncate<line_layout>(m)};
#endif

  const auto by_layout = gc::truncate<line_layout>(m);
#ifdef LOSSY
  const Line lost{m};
#endif
  return back == copy && by_layout == l ? 0 : 1;
}
