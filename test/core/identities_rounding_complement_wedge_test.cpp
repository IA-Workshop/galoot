// The De Morgan laws for the complements of a wedge and of an antiwedge over
// double, for the test case in identities_rounding_test.cpp.
#include <string>

// The products of multivectors, found by argument-dependent lookup, which
// the include check does not see.
#include <galoot/core/product.hpp> // IWYU pragma: keep
#include <galoot/core/unary.hpp>

#include "identities_rounding_checks.hpp"

namespace galoot_test::identities_rounding {

template <class Alg>
void right_complement_of_wedge(const full_tracked<Alg>& a,
                               const full_tracked<Alg>& b,
                               const full_double<Alg>& da,
                               const full_double<Alg>& db,
                               const std::string& form, tally& out) {
  using core::right_complement;
  compare(form + " right(a ^ b)", right_complement(a ^ b),
          right_complement(a) & right_complement(b), right_complement(da ^ db),
          right_complement(da) & right_complement(db), out);
}
template void right_complement_of_wedge<rga3>(const full_tracked<rga3>& a,
                                              const full_tracked<rga3>& b,
                                              const full_double<rga3>& da,
                                              const full_double<rga3>& db,
                                              const std::string& form,
                                              tally& out);
template void right_complement_of_wedge<cga3>(const full_tracked<cga3>& a,
                                              const full_tracked<cga3>& b,
                                              const full_double<cga3>& da,
                                              const full_double<cga3>& db,
                                              const std::string& form,
                                              tally& out);

template <class Alg>
void left_complement_of_wedge(const full_tracked<Alg>& a,
                              const full_tracked<Alg>& b,
                              const full_double<Alg>& da,
                              const full_double<Alg>& db,
                              const std::string& form, tally& out) {
  using core::left_complement;
  compare(form + " left(a ^ b)", left_complement(a ^ b),
          left_complement(a) & left_complement(b), left_complement(da ^ db),
          left_complement(da) & left_complement(db), out);
}
template void left_complement_of_wedge<rga3>(const full_tracked<rga3>& a,
                                             const full_tracked<rga3>& b,
                                             const full_double<rga3>& da,
                                             const full_double<rga3>& db,
                                             const std::string& form,
                                             tally& out);
template void left_complement_of_wedge<cga3>(const full_tracked<cga3>& a,
                                             const full_tracked<cga3>& b,
                                             const full_double<cga3>& da,
                                             const full_double<cga3>& db,
                                             const std::string& form,
                                             tally& out);

template <class Alg>
void right_complement_of_antiwedge(const full_tracked<Alg>& a,
                                   const full_tracked<Alg>& b,
                                   const full_double<Alg>& da,
                                   const full_double<Alg>& db,
                                   const std::string& form, tally& out) {
  using core::right_complement;
  compare(form + " right(a v b)", right_complement(a & b),
          right_complement(a) ^ right_complement(b), right_complement(da & db),
          right_complement(da) ^ right_complement(db), out);
}
template void right_complement_of_antiwedge<rga3>(const full_tracked<rga3>& a,
                                                  const full_tracked<rga3>& b,
                                                  const full_double<rga3>& da,
                                                  const full_double<rga3>& db,
                                                  const std::string& form,
                                                  tally& out);
template void right_complement_of_antiwedge<cga3>(const full_tracked<cga3>& a,
                                                  const full_tracked<cga3>& b,
                                                  const full_double<cga3>& da,
                                                  const full_double<cga3>& db,
                                                  const std::string& form,
                                                  tally& out);

template <class Alg>
void left_complement_of_antiwedge(const full_tracked<Alg>& a,
                                  const full_tracked<Alg>& b,
                                  const full_double<Alg>& da,
                                  const full_double<Alg>& db,
                                  const std::string& form, tally& out) {
  using core::left_complement;
  compare(form + " left(a v b)", left_complement(a & b),
          left_complement(a) ^ left_complement(b), left_complement(da & db),
          left_complement(da) ^ left_complement(db), out);
}
template void left_complement_of_antiwedge<rga3>(const full_tracked<rga3>& a,
                                                 const full_tracked<rga3>& b,
                                                 const full_double<rga3>& da,
                                                 const full_double<rga3>& db,
                                                 const std::string& form,
                                                 tally& out);
template void left_complement_of_antiwedge<cga3>(const full_tracked<cga3>& a,
                                                 const full_tracked<cga3>& b,
                                                 const full_double<cga3>& da,
                                                 const full_double<cga3>& db,
                                                 const std::string& form,
                                                 tally& out);

} // namespace galoot_test::identities_rounding
