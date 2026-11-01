#!/usr/bin/env -S python3 -I
"""Copies of a Galoot include/ tree whose metric_like is written another way.

Usage:
  variants.py <include> <out-dir>

Writes <out-dir>/<name>/include for every variant below and prints the names.
Each copy differs from <include> only in the definition of metric_like (and
the helpers it needs) in galoot/core/metric.hpp. None of these is library code;
they exist to be compiled and measured side by side.

  base           the header as given
  type           the type test alone (no value is read): the cost floor of
                 any scheme that trusts how the value was built
  single         one pass that reads each entry once, as one atomic
                 constraint that is false when the value is not a constant
  single-guarded the same pass, guarded as the header guards today (the pass
                 inside std::bool_constant, then the pass again)
  cached-orig    today's three conditions, held in a variable template per tag
                 behind a guard that reads one entry
  cached         the one pass, held in a variable template per tag behind a
                 guard that reads one entry
  cached-flat    as cached, but the pass reads a copy of the matrix made by
                 std::bit_cast into a plain array, so that each read is a
                 built-in subscript rather than a call of std::array's
"""

import pathlib
import re
import shutil
import sys

CONCEPT = re.compile(
    r"/// A tag type that names an algebra and carries its metric\..*?"
    r"concept metric_like =\n.*?;\n", re.S)

DOC = """/// A tag type that names an algebra and carries its metric.
///
/// Requires of Alg: a static data member `value`, usable in a constant
/// expression, of type metric<N> with N >= 1, a symmetric Gram matrix, and
/// every entry within max_form_entry in magnitude. The concept is false
/// rather than an error for a type that has no such member.
"""

TYPE_TEST = """template <class Alg>
concept metric_like =
    detail::is_metric_v<std::remove_cvref_t<decltype(Alg::value)>>;
"""

PASS = """namespace detail {

// Whether m is symmetric with every entry within max_form_entry. Each pair
// g_ij, g_ji is read once, for j <= i; when the two are equal, the range of
// one is the range of both.
template <std::size_t N>
constexpr bool is_valid_form(const metric<N>& m) noexcept {
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j <= i; ++j) {
      const entry_type a = m.gram[i][j];
      const entry_type b = m.gram[j][i];
      if (a != b || a < -max_form_entry || a > max_form_entry) {
        return false;
      }
    }
  }
  return true;
}

} // namespace detail

"""

FLAT_PASS = """namespace detail {

template <std::size_t N>
struct flat_gram {
  // NOLINTNEXTLINE(*-avoid-c-arrays): built-in subscripts, read without a call.
  entry_type e[N][N];
};

// As is_valid_form in the other variants, on a copy of the matrix whose
// reads are built-in subscripts.
template <std::size_t N>
constexpr bool is_valid_form(const metric<N>& m) noexcept {
  static_assert(sizeof(flat_gram<N>) == sizeof(m.gram));
  const flat_gram<N> g = std::bit_cast<flat_gram<N>>(m.gram);
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j <= i; ++j) {
      const entry_type a = g.e[i][j];
      const entry_type b = g.e[j][i];
      if (a != b || a < -max_form_entry || a > max_form_entry) {
        return false;
      }
    }
  }
  return true;
}

} // namespace detail

"""

FORM = "std::remove_cvref_t<decltype(Alg::value)>"

SINGLE = f"""template <class Alg>
concept metric_like =
    detail::is_metric_v<{FORM}> &&
    ({FORM}::generator_count >= 1) &&
    std::bool_constant<detail::is_valid_form(Alg::value)>::value;
"""

SINGLE_GUARDED = f"""template <class Alg>
concept metric_like =
    detail::is_metric_v<{FORM}> &&
    requires {{ typename std::bool_constant<detail::is_valid_form(Alg::value)>; }} &&
    ({FORM}::generator_count >= 1) && detail::is_valid_form(Alg::value);
"""

CACHE_PASS = """namespace detail {

// One evaluation per tag and translation unit: later checks read the stored
// result.
template <class Alg>
inline constexpr bool valid_form_v = is_valid_form(Alg::value);

} // namespace detail

"""

CACHE_ORIG = """namespace detail {

template <class Alg>
inline constexpr bool valid_form_v =
    Alg::value.is_symmetric() && Alg::value.entries_in_range();

} // namespace detail

"""

# Reading one entry decides whether the value can be read at compile time at
# all, so the variable template's initializer is a constant expression
# whenever it is reached.
CACHED = f"""template <class Alg>
concept metric_like =
    detail::is_metric_v<{FORM}> &&
    ({FORM}::generator_count >= 1) &&
    requires {{
      typename std::integral_constant<entry_type, Alg::value.gram[0][0]>;
    }} && detail::valid_form_v<Alg>;
"""

VARIANTS = {
    "base": None,
    "type": DOC + TYPE_TEST,
    "single": PASS + DOC + SINGLE,
    "single-guarded": PASS + DOC + SINGLE_GUARDED,
    "cached-orig": CACHE_ORIG + DOC + CACHED,
    "cached": PASS + CACHE_PASS + DOC + CACHED,
    "cached-flat": FLAT_PASS + CACHE_PASS + DOC + CACHED,
}


def make(include, out):
    trees = {}
    for name, text in VARIANTS.items():
        tree = out / name / "include"
        if tree.parent.exists():
            shutil.rmtree(tree.parent)
        shutil.copytree(include, tree)
        if text is not None:
            path = tree / "galoot/core/metric.hpp"
            source = path.read_text(encoding="utf-8")
            source, count = CONCEPT.subn(lambda _m, t=text: t, source)
            assert count == 1, f"{name}: metric_like not found"
            if name == "cached-flat":
                source = source.replace("#include <array>\n",
                                        "#include <array>\n#include <bit>\n")
            path.write_text(source, encoding="utf-8")
        trees[name] = tree
    return trees


def main(argv):
    if len(argv) != 2:
        sys.exit(__doc__)
    trees = make(pathlib.Path(argv[0]).resolve(),
                 pathlib.Path(argv[1]).resolve())
    for name, tree in trees.items():
        print(name, tree)


if __name__ == "__main__":
    main(sys.argv[1:])
