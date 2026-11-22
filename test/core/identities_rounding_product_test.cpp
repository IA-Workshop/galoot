// Associativity of the geometric product over double, for the test case in
// identities_rounding_test.cpp.
#include <string>

// The products of multivectors, found by argument-dependent lookup, which
// the include check does not see.
#include <galoot/core/product.hpp> // IWYU pragma: keep

#include "identities_rounding_checks.hpp"

namespace galoot_test::identities_rounding {

template <class Alg>
void product_associativity(const full_tracked<Alg>& a,
                           const full_tracked<Alg>& b,
                           const full_tracked<Alg>& c,
                           const full_double<Alg>& da,
                           const full_double<Alg>& db,
                           const full_double<Alg>& dc, const std::string& form,
                           tally& out) {
  compare(form + " (a ⟑ b) ⟑ c = a ⟑ (b ⟑ c)", (a * b) * c, a * (b * c),
          (da * db) * dc, da * (db * dc), out);
}

template void product_associativity<rga3>(const full_tracked<rga3>& a,
                                          const full_tracked<rga3>& b,
                                          const full_tracked<rga3>& c,
                                          const full_double<rga3>& da,
                                          const full_double<rga3>& db,
                                          const full_double<rga3>& dc,
                                          const std::string& form, tally& out);
template void product_associativity<cga3>(const full_tracked<cga3>& a,
                                          const full_tracked<cga3>& b,
                                          const full_tracked<cga3>& c,
                                          const full_double<cga3>& da,
                                          const full_double<cga3>& db,
                                          const full_double<cga3>& dc,
                                          const std::string& form, tally& out);

} // namespace galoot_test::identities_rounding
