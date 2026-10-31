// Spike C probe: includes sys/ugsa.hpp and instantiates f<FK>; see
// run_probes.sh.
#include <ugsa.hpp>
template void f<FK>();
int main() {}
