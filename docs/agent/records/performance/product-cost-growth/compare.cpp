// Compares the core headers under test (namespace galoot) with a renamed copy
// of an older revision (namespace galoot_old), value by value: reorder_sign
// and the complement signs, blade_product, and generate_gp at run time, and
// gp_plan in constant evaluations. The products are compared over every pair
// of masks of each listed form of up to eight generators, and over seeded
// random forms and masks of 9 to 32 generators. Prints the number of
// comparisons and mismatches of each group, and the first mismatches of each,
// and exits 1 if any comparison differs.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <type_traits>
#include <utility>
#include <vector>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>
#include <galoot_old/core/bits.hpp>
#include <galoot_old/core/blade_product.hpp>
#include <galoot_old/core/blade_set.hpp>
#include <galoot_old/core/metric.hpp>
#include <galoot_old/core/plan.hpp>

namespace {

namespace now = galoot::core;
namespace old = galoot_old::core;
using mask_t = now::mask_t;
static_assert(std::is_same_v<mask_t, old::mask_t>);
static_assert(now::mask_bits == 32 && old::mask_bits == 32);

constexpr std::uint64_t seed = 0x5eed0012;

// SplitMix64 (Steele, Lea, and Flood, "Fast splittable pseudorandom number
// generators", OOPSLA 2014), written here so that the inputs do not depend
// on a standard library's distributions.
struct random_bits {
  std::uint64_t state;

  std::uint64_t next() {
    state += 0x9e3779b97f4a7c15U;
    std::uint64_t z = state;
    z = (z ^ (z >> 30U)) * 0xbf58476d1ce4e5b9U;
    z = (z ^ (z >> 27U)) * 0x94d049bb133111ebU;
    return z ^ (z >> 31U);
  }

  // Uniform enough for the small bounds used here.
  std::size_t below(std::size_t bound) {
    return static_cast<std::size_t>(next() % bound);
  }
};

struct tally {
  const char* group;
  std::uint64_t compared = 0;
  std::uint64_t mismatched = 0;
};

std::uint64_t total_mismatched = 0;
constexpr std::uint64_t shown_per_group = 10;

void note(tally& t, bool equal, const char* form, std::size_t n, mask_t a,
          mask_t b) {
  ++t.compared;
  if (equal) {
    return;
  }
  ++t.mismatched;
  if (t.mismatched <= shown_per_group) {
    std::printf("MISMATCH\t%s\t%s\tn=%zu\ta=%u\tb=%u\n", t.group, form, n, a,
                b);
  }
}

void report(const tally& t) {
  std::printf("%s\t%llu compared\t%llu mismatched\n", t.group,
              static_cast<unsigned long long>(t.compared),
              static_cast<unsigned long long>(t.mismatched));
  // A failed assertion aborts, which would lose buffered lines.
  std::fflush(stdout);
  total_mismatched += t.mismatched;
}

constexpr mask_t low_bits(std::size_t n) {
  return n == 0 ? 0 : ~mask_t{0} >> (32 - n);
}

// A mask of n generators with at most max_grade of them, holding e_n a
// quarter of the time, so that the top bit of the widest masks is visited.
mask_t random_mask(random_bits& r, std::size_t n, std::size_t max_grade) {
  const std::size_t grade = r.below(max_grade + 1);
  mask_t m = 0;
  for (std::size_t k = 0; k < grade; ++k) {
    m |= mask_t{1} << r.below(n);
  }
  if (r.below(4) == 0) {
    m |= mask_t{1} << (n - 1);
  }
  return m;
}

// Signs.

void compare_signs(random_bits& r) {
  constexpr std::size_t exhaustive = 12;
  tally reorder{"reorder_sign, every pair below 2^12, then random to n = 32"};
  for (mask_t a = 0; a < (mask_t{1} << exhaustive); ++a) {
    for (mask_t b = 0; b < (mask_t{1} << exhaustive); ++b) {
      note(reorder, now::reorder_sign(a, b) == old::reorder_sign(a, b),
           "masks", exhaustive, a, b);
    }
  }
  for (std::size_t n = exhaustive + 1; n <= 32; ++n) {
    for (int k = 0; k < 200000; ++k) {
      const auto a = static_cast<mask_t>(r.next()) & low_bits(n);
      const auto b = static_cast<mask_t>(r.next()) & low_bits(n);
      note(reorder, now::reorder_sign(a, b) == old::reorder_sign(a, b),
           "masks", n, a, b);
    }
    note(reorder,
         now::reorder_sign(low_bits(n), low_bits(n)) ==
             old::reorder_sign(low_bits(n), low_bits(n)),
         "antiscalar", n, low_bits(n), low_bits(n));
  }
  report(reorder);

  tally complements{"complement signs, every blade to n = 12, then random"};
  for (std::size_t n = 1; n <= 32; ++n) {
    const std::size_t count = n <= exhaustive ? std::size_t{1} << n : 100000;
    for (std::size_t k = 0; k < count; ++k) {
      const mask_t a = n <= exhaustive
                           ? static_cast<mask_t>(k)
                           : static_cast<mask_t>(r.next()) & low_bits(n);
      note(complements,
           now::right_complement_sign(a, n) ==
                   old::right_complement_sign(a, n) &&
               now::left_complement_sign(a, n) ==
                   old::left_complement_sign(a, n),
           "blade", n, a, 0);
    }
  }
  report(complements);
}

// Products.

template <std::size_t N>
using gram_t = std::array<std::array<int, N>, N>;

template <std::size_t N> struct form_pair {
  now::metric<N> current;
  old::metric<N> previous;
};

template <std::size_t N> form_pair<N> both(const gram_t<N>& g) {
  form_pair<N> f{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      f.current.gram[i][j] = g[i][j];
      f.previous.gram[i][j] = g[i][j];
    }
  }
  return f;
}

