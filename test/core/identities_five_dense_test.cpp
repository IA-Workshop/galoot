// The identities over two random forms of five generators whose entries are
// almost all nonzero, on bivectors, and on vectors for the products of
// three factors: the products of all their blades, the antiproduct of a
// vector with a bivector, and a product of three bivectors are beyond what
// one compiler evaluates at its default limits.
#include <random>

#include <galoot/core/metric.hpp>

#include "identities_checks.hpp"

namespace galoot_test::identities {

namespace {

using dense_forms = forms<core::from_gram({
                              {3, 1, -3, -1, -2},
                              {1, -3, -2, 1, 0},
                              {-3, -2, -2, 3, 0},
                              {-1, 1, 3, 3, 2},
                              {-2, 0, 0, 2, -1},
                          }),
                          core::from_gram({
                              {-3, 1, 2, -1, -2},
                              {1, -3, 0, -3, -1},
                              {2, 0, 0, 3, -3},
                              {-1, -3, 3, 1, -2},
                              {-2, -1, -3, -2, 2},
                          })>;

} // namespace

// Each identity on its own layouts, so that no other product of these forms
// is built.
void dense_forms_of_five(Identity identity, std::mt19937_64& source) {
  []<auto... G>(Identity which, forms<G...> /*list*/, std::mt19937_64& s) {
    switch (which) {
    case Identity::de_morgan:
      (check_de_morgan<bivectors_mv<G>, bivectors_mv<G>>(s), ...);
      return;
    case Identity::antiproduct_laws:
      (check_antiproduct_laws<vectors_mv<G>, vectors_mv<G>>(s), ...);
      return;
    case Identity::product_rules:
      (check_product_rules<bivectors_mv<G>, bivectors_mv<G>>(s), ...);
      return;
    case Identity::product_laws:
      (check_product_laws<vectors_mv<G>, vectors_mv<G>>(s), ...);
      return;
    case Identity::defining_relation:
      (check_defining_relation<form<G>>(s), ...);
      return;
    }
  }(identity, dense_forms{}, source);
}

} // namespace galoot_test::identities
