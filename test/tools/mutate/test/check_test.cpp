// Checks twice() at compile time and at run time. A failed run-time check
// prints its name on a line of the form doctest gives a failing test case,
// and the program exits 1.
#include <cstdio>

#include <lib.hpp>

static_assert(lib::twice(3) == 6);
static_assert(lib::twice(-4) == -8);

namespace {

int failures = 0;

void check(bool passed, const char* name) {
  if (!passed) {
    std::printf("TEST CASE:  %s\n", name);
    ++failures;
  }
}

} // namespace

int main() {
  // Computed at compile time and compared at run time, so that the
  // compile-time route is checked even with the static_asserts turned off.
  constexpr int folded = lib::twice(3);
  check(folded == 6, "twice folded at compile time");
  // A volatile argument keeps the call out of constant evaluation.
  volatile int three = 3;
  check(lib::twice(three) == 6, "twice at run time");
  // Constant-evaluated, so that checked()'s guard is judged at compile time.
  constexpr int accepted = lib::checked(5);
  check(accepted == 5, "checked accepts a valid value");
  // Called at run time; the header's table is computed at compile time.
  check(lib::below_eight_table<5>() == 5, "the table holds a valid value");
  return failures == 0 ? 0 : 1;
}
