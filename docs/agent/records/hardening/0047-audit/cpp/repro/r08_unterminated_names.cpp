// A list of names given as a char array without a terminating null. The
// list's text is taken to be all characters but the last, so the last
// character of the array is dropped without a diagnostic: "e12" written as
// {'e', '1', '2'} names e1. get and has_name read a name the same way. (An
// algebra tag's names member without a null is different: it converts to
// std::string_view by a search for the null, which reads past the array, and
// the constant evaluation fails on both compilers.)
#include <cstdio>
#include <galoot/core.hpp>
struct rga3 { static constexpr auto value = galoot::core::rigid<3>(); };
constexpr char names[] = {'e', '1', '2'};
constexpr auto set = galoot::core::blades<rga3, names>;
static_assert(set.size() == 1);
static_assert(set.blades[0].mask == 0b01);  // e1, not e12 (0b11)
using Point = galoot::core::mv<rga3, galoot::core::blades<rga3, "e1 e2 e3 e4">, double>;
static_assert(galoot::core::has_name<Point, names>);  // read as "e1"
static_assert(galoot::core::get<names>(Point{5, 6, 7, 8}) == 5);
int main() {
  std::printf("blades<rga3, {'e','1','2'}> holds mask %u\n", set.blades[0].mask);
  return 0;
}
