// A rotation about a line over std::int64_t, which has no std::sin or
// std::cos. This file must not compile.
#include <cstdint>

#include <galoot/rga3.hpp>

namespace {

namespace rga3 = galoot::rga3;

[[maybe_unused]] void use(const rga3::Line<std::int64_t>& l) {
  [[maybe_unused]] const auto r = rga3::rotation(l, std::int64_t{1});
}

} // namespace
