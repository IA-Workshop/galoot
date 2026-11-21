// Prints the classical action and transform(x, bulk_dual(V)) for one rotor
// in R(3, 0, 0) and in diag(2, 1, 1).
#include <galoot/core.hpp>
#include <cstdio>
namespace core = galoot::core;
struct d2 { static constexpr auto value = core::from_gram({{2, 0, 0}, {0, 1, 0}, {0, 0, 1}}); };
struct e3 { static constexpr auto value = core::R<3, 0, 0>(); };
template <class Tag> void run(const char* name) {
  using V = core::mv<Tag, core::blades<Tag, "e1 e2 e3">, int>;
  using Ev = core::mv<Tag, core::blades<Tag, "1 e12 e13 e23">, int>;
  constexpr V a{1, 1, 0}, b{1, 0, 0}, x{2, -3, 5};
  constexpr Ev r = core::truncate<Ev>(a * b);
  constexpr V cl = core::truncate<V>(r * x * ~r);
  constexpr V t = core::transform(x, core::bulk_dual(r));
  std::printf("%s classical (%d %d %d) transform(x, bulk_dual) (%d %d %d)\n", name,
              core::get<"e1">(cl), core::get<"e2">(cl), core::get<"e3">(cl),
              core::get<"e1">(t), core::get<"e2">(t), core::get<"e3">(t));
}
int main() { run<e3>("R300"); run<d2>("diag(2,1,1)"); }
