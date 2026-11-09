// GCC 16.2.1 wrong code, smallest form found: make(2) evaluated in a
// constant expression writes element 2 of the value-initialized local `a`
// first, and the run-time make(0) of the same translation unit then leaves
// a[2] zero-filled, d = 0. Exit status 1 means the defect is present.
#include <array>

struct q {
  long n = 0;
  long d = 1;
};

constexpr std::array<q, 3> make(int i) {
  std::array<q, 3> a{};
  a[static_cast<unsigned>(i)].n = 1;
  return a;
}

[[maybe_unused]] constexpr auto once = make(2);

int main() {
  volatile int i = 0;
  return make(i)[2].d == 1 ? 0 : 1;
}
