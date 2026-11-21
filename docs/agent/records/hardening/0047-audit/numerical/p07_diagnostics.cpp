// Compile-time cases of what a user sees for a few misuses, chosen by
// -DCASE=<n>. Each case's comment says what the header promises.
#include <cstdint>

#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;

struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};

// A ring scalar with + - * and unary -, made from std::int64_t only
// explicitly, and with no division.
struct ring {
  std::int64_t v{};
  ring() = default;
  constexpr explicit ring(std::int64_t x) : v{x} {}
  friend constexpr ring operator+(ring a, ring b) { return ring{a.v + b.v}; }
  friend constexpr ring operator-(ring a, ring b) { return ring{a.v - b.v}; }
  friend constexpr ring operator*(ring a, ring b) { return ring{a.v * b.v}; }
  friend constexpr ring operator-(ring a) { return ring{-a.v}; }
};
static_assert(core::ring_scalar<ring>);

#if CASE == 1
// Names are separated by spaces; a tab is read as part of a name, and the
// message speaks of digits.
constexpr auto set = core::blades<r300, "e1\te2">;
#elif CASE == 2
// A set with one mask twice is accepted by the public layout templates,
// which do not check it; no multivector can be built on it.
constexpr core::blade_set<2> twice{{{{.mask = 1, .sign = 1},
                                     {.mask = 1, .sign = 1}}}};
constexpr auto product = core::geometric_product_layout<r300, twice, twice>;
constexpr auto complement = core::right_complement_layout<r300, twice>;
static_assert(product.size() == 1 && complement.size() == 1);
#elif CASE == 3
// A one-blade value from a source that converts to the scalar only
// explicitly: the compiler's own error, which the header says has no short
// message of the library's.
using E1 = core::mv<r300, core::blades<r300, "e1">, ring>;
const E1 x{1};
#elif CASE == 4
// translation over a ring scalar without division: the header requires
// division, and the call gets the compiler's error from the body.
const auto t = galoot::presets::rga3::translation(ring{1}, ring{2}, ring{3});
#elif CASE == 5
// A metric written by hand with an entry beyond max_form_entry (32767):
// metric_like is false, so the multivector type does not match its
// constraint, and the error is the compiler's account of a failed concept.
struct big {
  static constexpr core::metric<2> value{{{{40000, 0}, {0, 1}}}};
};
using M = core::mv<big, core::full<2>, double>;
const M m{};
#endif

int main() { return 0; }
