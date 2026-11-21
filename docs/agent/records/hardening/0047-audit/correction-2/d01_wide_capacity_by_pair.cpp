// Which operations Clang refuses in R(GEN, 0, 0) built with MaxN 32, for
// single-blade operands written as masks. product_capacity gives a pair of
// blades a and b a term list of 2^(|a ∪ b| - 1) places, and pair_capacity
// takes the complements of a and b for the geometric antiproduct. Clang
// limits an array in a constant evaluation to 2^20 elements, so the formula
// predicts a refusal exactly when the pair, or for an antiproduct the pair of
// complements, spans 22 or more generators. Every bulk, weight and dual
// multiplies by the antiscalar, whose pair with any blade spans all GEN.
//   -DGEN=<generators> -DCASE=<k>, k from 1 to 12, one operation per build:
//   CASE  operation                      generators the pair spans at GEN = 22
//    1    e1 % e1                        21 (complements)
//    2    e1 % e2                        22 (complements)
//    3    antiscalar % antiscalar         0 (complements)
//    4    e1 * e2                         2
//    5    e1...e11 * e12...e22           22
//    6    e1 * antiscalar                22
//    7    bulk_dual(1)                   22
//    8    bulk_dual(antiscalar)          22
//    9    weight_dual(e1)                22 (through bulk)
//   10    left_weight_dual(antiscalar)   22 (through bulk)
//   11    right_complement(e1)           a unary map, no product
//   12    bulk_dual of an empty layout   no pair, capacity 1
// Each result is checked by a static_assert on a value derived by hand where
// one is short to state.
#include <galoot/core.hpp>

namespace core = galoot::core;

struct wide {
  static constexpr auto value = core::R<GEN, 0, 0, 32>();
};

constexpr core::mask_t all = core::detail::low_bits(GEN);
constexpr core::mask_t e1 = 1;
constexpr core::mask_t e2 = 2;
constexpr core::mask_t low_half = core::detail::low_bits(GEN / 2);

template <core::mask_t M>
inline constexpr core::blade_set<1> one{{{{.mask = M, .sign = 1}}}};

template <core::mask_t M>
using B = core::mv<wide, one<M>, double>;

#if CASE == 1
constexpr auto r = B<e1>{1.0} % B<e1>{1.0};
#elif CASE == 2
constexpr auto r = B<e1>{1.0} % B<e2>{1.0};
#elif CASE == 3
// The antiscalar is the identity of the antiproduct.
constexpr auto r = B<all>{2.0} % B<all>{3.0};
static_assert(r.coefficients[0] == 6.0);
#elif CASE == 4
constexpr auto r = B<e1>{1.0} * B<e2>{1.0};
static_assert(r.coefficients[0] == 1.0);
#elif CASE == 5
constexpr auto r = B<low_half>{1.0} * B<all & ~low_half>{1.0};
static_assert(r.coefficients[0] == 1.0);
#elif CASE == 6
constexpr auto r = B<e1>{1.0} * B<all>{1.0};
#elif CASE == 7
// The bulk dual of the scalar 1 is the antiscalar.
constexpr auto r = core::bulk_dual(B<0>{1.0});
static_assert(r.coefficients[0] == 1.0);
#elif CASE == 8
constexpr auto r = core::bulk_dual(B<all>{1.0});
#elif CASE == 9
constexpr auto r = core::weight_dual(B<e1>{1.0});
#elif CASE == 10
constexpr auto r = core::left_weight_dual(B<all>{1.0});
#elif CASE == 11
// right(e1) = e2...n, since e1 ∧ e2...n = e1...n.
constexpr auto r = core::right_complement(B<e1>{1.0});
static_assert(r.layout.blades[0].mask == (all & ~e1) &&
              r.coefficients[0] == 1.0);
#elif CASE == 12
constexpr auto r = core::bulk_dual(core::mv<wide, core::blade_set<0>{}, double>{});
static_assert(r.component_count == 0);
#endif

int main() { return 0; }
