#ifndef GALOOT_TEST_CORE_TRANSFORM_IDENTITIES_CHECKS_HPP
#define GALOOT_TEST_CORE_TRANSFORM_IDENTITIES_CHECKS_HPP

// The checks of the identities of galoot/core/transform.hpp that
// transform_identities_test.cpp runs over many signatures, and the groups
// of forms it runs them over.
//
// The test cases, in transform_identities_test.cpp, check every group of
// forms in turn, drawing from one seeded source. Each group is checked in a
// source of its own, transform_identities_<group>_test.cpp, so that the
// groups compile in parallel: at -O0 GCC holds the code of every form until
// the end of its source, so one source for all of them needs the memory of
// all of them at once.

#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <type_traits>
#include <utility>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
// The scaling of a multivector, found by argument-dependent lookup, which the
// include check does not see.
#include <galoot/core/linear.hpp> // IWYU pragma: keep
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>
#include <galoot/core/transform.hpp>
#include <galoot/core/unary.hpp>

#include <doctest/doctest.h>

#include <support/rational.hpp>

namespace galoot_test::transform_identities {

// The checks have internal linkage, as they had when the test was one
// source, so that each source compiles them as that source did.
// NOLINTNEXTLINE(misc-anonymous-namespace-in-header): one copy per source
namespace {

namespace core = galoot::core;
using test_support::rational;

// An algebra whose metric is the value G.
template <auto G>
struct form {
  static constexpr auto value = G;
};

// A list of metrics.
template <auto... G>
struct forms {};

// The number of generators of an algebra.
template <class Alg>
inline constexpr std::size_t generators =
    std::remove_cvref_t<decltype(Alg::value)>::generator_count;

// The Gram matrix of the algebra as text, which names the form in a failure.
template <class Alg>
std::string gram_text() {
  std::string text = "[";
  for (std::size_t i = 0; i < generators<Alg>; ++i) {
    text += i == 0 ? "[" : ", [";
    for (std::size_t j = 0; j < generators<Alg>; ++j) {
      text +=
          (j == 0 ? "" : ", ") + std::to_string(Alg::value.gram.at(i).at(j));
    }
    text += ']';
  }
  return text + "]";
}

// The bound of the entries of the random vectors and multivectors.
inline constexpr std::int64_t entry_bound = 2;

// The bound of the entries of the random hyperplanes. Each side of the
// outermorphism identity at grade 4 for a versor of three hyperplanes is a
// polynomial of degree 24 in the hyperplanes' entries and 4 in the vectors'.
// With hyperplane entries from -2 to 2, over the random form of four
// generators, its terms needed more than the 63 bits a coefficient of the
// test rational holds at about one seed in five, and the test aborted; from
// -1 to 1 they stay well within it.
inline constexpr std::int64_t hyperplane_entry_bound = 1;

// A random multivector of the layout of X, with integer entries from -bound
// to bound.
template <class X>
X random_small(std::mt19937_64& source, std::int64_t bound) {
  std::uniform_int_distribution<std::int64_t> digit(-bound, bound);
  X x{};
  for (rational& c : x.coefficients) {
    c = rational{digit(source)};
  }
  return x;
}

// The antiproduct of Count random hyperplanes of the layout of H, the last
// drawn applied last.
template <class H, std::size_t Count>
auto random_versor(std::mt19937_64& source) {
  if constexpr (Count == 1) {
    return random_small<H>(source, hyperplane_entry_bound);
  } else {
    const auto first = random_versor<H, Count - 1>(source);
    return core::compose(random_small<H>(source, hyperplane_entry_bound),
                         first);
  }
}

// lambda, the coefficient of V ⟇ antireverse(V) on the antiscalar, and
// whether that product has no other part.
template <class V>
std::pair<rational, bool> lambda_of(const V& v) {
  using Alg = V::algebra_type;
  const auto w = core::geometric_antiproduct(v, core::antireverse(v));
  const core::mask_t antiscalar =
      (core::mask_t{1} << generators<Alg>)-core::mask_t{1};
  rational lambda{};
  bool only_antiscalar = true;
  for (std::size_t k = 0; k < w.coefficients.size(); ++k) {
    if (std::remove_cvref_t<decltype(w)>::layout.blades.at(k).mask ==
        antiscalar) {
      lambda =
          rational{std::remove_cvref_t<decltype(w)>::layout.blades.at(k).sign} *
          w.coefficients.at(k);
    } else {
      only_antiscalar = only_antiscalar && w.coefficients.at(k) == rational{};
    }
  }
  return {lambda, only_antiscalar};
}

// lambda^(k-1) T(a1 ^ ... ^ ak) = T(a1) ^ ... ^ T(ak) for random vectors.
template <std::size_t K, class V>
void check_outermorphism_at(std::mt19937_64& source, const V& v,
                            const rational& lambda) {
  using Alg = V::algebra_type;
  using A = core::mv<Alg, core::of_grade<generators<Alg>, 1>, rational>;
  CAPTURE(K);
  std::array<A, K> a{};
  std::array<A, K> moved{};
  for (std::size_t i = 0; i < K; ++i) {
    a.at(i) = random_small<A>(source, entry_bound);
    moved.at(i) = core::transform(a.at(i), v);
  }
  const auto wedge_all = []<std::size_t... I>(const std::array<A, K>& xs,
                                              std::index_sequence<I...>) {
    return (xs[I] ^ ...);
  };
  rational power{1};
  for (std::size_t i = 1; i < K; ++i) {
    power = power * lambda;
  }
  const auto blade = wedge_all(a, std::make_index_sequence<K>{});
  CHECK(core::transform(blade, v) * power ==
        wedge_all(moved, std::make_index_sequence<K>{}));
}

// transform(x, compose(W, V)) = transform(transform(x, V), W) for random x
// of grade K.
template <std::size_t K, class W, class V>
void check_composition_at(std::mt19937_64& source, const W& w, const V& v) {
  using Alg = V::algebra_type;
  using X = core::mv<Alg, core::of_grade<generators<Alg>, K>, rational>;
  CAPTURE(K);
  const X x = random_small<X>(source, entry_bound);
  CHECK(core::transform(x, core::compose(w, v)) ==
        core::transform(core::transform(x, v), w));
}

// A random versor of Count hyperplanes of the layout H with lambda not
// zero, after at most eight draws, or none; each draw checks that
// V ⟇ antireverse(V) is lambda times the antiscalar.
template <class Alg, core::blade_set H, std::size_t Count>
auto versor_with_lambda(std::mt19937_64& source, rational& lambda) {
  using Hyperplane = core::mv<Alg, H, rational>;
  for (int draw = 0; draw < 8; ++draw) {
    const auto v = random_versor<Hyperplane, Count>(source);
    const auto [l, only_antiscalar] = lambda_of(v);
    CHECK(only_antiscalar);
    if (l != rational{}) {
      lambda = l;
      return v;
    }
  }
  lambda = rational{};
  return decltype(random_versor<Hyperplane, Count>(source)){};
}

// The identities for the algebra Alg with hyperplanes of the layout H: the
// outermorphism for versors of one to MaxCount hyperplanes, at most three,
// at grades 2 to MaxGrade, and composition at grades 0 to MaxGrade for pairs
// of versors of one and one hyperplane and, when MaxCount is 3, of one and
// two, two and one, and three and one.
template <class Alg, core::blade_set H, std::size_t MaxGrade,
          std::size_t MaxCount = 3>
void check_identities(std::mt19937_64& source) {
  INFO("form: ", gram_text<Alg>());
  int accepted = 0;
  const auto outermorphism =
      [&]<std::size_t Count>(std::integral_constant<std::size_t, Count>) {
        CAPTURE(Count);
        for (int trial = 0; trial < 2; ++trial) {
          rational lambda{};
          const auto v = versor_with_lambda<Alg, H, Count>(source, lambda);
          if (lambda == rational{}) {
            continue;
          }
          ++accepted;
          [&]<std::size_t... K>(std::index_sequence<K...>) {
            (check_outermorphism_at<K + 2>(source, v, lambda), ...);
          }(std::make_index_sequence<MaxGrade - 1>{});
        }
      };
  outermorphism(std::integral_constant<std::size_t, 1>{});
  outermorphism(std::integral_constant<std::size_t, 2>{});
  if constexpr (MaxCount >= 3) {
    outermorphism(std::integral_constant<std::size_t, 3>{});
  }

  const auto composition = [&]<std::size_t Second, std::size_t First>(
                               std::integral_constant<std::size_t, Second>,
                               std::integral_constant<std::size_t, First>) {
    CAPTURE(Second);
    CAPTURE(First);
    rational lambda_v{};
    rational lambda_w{};
    const auto v = versor_with_lambda<Alg, H, First>(source, lambda_v);
    const auto w = versor_with_lambda<Alg, H, Second>(source, lambda_w);
    if (lambda_v == rational{} || lambda_w == rational{}) {
      return;
    }
    ++accepted;
    [&]<std::size_t... K>(std::index_sequence<K...>) {
      (check_composition_at<K>(source, w, v), ...);
    }(std::make_index_sequence<MaxGrade + 1>{});
  };
  composition(std::integral_constant<std::size_t, 1>{},
              std::integral_constant<std::size_t, 1>{});
  if constexpr (MaxCount >= 3) {
    composition(std::integral_constant<std::size_t, 1>{},
                std::integral_constant<std::size_t, 2>{});
    composition(std::integral_constant<std::size_t, 2>{},
                std::integral_constant<std::size_t, 1>{});
    composition(std::integral_constant<std::size_t, 3>{},
                std::integral_constant<std::size_t, 1>{});
  }
  CHECK(accepted > 0);
}

// The identities for each form of the list, in its order, with hyperplanes
// of grade n - 1, at every grade up to n, and for versors of up to three
// hyperplanes.
template <auto... G>
void check_each(forms<G...> /*list*/, std::mt19937_64& source) {
  (check_identities<
       form<G>, core::of_grade<generators<form<G>>, generators<form<G>> - 1>,
       generators<form<G>>>(source),
   ...);
}

} // namespace

// Each group checks the identities over its forms, drawing from source in
// the order of its forms. The test cases call them in this order, so that
// one seed gives the same draws in every build.
void forms_up_to_three(std::mt19937_64& source);
void named_forms_of_four(std::mt19937_64& source);
void gram_forms_of_four(std::mt19937_64& source);
void rigid_and_conformal_forms_of_five(std::mt19937_64& source);
void diagonal_dense_and_large_forms(std::mt19937_64& source);

} // namespace galoot_test::transform_identities

#endif // GALOOT_TEST_CORE_TRANSFORM_IDENTITIES_CHECKS_HPP
