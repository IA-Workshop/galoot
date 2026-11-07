// A rotation about a line over float by the double 0.5, which must be
// written 0.5f. This file must not compile.
#include <galoot/rga3.hpp>

namespace {

namespace rga3 = galoot::rga3;

[[maybe_unused]] void use(const rga3::Line<float>& l) {
  [[maybe_unused]] const auto r = rga3::rotation(l, 0.5);
}

} // namespace
