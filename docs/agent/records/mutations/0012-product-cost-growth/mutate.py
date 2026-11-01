"""Mutation runs for the set-bit loops of reorder_sign (bits.hpp) and of
add_generator_product and add_blade_product (blade_product.hpp): apply one
mutation, build `all`, run the whole suite, and restore the header. bits.hpp
is included by every core header, so every test can be affected. A mutant is
killed when the build fails or any test fails. Each mutation's anchors must
occur exactly once.

For a failed build the record gives the first error line and the
static_asserts that failed, as file:line, so that a kill by a static_assert
can be told from a kill by another error. CTest is not run after a failed
build: the mutant is killed already, and CTest run in parallel under the
shared job pool can keep a slot it took for a test whose program is missing.
After the last mutant the headers are restored and `all` is built again.

With --runtime, every static_assert of the three doctest files that use the
changed headers (bits_test.cpp, blade_product_test.cpp, plan_test.cpp)
becomes `static_assert(true ? true : ...)` for the run, so that their programs
build on a mutant those checks would reject, and the record lists the test
cases that fail at run time. The conditional operator binds more loosely
than every operator a condition uses, so the condition stays one operand
and no -Wparentheses warning ends the build, as `true || a && b` would. The
checks inside plan.hpp stay.

With --without-top-pairing, the test case "blade_product reads a pairing
with the top generator of the widest algebra" is removed for the run, to
show which mutants only that case kills.

Usage, from the worktree root with the job pool exported:
  python3 -I docs/agent/records/mutations/0012-product-cost-growth/mutate.py \\
      <preset> [--runtime] [--without-top-pairing] [mutation...]
"""
import re
import subprocess
import sys

ROOT = subprocess.run(["git", "rev-parse", "--show-toplevel"],
                      capture_output=True, text=True, check=True).stdout.strip()
BITS = f"{ROOT}/include/galoot/core/bits.hpp"
PRODUCT = f"{ROOT}/include/galoot/core/blade_product.hpp"
TESTS = [f"{ROOT}/test/core/{name}_test.cpp"
         for name in ("bits", "blade_product", "plan")]
PRODUCT_TEST = TESTS[1]

TOP_PAIRING_START = ("// Reference: independent value: in the form of 32 "
                     "generators whose only\n")
TOP_PAIRING_END = "  CHECK(product(witt, pair, pair) == written({{0, 1}}));\n}\n"

RS_LOOP = "  for (; a != 0; a &= a - 1) {\n"
RS_INDEX = "    const auto i = static_cast<unsigned>(std::countr_zero(a));\n"
RS_BELOW = "(mask_t{1} << i) - 1U"

CT_LOOP = ("    for (mask_t remaining = blade; remaining != 0; "
           "remaining &= remaining - 1) {\n"
           "      const auto j = static_cast<std::size_t>("
           "std::countr_zero(remaining));\n")
CR_LOOP = ("  for (mask_t remaining = rest; remaining != 0; "
           "remaining &= remaining - 1) {\n"
           "    const auto j = static_cast<std::size_t>("
           "std::countr_zero(remaining));\n")


def loop_mutants(prefix, loop, first):
    """The bound mutants of one set-bit loop over `first`."""
    indent = loop[:len(loop) - len(loop.lstrip())]
    inner = indent + "  "
    head = f"{indent}for (mask_t remaining = {first}; "
    step = "remaining &= remaining - 1) {\n"
    index = (f"{inner}const auto j = static_cast<std::size_t>("
             "std::countr_zero(remaining));\n")
    top = "static_cast<unsigned>(std::bit_width(remaining) - 1)"
    return [
        (f"{prefix}_skip_lowest", [(loop, (
            f"{indent}for (mask_t remaining = {first} & ({first} - 1); "
            f"remaining != 0; {step}{index}"))]),
        (f"{prefix}_skip_highest", [(loop, (
            f"{head}(remaining & (remaining - 1)) != 0; {step}{index}"))]),
        (f"{prefix}_top_bit_ignored", [(loop, (
            f"{indent}for (mask_t remaining = {first} & "
            "~(mask_t{1} << (mask_bits - 1)); "
            f"remaining != 0; {step}{index}"))]),
        (f"{prefix}_index_plus_one", [(loop, (
            f"{head}remaining != 0; {step}"
            f"{inner}const auto j = static_cast<std::size_t>("
            "std::countr_zero(remaining)) + 1;\n"))]),
        (f"{prefix}_highest_first", [(loop, (
            f"{head}remaining != 0; "
            f"remaining &= ~(mask_t{{1}} << {top})) {{\n"
            f"{inner}const auto j = static_cast<std::size_t>({top});\n"))]),
    ]


