// The edges of the exact coefficients, each a compile-time case chosen by
// -DCASE=<n>. The expected outcome of each case is in the comment above it,
// derived from the definitions, not from the code.
#include <cstdint>

#include <galoot/core.hpp>

namespace core = galoot::core;

template <int K>
struct diagonal {
  // diag(K, ..., K) in 5 generators.
  static constexpr auto value = core::from_gram(
      {{K, 0, 0, 0, 0}, {0, K, 0, 0, 0}, {0, 0, K, 0, 0}, {0, 0, 0, K, 0},
       {0, 0, 0, 0, K}});
};

#if CASE == 1
// e12 e12 = -g11 g22 = -4096^2 = -2^24, which float holds exactly: accepted.
using A = diagonal<4096>;
using B = core::mv<A, core::blades<A, "e12">, float>;
constexpr auto r = B{1.0F} * B{1.0F};
static_assert(core::get<"1">(r) == -16777216.0F);
#elif CASE == 2
// e12 e12 = -4097^2 = -16785409, odd and of 25 bits, which float does not
// hold: rejected with the message for x * y.
using A = diagonal<4097>;
using B = core::mv<A, core::blades<A, "e12">, float>;
constexpr auto r = B{1.0F} * B{1.0F};
#elif CASE == 3
// e1234 e1234 = g11 g22 g33 g44 = 32767^4, about 1.15e18, below 2^63:
// generated, and held by std::int64_t.
using A = diagonal<32767>;
using B = core::mv<A, core::blades<A, "e1234">, std::int64_t>;
constexpr auto r = B{1} * B{1};
static_assert(core::get<"1">(r) == 1152780773560811521);
#elif CASE == 4
// e12345 e12345 = 32767^5, about 3.8e22, beyond 2^63, though every entry of
// the form is within max_form_entry: the generation fails with the message
// that a product coefficient does not fit coefficient_type.
using A = diagonal<32767>;
using B = core::mv<A, core::blades<A, "e12345">, std::int64_t>;
constexpr auto r = B{1} * B{1};
#elif CASE == 5
// The same as case 3 over double: 32767^4 is odd and of 60 bits, so double
// cannot hold it, and the product is rejected with the message for x * y.
using A = diagonal<32767>;
using B = core::mv<A, core::blades<A, "e1234">, double>;
constexpr auto r = B{1.0} * B{1.0};
#endif

int main() { return 0; }
