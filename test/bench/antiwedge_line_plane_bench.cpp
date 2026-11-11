// Times l & g, the point where a line meets a plane, as the rga3
// Point{l & g}, against its straight-line reference kernel; harness.hpp gives
// the command line.
#include <cstddef>
#include <span>

#include <galoot/presets/rga3.hpp>

#include <bench/harness.hpp>
#include <bench/reference_kernels.hpp>

namespace {

namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Line = rga3::Line<double>;
using Plane = rga3::Plane<double>;

} // namespace

// A failure to print ends the run with the exception's message, as an
// uncaught exception does.
// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv) {
  return bench::main<Line, Plane>(
      "antiwedge_line_plane", std::span(argv, static_cast<std::size_t>(argc)),
      [] [[gnu::always_inline]] (const Line& l, const Plane& g) {
        return Point{l & g};
      },
      [] [[gnu::always_inline]] (const Line& l, const Plane& g) {
        return bench::as<Point>(reference_kernels::antiwedge_line_plane(
            l.coefficients, g.coefficients));
      });
}
