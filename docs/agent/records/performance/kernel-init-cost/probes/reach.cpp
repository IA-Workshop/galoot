#include <galoot/core.hpp>
#include <cstdio>
namespace core = galoot::core;
namespace d = galoot::core::detail;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
template <core::blade_set V, core::blade_set X>
void report(const char* vn, const char* xn) {
  constexpr const auto& k = d::second_kernel<rga3, V, X, true>;
  std::array<bool, X.size()> hit{};
  std::size_t first_out = k.size() ? k[0].out : 99;
  for (const auto& t : k) hit[t.out] = true;
  std::printf("V=%-28s X=%-24s first=%zu unreached:", vn, xn, first_out);
  for (std::size_t i = 0; i < X.size(); ++i) if (!hit[i]) std::printf(" %zu", i);
  std::printf("\n");
}
#define L(s) core::blades<rga3, s>
#define R(v, x) report<L(v), L(x)>(v, x)
int main() {
#define ALLX(v) R(v, "e1 e2 e3 e4"); R(v, "e41 e42 e43 e23 e31 e12"); R(v, "e423 e431 e412 e321"); R(v, "1 e41 e42 e43 e23 e31 e12 e1234"); R(v, "e1 e2 e3 e4 e423 e431 e412 e321")
  ALLX("1 e41 e42 e43 e23 e31 e12 e1234");
  ALLX("e23 e31 e12 e1234");
  ALLX("e41 e42 e43 e1234");
  ALLX("e12");
  ALLX("e41");
  ALLX("e12 e1234");
  ALLX("1");
  ALLX("e1234");
  ALLX("1 e12");
  ALLX("e1 e2 e3 e4");
  ALLX("e423 e431 e412 e321");
  ALLX("e4");
  ALLX("e321");
  ALLX("e1");
}
