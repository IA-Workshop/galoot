// A precondition violation in an unchecked build: b names e5 in an algebra
// of four generators.
#include <cstdio>
#include <cstdlib>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/metric.hpp>
namespace core = galoot::core;
int main(int argc, char**) {
  static constexpr auto g = core::R<4, 0, 0>();
  const core::mask_t b = static_cast<core::mask_t>(0x11U + static_cast<unsigned>(argc - 1));
  const auto p = core::detail::blade_product<32>(g, 1U, b);
  for (std::size_t k = 0; k < p.size(); ++k)
    std::printf("%+lld e%x\n", static_cast<long long>(p.terms[k].coefficient), p.terms[k].blade);
}