template <std::size_t Capacity>
bool same(const now::detail::term_list<Capacity>& x,
          const old::detail::term_list<Capacity>& y) {
  if (x.length != y.length) {
    return false;
  }
  for (std::size_t k = 0; k < Capacity; ++k) {
    if (x.terms[k].blade != y.terms[k].blade ||
        x.terms[k].coefficient != y.terms[k].coefficient) {
      return false;
    }
  }
  return true;
}

bool same_plan(const std::vector<now::detail::plan_term>& x,
               const std::vector<old::detail::plan_term>& y) {
  if (x.size() != y.size()) {
    return false;
  }
  for (std::size_t k = 0; k < x.size(); ++k) {
    if (x[k].a != y[k].a || x[k].b != y[k].b || x[k].out != y[k].out ||
        x[k].coefficient != y[k].coefficient) {
      return false;
    }
  }
  return true;
}

// The plans of the two headers for the same sets, compared term by term.
template <std::size_t Capacity, std::size_t N>
bool same_generated_plan(const form_pair<N>& f,
                         const std::vector<now::oriented_blade>& as,
                         const std::vector<now::oriented_blade>& bs) {
  std::vector<old::oriented_blade> old_as;
  std::vector<old::oriented_blade> old_bs;
  for (const auto& x : as) {
    old_as.push_back({.mask = x.mask, .sign = x.sign});
  }
  for (const auto& x : bs) {
    old_bs.push_back({.mask = x.mask, .sign = x.sign});
  }
  const std::size_t count =
      now::detail::generate_gp<Capacity>(f.current, as, bs, {});
  const std::size_t old_count =
      old::detail::generate_gp<Capacity>(f.previous, old_as, old_bs, {});
  if (count != old_count) {
    return false;
  }
  std::vector<now::detail::plan_term> plan(count);
  std::vector<old::detail::plan_term> old_plan(old_count);
  now::detail::generate_gp<Capacity>(f.current, as, bs, plan);
  old::detail::generate_gp<Capacity>(f.previous, old_as, old_bs, old_plan);
  return same_plan(plan, old_plan);
}

// Every pair of masks of the form, and for n <= 5 the plan of every blade
// times every blade with +1 signs and with -1 on the blades of odd mask.
template <std::size_t N>
void compare_every_pair(tally& products, tally& plans, const char* name,
                        const gram_t<N>& g) {
  constexpr std::size_t capacity = std::size_t{1} << (N - 1);
  const form_pair<N> f = both(g);
  constexpr mask_t count = mask_t{1} << N;
  for (mask_t a = 0; a < count; ++a) {
    for (mask_t b = 0; b < count; ++b) {
      note(products,
           same(now::detail::blade_product<capacity>(f.current, a, b),
                old::detail::blade_product<capacity>(f.previous, a, b)),
           name, N, a, b);
    }
  }
  if constexpr (N <= 5) {
    std::vector<now::oriented_blade> plus;
    std::vector<now::oriented_blade> mixed;
    for (mask_t a = 0; a < count; ++a) {
      plus.push_back({.mask = a, .sign = 1});
      mixed.push_back({.mask = a, .sign = a % 2 == 0 ? 1 : -1});
    }
    note(plans, same_generated_plan<capacity>(f, plus, plus), name, N, 0, 0);
    note(plans, same_generated_plan<capacity>(f, mixed, plus), name, N, 1, 0);
    note(plans, same_generated_plan<capacity>(f, plus, mixed), name, N, 0, 1);
  }
}

template <std::size_t N> gram_t<N> diagonal(std::size_t p, std::size_t q) {
  gram_t<N> g{};
  for (std::size_t i = 0; i < N; ++i) {
    g[i][i] = i < p ? 1 : (i < p + q ? -1 : 0);
  }
  return g;
}

