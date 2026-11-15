// Prints, for one set of Galoot headers, the values of reorder_sign, the
// complement signs, blade_product, and plans over fixed and seeded inputs, so
// that two builds of this one file, against the headers before and after a
// change, can be compared with cmp. Each value is also checked against a
// direct count from the definitions (pairs i > j for the signs), and the
// number of disagreements is printed.
//
// Large sections print a 64-bit FNV-1a hash of every value in order, with a
// count, rather than the values themselves. Compile-time sections print values
// computed in constant evaluation.

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <span>
#include <utility>
#include <vector>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using core::mask_t;

namespace {

struct hasher {
  std::uint64_t h = 1469598103934665603ULL;
  std::uint64_t count = 0;
  void add(std::uint64_t v) {
    for (int k = 0; k < 8; ++k) {
      h ^= (v >> (8 * k)) & 0xFFU;
      h *= 1099511628211ULL;
    }
    ++count;
  }
};

struct rng {
  std::uint64_t s;
  std::uint64_t next() {
    s ^= s << 13U;
    s ^= s >> 7U;
    s ^= s << 17U;
    return s;
  }
  mask_t mask() { return static_cast<mask_t>(next() >> 16U); }
};

// The sign of e_A e_B reordered, from the pairs i in A, j in B with i > j.
int direct_reorder(mask_t a, mask_t b) {
  int pairs = 0;
  for (unsigned i = 0; i < 32; ++i) {
    for (unsigned j = 0; j < i; ++j) {
      if (((a >> i) & 1U) != 0 && ((b >> j) & 1U) != 0) {
        ++pairs;
      }
    }
  }
  return pairs % 2 == 0 ? 1 : -1;
}

mask_t low(std::size_t n) {
  return n == 32 ? ~mask_t{0} : (mask_t{1} << n) - 1U;
}

void reorder_section() {
  hasher all;
  std::uint64_t wrong = 0;
  for (mask_t a = 0; a < 4096; ++a) {
    hasher block;
    for (mask_t b = 0; b < 4096; ++b) {
      const int s = core::reorder_sign(a, b);
      block.add(static_cast<std::uint64_t>(s + 1));
      all.add(static_cast<std::uint64_t>(s + 1));
      if (a < 256 || b < 64 || (a & b) == 0) {
        wrong += s != direct_reorder(a, b) ? 1U : 0U;
      }
    }
    if (a % 512 == 0) {
      std::printf("reorder block a=%u hash %016llx\n", a,
                  static_cast<unsigned long long>(block.h));
    }
  }
  std::printf("reorder exhaustive 2^12 x 2^12: %llu values hash %016llx\n",
              static_cast<unsigned long long>(all.count),
              static_cast<unsigned long long>(all.h));

  const std::array<mask_t, 14> edges{
      0U,          1U,          2U,          3U,          0x80000000U,
      0xC0000000U, 0x80000001U, 0x7FFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFEU,
      0xAAAAAAAAU, 0x55555555U, 0x00010000U, 0x0000FFFFU};
  for (const mask_t a : edges) {
    for (const mask_t b : edges) {
      const int s = core::reorder_sign(a, b);
      wrong += s != direct_reorder(a, b) ? 1U : 0U;
      std::printf("reorder %08x %08x %+d\n", a, b, s);
    }
  }

  rng r{0x9E3779B97F4A7C15ULL};
  for (std::size_t n = 13; n <= 32; ++n) {
    hasher h;
    for (int k = 0; k < 100000; ++k) {
      mask_t a = r.mask() & low(n);
      mask_t b = r.mask() & low(n);
      if (k % 4 == 0) {
        a |= mask_t{1} << (n - 1);
      }
      if (k % 7 == 0) {
        b |= mask_t{1} << (n - 1);
      }
      const int s = core::reorder_sign(a, b);
      h.add(static_cast<std::uint64_t>(s + 1));
      wrong += s != direct_reorder(a, b) ? 1U : 0U;
    }
    std::printf("reorder random n=%zu: %llu hash %016llx\n", n,
                static_cast<unsigned long long>(h.count),
                static_cast<unsigned long long>(h.h));
  }
  std::printf("reorder disagreements with the direct count: %llu\n",
              static_cast<unsigned long long>(wrong));
}

void complement_section() {
  std::uint64_t wrong = 0;
  rng r{12345};
  for (std::size_t n = 1; n <= 32; ++n) {
    hasher h;
    auto one = [&](mask_t a) {
      const mask_t c = core::complement_mask(a, n);
      const int right = core::right_complement_sign(a, n);
      const int left = core::left_complement_sign(a, n);
      h.add(c);
      h.add(static_cast<std::uint64_t>(right + 1));
      h.add(static_cast<std::uint64_t>(left + 1));
      wrong += right != direct_reorder(a, c) ? 1U : 0U;
      wrong += left != direct_reorder(c, a) ? 1U : 0U;
    };
    if (n <= 14) {
      for (mask_t a = 0; a <= low(n); ++a) {
        one(a);
        if (a == low(n)) {
          break;
        }
      }
    } else {
      one(0);
      one(low(n));
      one(mask_t{1} << (n - 1));
      one(low(n) & ~(mask_t{1} << (n - 1)));
      for (int k = 0; k < 20000; ++k) {
        one(r.mask() & low(n));
      }
    }
    std::printf("complement n=%zu: %llu hash %016llx\n", n,
                static_cast<unsigned long long>(h.count),
                static_cast<unsigned long long>(h.h));
  }
  std::printf("complement disagreements with the direct count: %llu\n",
              static_cast<unsigned long long>(wrong));
}

template <std::size_t N>
using form = core::metric<N>;

template <std::size_t N>
form<N> diagonal(std::size_t p, std::size_t q) {
  form<N> g{};
  for (std::size_t i = 0; i < N; ++i) {
    g.gram[i][i] = i < p ? 1 : (i < p + q ? -1 : 0);
  }
  return g;
}

template <std::size_t N>
form<N> random_form(std::uint64_t seed, int density) {
  rng r{seed};
  form<N> g{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j <= i; ++j) {
      int v = 0;
      if (static_cast<int>(r.next() % 8) < density) {
        v = static_cast<int>(r.next() % 7) - 3;
      }
      g.gram[i][j] = v;
      g.gram[j][i] = v;
    }
  }
  return g;
}

