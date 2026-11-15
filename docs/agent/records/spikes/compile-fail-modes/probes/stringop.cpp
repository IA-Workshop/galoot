#include <cstring>
void f(char* out) {
  char b[2];
  std::strcpy(b, "hello");
  std::memcpy(out, b, 2);
}
