// Two errors, one for each name the header does not declare.
#include <lib.hpp>

int main() { return lib::first_missing() + lib::second_missing(); }
