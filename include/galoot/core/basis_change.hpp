#ifndef GALOOT_CORE_BASIS_CHANGE_HPP
#define GALOOT_CORE_BASIS_CHANGE_HPP

// Exact changes of basis between two algebras with the same number of
// generators, whose forms are congruent over the rationals, such as
// conformal(3) and its diagonal counterpart R(4, 1, 0):
//
//   struct cga3 {
//     static constexpr auto value = galoot::core::conformal<3>();
//   };
//   struct r410 {
//     static constexpr auto value = galoot::core::R<4, 1, 0>();
//   };
//   struct cga3_to_r410 {
//     using source = cga3;
//     using target = r410;
//     static constexpr auto value = galoot::core::conformal_change<3>();
//   };
//
//   constexpr auto point_layout = galoot::core::blades<cga3, "e1 e2 e3 e4 e5">;
//   using Point = galoot::core::mv<cga3, point_layout, double>;
//   Point p{1, 2, 3, 1, 7};
//   auto q = galoot::core::change_basis<cga3_to_r410>(p);  // in r410
//   Point p2 = galoot::core::change_basis_back<cga3_to_r410>(q);  // p2 == p
//
// A layout whose image holds other blades comes back on more blades than it
// had, and truncate reads it as its own type:
//
//   using Origin = galoot::core::mv<cga3, galoot::core::blades<cga3, "e4">,
//                                   double>;
//   auto o = galoot::core::change_basis<cga3_to_r410>(Origin{1});  // e4', e5'
//   Origin o2 = galoot::core::truncate<Origin>(
//       galoot::core::change_basis_back<cga3_to_r410>(o));  // e5 is 0
//
// A change is a matrix P whose row i writes the source's generator e_i in
// the target's generators e'_1 to e'_n: e_i = P_i1 e'_1 + ... + P_in e'_n.
// It is held exactly, as integer rows over one positive denominator per row,
// P_ij = rows[i][j] / denominators[i], so no entry is ever rounded. It must
// keep every inner product, P G' P^T = G for the Gram matrices G of the
// source and G' of the target, and it must be invertible; both are checked,
// in integers, the first time a program uses the change.
//
// The change sends a blade e_A = e_a1 ∧ ... ∧ e_ak to the wedge of the
// images of its generators, which is the sum over the target's blades e'_B
// of A's grade of det P[A, B] e'_B, the minor of P on the rows of A and the
// columns of B. So it keeps grades, sends the scalar 1 to 1, and sends the
// antiscalar of the source to det(P) times the antiscalar of the target.
// change_basis_back applies the inverse change, P^-1, which the library
// computes exactly from the rows.
//
// Because P keeps the inner products, the change, written phi below with
// d = det(P), is an isomorphism of the two algebras. It preserves the
// geometric product, the wedge, the dot, the reverse, the antireverse, and
// the grade involution: change_basis(x ⟑ y) is
// change_basis(x) ⟑ change_basis(y), and likewise for the others. Through
// the antiscalar it scales the operations defined by the complements:
//
//   phi(a ∨ b) = phi(a) ∨ phi(b) / d, for every change;
//   phi(bulk_dual(x)) = d bulk_dual(phi(x)), and the same for the left bulk
//   dual, for every change;
//   phi(a ⟇ b) = phi(a) ⟇ phi(b) / d, and the same factor 1 / d for the
//   antidot, both weight duals, and compose, exactly when the inverse
//   transpose of P keeps the inner products too, P^-T G' P^-1 = G;
//   transform, project, and antiproject then with 1 / d^2.
//
// That last condition holds for every change between forms whose Gram
// matrices are their own inverses, such as conformal_change and the change
// of a Witt form to R(m, m, 0). For a nondegenerate form it forces
// d = 1 or -1, so 1 / d and d agree there; they differ only for a change of
// a degenerate form, such as rigid(3) with e4 = 2 e'4, where d = 2. A
// transform agrees across bases as stated where it drops nothing, as for a
// versor applied to an object of whole grades.
//
// The complements, the bulk, and the weight are not preserved in general,
// because they are defined on the coordinates of one basis. The complements
// are preserved, with the factor d, exactly when P is orthogonal; the bulk,
// with the factor 1, exactly when P P^T G = G, and the weight exactly when
// G' P^T P = G'. For a nondegenerate form each of these means P is
// orthogonal. In conformal(3) the bulk of the origin e4 is -e5, while
// computed by way of R(4, 1, 0) and mapped back it is -e5 / 2. So a result
// of these depends on the basis it was computed in.
//
// Result layouts: the image of a layout is the set of the target's blades
// that the images of its blades reach, each with orientation +1, in order of
// grade and then of index list, as for a product; change_basis_layout<C, S>
// names it. Convert it to a named type, Type{q}, to read it under that
// type's orientations. A round trip can hold more blades than it started
// with: the origin e4 alone maps onto e4' and e5', and those map back onto
// e4 and e5, with zero on e5 in exact arithmetic. truncate<Origin> reads it
// back and, for an exact scalar, checks that what it drops is zero. Nothing
// here drops a component, so nothing here checks one.
//
// Scalars: each result coefficient is an integer combination of the input's
// coefficients, divided once by a positive integer where the denominators
// reach that blade, y_B = (c_1 x_A1 + ... + c_m x_Am) / q_B, with q_B the
// least denominator over which every term of y_B is an integer multiple. A
// scalar with exact division, such as a rational type, gives the exact
// result; a floating scalar rounds as one sum of m products and one
// division per coefficient, the division by q_B itself and never a product
// with its reciprocal, which would round twice; an integer scalar is
// accepted only on a layout where every q_B is 1.
//
// Compile cost: the checks of a change run once per change tag, in about
// 2 n^3 checked multiplications for the inner products and 5 n^3 for the
// inverse and its check, whatever the layouts. The terms of a layout cost what
// the images of its blades cost: the image of a blade of grade k is built by
// wedging the rows of its generators one at a time, so its cost grows with the
// number of nonzero entries in those rows and, for a dense P, with the
// binomial coefficients C(n, j) for j up to k. conformal_change has at most
// two entries per row, so its images have at most two terms for each null
// generator they hold. The compilers' limits on constant evaluation bound
// these costs; Clang's -fconstexpr-steps and GCC's -fconstexpr-ops-limit
// raise those limits, and the sizes measured on each compiler, with how a
// failure is reported, are in docs/guide/limits.md, under
// 'Compile-time limits: change of basis'.

#include <array>
#include <bit>
#include <cstddef>
#include <limits>
#include <span>
#include <type_traits>
#include <utility>

