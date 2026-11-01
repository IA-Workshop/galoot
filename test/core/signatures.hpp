#ifndef GALOOT_TEST_CORE_SIGNATURES_HPP
#define GALOOT_TEST_CORE_SIGNATURES_HPP

// The signatures that the exhaustive tests of the product run over, each
// with a name for the failure messages.

#include <galoot/core/metric.hpp>

namespace galoot_test {

// Every signature of the exhaustive checks: the diagonal R(p, q, r) with
// null generators last, conformal(d), the Witt forms of one and two modes,
// scaled so that a a† + a† a = 2, a degenerate form with a zero first pivot,
// rigid(3) with its null generator first, and ten seeded random symmetric
// forms with entries from -3 to 3.
template <class Visit>
void for_each_signature(const Visit& visit) {
  visit("R(1,0,0)", galoot::core::R<1, 0, 0>());
  visit("R(0,1,0)", galoot::core::R<0, 1, 0>());
  visit("R(0,0,1)", galoot::core::R<0, 0, 1>());
  visit("R(2,0,0)", galoot::core::R<2, 0, 0>());
  visit("R(0,2,0)", galoot::core::R<0, 2, 0>());
  visit("R(1,1,0)", galoot::core::R<1, 1, 0>());
  visit("R(1,0,1)", galoot::core::R<1, 0, 1>());
  visit("R(0,0,2)", galoot::core::R<0, 0, 2>());
  visit("R(3,0,0)", galoot::core::R<3, 0, 0>());
  visit("R(2,1,0)", galoot::core::R<2, 1, 0>());
  visit("R(2,0,1)", galoot::core::R<2, 0, 1>());
  visit("R(1,1,1)", galoot::core::R<1, 1, 1>());
  visit("R(0,0,3)", galoot::core::R<0, 0, 3>());
  visit("R(4,0,0)", galoot::core::R<4, 0, 0>());
  visit("rigid(3)", galoot::core::rigid<3>());
  visit("R(1,3,0)", galoot::core::R<1, 3, 0>());
  visit("R(3,1,0)", galoot::core::R<3, 1, 0>());
  visit("R(2,2,0)", galoot::core::R<2, 2, 0>());
  visit("R(2,0,2)", galoot::core::R<2, 0, 2>());
  visit("R(0,0,4)", galoot::core::R<0, 0, 4>());
  visit("R(4,1,0)", galoot::core::R<4, 1, 0>());
  visit("R(4,0,1)", galoot::core::R<4, 0, 1>());
  visit("R(4,2,0)", galoot::core::R<4, 2, 0>());
  visit("R(3,3,0)", galoot::core::R<3, 3, 0>());
  visit("conformal(1)", galoot::core::conformal<1>());
  visit("conformal(2)", galoot::core::conformal<2>());
  visit("conformal(3)", galoot::core::conformal<3>());
  visit("conformal(4)", galoot::core::conformal<4>());
  visit("Witt, one mode", galoot::core::from_gram({
                              {0, 1},
                              {1, 0},
                          }));
  visit("Witt, two modes", galoot::core::from_gram({
                               {0, 1, 0, 0},
                               {1, 0, 0, 0},
                               {0, 0, 0, 1},
                               {0, 0, 1, 0},
                           }));
  visit("degenerate", galoot::core::from_gram({
                          {0, 1, 0},
                          {1, 0, 0},
                          {0, 0, 0},
                      }));
  visit("rigid(3), null first", galoot::core::from_gram({
                                    {0, 0, 0, 0},
                                    {0, 1, 0, 0},
                                    {0, 0, 1, 0},
                                    {0, 0, 0, 1},
                                }));
  visit("random 0", galoot::core::from_gram({
                        {-1, -2},
                        {-2, 3},
                    }));
  visit("random 1", galoot::core::from_gram({
                        {2, 0, -3},
                        {0, 1, -3},
                        {-3, -3, -2},
                    }));
  visit("random 2", galoot::core::from_gram({
                        {-2, 0, -2, 2},
                        {0, -2, -3, 3},
                        {-2, -3, 1, 3},
                        {2, 3, 3, 3},
                    }));
  visit("random 3", galoot::core::from_gram({
                        {3, 1, -3, -1, -2},
                        {1, -3, -2, 1, 0},
                        {-3, -2, -2, 3, 0},
                        {-1, 1, 3, 3, 2},
                        {-2, 0, 0, 2, -1},
                    }));
  visit("random 4", galoot::core::from_gram({
                        {3, 1},
                        {1, -2},
                    }));
  visit("random 5", galoot::core::from_gram({
                        {-3, -3, 0},
                        {-3, -2, 2},
                        {0, 2, 0},
                    }));
  visit("random 6", galoot::core::from_gram({
                        {0, 1, -1, 0},
                        {1, -1, 0, -1},
                        {-1, 0, 2, 0},
                        {0, -1, 0, -1},
                    }));
  visit("random 7", galoot::core::from_gram({
                        {-3, 1, 2, -1, -2},
                        {1, -3, 0, -3, -1},
                        {2, 0, 0, 3, -3},
                        {-1, -3, 3, 1, -2},
                        {-2, -1, -3, -2, 2},
                    }));
  visit("random 8", galoot::core::from_gram({
                        {3, -2},
                        {-2, -2},
                    }));
  visit("random 9", galoot::core::from_gram({
                        {-3, 1, -2},
                        {1, -3, -2},
                        {-2, -2, 1},
                    }));
}

} // namespace galoot_test

#endif // GALOOT_TEST_CORE_SIGNATURES_HPP
