// Prints what galoot's change of basis computes, for a black-box comparison
// with another implementation: for each change, the two Gram matrices, the
// rows and denominators of P, the inverse the library computes, and the image
// of every blade of the source under change_basis and of every blade of the
// target under change_basis_back.
//
// Two routes. "public" maps each unit blade of full<n> over the test rational
// through change_basis and change_basis_back, exactly as a user would.
// "terms" reads the per-blade images the library generates
// (detail::blade_image, over the product of the blade's row denominators) and
// the inverse it computes (detail::inverse_change), at run time, for every
// form of the exhaustive tests against its diagonal counterpart; that is
// what the structure-layer identity test reads.
//
// Build against a checkout's headers (include/ and test/), C++23, once for
// each route: the public route by default, the terms route with
// -DDUMP_TERMS. The terms route runs the test diagonalization at run time,
// and the public route evaluates it in constant expressions, so they are two
// programs: GCC 16.2.1 miscompiles the run-time call of the diagonalization
// in a translation unit that also evaluates it in a constant expression (see
// gcc_constexpr_init_repro.cpp beside this file). The output is plain text,
// one line per item; masks are decimal, bit i for e(i+1).
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <type_traits>

#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

#include <core/basis_change_forms.hpp>
#include <core/signatures.hpp>
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
namespace bc = galoot_test::basis_changes;
using test_support::rational;

std::string text(const rational& r) {
  return std::to_string(r.numerator) + "/" + std::to_string(r.denominator);
}

template <std::size_t N>
void print_gram(const char* label, const core::metric<N>& g) {
  std::printf("%s", label);
  for (std::size_t i = 0; i < N; ++i) {
    std::printf(i == 0 ? " " : " ; ");
    for (std::size_t j = 0; j < N; ++j) {
      std::printf("%s%lld", j == 0 ? "" : " ",
                  static_cast<long long>(g.gram[i][j]));
    }
  }
  std::printf("\n");
}

template <class Rows>
void print_rows(const char* label, const Rows& r, std::size_t n) {
  std::printf("%s", label);
  for (std::size_t i = 0; i < n; ++i) {
    std::printf(i == 0 ? " " : " ; ");
    for (std::size_t j = 0; j < n; ++j) {
      std::printf("%s%lld", j == 0 ? "" : " ",
                  static_cast<long long>(r.rows[i][j]));
    }
  }
  std::printf("\ndenominators_%s", label);
  for (std::size_t i = 0; i < n; ++i) {
    std::printf(" %lld", static_cast<long long>(r.denominators[i]));
  }
  std::printf("\n");
}

#ifndef DUMP_TERMS

// The coefficient of x on the canonical blade of this mask.
template <class X>
rational value_on(const X& x, core::mask_t mask) {
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const core::oriented_blade blade = X::layout.blades[k];
    if (blade.mask == mask) {
      return rational{blade.sign} * x.coefficients[k];
    }
  }
  return rational{};
}

// The multivector of type X that is the canonical blade of this mask.
template <class X>
X unit(core::mask_t mask) {
  X x{};
  for (std::size_t k = 0; k < X::component_count; ++k) {
    if (X::layout.blades[k].mask == mask) {
      x.coefficients[k] = rational{X::layout.blades[k].sign};
    }
  }
  return x;
}

template <class Y>
void print_image(const char* label, core::mask_t a, const Y& y, std::size_t n) {
  std::printf("%s %u:", label, static_cast<unsigned>(a));
  for (core::mask_t b = 0; b < (core::mask_t{1} << n); ++b) {
    const rational v = value_on(y, b);
    if (v != rational{}) {
      std::printf(" %u %s", static_cast<unsigned>(b), text(v).c_str());
    }
  }
  std::printf("\n");
}

// The change C through change_basis and change_basis_back on full<n>.
template <class C>
void dump_public(const char* name) {
  constexpr std::size_t n = C::value.generator_count;
  using X = core::mv<typename C::source, core::full<n>, rational>;
  using Y = core::mv<typename C::target, core::full<n>, rational>;
  std::printf("change %s\nn %zu\nroute public\n", name, n);
  print_gram("source_gram", C::source::value);
  print_gram("target_gram", C::target::value);
  print_rows("rows", C::value, n);
  print_rows("inverse", core::detail::inverse_change(C::value), n);
  for (core::mask_t a = 0; a < (core::mask_t{1} << n); ++a) {
    print_image("forward", a, core::change_basis<C>(unit<X>(a)), n);
  }
  for (core::mask_t b = 0; b < (core::mask_t{1} << n); ++b) {
    print_image("back", b, core::change_basis_back<C>(unit<Y>(b)), n);
  }
  std::printf("end\n");
}

#else

// The images the library generates for the rows of a change or of its
// inverse, each over the product of its blade's row denominators.
template <std::size_t N, class Rows>
void print_term_images(const char* label, const Rows& rows) {
  constexpr std::size_t capacity = 32;
  for (core::mask_t a = 0; a < (core::mask_t{1} << N); ++a) {
    std::int64_t denominator = 1;
    for (std::size_t i = 0; i < N; ++i) {
      if (((a >> i) & 1U) != 0) {
        denominator *= rows.denominators[i];
      }
    }
    const auto image = core::detail::blade_image<capacity>(rows, a);
    std::printf("%s %u:", label, static_cast<unsigned>(a));
    for (std::size_t t = 0; t < image.size(); ++t) {
      const rational v{image.terms[t].coefficient, denominator};
      if (v != rational{}) {
        std::printf(" %u %s", static_cast<unsigned>(image.terms[t].blade),
                    text(v).c_str());
      }
    }
    std::printf("\n");
  }
}