#include <galoot/core/assert.hpp>
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace galoot::core {

/// An exact change of basis between two algebras of N generators: row i
/// writes the source's generator e_(i+1) in the target's generators e'_1 to
/// e'_N as
///   e_(i+1) = (rows[i][0] e'_1 + ... + rows[i][N-1] e'_N) / denominators[i],
/// so the matrix P of the change has entries rows[i][j] / denominators[i],
/// held exactly as integer rows over one denominator per row. from_rows and
/// conformal_change build one. A tag whose `value` is a basis_change, beside
/// its `source` and `target` algebra tags, is a change that change_basis and
/// change_basis_back apply; basis_change_like says what the tag requires and
/// what is checked. The function that applies a change is change_basis, a
/// verb; basis_change is the value that a tag holds. same_rows compares two
/// values as written. The == of basis_change is deleted, because two values
/// for one matrix can differ, so a == b does not compile, and the error
/// shows the deleted declaration, which names same_rows.
///
/// Requires: N <= mask_bits, checked at compile time with a message that
/// names it. The values are checked only when a tag that holds them is first
/// used.
/// Guarantees: a structural type, so a change can be a template argument;
/// trivially copyable, and never allocates.
template <std::size_t N>
struct basis_change {
  static_assert(N <= mask_bits, "basis_change: n is larger than the number "
                                "of bits in a blade mask");

  /// rows[i][j] is the numerator of P_ij, the coefficient of e'_(j+1) in the
  /// image of e_(i+1).
  std::array<std::array<entry_type, N>, N> rows;

  /// denominators[i] is the denominator of every entry of row i, as
  /// P_ij = rows[i][j] / denominators[i]. Each must be positive.
  std::array<entry_type, N> denominators;

  /// The number of generators of the source and of the target, n.
  static constexpr std::size_t generator_count = N;

  /// Deleted, and with it !=, which the language rewrites as the negation
  /// of ==: two values for one matrix can differ, as a row and its
  /// denominator scaled by one factor do, so a comparison of the values
  /// would not say whether two changes are one. same_rows(a, b) compares
  /// two values as written. The comment at the end of the line is for the
  /// reader of a compiler's error, which shows this line.
  bool operator==(const basis_change&) const = delete; // use same_rows(a, b)
};

/// Whether a and b hold the same rows over the same denominators, entry by
/// entry, as from_rows given the same arguments makes them. It compares the
/// values as written, not the matrices they stand for: a row scaled together
/// with its denominator is the same matrix P, and same_rows says false.
template <std::size_t N>
[[nodiscard]] constexpr bool same_rows(const basis_change<N>& a,
                                       const basis_change<N>& b) noexcept {
  return a.rows == b.rows && a.denominators == b.denominators;
}

/// The change whose row i is rows[i] over denominators[i]:
/// e_(i+1) = (rows[i][0] e'_1 + ... + rows[i][N-1] e'_N) / denominators[i].
/// For the form [[0, 1], [1, 0]] against R(1, 1, 0), with e1 = e'1 + e'2 and
/// e2 = (e'1 - e'2) / 2: from_rows({{1, 1}, {1, -1}}, {1, 2}). The parameters
/// are C arrays so that N is deduced from the braces. A row need not be
/// reduced: {{2, 2}, ...} over {2, ...} is the same matrix as {{1, 1}, ...}
/// over {1, ...}. The checks of a tag and the terms it generates divide each
/// row and its denominator by their greatest common divisor first, so a
/// common factor costs no range; the value keeps the rows as given, and
/// same_rows tells the two apart.
///
/// Requires: N rows of N entries and N denominators, or the call does not
/// match; N <= mask_bits, checked as for basis_change. Nothing else is
/// checked here, because the algebras are not known here: that every
/// denominator is positive, that the rows are independent, and that P keeps
/// the inner products of the two algebras are checked when a tag that holds
/// the result is first used, each with a message that names it.
/// Guarantees: rows and denominators equal the arguments entry by entry.
template <std::size_t N>
[[nodiscard]] consteval basis_change<N>
// NOLINTNEXTLINE(*-avoid-c-arrays): a C array lets N be deduced from braces.
from_rows(const entry_type (&rows)[N][N], const entry_type (&denominators)[N]) {
  basis_change<N> change{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      change.rows[i][j] = rows[i][j];
    }
    change.denominators[i] = denominators[i];
  }
  return change;
}

/// The change of basis from conformal(D) to its diagonal counterpart
/// R(D+1, 1, 0). The generators e1 to eD are kept, e_i = e'_i; the origin
/// e(D+1) is (e'(D+2) - e'(D+1)) / 2; and the point at infinity e(D+2) is
/// e'(D+2) + e'(D+1), where e'(D+1) squares to +1 and e'(D+2) to -1. Its
/// determinant is -1, so e(D+1)(D+2) maps to -e'(D+1)(D+2) and the antiscalar
/// to minus the target's antiscalar.
///
/// Requires: D + 2 <= mask_bits, checked as for basis_change. A tag that
/// holds it names an algebra of conformal<D>() as its source and one of
/// R<D+1, 1, 0>() as its target; any other pair is rejected when the tag is
/// first used, since the change then does not keep the inner products.
/// Guarantees: the rows of that change, with every denominator 1 but the
/// origin's, which is 2.
template <std::size_t D>
[[nodiscard]] consteval basis_change<D + 2> conformal_change() {
  basis_change<D + 2> change{};
  for (std::size_t i = 0; i < D + 2; ++i) {
    change.denominators[i] = 1;
  }
  for (std::size_t i = 0; i < D; ++i) {
    change.rows[i][i] = 1;
  }

  // The origin, (e'(D+2) - e'(D+1)) / 2, and the point at infinity,
  // e'(D+2) + e'(D+1): each squares to 0, and their inner product is
  // (-1 - 1) / 2 = -1, as in conformal(D).
  change.rows[D][D] = -1;
  change.rows[D][D + 1] = 1;
  change.denominators[D] = 2;
  change.rows[D + 1][D] = 1;
  change.rows[D + 1][D + 1] = 1;
  return change;
}

