// Times transform(x, Q) of an rga3 Plane by a Motor, against its straight-line
// reference kernel; harness.hpp gives the command line.
#include <cstddef>
#include <span>

#include <galoot/presets/rga3.hpp>

#include <bench/harness.hpp>
#include <bench/reference_kernels.hpp>

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Plane = rga3::Plane<double>;
using Motor = rga3::Motor<double>;

} // namespace

// A failure to print ends the run with the exception's message, as an
// uncaught exception does.
// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv) {
  return bench::main<Plane, Motor>(
      "transform_plane", std::span(argv, static_cast<std::size_t>(argc)),
      [](const Plane& x, const Motor& q) { return core::transform(x, q); },
      [](const Plane& x, const Motor& q) {
        return bench::as<Plane>(
            reference_kernels::transform_plane(x.coefficients, q.coefficients));
      });
}
