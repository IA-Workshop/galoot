// The identities of galoot/core/transform.hpp over many signatures:
// diagonal, degenerate, indefinite, and not diagonal.
//
// A versor here is the antiproduct of one to three random hyperplanes, each
// a random element of grade n - 1, which is always a blade, so its antigrade
// is 1. For such a V, V ⟇ antireverse(V) is lambda times the antiscalar, and
// when lambda is not zero the transform by V is lambda times an
// outermorphism: lambda^(k-1) T(a1 ^ ... ^ ak) = T(a1) ^ ... ^ T(ak) for
// vectors a1 to ak; and transforms compose, T_(W ⟇ V) = T_W after T_V. Draws
// with lambda = 0 are skipped. Every value is an exact rational with
// integer entries, so both sides of each identity must be equal.
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

// The forms of two to five generators: diagonal R(p, q, r) with null
// generators last, conformal(1) to conformal(3), the Witt forms of one and
// two modes, a degenerate form that is not diagonal, and seeded random
// symmetric forms of two to four generators with entries from -3 to 3.
using small_forms = forms<
    // n = 2
    core::R<2, 0, 0>(), core::R<1, 1, 0>(), core::R<1, 0, 1>(),
    core::from_gram({
        {0, 1},
        {1, 0},
    }),
    core::from_gram({
        {3, 1},
        {1, -2},
    }),
    // n = 3
    core::R<3, 0, 0>(), core::rigid<2>(), core::R<2, 1, 0>(),
    core::R<1, 1, 1>(), core::conformal<1>(),
    core::from_gram({
        {2, 0, -3},
        {0, 1, -3},
        {-3, -3, -2},
    }),
    // n = 4
    core::rigid<3>(), core::R<4, 0, 0>(), core::R<2, 2, 0>(),
    core::R<1, 3, 0>(), core::conformal<2>(),
    core::from_gram({
        {0, 1, 0, 0},
        {1, 0, 0, 0},
        {0, 0, 0, 1},
        {0, 0, 1, 0},
    }),
    core::from_gram({
        {0, 1, 0, 0},
        {1, 0, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0},
    }),
    core::from_gram({
        {-2, 0, -2, 2},
        {0, -2, -3, 3},
        {-2, -3, 1, 3},
        {2, 3, 3, 3},
    }),
    // n = 5
    core::rigid<4>(), core::conformal<3>(), core::R<4, 1, 0>()>;

// A random form of five generators whose entries are almost all nonzero.
using dense_forms = forms<core::from_gram({
    {3, 1, -3, -1, -2},
    {1, -3, -2, 1, 0},
    {-3, -2, -2, 3, 0},
    {-1, 1, 3, 3, 2},
    {-2, 0, 0, 2, -1},
})>;

// Forms of eight generators, checked with sparse hyperplanes.
using large_forms = forms<core::R<8, 0, 0>(), core::rigid<7>()>;

// The number of generators of an algebra.
template <class Alg>
inline constexpr std::size_t generators =
    std::remove_cvref_t<decltype(Alg::value)>::generator_count;

// The hyperplanes of eight generators used here: the blades e1...e7 and
// e2...e8, of grade 7.
inline constexpr core::blade_set<2> sparse_hyperplanes{
    {
        {
            {.mask = 0x7F, .sign = 1},
            {.mask = 0xFE, .sign = 1},
        },
    },
};

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

// The identities over every form of the lists above, drawn from `source`.
void check_all_forms(std::mt19937_64& source) {
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_identities<
         form<G>, core::of_grade<generators<form<G>>, generators<form<G>> - 1>,
         generators<form<G>>>(s),
     ...);
  }(small_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_identities<form<G>, core::of_grade<5, 4>, 3, 2>(s), ...);
  }(dense_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_identities<form<G>, sparse_hyperplanes, 2>(s), ...);
  }(large_forms{}, source);
}

} // namespace

// Reference: identity: for V the antiproduct of one to three random
//   hyperplanes, V ⟇ antireverse(V) is lambda times the antiscalar, and when
//   lambda is not zero the transform by V is lambda times an
//   outermorphism, lambda^(k-1) T(a1 ^ ... ^ ak) = T(a1) ^ ... ^ T(ak) at
//   every grade k from 2 to n, and transforms compose,
//   transform(x, compose(W, V)) = transform(transform(x, V), W) at every
//   grade, over the test rational with integer entries, for each form of two
//   to five generators, a dense random form of five, where the grades stop
//   at 3, the versors at two hyperplanes, and the compositions at pairs of
//   single hyperplanes, and R(8,0,0) and rigid(7) with
//   sparse hyperplanes at grades up to 2
TEST_CASE("versors of hyperplanes act as outermorphisms and compose over "
          "many forms") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  check_all_forms(source);
}

// Reference: identity: the outermorphism and composition identities of the
//   case above, at the fixed seeds 8, 23, 27, 37, 38, 39, and 43, at which
//   hyperplane entries from -2 to 2 made the grade-four check over the
//   random form of four generators overflow the test rational
TEST_CASE("versors of hyperplanes act as outermorphisms and compose at the "
          "seeds that once overflowed the test rational") {
  for (const unsigned seed : {8U, 23U, 27U, 37U, 38U, 39U, 43U}) {
    CAPTURE(seed);
    std::mt19937_64 source(seed);
    check_all_forms(source);
  }
}