template <std::size_t N>
void dump_all_pairs(const char* name, const form<N>& g) {
  constexpr std::size_t capacity = std::size_t{1} << (N - 1);
  hasher h;
  std::uint64_t terms = 0;
  for (mask_t a = 0; a <= low(N); ++a) {
    for (mask_t b = 0; b <= low(N); ++b) {
      const auto p = core::detail::blade_product<capacity>(g, a, b);
      h.add(p.size());
      for (std::size_t k = 0; k < p.size(); ++k) {
        h.add(p.terms[k].blade);
        h.add(static_cast<std::uint64_t>(p.terms[k].coefficient));
      }
      terms += p.size();
      if (b == low(N)) {
        break;
      }
    }
    if (a == low(N)) {
      break;
    }
  }
  std::printf("products %s n=%zu: %llu terms hash %016llx\n", name, N,
              static_cast<unsigned long long>(terms),
              static_cast<unsigned long long>(h.h));
}

template <std::size_t N>
void forms_of() {
  char name[64];
  for (std::size_t p = 0; p <= N; ++p) {
    for (std::size_t q = 0; p + q <= N; ++q) {
      std::snprintf(name, sizeof name, "R(%zu,%zu,%zu)", p, q, N - p - q);
      dump_all_pairs<N>(name, diagonal<N>(p, q));
    }
  }
  if constexpr (N >= 2) {
    form<N> witt{};
    witt.gram[0][N - 1] = 1;
    witt.gram[N - 1][0] = 1;
    dump_all_pairs<N>("witt e1 e_n", witt);
    form<N> null_pair = diagonal<N>(N - 2, 0);
    null_pair.gram[N - 2][N - 1] = -1;
    null_pair.gram[N - 1][N - 2] = -1;
    dump_all_pairs<N>("null pair last", null_pair);
  }
  form<N> dense{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      dense.gram[i][j] = i == j ? 2 : 1;
    }
  }
  dump_all_pairs<N>("dense", dense);
  for (std::uint64_t s = 1; s <= 3; ++s) {
    std::snprintf(name, sizeof name, "random seed %llu",
                  static_cast<unsigned long long>(s));
    dump_all_pairs<N>(name, random_form<N>(s * 7919U + N, 3));
  }
}

template <std::size_t... D>
void conformal_forms(std::index_sequence<D...>) {
  (dump_all_pairs<D + 3>("conformal", core::conformal<D + 1>()), ...);
}

