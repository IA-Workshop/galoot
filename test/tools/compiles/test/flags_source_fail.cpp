// Its compile-fail test lends its flags to the sources that have no test of
// their own.
#include <lib.hpp>

int main() { return lib::twice(); }
