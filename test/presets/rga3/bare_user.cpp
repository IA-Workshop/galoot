// A user of the rga3 preset with no build system, who adds the headers to a
// project and nothing else: rga3.bare_compile compiles this file with only
// -std=c++23 -pedantic-errors -I include. It joins two points into a line,
// meets the line with a plane, and moves a point by a translator and a
// rotor composed into one motor.
#include <galoot/presets/rga3.hpp>

int main() {
  using Point = galoot::presets::rga3::Point<double>;
  using Line = galoot::presets::rga3::Line<double>;
  using Plane = galoot::presets::rga3::Plane<double>;
  using Motor = galoot::presets::rga3::Motor<double>;

  const Point p{0, 0, 0, 1};
  const Point q{0, 0, 1, 1};
  const Line axis{p ^ q};
  const Plane floor{0, 0, 1, -2};
  const Point hit{axis & floor};

  const Motor turn{0, 0, 0.6, 0.8, 0, 0, 0, 0};
  const Motor move{0, 0, 0, 1, 1, 2, -1, 0};
  const Motor both{compose(move, turn)};
  const Point moved = transform(Point{1, 0, 0, 1}, both);

  // The meet is the point (0, 0, 2) at some weight, and a motor keeps a
  // point's weight positive.
  const bool on_floor = get<"e3">(hit) == 2.0 * get<"e4">(hit);
  return on_floor && get<"e4">(moved) > 0.0 ? 0 : 1;
}
