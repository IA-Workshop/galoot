// Times compose(a, b) of two rga3 Motors, as Motor{compose(a, b)}, against its
// straight-line reference kernel; harness.hpp gives the command line.
#include <cstddef>
#include <span>

#include <galoot/presets/rga3.hpp>

#include <bench/harness.hpp>
#include <bench/reference_kernels.hpp>

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Motor = rga3::Motor<double>;

} // namespace

// A failure to print ends the run with the exception's message, as an
// uncaught exception does.
// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv) {
  return bench::main<Motor, Motor>(
      "compose_motors", std::span(argv, static_cast<std::size_t>(argc)),
      [] [[gnu::always_inline]] (const Motor& a, const Motor& b) {
        return Motor{core::compose(a, b)};
      },
      [] [[gnu::always_inline]] (const Motor& a, const Motor& b) {
        return bench::as<Motor>(
            reference_kernels::compose_motors(a.coefficients, b.coefficients));
      });
}