namespace detail {

// x y and x + y, each within -max to max, the range that generated
// coefficients keep. Each clears `fits` and returns 0 when its exact result
// would leave that range, so that the checks of a change tag can note an
// overflow and report it with one static_assert rather than stop inside an
// evaluation.
[[nodiscard]] constexpr coefficient_type
multiply_noting(coefficient_type x, coefficient_type y, bool& fits) noexcept {
  constexpr coefficient_type max = std::numeric_limits<coefficient_type>::max();
  const coefficient_type magnitude_x = x < 0 ? -x : x;
  const coefficient_type magnitude_y = y < 0 ? -y : y;
  if (magnitude_x != 0 && magnitude_y > max / magnitude_x) {
    fits = false;
    return 0;
  }
  return x * y;
}

[[nodiscard]] constexpr coefficient_type
add_noting(coefficient_type x, coefficient_type y, bool& fits) noexcept {
  constexpr coefficient_type max = std::numeric_limits<coefficient_type>::max();
  if (y > 0 ? x > max - y : x < -max - y) {
    fits = false;
    return 0;
  }
  return x + y;
}

// Fails unless a value the change computes fits coefficient_type. A compiler
// quotes the source line of a failed assertion, so the message is kept on
// one line, where both show it whole.
constexpr void check_fits(bool fits) noexcept {
  // clang-format off
  GALOOT_ASSERT(fits, "basis_change: an exact value the change computes does not fit coefficient_type; use rows with smaller entries");
  // clang-format on
}

// The same for the term generation, which stops at an overflow, with a
// message that names the change rather than a product.
[[nodiscard]] constexpr coefficient_type
change_multiply(coefficient_type x, coefficient_type y) noexcept {
  bool fits = true;
  const coefficient_type product = multiply_noting(x, y, fits);
  check_fits(fits);
  return product;
}

[[nodiscard]] constexpr coefficient_type
change_add(coefficient_type x, coefficient_type y) noexcept {
  bool fits = true;
  const coefficient_type sum = add_noting(x, y, fits);
  check_fits(fits);
  return sum;
}

// The greatest common divisor of |x| and |y|, and 0 for two zeros.
[[nodiscard]] constexpr coefficient_type
change_gcd(coefficient_type x, coefficient_type y) noexcept {
  x = x < 0 ? -x : x;
  y = y < 0 ? -y : y;
  while (y != 0) {
    const coefficient_type rest = x % y;
    x = y;
    y = rest;
  }
  return x;
}

// The rows of a change, or of its inverse, as coefficient_type: the inverse's
// entries can outgrow entry_type.
template <std::size_t N>
struct change_rows {
  std::array<std::array<coefficient_type, N>, N> rows;
  std::array<coefficient_type, N> denominators;
};

template <std::size_t N>
[[nodiscard]] constexpr change_rows<N>
widened(const basis_change<N>& change) noexcept {
  change_rows<N> out{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      out.rows[i][j] = change.rows[i][j];
    }
    out.denominators[i] = change.denominators[i];
  }
  return out;
}

template <std::size_t N>
[[nodiscard]] constexpr bool
denominators_positive(const basis_change<N>& change) noexcept {
  // NOLINTNEXTLINE(readability-use-anyofallof): avoids <algorithm> here.
  for (const entry_type d : change.denominators) {
    if (d <= 0) {
      return false;
    }
  }
  return true;
}

// The change with each row and its denominator divided by their greatest
// common divisor g_i: the same matrix, since rows[i][j] / denominators[i] is
// unchanged, with a positive denominator wherever the denominator was
// positive. Each product and sum that the checks and the term generation
// form from the reduced rows, the values whose range they check, is the one
// they form from the rows as written divided by a positive integer, because
// each is built from minors of the rows and from the denominators, and both
// scale with the g_i. So the reduction can only bring a value into range,
// and what is generated, which depends on the matrix alone, is unchanged. A
// row whose divisor is 0 or 1 is kept as it is.
template <std::size_t N>
[[nodiscard]] constexpr basis_change<N>
reduced(const basis_change<N>& change) noexcept {
  basis_change<N> out = change;
  for (std::size_t i = 0; i < N; ++i) {
    coefficient_type common = change.denominators[i];
    for (const entry_type x : change.rows[i]) {
      common = change_gcd(common, x);
    }
    if (common > 1) {
      for (entry_type& x : out.rows[i]) {
        x = static_cast<entry_type>(x / common);
      }
      out.denominators[i] =
          static_cast<entry_type>(out.denominators[i] / common);
    }
  }
  return out;
}

// The end of a fraction-free Gauss-Jordan elimination of [M | I] for the
// integer rows M (Bareiss's method, in its Gauss-Jordan form): [p I | p M^-1],
// with p = det M or -det M by the row swaps made, or p = 0 when M is
// singular; and whether every intermediate fit coefficient_type, without
// which the rest means nothing.
template <std::size_t N>
struct elimination {
  coefficient_type pivot;
  std::array<std::array<coefficient_type, N>, N> scaled_inverse;
  bool fits;
};

// One row's part of step k of the elimination below, with `pivot_row` the
// row of the pivot a_kk and `previous` the pivot of the step before, which
// is not zero.
template <std::size_t M>
constexpr void
eliminate_entries(std::array<coefficient_type, M>& row,
                  const std::array<coefficient_type, M>& pivot_row,
                  std::size_t k, coefficient_type previous,
                  bool& fits) noexcept {
  for (std::size_t j = 0; j < M; ++j) {
    if (j != k) {
      row[j] = add_noting(multiply_noting(pivot_row[k], row[j], fits),
                          -multiply_noting(row[k], pivot_row[j], fits), fits) /
               previous;
    }
  }
  row[k] = 0;
}

// Each step k replaces every entry outside row k and column k by
// (a_kk a_ij - a_ik a_kj) / p, with p the pivot of the step before (1 at the
// first). Every entry is then a minor of [M | I], so the division is exact,
// and after the last step every diagonal entry of the left block is the last
// pivot. A zero pivot is replaced by a row below it; a row swap changes the
// sign of the pivots after it, not the right block's ratio to them, so the
// right block is p M^-1 whatever rows were swapped. An overflow ends the
// elimination at the step where it happens.
template <std::size_t N>
[[nodiscard]] constexpr elimination<N>
eliminate(const basis_change<N>& change) noexcept {
  std::array<std::array<coefficient_type, 2 * N>, N> a{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      a[i][j] = change.rows[i][j];
    }
    a[i][N + i] = 1;
  }

  bool fits = true;
  coefficient_type previous = 1;
  for (std::size_t k = 0; k < N; ++k) {
    std::size_t row = k;
    while (row < N && a[row][k] == 0) {
      ++row;
    }
    if (row == N) {
      return {.pivot = 0, .scaled_inverse = {}, .fits = fits};
    }
    if (row != k) {
      std::swap(a[row], a[k]);
    }
    for (std::size_t i = 0; i < N; ++i) {
      if (i != k) {
        eliminate_entries(a[i], a[k], k, previous, fits);
      }
    }
    if (!fits) {
      return {.pivot = 0, .scaled_inverse = {}, .fits = false};
    }
    previous = a[k][k];
  }

  elimination<N> out{.pivot = previous, .scaled_inverse = {}, .fits = fits};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      out.scaled_inverse[i][j] = a[i][N + j];
    }
  }
  return out;
}

// What a check of an identity in integers found: whether every value fit
// coefficient_type, and, if so, whether the identity holds.
struct identity_check {
  bool fits;
  bool holds;
};

