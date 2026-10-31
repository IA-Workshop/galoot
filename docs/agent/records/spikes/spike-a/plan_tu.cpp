// Spike A measurement unit: constant-evaluates the full geometric-product
// plan (all blades times all blades) for one metric and uses it at run time,
// so that the table is both generated and emitted.
//
// -DSPIKE_N=<n> sets the number of generators; -DSPIKE_METRIC=<f> picks the
// metric factory in gen.hpp: euclidean, conformal, or dense. -DSPIKE_CAP=<c>
// sets the scratch capacity (default 2^n).
#include "gen.hpp"

#ifndef SPIKE_N
#error "define SPIKE_N"
#endif
#ifndef SPIKE_METRIC
#error "define SPIKE_METRIC"
#endif

namespace {
constexpr auto m = spike::SPIKE_METRIC<SPIKE_N>();
constexpr auto blades = spike::all<SPIKE_N>();
#ifdef SPIKE_CAP
constexpr auto& gp = spike::plan<m, blades, blades, SPIKE_CAP>;
#else
constexpr auto& gp = spike::plan<m, blades, blades>;
#endif
static_assert(gp.size() >= blades.masks.size() * blades.masks.size() / 2);
} // namespace

int main(int argc, char**) {
  // A run-time index keeps the whole table in the object at every level.
  const auto& t = gp[static_cast<std::size_t>(argc) % gp.size()];
  return static_cast<int>(t.coef + t.out);
}
