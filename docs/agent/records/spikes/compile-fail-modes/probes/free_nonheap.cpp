#include <cstdlib>
void f() {
  int x = 0;
  std::free(&x);
}
