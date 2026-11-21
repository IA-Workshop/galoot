// Sparse use of an algebra of n generators, 16 < n <= 32, which the guide
// says every later part of the library accepts, untested. The layout holds
// e1 and en alone, written as masks, since names are read only up to e9.
// -DWIDE_N=<n> -DWIDE_OP=<k> picks one operation, evaluated in a constant
// expression with its expected value, derived by hand from the definitions:
//   OP 1  geometric product: (a e1 + b en)^2 = a^2 + b^2 on the scalar
//   OP 2  wedge: (e1 + 2 en) ^ (3 e1 + en) = (1 - 6) e1n
//   OP 3  right complement: right(e1) = +e2...n, since e1 ^ e2...n = e1...n
//   OP 4  bulk dual: in R(n,0,0) it is the reverse times the antiscalar,
//         and the bulk dual of e1 is e1 e1...n = +e2...n
//   OP 5  antiwedge of the complements of e1 and en: left(e1 ^ en), the
//         blade e2...(n-1) with a sign of +1 or -1
//   OP 6  geometric antiproduct of e1 with itself: left(right(e1)^2), +1
//         or -1 times the antiscalar
#include <cstddef>

#include <galoot/core.hpp>

namespace core = galoot::core;

struct wide {
  static constexpr auto value = core::R<WIDE_N, 0, 0, 32>();
};

constexpr core::mask_t first = 1;
constexpr core::mask_t last = core::mask_t{1} << (WIDE_N - 1);
constexpr core::mask_t all = core::detail::low_bits(WIDE_N);
constexpr core::blade_set<2> layout{{{{.mask = first, .sign = 1},
                                      {.mask = last, .sign = 1}}}};
using V = core::mv<wide, layout, long long>;

#if WIDE_OP == 1
constexpr auto r = core::geometric_product(V{2, 3}, V{2, 3});
static_assert(r.coefficients[0] == 13);
#elif WIDE_OP == 2
constexpr auto r = core::wedge(V{1, 2}, V{3, 1});
static_assert(r.component_count == 1 && r.coefficients[0] == -5);
#elif WIDE_OP == 3
constexpr auto r = core::right_complement(V{1, 0});
// Canonical order puts e1...(n-1), the image of en, before e2...n.
static_assert(r.layout.blades[1].mask == (all & ~first) &&
              r.coefficients[1] == 1 && r.coefficients[0] == 0);
#elif WIDE_OP == 4
constexpr auto r = core::bulk_dual(V{1, 0});
// Canonical order puts e1...(n-1), the image of en, before e2...n.
static_assert(r.layout.blades[1].mask == (all & ~first) &&
              r.coefficients[1] == 1 && r.coefficients[0] == 0);
#elif WIDE_OP == 5
constexpr auto r = core::antiwedge(core::right_complement(V{1, 0}),
                                   core::right_complement(V{0, 1}));
static_assert(r.component_count == 1 && r.coefficients[0] != 0 &&
              r.layout.blades[0].mask == (all & ~first & ~last));
#elif WIDE_OP == 6
constexpr auto r = core::geometric_antiproduct(V{1, 0}, V{1, 0});
// The antiscalar, of the highest grade, is the last blade of the result.
constexpr auto top = r.component_count - 1;
static_assert(r.layout.blades[top].mask == all &&
              (r.coefficients[top] == 1 || r.coefficients[top] == -1));
#endif

int main() { return 0; }
