// Calls twice() with a double, which the catch-all refuses at compile time.
#include <lib.hpp>

int main() { return lib::twice(2.5); }
