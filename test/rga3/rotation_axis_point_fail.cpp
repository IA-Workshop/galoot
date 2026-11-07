// A rotation about a Point, which is not a line. This file must not
// compile.
#include <galoot/rga3.hpp>

namespace {

namespace rga3 = galoot::rga3;

[[maybe_unused]] void use(const rga3::Point<double>& p) {
  [[maybe_unused]] const auto r = rga3::rotation(p, 0.5);
}

} // namespace