// Whether M times the right block is p I, as it is when the elimination is
// right: a check of the inverse at the cost of one matrix product.
template <std::size_t N>
[[nodiscard]] constexpr identity_check
inverts(const basis_change<N>& change, const elimination<N>& e) noexcept {
  bool fits = true;
  bool holds = true;
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      coefficient_type sum = 0;
      for (std::size_t k = 0; k < N; ++k) {
        sum = add_noting(
            sum,
            multiply_noting(change.rows[i][k], e.scaled_inverse[k][j], fits),
            fits);
      }
      holds = holds && sum == (i == j ? e.pivot : 0);
    }
  }
  return {.fits = fits, .holds = fits && holds};
}

// The rows of the inverse, and whether every value fit coefficient_type.
template <std::size_t N>
struct noted_rows {
  change_rows<N> rows;
  bool fits;
};

// P^-1 = M^-1 Δ for P = Δ^-1 M, with Δ the diagonal of the denominators:
// row i of P^-1 is (p M^-1)_ij Δ_j over p. The sign of p moves into the row,
// so that the denominator is positive, and each row is reduced by the gcd of
// its entries and its denominator.
template <std::size_t N>
[[nodiscard]] constexpr noted_rows<N>
inverse_rows(const basis_change<N>& change, const elimination<N>& e) noexcept {
  const coefficient_type sign = e.pivot < 0 ? -1 : 1;
  const coefficient_type magnitude = sign * e.pivot;
  noted_rows<N> out{.rows = {}, .fits = true};
  for (std::size_t i = 0; i < N; ++i) {
    coefficient_type common = magnitude;
    for (std::size_t j = 0; j < N; ++j) {
      out.rows.rows[i][j] = multiply_noting(sign * e.scaled_inverse[i][j],
                                            change.denominators[j], out.fits);
      common = change_gcd(common, out.rows.rows[i][j]);
    }
    for (std::size_t j = 0; j < N; ++j) {
      out.rows.rows[i][j] /= common;
    }
    out.rows.denominators[i] = magnitude / common;
  }
  return out;
}

// Whether P G' P^T = G, in integers as M G' M^T = Δ G Δ for the integer rows
// M and the diagonal Δ of the denominators. M G' is formed once, so the
// check costs 2 n^3 multiplications.
template <std::size_t N>
[[nodiscard]] constexpr identity_check
keeps_inner_products(const basis_change<N>& change, const metric<N>& source,
                     const metric<N>& target) noexcept {
  bool fits = true;
  std::array<std::array<coefficient_type, N>, N> rows_times_target{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t c = 0; c < N; ++c) {
      coefficient_type sum = 0;
      for (std::size_t k = 0; k < N; ++k) {
        sum = add_noting(
            sum, multiply_noting(change.rows[i][k], target.gram[k][c], fits),
            fits);
      }
      rows_times_target[i][c] = sum;
    }
  }

  bool holds = true;
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      coefficient_type left = 0;
      for (std::size_t c = 0; c < N; ++c) {
        left = add_noting(
            left,
            multiply_noting(rows_times_target[i][c], change.rows[j][c], fits),
            fits);
      }
      const coefficient_type right = multiply_noting(
          multiply_noting(change.denominators[i], source.gram[i][j], fits),
          change.denominators[j], fits);
      holds = holds && left == right;
    }
  }
  return {.fits = fits, .holds = fits && holds};
}

template <class T>
inline constexpr bool is_basis_change_v = false;

template <std::size_t N>
inline constexpr bool is_basis_change_v<basis_change<N>> = true;

} // namespace detail

/// A tag type that names a change of basis: its source algebra, its target
/// algebra, and the change between them.
///
/// Requires of C: member types `source` and `target` that are metric_like,
/// and a static data member `value`, usable in a constant expression, of type
/// basis_change<N> with N >= 1. The concept tests these data only, and is
/// false rather than an error for a type that lacks one of them, such as an
/// algebra tag. The rest is checked once per tag in each translation unit,
/// when change_basis, change_basis_back, or one of the layouts below first
/// uses it, each with a message that names it: the source and the target
/// each have N generators; every denominator is positive; the rows are
/// linearly independent, so the change has an inverse; and P G' P^T = G for
/// the Gram matrices G of the source and G' of the target, so that the
/// change keeps every inner product. Once the denominators are checked, each
/// row and its denominator are divided by their greatest common divisor,
/// which leaves P as it is, and the checks after it read the reduced rows.
/// The last is computed exactly in integers, as M G' M^T = Δ G Δ for the
/// integer rows M and the diagonal Δ of the denominators. A value of these
/// checks or of the inverse that does not fit coefficient_type is reported
/// with a message of its own, and one of the generated terms fails the
/// constant evaluation with that message, so neither can pass silently.
template <class C>
concept basis_change_like =
    metric_like<typename C::source> && metric_like<typename C::target> &&
    detail::is_basis_change_v<std::remove_cvref_t<decltype(C::value)>> &&
    (std::remove_cvref_t<decltype(C::value)>::generator_count >= 1) &&
    // As metric_like does, one entry is read first, so that a value that
    // cannot be read at compile time makes the concept false, not an error.
    requires {
      typename std::integral_constant<entry_type, C::value.rows[0][0]>;
    };

