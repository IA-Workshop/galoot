#include <galoot/core.hpp>
#include <cstdio>
namespace core = galoot::core;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
int main() {
  constexpr auto a = core::blades<rga3, "e42 e43">;
  constexpr auto m = core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
  for (auto b : a.blades) std::printf("src mask %u sign %d\n", unsigned(b.mask), int(b.sign));
  for (auto b : m.blades) std::printf("motor mask %u sign %d\n", unsigned(b.mask), int(b.sign));
}
