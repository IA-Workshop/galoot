// A translation over std::int64_t, whose halves would truncate. This file
// must not compile.
#include <cstdint>

#include <galoot/presets/rga3.hpp>

namespace {

namespace rga3 = galoot::presets::rga3;

[[maybe_unused]] void use(std::int64_t dx, std::int64_t dy, std::int64_t dz) {
  [[maybe_unused]] const auto r = rga3::translation(dx, dy, dz);
}

} // namespace
