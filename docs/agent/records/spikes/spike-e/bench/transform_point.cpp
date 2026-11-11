// Times the forms of transform(x, Q) of a rigid(3) Point by a Motor;
// harness.hpp gives the command line.
#include <cstddef>
#include <span>

#include <galoot/presets/rga3.hpp>

#include "forms.hpp"
#include "harness.hpp"
#include "wrappers.hpp"

// A failure to print ends the run with the exception's message.
// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv) {
  using X = galoot::presets::rga3::Point<double>;
  return spike_bench::main<4, spike_bench::library<X>,
                           forms::transform_point_a<double>,
                           forms::transform_point_b<double>,
                           spike_bench::collected<X>,
                           forms::transform_point_c<double>>(
      "transform_point", std::span(argv, static_cast<std::size_t>(argc)));
}
