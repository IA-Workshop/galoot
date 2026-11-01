// A one-blade multivector initialized from its scalar with =, which needs an
// implicit conversion; the constructor from one value is explicit. This file
// must not compile, and the diagnostic is the compiler's own.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

using Scalar = galoot::core::mv<rga3, galoot::core::blades<rga3, "1">, double>;

[[maybe_unused]] const Scalar wrong = 2.0;

} // namespace
