// Whether the compilers' constexpr __builtin_memcpy can overwrite the bytes
// of an object whose constructor is private and whose matrix is const. This
// file compiles only if it can.
#include <array>

struct sealed {
  // NOLINTNEXTLINE(*-avoid-c-arrays): as in sealed.cpp.
  const int gram[2][2];
  static consteval sealed make() { return sealed{}; }

private:
  consteval sealed() : gram{{1, 0}, {0, 1}} {}
};

constexpr sealed forge() {
  sealed s = sealed::make();
  const std::array<int, 4> raw{0, 1, 2, 0};
  __builtin_memcpy(&s, &raw, sizeof s);
  return s;
}
constexpr sealed forged = forge();
static_assert(forged.gram[0][1] == 1);

int main() { return 0; }
