// Spike A: the generator is one constexpr function usable at compile time
// and at run time. This program
//   1. compares a constant-evaluated plan with the same generator run at
//      run time on a metric read from the command line, and
//   2. prints the run-time table in the format of
//      `clifford_reference.py table`, so the two can be diffed.
//
// Usage: runtime_check N g11,g12,...;g21,... (N <= 6; rows split by ';')
#include "gen.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

constexpr auto conf3 = spike::conformal<5>();
constexpr auto all5 = spike::all<5>();
constexpr auto& compiled = spike::plan<conf3, all5, all5>;

std::string name(spike::mask_t m) {
  if (m == 0) {
    return "1";
  }
  std::string s = "e";
  for (int i = 0; i < 32; ++i) {
    if ((m >> i & 1U) != 0) {
      s += std::to_string(i + 1);
    }
  }
  return s;
}

template <std::size_t N> int run(const std::vector<int>& g) {
  spike::metric<N> m{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      m.g[i][j] = g[i * N + j];
    }
  }
  const auto blades = spike::all<N>();
  const std::size_t count = spike::generate(m, blades.masks, blades.masks, {});
  std::vector<spike::plan_term> terms(count);
  spike::generate(m, blades.masks, blades.masks, terms);

  // Order terms as the reference does: by pair, then grade, then indices.
  std::ranges::stable_sort(terms, [](const auto& x, const auto& y) {
    if (x.a != y.a || x.b != y.b) {
      return x.a != y.a ? x.a < y.a : x.b < y.b;
    }
    const int gx = std::popcount(x.out);
    const int gy = std::popcount(y.out);
    if (gx != gy) {
      return gx < gy;
    }
    // Lexicographic order of the index lists: compare lowest indices first.
    spike::mask_t u = x.out;
    spike::mask_t v = y.out;
    while (u != 0 && (u & -u) == (v & -v)) {
      u &= u - 1;
      v &= v - 1;
    }
    return u != 0 && (u & -u) < (v & -v);
  });
  std::size_t k = 0;
  for (std::size_t a = 0; a < blades.masks.size(); ++a) {
    for (std::size_t b = 0; b < blades.masks.size(); ++b) {
      std::string body;
      while (k < terms.size() && terms[k].a == a && terms[k].b == b) {
        body += (body.empty() ? "" : ", ") + std::to_string(terms[k].coef) +
                " " + name(terms[k].out);
        ++k;
      }
      std::printf("%s %s -> %s\n", name(blades.masks[a]).c_str(),
                  name(blades.masks[b]).c_str(),
                  body.empty() ? "0" : body.c_str());
    }
  }
  return 0;
}

} // namespace

int main(int argc, char** argv) {
  // 1. Compile-time plan equals the run-time plan for conformal(3).
  const std::size_t count = spike::generate(conf3, all5.masks, all5.masks, {});
  std::vector<spike::plan_term> rt(count);
  spike::generate(conf3, all5.masks, all5.masks, rt);
  const bool same =
      count == compiled.size() &&
      std::ranges::equal(rt, compiled, [](const auto& x, const auto& y) {
        return x.a == y.a && x.b == y.b && x.out == y.out && x.coef == y.coef;
      });
  std::fprintf(stderr, "compile-time and run-time conformal(3) plans: %s "
               "(%zu terms)\n", same ? "equal" : "DIFFER", count);
  if (!same) {
    return 1;
  }
  if (argc < 3) {
    return 0;
  }

  // 2. Run-time table for a metric known only at run time.
  const int n = std::atoi(argv[1]);
  std::vector<int> g;
  for (const char* p = argv[2]; *p != '\0';) {
    char* end = nullptr;
    g.push_back(static_cast<int>(std::strtol(p, &end, 10)));
    p = *end != '\0' ? end + 1 : end;
  }
  if (g.size() != static_cast<std::size_t>(n * n)) {
    std::fprintf(stderr, "expected %d entries\n", n * n);
    return 2;
  }
  switch (n) {
  case 2: return run<2>(g);
  case 3: return run<3>(g);
  case 4: return run<4>(g);
  case 5: return run<5>(g);
  case 6: return run<6>(g);
  default: std::fprintf(stderr, "n must be 2 to 6\n"); return 2;
  }
}
