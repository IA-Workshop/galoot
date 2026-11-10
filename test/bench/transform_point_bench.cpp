// Times transform(x, Q) of an rga3 Point by a Motor, against its straight-line
// reference kernel; harness.hpp gives the command line.
#include <cstddef>
#include <span>

#include <galoot/presets/rga3.hpp>

#include <bench/harness.hpp>
#include <bench/reference_kernels.hpp>

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Motor = rga3::Motor<double>;

} // namespace

// A failure to print ends the run with the exception's message, as an
// uncaught exception does.
// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv) {
  return bench::main<Point, Motor>(
      "transform_point", std::span(argv, static_cast<std::size_t>(argc)),
      [](const Point& x, const Motor& q) { return core::transform(x, q); },
      [](const Point& x, const Motor& q) {
        return bench::as<Point>(
            reference_kernels::transform_point(x.coefficients, q.coefficients));
      });
}
