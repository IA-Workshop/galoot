// How each compiler prints a blade set held in a std::array against one held
// in a C array, when the set is a template argument named in a diagnostic.
// PROBE_HOLDER 1 uses std::array, 2 a C array. The program must not compile.
#include <array>
#include <cstddef>

#ifndef PROBE_HOLDER
#define PROBE_HOLDER 1
#endif

struct oriented_blade {
  unsigned mask;
  int sign;
  constexpr bool operator==(const oriented_blade&) const = default;
};

template <std::size_t K>
struct blade_set {
#if PROBE_HOLDER == 1
  std::array<oriented_blade, K> blades;
#else
  oriented_blade blades[K]; // NOLINT
#endif
  constexpr bool operator==(const blade_set&) const = default;
};

template <blade_set B>
struct mv {
  static_assert(B.blades[0].mask == 0, "the first blade is not the scalar");
};

constexpr blade_set<2> line_start{
    {oriented_blade{9, -1}, oriented_blade{10, -1}}};
[[maybe_unused]] constexpr mv<line_start> wrong{};
