"""Writes the probe: a copy of test/core/transform_identities_test.cpp as it
was at f09007f, with the entry bounds of its hyperplanes (HYP_BOUND) and of
its vectors and multivectors (VEC_BOUND) taken from macros.

Usage: python3 -I make_probe.py <transform_identities_test.cpp at f09007f>
       <probe.cpp>

Built with -I inc first, so that <support/rational.hpp> is the probe's
128-bit rational, which records in MAXBITS the most bits any numerator,
denominator, product, or sum it forms needs, and with
-DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN, for example:
  git show f09007f:test/core/transform_identities_test.cpp > old.cpp
  python3 -I make_probe.py old.cpp probe.cpp
  g++ -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN -DDOCTEST_CONFIG_SUPER_FAST_ASSERTS
      -DGALOOT_CHECKING=1 -DHYP_BOUND=1 -DVEC_BOUND=2 -Iinc -I<root>/include
      -I<root>/test -isystem <root>/include/ext -O1 -std=c++23
      -o probe_h1_v2 probe.cpp
  ./sweep.sh ./probe_h1_v2 0 2000 > bits-h1_v2.txt
The test rational holds a value only when it needs at most 63 bits.
"""
import sys

text = open(sys.argv[1]).read()
edits = [
    ("""template <class X>
X random_small(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> digit(-2, 2);""",
     """template <class X, int B = VEC_BOUND>
X random_small(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> digit(-B, B);"""),
    ("return random_small<H>(source);",
     "return random_small<H, HYP_BOUND>(source);"),
    ("core::compose(random_small<H>(source), first)",
     "core::compose(random_small<H, HYP_BOUND>(source), first)"),
]
for old, new in edits:
    if text.count(old) != 1:
        sys.exit(f"make_probe: expected one {old!r}")
    text = text.replace(old, new)
open(sys.argv[2], "w").write(text)