namespace detail {

// The faults the checks of a change tag can find, each but the counts of
// generators found by check_change.
// An int, so that a compiler prints its values and not a byte.
// NOLINTNEXTLINE(performance-enum-size)
enum class ChangeFault : int {
  generator_count,
  denominator,
  overflow,
  dependent,
  isometry,
  inverse,
  none,
};

// What the checks of a change tag found, and, when it passed them, its rows
// and those of its inverse, which the term generation reads.
template <std::size_t N>
struct checked_change_data {
  ChangeFault fault;
  change_rows<N> forward;
  change_rows<N> inverse;
};

// The checks of a change between algebras of N generators each, in the
// order a user would fix them: the denominators, the independence of the
// rows, the inner products, and the inverse. Independence comes before the
// inner products because in a degenerate form a singular P can keep them,
// and the fault is then that it has no inverse. An overflow in any of them
// is a fault of its own, since a check that overflowed decides nothing.
// Once the denominators are positive, every check runs on the reduced rows,
// which are also the forward rows, so that a common factor of a row and its
// denominator, as in m e'_i over m, overflows nothing.
template <std::size_t N>
[[nodiscard]] constexpr checked_change_data<N>
check_change(const basis_change<N>& change, const metric<N>& source,
             const metric<N>& target) noexcept {
  checked_change_data<N> out{
      .fault = ChangeFault::none,
      .forward = {},
      .inverse = {},
  };
  if (!denominators_positive(change)) {
    out.fault = ChangeFault::denominator;
    return out;
  }
  const basis_change<N> rows = reduced(change);
  out.forward = widened(rows);
  const elimination<N> e = eliminate(rows);
  if (!e.fits || e.pivot == 0) {
    out.fault = e.fits ? ChangeFault::dependent : ChangeFault::overflow;
    return out;
  }
  const identity_check isometry = keeps_inner_products(rows, source, target);
  if (!isometry.holds) {
    out.fault = isometry.fits ? ChangeFault::isometry : ChangeFault::overflow;
    return out;
  }
  const identity_check inverse_holds = inverts(rows, e);
  if (!inverse_holds.holds) {
    out.fault =
        inverse_holds.fits ? ChangeFault::inverse : ChangeFault::overflow;
    return out;
  }
  const noted_rows<N> inverse = inverse_rows(rows, e);
  if (!inverse.fits) {
    out.fault = ChangeFault::overflow;
    return out;
  }
  out.inverse = inverse.rows;
  return out;
}

// The checks of a change tag, each a static_assert with its message: the
// counts of generators, compared as written so that the compilers print
// both, the source's first and the target's only when the source's is
// right; then check_change's verdict, read once, at most one of whose
// assertions fails. So a tag gives one error, whatever its faults. Every
// local is static constexpr, so that it is evaluated once, as in
// make_product_plan.
template <basis_change_like C>
[[nodiscard]] consteval auto make_checked_change() {
  static constexpr std::size_t n = C::value.generator_count;
  using result = checked_change_data<n>;
  static_assert(C::source::value.generator_count == C::value.generator_count,
                "basis_change: the source algebra has a different number of "
                "generators than the change has rows; a change maps between "
                "algebras of one dimension");
  if constexpr (C::source::value.generator_count == n) {
    static_assert(C::target::value.generator_count == C::value.generator_count,
                  "basis_change: the target algebra has a different number of "
                  "generators than the change has rows; a change maps between "
                  "algebras of one dimension");
    if constexpr (C::target::value.generator_count == n) {
      static constexpr result checked =
          check_change(C::value, C::source::value, C::target::value);
      static constexpr bool positive =
          checked.fault != ChangeFault::denominator;
      static constexpr bool fits = checked.fault != ChangeFault::overflow;
      static constexpr bool independent =
          checked.fault != ChangeFault::dependent;
      static constexpr bool isometry = checked.fault != ChangeFault::isometry;
      static constexpr bool inverse_holds =
          checked.fault != ChangeFault::inverse;
      static_assert(positive, "basis_change: every denominator must be "
                              "positive; put a row's sign in its entries");
      static_assert(fits, "basis_change: an exact value the change computes "
                          "does not fit coefficient_type; use rows with "
                          "smaller entries");
      static_assert(independent,
                    "basis_change: the rows are linearly dependent, so the "
                    "change has no inverse; the images of the source's "
                    "generators must be independent");
      static_assert(isometry,
                    "basis_change: the change does not keep the inner products "
                    "(P G' P^T is not G); check that row i writes e_i in the "
                    "target's generators, and that source and target are not "
                    "swapped");
      static_assert(inverse_holds, "basis_change: the computed inverse does "
                                   "not invert the rows");
      return checked;
    }
  }
  return result{
      .fault = ChangeFault::generator_count,
      .forward = {},
      .inverse = {},
  };
}

template <basis_change_like C>
inline constexpr auto checked_change = make_checked_change<C>();

/// The image of the blade of the mask `a` under the integer rows: the wedge
/// of rows a1 to ak of its generators, folded one row at a time with the
/// wedge's pair rule, which is the sum over the blades e_B of A's grade of
/// the minor det M[A, B] of the rows on A's rows and B's columns. The
/// denominators are not applied: the image of e_A under the change is this
/// over the product of the denominators of A's rows.
///
/// Requires: `a` names only e_1 to e_N, checked by GALOOT_ASSERT; Capacity is
/// at least C(N, min(k, N / 2)) for the grade k of `a`, the most terms a
/// partial wedge of grade at most k can have, checked by GALOOT_ASSERT as
/// the terms are added; every coefficient fits coefficient_type, checked by
/// GALOOT_ASSERT.
/// Guarantees: every term has the grade of `a`, none has a zero coefficient,
/// and the terms are in canonical order.
template <std::size_t Capacity, std::size_t N>
[[nodiscard]] constexpr term_list<Capacity>
blade_image(const change_rows<N>& rows, mask_t a) noexcept {
  GALOOT_ASSERT(in_algebra(a, N), "blade has a generator beyond e_n");
  // The wedge reads no metric; pair_terms takes one for the geometric
  // product.
  const metric<N> no_metric{};
  term_list<Capacity> image{};
  image.add(0, 1);
  for (mask_t rest = a; rest != 0; rest &= rest - 1) {
    const auto i = static_cast<std::size_t>(std::countr_zero(rest));
    term_list<Capacity> next{};
    for (std::size_t t = 0; t < image.size(); ++t) {
      const term& partial = image.terms[t];
      for (std::size_t j = 0; j < N; ++j) {
        if (rows.rows[i][j] == 0) {
          continue;
        }
        const term_list<1> wedge = pair_terms<BinaryOperation::wedge, 1>(
            no_metric, partial.blade, mask_t{1} << j);
        if (wedge.size() == 1) {
          next.add(wedge.terms[0].blade,
                   change_multiply(
                       change_multiply(partial.coefficient, rows.rows[i][j]),
                       wedge.terms[0].coefficient));
        }
      }
    }
    image = next;
  }
  return image;
}

/// The same for the rows of a change as written, before they are widened.
template <std::size_t Capacity, std::size_t N>
[[nodiscard]] constexpr term_list<Capacity>
blade_image(const basis_change<N>& change, mask_t a) noexcept {
  return blade_image<Capacity>(widened(change), a);
}

// The capacity blade_image needs for every blade of the set, in an algebra
// of n generators: C(n, min(k, n / 2)) for the largest grade k of the set.
[[nodiscard]] constexpr std::size_t
image_capacity(std::span<const oriented_blade> set, std::size_t n) noexcept {
  std::size_t capacity = 1;
  for (const oriented_blade& blade : set) {
    const auto k = static_cast<std::size_t>(grade(blade.mask));
    const std::size_t largest = binomial(n, k < n / 2 ? k : n / 2);
    // NOLINTNEXTLINE(readability-use-std-min-max): avoids <algorithm> here.
    if (largest > capacity) {
      capacity = largest;
    }
  }
  return capacity;
}

// The product of the denominators of the rows of the blade's generators: the
// denominator of every term of its image.
template <std::size_t N>
[[nodiscard]] constexpr coefficient_type
image_denominator(const change_rows<N>& rows, mask_t a) noexcept {
  coefficient_type d = 1;
  for (mask_t rest = a; rest != 0; rest &= rest - 1) {
    d = change_multiply(
        d, rows.denominators[static_cast<std::size_t>(std::countr_zero(rest))]);
  }
  return d;
}

// Writes, for each position a of `set`, the terms of the image of its blade
// s_a e_A as plan terms with first position a and second position 0, the
// integer minors times s_a, so that the functions that type a plan type
// them; writes the first out.size() and returns the number of all, as
// generate_plan does.
template <std::size_t Capacity, std::size_t N>
constexpr std::size_t generate_change(const change_rows<N>& rows,
                                      std::span<const oriented_blade> set,
                                      std::span<plan_term> out) noexcept {
  std::size_t count = 0;
  for (std::size_t a = 0; a < set.size(); ++a) {
    const term_list<Capacity> image = blade_image<Capacity>(rows, set[a].mask);
    for (std::size_t k = 0; k < image.size(); ++k) {
      if (count < out.size()) {
        out[count] = {
            .a = a,
            .b = 0,
            .out = image.terms[k].blade,
            .coefficient = set[a].sign * image.terms[k].coefficient,
        };
      }
      ++count;
    }
  }
  return count;
}

template <std::size_t Count, std::size_t Capacity, std::size_t N>
[[nodiscard]] constexpr std::array<plan_term, Count>
filled_change(const change_rows<N>& rows,
              std::span<const oriented_blade> set) noexcept {
  std::array<plan_term, Count> terms{};
  generate_change<Capacity>(rows, set, terms);
  return terms;
}

// A change of a layout typed onto the blade set of its result: R result
// blades, T terms, and the denominator q_B of each result blade. Its terms
// are the straight-line code of the integer sums, and each sum is divided
// once by its q_B when q_B is not 1.
template <std::size_t R, std::size_t T>
struct change_map {
  blade_set<R> result;
  std::array<kernel_term, T> terms;
  std::array<coefficient_type, R> denominators;
};

// Puts each term over its result blade's denominator q_B: first the least
// common multiple of the denominators of the terms that reach B, each the
// product of its input blade's row denominators; then each coefficient times
// q_B over its own denominator; then q_B and those coefficients divided by
// their gcd. The last step leaves q_B the least denominator over which every
// term is an integer, so a blade whose terms are integers, such as the image
// of e45 under conformal_change<3>(), -e'45, needs no division.
template <std::size_t R, std::size_t T, std::size_t N>
[[nodiscard]] constexpr change_map<R, T>
typed_change(const std::array<plan_term, T>& plan,
             const collected_result<T>& found, const change_rows<N>& rows,
             std::span<const oriented_blade> set) noexcept {
  change_map<R, T> out{};
  for (std::size_t r = 0; r < R; ++r) {
    out.result.blades[r] = found.blades[r];
    out.denominators[r] = 1;
  }
  std::array<typed_term, T> typed{};
  type_terms(plan, out.result.blades, typed);

  std::array<coefficient_type, T> own{};
  for (std::size_t k = 0; k < T; ++k) {
    own[k] = image_denominator(rows, set[typed[k].a].mask);
    const coefficient_type q = out.denominators[typed[k].out];
    out.denominators[typed[k].out] =
        change_multiply(q / change_gcd(q, own[k]), own[k]);
  }

  for (std::size_t k = 0; k < T; ++k) {
    out.terms[k] = {
        .a = typed[k].a,
        .b = 0,
        .out = typed[k].out,
        .coefficient = change_multiply(typed[k].coefficient,
                                       out.denominators[typed[k].out] / own[k]),
        .first = false,
    };
  }

  std::array<coefficient_type, R> common = out.denominators;
  for (const kernel_term& t : out.terms) {
    common[t.out] = change_gcd(common[t.out], t.coefficient);
  }
  for (kernel_term& t : out.terms) {
    t.coefficient /= common[t.out];
  }
  for (std::size_t r = 0; r < R; ++r) {
    out.denominators[r] /= common[r];
  }
  mark_first_terms<R>(out.terms);
  return out;
}

// Whether every term keeps the grade of its input blade, as every minor
// pairs sets of one size.
template <std::size_t R, std::size_t T>
[[nodiscard]] constexpr bool
keeps_grades(const change_map<R, T>& map,
             std::span<const oriented_blade> set) noexcept {
  // NOLINTNEXTLINE(readability-use-anyofallof): avoids <algorithm> here.
  for (const kernel_term& t : map.terms) {
    if (grade(map.result.blades[t.out].mask) != grade(set[t.a].mask)) {
      return false;
    }
  }
  return true;
}

// Checks the layout once, before anything is generated, so that a misuse
// gives its own message and no other, naming the layout the set was given
// to: only change_basis_layout and change_basis_back_layout take a set that
// no multivector has checked. It then reads the checked rows of the tag in
// the direction asked for, and generates, counts, fills, and types the terms
// of the layout. A tag that failed its checks gives the empty map, so that a
// call adds no second error. Every local is static constexpr, as in
// make_product_plan and for the same reason.
template <basis_change_like C, bool Back, blade_set S>
[[nodiscard]] consteval auto make_change_map() {
  static constexpr const auto& checked = checked_change<C>;
  using from = std::conditional_t<Back, typename C::target, typename C::source>;
  static constexpr std::size_t n = from::value.generator_count;
  static constexpr bool in_range = all_in_algebra(S, n);
  static constexpr bool valid_signs = signs_valid<S>;
  if constexpr (Back) {
    static_assert(in_range, "change_basis_back_layout: the blade set names a "
                            "generator beyond e_n; use a set whose blades "
                            "are all in the change's target algebra");
    static_assert(valid_signs,
                  "change_basis_back_layout: the blade set holds a sign "
                  "other than +1 or -1; give each blade of a set written out "
                  "by hand the sign +1 or -1");
  } else {
    static_assert(in_range, "change_basis_layout: the blade set names a "
                            "generator beyond e_n; use a set whose blades are "
                            "all in the change's source algebra");
    static_assert(valid_signs,
                  "change_basis_layout: the blade set holds a sign other "
                  "than +1 or -1; give each blade of a set written out by "
                  "hand the sign +1 or -1");
  }

  if constexpr (checked.fault == ChangeFault::none && in_range && valid_signs) {
    static constexpr const auto& rows =
        Back ? checked.inverse : checked.forward;
    static constexpr std::size_t capacity = image_capacity(S.blades, n);
    static constexpr std::size_t count =
        generate_change<capacity>(rows, S.blades, {});
    static constexpr auto generated =
        filled_change<count, capacity>(rows, S.blades);
    static constexpr auto found = collected(generated);
    static constexpr auto map =
        typed_change<found.size>(generated, found, rows, S.blades);
    static constexpr bool graded = keeps_grades(map, S.blades);
    static_assert(graded, "basis_change: a term has another grade than its "
                          "input blade");
    return map;
  } else {
    return change_map<0, 0>{};
  }
}

// Generated once in each translation unit for each tag, direction, and
// layout that it uses.
template <basis_change_like C, bool Back, blade_set S>
// clang-tidy reads the layout, once substituted, as a C-style cast of its
// value, which the source does not have.
// NOLINTNEXTLINE(modernize-avoid-c-style-cast)
inline constexpr auto change_map_of = make_change_map<C, Back, S>();

// One division of a result coefficient by its denominator, a constant of the
// code, by the scalar's own division: a product with the reciprocal would
// round twice. A denominator of 1 is no division.
template <coefficient_type Q, class T>
constexpr void divide_by(T& out) {
  if constexpr (Q != 1) {
    out = static_cast<T>(out / static_cast<T>(Q));
  }
}

// The R coefficients of the image: the integer sums of the map's terms, one
// statement each, as for a product, then one division for each coefficient
// whose denominator is not 1.
// clang-tidy reads the parentheses that a fold expression needs as
// redundant once the fold is expanded.
// NOLINTBEGIN(readability-redundant-parentheses)
template <const auto& Map, class T, std::size_t K, std::size_t... R>
GALOOT_ALWAYS_INLINE constexpr std::array<T, sizeof...(R)>
run_change(const std::array<T, K>& x, std::index_sequence<R...> /*result*/) {
  std::array<T, sizeof...(R)> out = run_map<Map.terms, sizeof...(R)>(
      x, std::make_index_sequence<Map.terms.size()>{});
  (divide_by<Map.denominators[R]>(out[R]), ...);
  return out;
}
// NOLINTEND(readability-redundant-parentheses)

// Whether T holds every term coefficient and every denominator of the map
// exactly, as far as std::numeric_limits<T> tells.
template <class T, std::size_t R, std::size_t K>
[[nodiscard]] constexpr bool
change_held_exactly(const change_map<R, K>& map) noexcept {
  // NOLINTNEXTLINE(readability-use-anyofallof): avoids <algorithm> here.
  for (const kernel_term& t : map.terms) {
    if (!holds_exactly<T>(t.coefficient)) {
      return false;
    }
  }
  // NOLINTNEXTLINE(readability-use-anyofallof): avoids <algorithm> here.
  for (const coefficient_type q : map.denominators) {
    if (!holds_exactly<T>(q)) {
      return false;
    }
  }
  return true;
}

// Whether some coefficient of the map is divided.
template <std::size_t R, std::size_t K>
[[nodiscard]] constexpr bool divides(const change_map<R, K>& map) noexcept {
  // NOLINTNEXTLINE(readability-use-anyofallof): avoids <algorithm> here.
  for (const coefficient_type q : map.denominators) {
    if (q != 1) {
      return true;
    }
  }
  return false;
}

// A scalar whose division by an exact integer is the quotient, not its
// truncation: one with a / b convertible to it that std::numeric_limits does
// not describe as an integer type, as integer_limits reads it, so that a
// specialization without is_integer, as a rational type may have, counts as
// not an integer. A type whose division truncates but whose numeric_limits
// do not say it is an integer is accepted, and nothing here can tell.
template <class T>
concept divides_exactly = requires(const T& x, const T& y) {
  { x / y } -> std::convertible_to<T>;
} && !integer_limits<T>;

static_assert(divides_exactly<double>);
static_assert(!divides_exactly<int>);

} // namespace detail

