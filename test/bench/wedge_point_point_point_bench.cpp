// Times p ^ q ^ r, the plane through three points, as the rga3
// Plane{p ^ q ^ r}, against its straight-line reference kernel; harness.hpp
// gives the command line.
#include <cstddef>
#include <span>

#include <galoot/presets/rga3.hpp>

#include <bench/harness.hpp>
#include <bench/reference_kernels.hpp>

namespace {

namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Plane = rga3::Plane<double>;

} // namespace

// A failure to print ends the run with the exception's message, as an
// uncaught exception does.
// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv) {
  return bench::main<Point, Point, Point>(
      "wedge_point_point_point",
      std::span(argv, static_cast<std::size_t>(argc)),
      [](const Point& p, const Point& q, const Point& r) {
        return Plane{(p ^ q) ^ r};
      },
      [](const Point& p, const Point& q, const Point& r) {
        return bench::as<Plane>(reference_kernels::wedge_point_point_point(
            p.coefficients, q.coefficients, r.coefficients));
      });
}
