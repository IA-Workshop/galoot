// A name read in R(10, 0, 0), where names with one digit per index cannot
// reach e10. The layout is given by mask.
// This file must not compile.
#include <array>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct r10 {
  static constexpr auto value = galoot::core::R<10, 0, 0>();
};

constexpr auto blades = std::to_array<galoot::core::oriented_blade>({
    {.mask = 0b1, .sign = 1},
    {.mask = 0b10, .sign = 1},
});
constexpr galoot::core::blade_set<2> layout{blades};
using Wide = galoot::core::mv<r10, layout, double>;

[[maybe_unused]] double read(const Wide& x) { return get<"e1">(x); }

} // namespace
