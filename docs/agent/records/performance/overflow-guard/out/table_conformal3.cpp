#include <cstddef>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/metric.hpp>

namespace {

constexpr auto g = galoot::core::conformal<3>();

consteval std::size_t table_terms() {
  std::size_t count = 0;
  for (galoot::core::mask_t a = 0; a < 32; ++a) {
    for (galoot::core::mask_t b = 0; b < 32; ++b) {
      count += galoot::core::detail::blade_product<16>(g, a, b).size();
    }
  }
  return count;
}

static_assert(table_terms() == 1024);

} // namespace
