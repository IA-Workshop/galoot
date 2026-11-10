// Times p ^ q, the line through two points, as the rga3 Line{p ^ q}, against
// its straight-line reference kernel; harness.hpp gives the command line.
#include <cstddef>
#include <span>

#include <galoot/presets/rga3.hpp>

#include <bench/harness.hpp>
#include <bench/reference_kernels.hpp>

namespace {

namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Line = rga3::Line<double>;

} // namespace

// A failure to print ends the run with the exception's message, as an
// uncaught exception does.
// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv) {
  return bench::main<Point, Point>(
      "wedge_point_point", std::span(argv, static_cast<std::size_t>(argc)),
      [](const Point& p, const Point& q) { return Line{p ^ q}; },
      [](const Point& p, const Point& q) {
        return bench::as<Line>(reference_kernels::wedge_point_point(
            p.coefficients, q.coefficients));
      });
}
