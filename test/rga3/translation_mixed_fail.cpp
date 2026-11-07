// A translation by 1.0, 0, and 2, a double and two ints, which must all be
// written as doubles. This file must not compile.
#include <galoot/rga3.hpp>

namespace {

namespace rga3 = galoot::rga3;

[[maybe_unused]] void use() {
  [[maybe_unused]] const auto r = rga3::translation(1.0, 0, 2);
}

} // namespace