// The layouts of the results, named so that a program can name a result
// type and a compiler prints it in these terms. clang-tidy reads the layout,
// once substituted, as a C-style cast of its value, which the source does
// not have.
// NOLINTBEGIN(modernize-avoid-c-style-cast)

/// The layout of change_basis<C>(x) for x on the layout S of C's source
/// algebra: the target's blades that the images of S's blades reach, each
/// with orientation +1, in order of grade and then of index list, as for a
/// product. Each image keeps its blade's grade. For conformal_change<3>(),
/// the point e1 e2 e3 e4 e5 gives e1 to e5 of the target, and the origin e4
/// alone gives e4 and e5.
///
/// Requires: C passes the checks basis_change_like lists; every blade of S
/// names only e_1 to e_n of the source and has sign +1 or -1, each checked
/// at compile time with a message that names it.
template <basis_change_like C, blade_set S>
inline constexpr auto change_basis_layout =
    detail::change_map_of<C, false, S>.result;

/// The layout of change_basis_back<C>(y) for y on the layout S of C's target
/// algebra, as for change_basis_layout, through the inverse change. For
/// conformal_change<3>(), e4 alone gives e4 and e5 of the source.
template <basis_change_like C, blade_set S>
inline constexpr auto change_basis_back_layout =
    detail::change_map_of<C, true, S>.result;