template <std::size_t N>
void dump_terms(const char* name, const core::metric<N>& g) {
  const auto d = bc::to_diagonal(g);
  const auto inverse = core::detail::inverse_change(d.change);
  std::printf("change %s\nn %zu\nroute terms\n", name, N);
  print_gram("source_gram", g);
  print_gram("target_gram", d.target);
  print_rows("rows", d.change, N);
  print_rows("inverse", inverse, N);
  print_term_images<N>("forward", d.change);
  print_term_images<N>("back", inverse);
  std::printf("end\n");
}

#endif

#ifndef DUMP_TERMS

// The forms of the exhaustive tests that the public route maps, as tags.
using degenerate = bc::to_diagonal_tag<core::from_gram({
    {0, 1, 0},
    {1, 0, 0},
    {0, 0, 0},
})>;
using rigid_null_first = bc::to_diagonal_tag<core::from_gram({
    {0, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
})>;
using random_0 = bc::to_diagonal_tag<core::from_gram({
    {-1, -2},
    {-2, 3},
})>;
using random_1 = bc::to_diagonal_tag<core::from_gram({
    {2, 0, -3},
    {0, 1, -3},
    {-3, -3, -2},
})>;
using random_2 = bc::to_diagonal_tag<core::from_gram({
    {-2, 0, -2, 2},
    {0, -2, -3, 3},
    {-2, -3, 1, 3},
    {2, 3, 3, 3},
})>;
using random_3 = bc::to_diagonal_tag<core::from_gram({
    {3, 1, -3, -1, -2},
    {1, -3, -2, 1, 0},
    {-3, -2, -2, 3, 0},
    {-1, 1, 3, 3, 2},
    {-2, 0, 0, 2, -1},
})>;
using random_4 = bc::to_diagonal_tag<core::from_gram({
    {3, 1},
    {1, -2},
})>;
using random_5 = bc::to_diagonal_tag<core::from_gram({
    {-3, -3, 0},
    {-3, -2, 2},
    {0, 2, 0},
})>;
using random_6 = bc::to_diagonal_tag<core::from_gram({
    {0, 1, -1, 0},
    {1, -1, 0, -1},
    {-1, 0, 2, 0},
    {0, -1, 0, -1},
})>;
using random_7 = bc::to_diagonal_tag<core::from_gram({
    {-3, 1, 2, -1, -2},
    {1, -3, 0, -3, -1},
    {2, 0, 0, 3, -3},
    {-1, -3, 3, 1, -2},
    {-2, -1, -3, -2, 2},
})>;
using random_8 = bc::to_diagonal_tag<core::from_gram({
    {3, -2},
    {-2, -2},
})>;
using random_9 = bc::to_diagonal_tag<core::from_gram({
    {-3, 1, -2},
    {1, -3, -2},
    {-2, -2, 1},
})>;

// Whether the source form of the tag C is the form the exhaustive tests list
// under this name, so that the tags above restate that list faithfully.
template <class C>
bool listed_as(const char* name) {
  bool found = false;
  bool same = false;
  galoot_test::for_each_signature([&](const char* listed, const auto& g) {
    if (std::string{listed} != name) {
      return;
    }
    found = true;
    if constexpr (std::remove_cvref_t<decltype(g)>::generator_count ==
                  C::value.generator_count) {
      same = g.gram == C::source::value.gram;
    }
  });
  return found && same;
}

template <class C>
void dump_listed(const char* name) {
  std::printf("listed %s %s\n", name,
              listed_as<C>(name) ? "same" : "DIFFERENT");
  dump_public<C>(name);
}

#endif

} // namespace

int main() {
#ifndef DUMP_TERMS
  dump_public<bc::conformal_tag<1>>("conformal(1)");
  dump_public<bc::conformal_tag<2>>("conformal(2)");
  dump_public<bc::conformal_tag<3>>("conformal(3)");
  dump_public<bc::conformal_tag<4>>("conformal(4)");
  dump_public<bc::witt_one_tag>("Witt, one mode");
  dump_public<bc::witt_two_tag>("Witt, two modes");
  dump_public<bc::doubled_null_tag>("rigid(3), e4 doubled");
  dump_listed<degenerate>("degenerate");
  dump_listed<rigid_null_first>("rigid(3), null first");
  dump_listed<random_0>("random 0");
  dump_listed<random_1>("random 1");
  dump_listed<random_2>("random 2");
  dump_listed<random_3>("random 3");
  dump_listed<random_4>("random 4");
  dump_listed<random_5>("random 5");
  dump_listed<random_6>("random 6");
  dump_listed<random_7>("random 7");
  dump_listed<random_8>("random 8");
  dump_listed<random_9>("random 9");
#else
  galoot_test::for_each_signature(
      [](const char* name, const auto& g) { dump_terms(name, g); });
#endif
  return 0;
}
