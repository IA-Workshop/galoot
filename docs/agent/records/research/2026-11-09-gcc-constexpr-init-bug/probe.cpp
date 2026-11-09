// One probe of the GCC 16.2.1 wrong-code defect, chosen by macros. A
// constexpr function make() value-initializes a container of a class whose
// value-initialized state is not all zero bits, stores into some elements,
// and returns it. The program calls make() at run time and prints a mask of
// the elements whose d differs from what the language requires, and exits 1
// if any does. CE chooses whether the same translation unit also evaluates
// make() in a constant expression.
//
// ELEM  0  class, default member initializers n = 0, d = 1, defaulted
//          default constructor, explicit converting constructor
//       1  aggregate with the same default member initializers
//       2  class with a user-provided default constructor setting d = 1,
//          no default member initializers
//       3  aggregate with the members in the other order, d = 1 then n = 0
//       4  class with a default member initializer on d = 1 only, n set by
//          the default constructor
// CONT  0  std::array<std::array<E, N>, N>
//       1  std::array<E, N * N>
//       2  struct { E a[N][N]; } (a C array member)
//       3  struct { std::array<std::array<E, N>, N> a; }
// INIT  0  T t{};   1  T t = {};   2  T t = T();   3  T t = T{};   4  T t;
//       5  T t{} followed by a loop assigning E{} to every element
//       6  T t = zero(); where zero() is a constexpr function returning T{}
//       7  one initializer per element, from index packs (CONT 0 only)
// CE    0  none
//       1  constexpr T once = make();
//       2  static_assert(make()....d == 1);
//       3  constinit T once = make(); (static storage)
//       4  a template argument computed from make()
//       5  const T g = make(); at namespace scope, not declared constexpr
//       6  static const T s = make(); in a function main calls first
//       7  const T x = make(); in a function main calls first (automatic)
//       8  T x = make(); in a function main calls first (automatic, not const)
//       9  return make()[0][0].d; in a function main calls first
// ORDER 0  the constant evaluation precedes main, 1 it follows main
// STORES  a bit mask over the flattened elements that make() assigns
// STORE 0  t[i][j] = E{1};   1  t[i][j].n = 1;
// TEMPLATE  if defined, make is a function template, make<int>
// RT_STORES  if defined, make() takes the store mask as an argument: the
//          constant evaluation passes STORES and the run-time call passes
//          RT_STORES, read through a volatile, so that the run-time result
//          shows the damage to the initial value apart from the stores
#include <array>
#include <cstdio>
#include <utility>

#ifndef ELEM
#define ELEM 0
#endif
#ifndef CONT
#define CONT 0
#endif
#ifndef INIT
#define INIT 0
#endif
#ifndef CE
#define CE 1
#endif
#ifndef ORDER
#define ORDER 0
#endif
#ifndef N
#define N 3
#endif
#ifndef STORES
#define STORES 0x111 // the diagonal of a 3 x 3 matrix
#endif
#ifndef STORE
#define STORE 0
#endif

#if ELEM == 0
struct fraction {
  long n = 0;
  long d = 1;
  constexpr fraction() = default;
  explicit constexpr fraction(long x) : n(x) {}
};
#define ONE fraction{1}
#elif ELEM == 1
struct fraction {
  long n = 0;
  long d = 1;
};
#define ONE fraction{1, 1}
#elif ELEM == 2
struct fraction {
  long n;
  long d;
  constexpr fraction() : n(0), d(1) {}
  explicit constexpr fraction(long x) : n(x), d(1) {}
};
#define ONE fraction{1}
#elif ELEM == 3
struct fraction {
  long d = 1;
  long n = 0; // not written by the initializer list below
};
#define ONE fraction{1, 1}
#elif ELEM == 4
struct fraction {
  long n;
  long d = 1;
  constexpr fraction() : n(0) {}
  explicit constexpr fraction(long x) : n(x) {}
};
#define ONE fraction{1}
#endif

