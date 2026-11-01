// Does either compiler warn when an aggregate is given too few values?
// Each struct is initialized with three of its four values.
#include <array>

struct with_array {
  std::array<double, 4> c;
};
struct with_members {
  double e1, e2, e3, e4;
};
struct with_c_array {
  double c[4]; // NOLINT
};

double use() {
  const with_array a{1, 2, 3};
  const with_members m{1, 2, 3};
  const with_c_array r{1, 2, 3};
  const with_array b{{1, 2, 3}};
  return a.c[3] + m.e4 + r.c[3] + b.c[3];
}

int main() { return static_cast<int>(use()); }
