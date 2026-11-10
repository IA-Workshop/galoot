// Runs the plan's own generation, collection, and typing for the scalar times
// full<10> of R(10,0,0) at run time, where libstdc++'s assertions and the
// sanitizers check every index, and checks what make_product_plan's checks
// would: 1,024 terms, 1,024 result blades in canonical order, each term
// typed onto a place inside the result. Exit 0 when all hold.
#include <array>
#include <cstdio>

#include <galoot/core.hpp>

namespace d = galoot::core::detail;

int main() {
  static constexpr auto g = galoot::core::R<10, 0, 0>();
  static constexpr auto full = galoot::core::full<10>;
  static constexpr std::array<galoot::core::oriented_blade, 1> one{
      {{.mask = 0, .sign = 1}}};
  constexpr auto op = d::BinaryOperation::geometric_product;
  constexpr std::size_t capacity = d::plan_capacity(op, one, full.blades, 10);
  static std::array<d::plan_term, 1024> plan{};
  const std::size_t count =
      d::generate_plan<op, capacity>(g, one, full.blades, plan);
  static std::array<galoot::core::oriented_blade, 1024> result{};
  const std::size_t size = d::collect_result(plan, result);
  static std::array<d::typed_term, 1024> typed{};
  d::type_terms(plan, result, typed);
  bool ordered = true;
  for (std::size_t r = 1; r < size; ++r) {
    ordered = ordered &&
              galoot::core::canonical_less(result[r - 1].mask, result[r].mask);
  }
  bool inside = true;
  for (const d::typed_term& t : typed) {
    inside = inside && t.out < size;
  }
  std::printf("capacity %zu, terms %zu, result blades %zu, ordered %d, "
              "typed inside %d\n",
              capacity, count, size, ordered, inside);
  // product_capacity bounds e_A e_B in any metric by 2^(|A ∪ B| - 1), which
  // for the scalar times e_1...e_10 is 512, though every pair here has one
  // term.
  return capacity == 512 && count == 1024 && size == 1024 && ordered && inside
             ? 0
             : 1;
}