template <std::size_t N> void compare_forms_of(tally& products, tally& plans,
                                                 random_bits& r) {
  // Every diagonal signature R(p, q, r) with p + q + r = N.
  for (std::size_t p = 0; p <= N; ++p) {
    for (std::size_t q = 0; p + q <= N; ++q) {
      compare_every_pair<N>(products, plans, "diagonal", diagonal<N>(p, q));
    }
  }
  if constexpr (N >= 2) {
    // conformal(N - 2): the last two generators null with pairing -1.
    gram_t<N> c = diagonal<N>(N - 2, 0);
    c[N - 2][N - 1] = -1;
    c[N - 1][N - 2] = -1;
    compare_every_pair<N>(products, plans, "conformal", c);

    // A Witt pair on the lowest and highest generators, the rest
    // Euclidean, so that the pairing is read at both ends of the mask.
    gram_t<N> w = diagonal<N>(N, 0);
    w[0][0] = 0;
    w[N - 1][N - 1] = 0;
    w[0][N - 1] = 1;
    w[N - 1][0] = 1;
    compare_every_pair<N>(products, plans, "witt-ends", w);
  }
  // Every entry nonzero: 2 on the diagonal and 1 elsewhere.
  gram_t<N> dense{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      dense[i][j] = i == j ? 2 : 1;
    }
  }
  compare_every_pair<N>(products, plans, "dense", dense);
  // Seeded random symmetric forms with entries from -2 to 2.
  for (int k = 0; k < 3; ++k) {
    gram_t<N> g{};
    for (std::size_t i = 0; i < N; ++i) {
      for (std::size_t j = i; j < N; ++j) {
        g[i][j] = static_cast<int>(r.below(5)) - 2;
        g[j][i] = g[i][j];
      }
    }
    compare_every_pair<N>(products, plans, "random", g);
  }
}

// Random forms of N generators: diagonal with squares from -1 to 1, sparse
// with an eighth of the pairings nonzero, and every entry from -2 to 2. Each
// mask has at most six generators, five and e_n, so a list of 2^11 terms
// holds every product of two.
template <std::size_t N> void compare_random(tally& products, tally& plans,
                                              random_bits& r) {
  constexpr std::size_t capacity = 2048;
  constexpr std::size_t max_grade = 5;
  const char* names[] = {"random-diagonal", "random-sparse", "random-dense"};
  for (std::size_t family = 0; family < 3; ++family) {
    gram_t<N> g{};
    for (std::size_t i = 0; i < N; ++i) {
      for (std::size_t j = i; j < N; ++j) {
        int entry = 0;
        if (i == j) {
          entry = static_cast<int>(r.below(3)) - 1;
        } else if (family == 1 && r.below(8) == 0) {
          entry = static_cast<int>(r.below(5)) - 2;
        } else if (family == 2) {
          entry = static_cast<int>(r.below(5)) - 2;
        }
        g[i][j] = entry;
        g[j][i] = entry;
      }
    }
    const form_pair<N> f = both(g);
    for (int k = 0; k < 3000; ++k) {
      const mask_t a = random_mask(r, N, max_grade);
      const mask_t b = random_mask(r, N, max_grade);
      note(products,
           same(now::detail::blade_product<capacity>(f.current, a, b),
                old::detail::blade_product<capacity>(f.previous, a, b)),
           names[family], N, a, b);
    }
    for (int k = 0; k < 40; ++k) {
      std::vector<now::oriented_blade> as(r.below(7));
      std::vector<now::oriented_blade> bs(r.below(7));
      for (auto& x : as) {
        x = {.mask = random_mask(r, N, max_grade),
             .sign = r.below(2) == 0 ? 1 : -1};
      }
      for (auto& x : bs) {
        x = {.mask = random_mask(r, N, max_grade),
             .sign = r.below(2) == 0 ? 1 : -1};
      }
      note(plans, same_generated_plan<capacity>(f, as, bs), names[family], N,
           0, 0);
    }
  }
}

// Compile-time plans: the four typed rigid(3) pairs embedded in R(n-1,0,1),
// e4 becoming e_n, as the growth measurement uses them.
template <std::size_t N> struct rigid_current {
  static constexpr auto value = now::R<N - 1, 0, 1>();
};
template <std::size_t N> struct rigid_previous {
  static constexpr auto value = old::R<N - 1, 0, 1>();
};

// The mask and sign of the blade written with these indices, in this order.
template <class Blade, std::size_t K>
consteval Blade written(const std::array<std::size_t, K>& indices) {
  int sign = 1;
  mask_t mask = 0;
  for (std::size_t p = 0; p < K; ++p) {
    mask |= mask_t{1} << (indices[p] - 1);
    for (std::size_t q = p + 1; q < K; ++q) {
      sign = indices[p] > indices[q] ? -sign : sign;
    }
  }
  return {.mask = mask, .sign = sign};
}

