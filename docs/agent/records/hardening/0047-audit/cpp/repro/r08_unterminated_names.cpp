// A list of names given as a char array without a terminating null. The
// list's text is taken to be all characters but the last, so the last
// character of the array is dropped without a diagnostic: "e12" written as
// {'e', '1', '2'} names e1.
#include <cstdio>
#include <galoot/core.hpp>
struct rga3 { static constexpr auto value = galoot::core::rigid<3>(); };
constexpr char names[] = {'e', '1', '2'};
constexpr auto set = galoot::core::blades<rga3, names>;
static_assert(set.size() == 1);
static_assert(set.blades[0].mask == 0b01);  // e1, not e12 (0b11)
int main() {
  std::printf("blades<rga3, {'e','1','2'}> holds mask %u\n", set.blades[0].mask);
  return 0;
}