// Products of random masks of small grade at large n, with e_n often set.
template <std::size_t N>
void random_products() {
  constexpr std::size_t capacity = 1024;
  hasher h;
  for (std::uint64_t s = 1; s <= 3; ++s) {
    form<N> g = random_form<N>(s * 104729U + N, s == 1 ? 1 : 2);
    if (s == 3) {
      g.gram[0][N - 1] = 2;
      g.gram[N - 1][0] = 2;
    }
    rng r{s + 1000U * N};
    for (int k = 0; k < 4000; ++k) {
      mask_t a = 0;
      mask_t b = 0;
      const auto ga = r.next() % 6;
      const auto gb = r.next() % 6;
      for (std::uint64_t t = 0; t < ga; ++t) {
        a |= mask_t{1} << (r.next() % N);
      }
      for (std::uint64_t t = 0; t < gb; ++t) {
        b |= mask_t{1} << (r.next() % N);
      }
      if (k % 3 == 0) {
        a |= mask_t{1} << (N - 1);
      }
      if (k % 5 == 0) {
        b |= mask_t{1} << (N - 1);
      }
      const auto p = core::detail::blade_product<capacity>(g, a, b);
      h.add(p.size());
      for (std::size_t m = 0; m < p.size(); ++m) {
        h.add(p.terms[m].blade);
        h.add(static_cast<std::uint64_t>(p.terms[m].coefficient));
      }
    }
  }
  std::printf("random products n=%zu: %llu values hash %016llx\n", N,
              static_cast<unsigned long long>(h.count),
              static_cast<unsigned long long>(h.h));
}

template <std::size_t... I>
void random_products_all(std::index_sequence<I...>) {
  (random_products<I + 9>(), ...);
}

// Run-time plans: every blade of e_1 to e_n with seeded signs, times itself.
template <std::size_t N>
void runtime_plans(const char* name, const form<N>& g) {
  std::vector<core::oriented_blade> set;
  rng r{N * 31U + 7U};
  for (mask_t m = 0; m <= low(N); ++m) {
    set.push_back({m, (r.next() & 1U) != 0 ? 1 : -1});
  }
  constexpr std::size_t capacity = std::size_t{1} << (N - 1);
  const std::size_t count = core::detail::generate_gp<capacity>(
      g, set, set, std::span<core::detail::plan_term>{});
  std::vector<core::detail::plan_term> plan(count);
  core::detail::generate_gp<capacity>(g, set, set, plan);
  hasher h;
  for (const auto& t : plan) {
    h.add(t.a);
    h.add(t.b);
    h.add(t.out);
    h.add(static_cast<std::uint64_t>(t.coefficient));
  }
  std::printf("runtime plan %s n=%zu: %zu terms hash %016llx\n", name, N,
              count, static_cast<unsigned long long>(h.h));
}

// Compile-time values.
struct witt32 {
  static constexpr auto value = [] {
    core::metric<32> g{};
    g.gram.at(0).at(31) = 1;
    g.gram.at(31).at(0) = 1;
    g.gram.at(30).at(31) = -1;
    g.gram.at(31).at(30) = -1;
    g.gram.at(5).at(5) = 1;
    return g;
  }();
};

constexpr mask_t top = mask_t{1} << 31U;
constexpr std::array<std::pair<mask_t, mask_t>, 8> witt_pairs{{
    {1U, top},
    {top, 1U | top},
    {1U | top, top},
    {1U | top, 1U | top},
    {top | (mask_t{1} << 30U), top},
    {top, top | (mask_t{1} << 30U) | 1U},
    {(mask_t{1} << 30U) | 0x21U, top | 0x20U},
    {top | 0x21U, (mask_t{1} << 30U) | 0x20U | 1U},
}};

constexpr auto witt_products = [] {
  std::array<core::detail::term_list<16>, witt_pairs.size()> out{};
  for (std::size_t k = 0; k < witt_pairs.size(); ++k) {
    out[k] = core::detail::blade_product<16>(
        witt32::value, witt_pairs[k].first, witt_pairs[k].second);
  }
  return out;
}();

