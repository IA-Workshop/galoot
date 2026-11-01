// Whether a class whose only member is a const std::array is trivially
// copyable. Its copy and move constructors are trivial and its assignments
// are deleted, which the standard's definition of a trivially copyable class
// allows; this file compiles where the compiler agrees.
#include <array>
#include <type_traits>

struct holder {
  const std::array<int, 2> x;
};
static_assert(std::is_trivially_copy_constructible_v<holder>);
static_assert(std::is_trivially_move_constructible_v<holder>);
static_assert(!std::is_copy_assignable_v<holder>);
static_assert(!std::is_move_assignable_v<holder>);
static_assert(std::is_trivially_destructible_v<holder>);
static_assert(std::is_trivially_copyable_v<holder>);

int main() { return 0; }