// NOLINTEND(modernize-avoid-c-style-cast)

/// x written in the generators of C's target: the image of x under the
/// change, which sends each generator e_i to row i of P and each blade
/// e_A = e_a1 ∧ ... ∧ e_ak to the wedge of the images of its generators,
/// the sum of det P[A, B] e'_B over the target's blades e'_B of A's grade.
/// It keeps grades and the scalar, and sends the antiscalar to det(P) times
/// the target's antiscalar. It is an algebra isomorphism: the header's
/// comment lists the operations it preserves, with their factors, and those
/// it does not, such as the bulk.
///
/// Each result coefficient is computed by straight-line code, as for a
/// product, as one integer combination of x's coefficients divided once by a
/// positive integer where the change's denominators reach that blade:
/// y_B = (c_1 x_A1 + ... + c_m x_Am) / q_B, with the c and q_B exact integers
/// that share no factor, and no division where q_B is 1. Over float or
/// double that is one sum of products and one division, so the coefficient
/// rounds as they do; for conformal_change every q_B is 1 or 2, and the
/// division by 2 is exact unless it underflows.
///
/// Requires: C is basis_change_like and passes its checks; x is in C's
/// source algebra; the scalar holds every c and q_B exactly where its
/// numeric_limits can tell; and, where some q_B of this layout is not 1, the
/// scalar has a division and is not an integer type, so that no quotient is
/// truncated. Otherwise the call is rejected at compile time with a message
/// that names change_basis. An integer scalar is accepted on a layout that
/// needs no division, as for a permutation of the generators, or for
/// conformal_change on the Euclidean blades. A scalar whose division
/// truncates and whose numeric_limits do not say it is an integer, such as
/// some big-integer classes, is not detected.
/// Guarantees: on change_basis_layout<C, S>, in C's target algebra. The
/// result is exact for an exact scalar with exact division. Never checks: it
/// drops nothing. The terms are generated once in each translation unit for
/// each tag and layout it uses.
template <basis_change_like C, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<typename C::target, change_basis_layout<C, S>, T>
change_basis(const mv<typename C::source, S, T>& x) {
  constexpr const auto& map = detail::change_map_of<C, false, S>;
  constexpr bool scalar_holds_every_coefficient =
      detail::change_held_exactly<T>(map);
  static_assert(scalar_holds_every_coefficient,
                "change_basis: the scalar type cannot hold one of the "
                "change's exact coefficients; use a scalar type that holds "
                "larger integers exactly");
  constexpr bool scalar_divides_where_needed =
      !detail::divides(map) || detail::divides_exactly<T>;
  static_assert(scalar_divides_where_needed,
                "change_basis: the change divides by an integer on this "
                "layout, which the scalar type cannot do exactly; use a "
                "rational or floating scalar type");

  mv<typename C::target, change_basis_layout<C, S>, T> out;
  if constexpr (scalar_holds_every_coefficient && scalar_divides_where_needed) {
    out.coefficients = detail::run_change<map>(
        x.coefficients, std::make_index_sequence<map.result.size()>{});
  }
  return out;
}

