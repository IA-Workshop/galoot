// The program of the consumer projects that the README's CMake examples
// describe. It joins two points with the rga3 preset, so it builds only when
// the project finds the preset's headers and compiles them as C++23.
#include <galoot/presets/rga3.hpp>

int main() {
  using Point = galoot::presets::rga3::Point<double>;
  using Line = galoot::presets::rga3::Line<double>;

  // The line from p to q has the direction q - p on e41, e42, e43, here
  // (0, 0, 1).
  const Line axis{Point{0, 0, 0, 1} ^ Point{0, 0, 1, 1}};
  return get<"e43">(axis) == 1.0 ? 0 : 1;
}
