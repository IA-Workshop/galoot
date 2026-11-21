// Probes of preconditions in an unchecked build (GALOOT_CHECKING 0).
#include <galoot/core.hpp>
#include <galoot/core/plan.hpp>
#include <galoot/core/blade_product.hpp>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <span>
using namespace galoot::core;
volatile std::size_t vn; volatile mask_t va; volatile int vi;
int main(int argc, char** argv) {
  const char* w = argc > 1 ? argv[1] : "";
  if (!std::strcmp(w, "antigrade")) { vn = std::size_t{1} << 31; va = 1; std::printf("%d\n", antigrade(va, vn)); }
  if (!std::strcmp(w, "antireverse")) { vn = 50000; va = 0; std::printf("%d\n", antireverse_sign(va, vn)); }
  if (!std::strcmp(w, "complement0")) { vn = 0; va = 0; std::printf("%u\n", complement_mask(va, vn)); }
  if (!std::strcmp(w, "complement40")) { vn = 40; va = 0; std::printf("%d\n", right_complement_sign(va, vn)); }
  if (!std::strcmp(w, "collect")) {
    std::array<detail::plan_term, 3> plan{};
    for (std::size_t k = 0; k < 3; ++k) plan[k] = {.a = 0, .b = 0, .out = static_cast<mask_t>(va + k + 1), .coefficient = 1};
    vi = 1;
    auto* storage = new oriented_blade[static_cast<std::size_t>(vi)]{};
    std::span<oriented_blade> out(storage, static_cast<std::size_t>(vi));
    std::printf("%zu\n", detail::collect_result(plan, out));
  }
  if (!std::strcmp(w, "is_null")) { auto* g = new metric<3>{}; vn = 5; std::printf("%d\n", static_cast<int>(g->is_null(vn))); }
  if (!std::strcmp(w, "termlist")) {
    detail::term_list<2> t{};
    for (mask_t m = 1; m <= 3; ++m) t.add(m + va, 1);
    std::printf("%zu\n", t.size());
  }
  if (!std::strcmp(w, "canonical")) { va = mask_t{1} << 12 | 1; auto n = canonical_name(va); std::printf("%.*s\n", static_cast<int>(n.text().size()), n.text().data()); }
  return 0;
}
