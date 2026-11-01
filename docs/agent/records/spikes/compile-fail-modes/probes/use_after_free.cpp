#include <cstdlib>
int f() {
  int* p = static_cast<int*>(std::malloc(sizeof(int)));
  std::free(p);
  return *p;
}