#if CONT == 0
using matrix = std::array<std::array<fraction, N>, N>;
constexpr fraction& at(matrix& t, int i, int j) { return t[i][j]; }
#elif CONT == 1
using matrix = std::array<fraction, N * N>;
constexpr fraction& at(matrix& t, int i, int j) { return t[i * N + j]; }
#elif CONT == 2
struct matrix {
  fraction a[N][N];
};
constexpr fraction& at(matrix& t, int i, int j) { return t.a[i][j]; }
#elif CONT == 3
struct matrix {
  std::array<std::array<fraction, N>, N> a;
};
constexpr fraction& at(matrix& t, int i, int j) { return t.a[i][j]; }
#endif
constexpr const fraction& at(const matrix& t, int i, int j) {
  return at(const_cast<matrix&>(t), i, j);
}

#if INIT == 6
constexpr matrix zero() { return matrix{}; }
#elif INIT == 7
template <std::size_t... J>
constexpr std::array<fraction, N> zero_row(std::index_sequence<J...>) {
  return {((void)J, fraction{})...};
}
template <std::size_t... I>
constexpr matrix zero_rows(std::index_sequence<I...>) {
  return {((void)I, zero_row(std::make_index_sequence<N>{}))...};
}
#endif

#ifdef TEMPLATE
#define MAKE make<int>
template <class>
#else
#define MAKE make
#endif
#ifdef RT_STORES
constexpr matrix make(unsigned long stores = STORES) {
#else
constexpr matrix make() {
  const unsigned long stores = STORES;
#endif
#if INIT == 0
  matrix t{};
#elif INIT == 1
  matrix t = {};
#elif INIT == 2
  matrix t = matrix();
#elif INIT == 3
  matrix t = matrix{};
#elif INIT == 4
  matrix t;
#elif INIT == 6
  matrix t = zero();
#elif INIT == 7
  matrix t = zero_rows(std::make_index_sequence<N>{});
#elif INIT == 5
  matrix t{};
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      at(t, i, j) = fraction{};
    }
  }
#endif
  for (int k = 0; k < N * N; ++k) {
    if ((stores >> k) & 1) {
#if STORE == 0
      at(t, k / N, k % N) = ONE;
#else
      at(t, k / N, k % N).n = 1;
#endif
    }
  }
  return t;
}

#if ORDER == 0 && CE == 1
constexpr matrix once = MAKE();
#elif ORDER == 0 && CE == 2
static_assert(at(MAKE(), 0, 0).d == 1);
#elif ORDER == 0 && CE == 3
constinit matrix once = MAKE();
#elif ORDER == 0 && CE == 4
template <long D> struct tag {};
tag<at(MAKE(), 0, 0).d> tagged;
#elif ORDER == 0 && CE == 5
const matrix g = MAKE();
#endif

#if CE == 6
long first() {
  static const matrix s = MAKE();
  return at(s, 0, 0).d;
}
#elif CE == 7
long first() {
  const matrix x = MAKE();
  return at(x, 0, 0).d;
}
#elif CE == 8
long first() {
  matrix x = MAKE();
  return at(x, 0, 0).d;
}
#elif CE == 9
long first() { return at(MAKE(), 0, 0).d; }
#endif

int main() {
#if CE >= 6
  std::printf("first %ld\n", first());
#endif
#ifdef RT_STORES
  volatile unsigned long rt = RT_STORES;
  const matrix t = MAKE(rt);
#else
  const matrix t = MAKE();
#endif
  unsigned long mask = 0;
  for (int k = 0; k < N * N; ++k) {
    if (at(t, k / N, k % N).d != 1) {
      mask |= 1ul << k;
    }
  }
  std::printf("wrong %#lx\n", mask);
  return mask == 0 ? 0 : 1;
}

#if ORDER == 1 && CE == 1
constexpr matrix once = MAKE();
#elif ORDER == 1 && CE == 2
static_assert(at(MAKE(), 0, 0).d == 1);
#elif ORDER == 1 && CE == 3
constinit matrix once = MAKE();
#elif ORDER == 1 && CE == 4
template <long D> struct tag {};
tag<at(MAKE(), 0, 0).d> tagged;
#elif ORDER == 1 && CE == 5
const matrix g = MAKE();
#endif
