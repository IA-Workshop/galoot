// Spike F: does a requires-expression see a missing name? Compile with the
// S4 flags and -fsyntax-only; the compile fails on purpose, and the error
// names the packed answers through an incomplete type. Bit 0: get<"e1">(line) well-formed;
// bit 1: line["e1"] well-formed; bit 2: get<"e41">(line); bit 3: line["e41"].
#include "fixture.hpp"
using namespace cdf::rga3_types;
template <class X> constexpr bool g_e1 = requires(const X& x) { cdf::get<"e1">(x); };
template <class X> constexpr bool s_e1 = requires(const X& x) { x["e1"]; };
template <class X> constexpr bool g_e41 = requires(const X& x) { cdf::get<"e41">(x); };
template <class X> constexpr bool s_e41 = requires(const X& x) { x["e41"]; };
template <int A> struct answers;
answers<(g_e1<Line> ? 1 : 0) + (s_e1<Line> ? 2 : 0) + (g_e41<Line> ? 4 : 0) + (s_e41<Line> ? 8 : 0)> show;
