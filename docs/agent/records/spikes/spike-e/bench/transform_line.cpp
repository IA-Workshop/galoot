// Times the forms of transform(x, Q) of a rigid(3) Line by a Motor;
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
  using X = galoot::presets::rga3::Line<double>;
  return spike_bench::main<
      6, spike_bench::library<X>, forms::transform_line_a<double>,
      forms::transform_line_b<double>, spike_bench::collected<X>,
      forms::transform_line_c<double>>(
      "transform_line", std::span(argv, static_cast<std::size_t>(argc)));
}