template <template <std::size_t> class Set, class Blade, std::size_t N>
struct typed {
  using one = std::array<std::size_t, 1>;
  using two = std::array<std::size_t, 2>;
  using three = std::array<std::size_t, 3>;
  using four = std::array<std::size_t, 4>;
  static constexpr Set<4> point{{{written<Blade>(one{1}), written<Blade>(one{2}),
                                  written<Blade>(one{3}),
                                  written<Blade>(one{N})}}};
  static constexpr Set<6> line{{{
      written<Blade>(two{N, 1}),
      written<Blade>(two{N, 2}),
      written<Blade>(two{N, 3}),
      written<Blade>(two{2, 3}),
      written<Blade>(two{3, 1}),
      written<Blade>(two{1, 2}),
  }}};
  static constexpr Set<4> plane{{{
      written<Blade>(three{N, 2, 3}),
      written<Blade>(three{N, 3, 1}),
      written<Blade>(three{N, 1, 2}),
      written<Blade>(three{3, 2, 1}),
  }}};
  static constexpr Set<8> motor{{{
      Blade{.mask = 0, .sign = 1},
      written<Blade>(two{N, 1}),
      written<Blade>(two{N, 2}),
      written<Blade>(two{N, 3}),
      written<Blade>(two{2, 3}),
      written<Blade>(two{3, 1}),
      written<Blade>(two{1, 2}),
      written<Blade>(four{1, 2, 3, N}),
  }}};
};

template <class X, class Y> consteval bool same_terms(const X& x, const Y& y) {
  if (x.size() != y.size()) {
    return false;
  }
  for (std::size_t k = 0; k < x.size(); ++k) {
    if (x[k].a != y[k].a || x[k].b != y[k].b || x[k].out != y[k].out ||
        x[k].coefficient != y[k].coefficient) {
      return false;
    }
  }
  return true;
}

template <std::size_t N> consteval bool typed_plans_agree() {
  using c = typed<now::blade_set, now::oriented_blade, N>;
  using p = typed<old::blade_set, old::oriented_blade, N>;
  using ac = rigid_current<N>;
  using ap = rigid_previous<N>;
  using now::detail::gp_plan;
  return same_terms(gp_plan<ac, c::point, c::point>,
                    old::detail::gp_plan<ap, p::point, p::point>) &&
         same_terms(gp_plan<ac, c::line, c::plane>,
                    old::detail::gp_plan<ap, p::line, p::plane>) &&
         same_terms(gp_plan<ac, c::motor, c::point>,
                    old::detail::gp_plan<ap, p::motor, p::point>) &&
         same_terms(gp_plan<ac, c::point, c::motor>,
                    old::detail::gp_plan<ap, p::point, p::motor>) &&
         gp_plan<ac, c::point, c::point>.size() == 15 &&
         gp_plan<ac, c::motor, c::point>.size() == 28;
}

static_assert(typed_plans_agree<4>());
static_assert(typed_plans_agree<8>());
static_assert(typed_plans_agree<12>());
static_assert(typed_plans_agree<16>());

template <std::size_t... I>
void compare_exhaustive(tally& products, tally& plans, random_bits& r,
                        std::index_sequence<I...>) {
  (compare_forms_of<I + 1>(products, plans, r), ...);
}

template <std::size_t... I>
void compare_wide(tally& products, tally& plans, random_bits& r,
                  std::index_sequence<I...>) {
  (compare_random<I + 9>(products, plans, r), ...);
}

} // namespace

int main() {
  std::printf("seed\t%llx\n", static_cast<unsigned long long>(seed));
  random_bits r{seed};
  compare_signs(r);

  tally products{"blade_product, every pair of masks, n = 1 to 8"};
  tally plans{"generate_gp, full sets with two sign patterns, n = 1 to 5"};
  compare_exhaustive(products, plans, r, std::make_index_sequence<8>{});
  report(products);
  report(plans);

  tally wide_products{"blade_product, random forms and masks, n = 9 to 32"};
  tally wide_plans{"generate_gp, random forms and sets, n = 9 to 32"};
  compare_wide(wide_products, wide_plans, r, std::make_index_sequence<24>{});
  report(wide_products);
  report(wide_plans);

  std::printf("gp_plan, typed rigid(3) pairs at n = 4, 8, 12, 16\tequal in "
              "constant evaluations (static_assert)\n");
  std::printf("total mismatched\t%llu\n",
              static_cast<unsigned long long>(total_mismatched));
  return total_mismatched == 0 ? 0 : 1;
}
