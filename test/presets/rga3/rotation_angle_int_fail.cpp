// A rotation about a line over double by the int 1, which must be written
// 1.0. This file must not compile.
#include <galoot/presets/rga3.hpp>

namespace {

namespace rga3 = galoot::presets::rga3;

[[maybe_unused]] void use(const rga3::Line<double>& l) {
  [[maybe_unused]] const auto r = rga3::rotation(l, 1);
}

} // namespace