constexpr auto reorder_table = [] {
  std::array<int, 64 * 64 + 9> out{};
  for (mask_t a = 0; a < 64; ++a) {
    for (mask_t b = 0; b < 64; ++b) {
      out[a * 64 + b] = core::reorder_sign(a, b);
    }
  }
  out[4096] = core::reorder_sign(0xFFFFFFFFU, 0xFFFFFFFFU);
  out[4097] = core::reorder_sign(top, 0x7FFFFFFFU);
  out[4098] = core::reorder_sign(0x7FFFFFFFU, top);
  out[4099] = core::reorder_sign(top, top);
  out[4100] = core::reorder_sign(0xAAAAAAAAU, 0x55555555U);
  out[4101] = core::reorder_sign(0x55555555U, 0xAAAAAAAAU);
  out[4102] = core::right_complement_sign(0x5U, 32);
  out[4103] = core::left_complement_sign(0x5U | top, 32);
  out[4104] = core::left_complement_sign(0xFFFFU, 17);
  return out;
}();

struct rigid16 {
  static constexpr auto value = core::R<15, 0, 1>();
};
struct conformal3 {
  static constexpr auto value = core::conformal<3>();
};
struct dense4 {
  static constexpr auto value = core::from_gram(
      {{2, 1, 1, 1}, {1, 2, 1, 1}, {1, 1, 2, 1}, {1, 1, 1, 2}});
};
struct degenerate3 {
  static constexpr auto value =
      core::from_gram({{0, 1, 0}, {1, 0, 0}, {0, 0, 0}});
};

constexpr core::blade_set<4> point16{{{{1U, 1}, {2U, 1}, {4U, 1}, {0x8000U, 1}}}};
constexpr core::blade_set<8> motor16{{{{0U, 1},
                                        {0x8001U, -1},
                                        {0x8002U, -1},
                                        {0x8004U, -1},
                                        {6U, 1},
                                        {5U, -1},
                                        {3U, 1},
                                        {0x8007U, 1}}}};

template <const auto& Plan>
void print_plan(const char* name) {
  hasher h;
  for (const auto& t : Plan) {
    h.add(t.a);
    h.add(t.b);
    h.add(t.out);
    h.add(static_cast<std::uint64_t>(t.coefficient));
  }
  std::printf("compile-time plan %s: %zu terms hash %016llx\n", name,
              Plan.size(), static_cast<unsigned long long>(h.h));
}

void compile_time_section() {
  for (std::size_t k = 0; k < witt_products.size(); ++k) {
    std::printf("constexpr witt32 %08x %08x:", witt_pairs[k].first,
                witt_pairs[k].second);
    for (std::size_t m = 0; m < witt_products[k].size(); ++m) {
      std::printf(" %+lld e%08x",
                  static_cast<long long>(witt_products[k].terms[m].coefficient),
                  witt_products[k].terms[m].blade);
    }
    std::printf("\n");
  }
  hasher h;
  for (const int s : reorder_table) {
    h.add(static_cast<std::uint64_t>(s + 1));
  }
  std::printf("constexpr reorder table hash %016llx, edges",
              static_cast<unsigned long long>(h.h));
  for (std::size_t k = 4096; k < reorder_table.size(); ++k) {
    std::printf(" %+d", reorder_table[k]);
  }
  std::printf("\n");
  print_plan<core::detail::gp_plan<rigid16, point16, motor16>>(
      "rigid16 point x motor");
  print_plan<core::detail::gp_plan<rigid16, motor16, motor16>>(
      "rigid16 motor x motor");
  print_plan<core::detail::gp_plan<conformal3, core::full<5>, core::full<5>>>(
      "conformal3 full x full");
  print_plan<core::detail::gp_plan<dense4, core::full<4>, core::full<4>>>(
      "dense4 full x full");
  print_plan<
      core::detail::gp_plan<degenerate3, core::full<3>, core::full<3>>>(
      "degenerate3 full x full");
}

} // namespace

int main() {
  compile_time_section();
  reorder_section();
  complement_section();
  forms_of<1>();
  forms_of<2>();
  forms_of<3>();
  forms_of<4>();
  forms_of<5>();
  forms_of<6>();
  forms_of<7>();
  forms_of<8>();
  conformal_forms(std::make_index_sequence<6>{});
  random_products_all(std::make_index_sequence<24>{});
  runtime_plans<4>("R(3,0,1)", diagonal<4>(3, 0));
  runtime_plans<5>("conformal(3)", core::conformal<3>());
  runtime_plans<6>("random", random_form<6>(99, 4));
  runtime_plans<6>("R(5,0,1)", diagonal<6>(5, 0));
  return 0;
}
