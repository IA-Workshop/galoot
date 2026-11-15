// A tag whose metric has no generators, given where an algebra is required.
// This file must not compile, and the diagnostic must name the requirement
// that fails, the number of generators (core.metric_like_no_generators).
#include <galoot/core/metric.hpp>

namespace {

namespace core = galoot::core;

struct no_generators {
  static constexpr core::metric<0> value{};
};

template <core::metric_like Alg>
struct algebra {};

static_assert(sizeof(algebra<no_generators>) == 1);

} // namespace