# (name, [(file, anchor, replacement), ...])
MUTATIONS = [
    ("rs_skip_lowest",
     [(BITS, RS_LOOP, "  for (a &= a - 1; a != 0; a &= a - 1) {\n")]),
    ("rs_skip_highest",
     [(BITS, RS_LOOP, "  for (; (a & (a - 1)) != 0; a &= a - 1) {\n")]),
    ("rs_top_bit_ignored",
     [(BITS, RS_LOOP, "  for (a &= ~(mask_t{1} << (mask_bits - 1)); a != 0; "
       "a &= a - 1) {\n")]),
    ("rs_below_inclusive", [(BITS, RS_BELOW, "(mask_t{2} << i) - 1U")]),
    ("rs_below_one_short",
     [(BITS, RS_BELOW, "((mask_t{1} << i) - 1U) >> 1U")]),
    ("rs_highest_index",
     [(BITS, RS_INDEX,
       "    const auto i = static_cast<unsigned>(std::bit_width(a)) - 1U;\n")]),
] + [(name, [(PRODUCT, a, r) for a, r in edits])
     for name, edits in loop_mutants("ct", CT_LOOP, "blade")
     + loop_mutants("cr", CR_LOOP, "rest")]

STATIC_ASSERT = re.compile(
    r"([\w./-]+\.(?:cpp|hpp)):(\d+):\d+: error: "
    r"(?:static assertion|static_assert)")


def run(args):
    return subprocess.run(args, cwd=ROOT, capture_output=True, text=True)


def failed_cases(output):
    names = []
    for line in output.splitlines():
        if line.startswith("TEST CASE:"):
            name = line[len("TEST CASE:"):].strip()
            if name not in names:
                names.append(name)
    return names


def write(path, text):
    with open(path, "w", encoding="utf-8") as f:
        f.write(text)


def main():
    args = sys.argv[1:]
    preset = args[0]
    runtime = "--runtime" in args[1:]
    without_top = "--without-top-pairing" in args[1:]
    only = {a for a in args[1:] if not a.startswith("--")}
    originals = {path: open(path, encoding="utf-8").read()
                 for path in [BITS, PRODUCT, *TESTS]}

    edited_tests = {path: originals[path] for path in TESTS}
    if runtime:
        for path in TESTS:
            count = edited_tests[path].count("static_assert(")
            assert count > 0, path
            edited_tests[path] = edited_tests[path].replace(
                "static_assert(", "static_assert(true ? true : ")
    if without_top:
        text = edited_tests[PRODUCT_TEST]
        start = text.index(TOP_PAIRING_START)
        end = text.index(TOP_PAIRING_END, start) + len(TOP_PAIRING_END)
        edited_tests[PRODUCT_TEST] = text[:start] + text[end:]

    print("mutation\tpreset\tmode\tbuild_exit\ttest_exit\tkilled\t"
          "failed_tests\tstatic_asserts\tfailed_cases\tfirst_build_error")
    mode = ("runtime" if runtime else "normal") + (
        "-without-top-pairing" if without_top else "")
    try:
        for path, text in edited_tests.items():
            write(path, text)
        for name, edits in MUTATIONS:
            if only and name not in only:
                continue
            mutated = {path: originals[path] for path in (BITS, PRODUCT)}
            for path, anchor, replacement in edits:
                assert mutated[path].count(anchor) == 1, (name, anchor)
                mutated[path] = mutated[path].replace(anchor, replacement)
            for path, text in mutated.items():
                write(path, text)
            build = run(["tools/timed", "build", preset,
                         f"mutation:{name}:{mode}", "--", "cmake", "--build",
                         "--preset", preset])
            if build.returncode == 0:
                test = run(["tools/timed", "test", preset,
                            f"mutation:{name}:{mode}", "--", "ctest",
                            "--preset", preset, "--output-on-failure"])
                test_exit, output = test.returncode, test.stdout
            else:
                test_exit, output = "-", ""
            failed = [line.split(" - ")[1].split()[0]
                      for line in output.splitlines()
                      if re.match(r"\s*\d+ - ", line)]
            killed = build.returncode != 0 or test_exit != 0
            errors = [line for line in build.stdout.splitlines()
                      if "error:" in line]
            first_error = errors[0].split(ROOT + "/")[-1] if errors else "-"
            asserts = sorted({f"{m.group(1).split('/')[-1]}:{m.group(2)}"
                              for m in STATIC_ASSERT.finditer(build.stdout)})
            print(f"{name}\t{preset}\t{mode}\t{build.returncode}\t{test_exit}\t"
                  f"{'killed' if killed else 'SURVIVED'}\t"
                  f"{','.join(failed) or '-'}\t{','.join(asserts) or '-'}\t"
                  f"{' | '.join(failed_cases(output)) or '-'}\t"
                  f"{first_error[:200]}", flush=True)
    finally:
        for path, text in originals.items():
            write(path, text)
        run(["tools/timed", "build", preset, "mutation:restored", "--", "cmake",
             "--build", "--preset", preset])


main()
