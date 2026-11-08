// The 128-bit rational of make_probe.py, a measuring copy of
// test/support/rational.hpp: the same arithmetic over __int128, recording
// the most bits a value needs and printing it as MAXBITS at exit. Not built
// by the project.
#ifndef PROBE_RATIONAL
#define PROBE_RATIONAL
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <type_traits>
namespace test_support {
__extension__ using wide = __int128;
inline int max_bits = 0;
inline void note(wide x) { if (x < 0) x = -x; int b = 0; while (x) { ++b; x >>= 1; } if (b > max_bits) max_bits = b; }
inline wide gcd(wide a, wide b) { if (a<0) a=-a; if (b<0) b=-b; while (b) { wide r = a % b; a = b; b = r; } return a; }
struct rational {
  wide numerator = 0; wide denominator = 1;
  constexpr rational() = default;
  explicit rational(wide n, wide d = 1) { wide g = gcd(n, d); wide s = d < 0 ? -1 : 1; numerator = s*(n/g); denominator = s*(d/g); note(numerator); note(denominator); }
  template <class I> requires std::is_integral_v<I> explicit rational(I n) : rational(wide{n}, wide{1}) {}
  bool operator==(const rational&) const = default;
};
inline wide mul(wide a, wide b) { if (a != 0 && (b > 0 ? b : -b) > ((wide{1} << 125) / (a > 0 ? a : -a))) { std::fputs("probe overflow\n", stderr); std::abort(); } note(a*b); return a*b; }
inline rational operator-(rational a) { return rational{-a.numerator, a.denominator}; }
inline rational operator+(rational a, rational b) { wide g = gcd(a.denominator, b.denominator); wide as = b.denominator/g, bs = a.denominator/g; wide x = mul(a.numerator, as), y = mul(b.numerator, bs); note(x+y); return rational{x + y, mul(a.denominator, as)}; }
inline rational operator-(rational a, rational b) { return a + (-b); }
inline rational operator*(rational a, rational b) { wide g = gcd(a.numerator, b.denominator), h = gcd(b.numerator, a.denominator); return rational{mul(a.numerator/g, b.numerator/h), mul(a.denominator/h, b.denominator/g)}; }
inline rational operator/(rational a, rational b) { return a * rational{b.denominator, b.numerator}; }
struct reporter { ~reporter() { std::fprintf(stdout, "MAXBITS %d\n", max_bits); } };
inline reporter report;
}
template <> struct std::numeric_limits<test_support::rational> { static constexpr bool is_specialized = true; static constexpr bool is_exact = true; };
#endif