/// y written in the generators of C's source: the image of y under the
/// inverse change P^-1, which the library computes exactly from C's rows
/// when the tag is first used. change_basis_back<C>(change_basis<C>(x)) is x
/// on x's blades, and zero on any other blade it holds, in exact
/// arithmetic; truncate<Type> reads it back as x's type. Otherwise as for
/// change_basis, with source and target exchanged and P^-1 for P: it sends
/// the target's antiscalar to 1 / det(P) times the source's.
///
/// Requires: as for change_basis, with y in C's target algebra, and the
/// message naming change_basis_back.
/// Guarantees: on change_basis_back_layout<C, S>, in C's source algebra.
template <basis_change_like C, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<typename C::source, change_basis_back_layout<C, S>,
                           T>
change_basis_back(const mv<typename C::target, S, T>& y) {
  constexpr const auto& map = detail::change_map_of<C, true, S>;
  constexpr bool scalar_holds_every_coefficient =
      detail::change_held_exactly<T>(map);
  static_assert(scalar_holds_every_coefficient,
                "change_basis_back: the scalar type cannot hold one of the "
                "change's exact coefficients; use a scalar type that holds "
                "larger integers exactly");
  constexpr bool scalar_divides_where_needed =
      !detail::divides(map) || detail::divides_exactly<T>;
  static_assert(scalar_divides_where_needed,
                "change_basis_back: the change divides by an integer on this "
                "layout, which the scalar type cannot do exactly; use a "
                "rational or floating scalar type");

  mv<typename C::source, change_basis_back_layout<C, S>, T> out;
  if constexpr (scalar_holds_every_coefficient && scalar_divides_where_needed) {
    out.coefficients = detail::run_change<map>(
        y.coefficients, std::make_index_sequence<map.result.size()>{});
  }
  return out;
}

/// Rejects a change_basis whose x is not in C's source algebra, with one
/// error that names the cause; the message says to use change_basis_back
/// when x is in the target. Because it exists, a requires-expression that
/// asks whether such a call is valid is told yes; compare x's algebra_type
/// with C::source instead.
template <basis_change_like C, multivector X>
  requires(!std::same_as<typename std::remove_cvref_t<X>::algebra_type,
                         typename C::source>)
constexpr std::remove_cvref_t<X> change_basis(const X& /*x*/) {
  static_assert(false, "change_basis: x is not in the change's source "
                       "algebra; use change_basis_back to map from the "
                       "target, or check that the tag's source is x's "
                       "algebra");
  return {};
}

/// Rejects a change_basis_back whose y is not in C's target algebra, as for
/// change_basis.
template <basis_change_like C, multivector X>
  requires(!std::same_as<typename std::remove_cvref_t<X>::algebra_type,
                         typename C::target>)
constexpr std::remove_cvref_t<X> change_basis_back(const X& /*y*/) {
  static_assert(false, "change_basis_back: y is not in the change's target "
                       "algebra; use change_basis to map from the source, or "
                       "check that the tag's target is y's algebra");
  return {};
}

/// Rejects a change_basis whose template argument is not a change tag, such
/// as an algebra tag, with one error whose message shows a change tag.
/// Because it exists, a requires-expression that asks is told yes; ask
/// basis_change_like instead.
template <class C, multivector X>
  requires(!basis_change_like<C>)
constexpr std::remove_cvref_t<X> change_basis(const X& /*x*/) {
  static_assert(false, "change_basis: the template argument is not a change "
                       "of basis; give a tag with source, target, and value, "
                       "as in struct cga3_to_r410 { using source = cga3; using "
                       "target = r410; static constexpr auto value = "
                       "conformal_change<3>(); }");
  return {};
}

/// Rejects a change_basis_back whose template argument is not a change tag,
/// as for change_basis.
template <class C, multivector X>
  requires(!basis_change_like<C>)
constexpr std::remove_cvref_t<X> change_basis_back(const X& /*y*/) {
  static_assert(false, "change_basis_back: the template argument is not a "
                       "change of basis; give a tag with source, target, and "
                       "value, as in struct cga3_to_r410 { using source = "
                       "cga3; using target = r410; static constexpr auto "
                       "value = conformal_change<3>(); }");
  return {};
}

/// Rejects a change_basis whose template argument is a value, such as
/// conformal_change<3>(), where a change tag belongs, with one error whose
/// message shows the tag to put it in. Any value given there is the same
/// mistake, so the parameter takes any value.
template <auto Value, multivector X>
constexpr std::remove_cvref_t<X> change_basis(const X& /*x*/) {
  static_assert(false, "change_basis: the template argument is a value, not a "
                       "change tag; put it in a tag, as in struct cga3_to_r410 "
                       "{ using source = cga3; using target = r410; static "
                       "constexpr auto value = conformal_change<3>(); }");
  return {};
}

/// Rejects a change_basis_back whose template argument is a value, as for
/// change_basis.
template <auto Value, multivector X>
constexpr std::remove_cvref_t<X> change_basis_back(const X& /*y*/) {
  static_assert(false, "change_basis_back: the template argument is a value, "
                       "not a change tag; put it in a tag, as in struct "
                       "cga3_to_r410 { using source = cga3; using target = "
                       "r410; static constexpr auto value = "
                       "conformal_change<3>(); }");
  return {};
}

} // namespace galoot::core

#endif // GALOOT_CORE_BASIS_CHANGE_HPP
