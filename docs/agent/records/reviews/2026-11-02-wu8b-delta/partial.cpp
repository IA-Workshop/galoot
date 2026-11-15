// A program's numeric_limits specialization that gives fewer members than
// the new check reads: is the coefficient still checked?
#include <cstdint>
#include <cstdio>
#include <limits>

#include <galoot/core/mv.hpp>

struct tiny_int { signed char v = 0; };
template <> struct std::numeric_limits<tiny_int> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_integer = true;
  static constexpr signed char max() { return 127; }
};
struct short_float { float v = 0; };
template <> struct std::numeric_limits<short_float> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_exact = false;
  static constexpr int radix = 2;
  static constexpr int digits = 11;
};

int main() {
  using galoot::core::detail::holds_exactly;
  std::printf("tiny_int 200: %d   short_float 4097: %d\n",
              holds_exactly<tiny_int>(200), holds_exactly<short_float>(4097));
}
