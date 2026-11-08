// Checks halve() at run time. A failure prints its name as doctest names a
// failing test case, and the program exits 1.
#include <cstdio>

#include <lib.hpp>

int main() {
  volatile int ten = 10;
  if (lib::halve(ten) != 5) {
    std::printf("TEST CASE:  halve at run time\n");
    return 1;
  }
  return 0;
}
