// A translation by the ints 1, 2, 3, whose halves an int would truncate.
// This file must not compile.
#include <galoot/rga3.hpp>

namespace {

namespace rga3 = galoot::rga3;

[[maybe_unused]] void use() {
  [[maybe_unused]] const auto r = rga3::translation(1, 2, 3);
}

} // namespace
