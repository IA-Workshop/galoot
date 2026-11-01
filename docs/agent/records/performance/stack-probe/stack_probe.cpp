// Runs e_A e_A for the antiscalar of R(16,0,0) with a term list of the
// capacity product_capacity gives, 2^15, at run time.
#include <cstdio>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/metric.hpp>
int main(int argc, char**) {
  namespace core = galoot::core;
  constexpr std::size_t capacity = std::size_t{1} << 15;
  const core::mask_t a = (core::mask_t{1} << 16) - static_cast<core::mask_t>(argc);
  const auto r = core::detail::blade_product<capacity>(core::R<16, 0, 0>(), a, a);
  std::printf("sizeof(term_list<2^15>) = %zu, terms = %zu, coefficient = %lld\n",
              sizeof(r), r.size(), static_cast<long long>(r.terms[0].coefficient));
}
