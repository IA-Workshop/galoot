#!/usr/bin/env -S python3 -I
"""Mutation runs for the antiproducts, the involutions, and the complements.

  mutate.py <preset> [mutant...]

Applies one mutant at a time to a core header, builds the three test
programs that judge it (core_product_test, core_unary_test,
core_identities_test) with ninja's -k 0 so that every program is tried,
runs the tests labeled core.product or core.unary (the programs, the
product compile-fail tests, and core.identities, which carries both labels),
and restores the header. Every build and test goes through tools/timed with
the selection mutant:<name>.

A mutant is killed when a test fails: a program that does not compile
because of an error in the test's own code (a static_assert of the test, or
a call the test makes that has no overload), or a compile-fail test or a
run-time check that fails. An error that comes from a warning made an error
does not count; the record lists each compile error's first line so that
this can be read off.

Writes out/<preset>.tsv: mutant, build exit, the programs that failed to
build with the first error line of each, and the failed tests.

Usage, from the worktree root with the job pool exported.
"""

import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
CORE = ROOT / "include/galoot/core"
OUT = pathlib.Path(__file__).resolve().parent / "out"
TARGETS = ["core_product_test", "core_unary_test", "core_identities_test"]

GP_CHECK = ("constexpr bool scalar_holds_every_coefficient_of_the_product =\n"
            "      detail::coefficients_exact<detail::BinaryOperation::"
            "geometric_product,\n                                 Alg, A, B, "
            "T>;")
AP_CHECK = ("constexpr bool scalar_holds_every_coefficient_of_the_antiproduct ="
            "\n      detail::coefficients_exact<detail::BinaryOperation::"
            "geometric_antiproduct,\n                                 Alg, A, "
            "B, T>;")
GP_DROPPED = "constexpr bool scalar_holds_every_coefficient_of_the_product = true;"
AP_DROPPED = ("constexpr bool scalar_holds_every_coefficient_of_the_antiproduct "
              "= true;")
CATCH_ALL = "requires detail::mismatched_operands<X, Y>"
NO_ALGEBRA = ("requires multivector<X> && multivector<Y> && "
              "(!detail::same_scalar_v<std::remove_cvref_t<X>, "
              "std::remove_cvref_t<Y>>)")
NO_SCALAR = ("requires multivector<X> && multivector<Y> && "
             "(!detail::same_algebra_v<std::remove_cvref_t<X>, "
             "std::remove_cvref_t<Y>>)")

# name: (header, [(old, new, occurrences)]). occurrences lists which
# occurrences of old, counted from 0, are replaced; None means old occurs
# exactly once.
MUTANTS = {
    "reverse_calls_antireverse": ("unary.hpp", [(
        "apply<detail::UnaryOperation::reverse, S>(x)",
        "apply<detail::UnaryOperation::antireverse, S>(x)", None)]),
    "grade_involution_calls_reverse": ("unary.hpp", [(
        "apply<detail::UnaryOperation::grade_involution, S>(x)",
        "apply<detail::UnaryOperation::reverse, S>(x)", None)]),
    "bang_calls_left_complement": ("unary.hpp", [(
        "  return right_complement(x);", "  return left_complement(x);",
        None)]),
    "reverse_returns_canonical_layout": ("unary.hpp", [
        ("[[nodiscard]] constexpr mv<Alg, S, T> reverse(const mv<Alg, S, T>& x)"
         " {\n  return detail::apply<detail::UnaryOperation::reverse, S>(x);",
         "[[nodiscard]] constexpr auto reverse(const mv<Alg, S, T>& x) {\n"
         "  return detail::apply<detail::UnaryOperation::reverse>(x);", None),
        ("[[nodiscard]] constexpr mv<Alg, S, T> operator~(",
         "[[nodiscard]] constexpr auto operator~(", None)]),
    # The whole body goes, since a deduced return type cannot keep the
    # check's `return {};`.
    "percent_calls_geometric_product": ("product.hpp", [
        ("[[nodiscard]] constexpr mv<Alg, geometric_antiproduct_layout<Alg, A, "
         "B>, T>\noperator%(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {\n"
         + AP_CHECK.replace("constexpr bool", "  constexpr bool") + "\n"
         "  static_assert(scalar_holds_every_coefficient_of_the_antiproduct,\n"
         "                \"a % b: the scalar type cannot hold one of the "
         "antiproduct's \"\n"
         "                \"exact coefficients; use a scalar type that holds "
         "larger \"\n"
         "                \"integers exactly\");\n"
         "  if constexpr (scalar_holds_every_coefficient_of_the_antiproduct) {\n"
         "    return geometric_antiproduct(x, y);\n"
         "  } else {\n"
         "    return {};\n"
         "  }\n"
         "}",
         "[[nodiscard]] constexpr auto\noperator%(const mv<Alg, A, T>& x, "
         "const mv<Alg, B, T>& y) {\n  return geometric_product(x, y);\n}",
         None)]),
    "antiwedge_uses_antiproduct_plan": ("product.hpp", [
        ("detail::product_plan<detail::BinaryOperation::antiwedge, Alg, A, "
         "B>.result;",
         "detail::product_plan<detail::BinaryOperation::geometric_antiproduct, "
         "Alg, A, B>.result;", None),
        ("  return detail::apply<detail::BinaryOperation::antiwedge,",
         "  return detail::apply<detail::BinaryOperation::geometric_antiproduct,",
         None)]),
    "ampersand_calls_wedge": ("product.hpp", [
        ("[[nodiscard]] constexpr mv<Alg, antiwedge_layout<Alg, A, B>, T>\n"
         "operator&", "[[nodiscard]] constexpr auto\noperator&", None),
        ("  return antiwedge(x, y);", "  return wedge(x, y);", None)]),
    "antiproduct_operands_swapped": ("product.hpp", [(
        "geometric_antiproduct_layout<Alg, A, B>>(x, y);",
        "geometric_antiproduct_layout<Alg, A, B>>(y, x);", None)]),
    "check_dropped_geometric_product": ("product.hpp", [
        (GP_CHECK, GP_DROPPED, [0])]),
    "check_dropped_times": ("product.hpp", [(GP_CHECK, GP_DROPPED, [1])]),
    "check_dropped_geometric_antiproduct": ("product.hpp", [
        (AP_CHECK, AP_DROPPED, [0])]),
    "check_dropped_percent": ("product.hpp", [(AP_CHECK, AP_DROPPED, [1])]),
    # The catch-alls in order: geometric_product, wedge, *, ^,
    # geometric_antiproduct, antiwedge, %, &.
    "new_catch_alls_without_algebra": ("product.hpp", [
        (CATCH_ALL, NO_ALGEBRA, [4, 5, 6, 7])]),
    "new_catch_alls_without_scalar": ("product.hpp", [
        (CATCH_ALL, NO_SCALAR, [4, 5, 6, 7])]),
    "left_complement_sign_flipped_for_e12": ("bits.hpp", [(
        "  return reorder_sign(complement_mask(a, n), a);",
        "  return a == 0b11U ? -reorder_sign(complement_mask(a, n), a)\n"
        "                    : reorder_sign(complement_mask(a, n), a);",
        None)]),
}


def mutate(text, old, new, occurrences):
    parts = text.split(old)
    if occurrences is None:
        if len(parts) != 2:
            sys.exit(f"anchor occurs {len(parts) - 1} times: {old!r}")
        return new.join(parts)
    out = parts[0]
    for k, part in enumerate(parts[1:]):
        out += (new if k in occurrences else old) + part
    if max(occurrences) >= len(parts) - 1:
        sys.exit(f"anchor occurs {len(parts) - 1} times: {old!r}")
    return out


def timed(step, preset, selection, command):
    return subprocess.run(
        [str(ROOT / "tools/timed"), step, preset, selection, "--", *command],
        cwd=ROOT, capture_output=True, text=True)


def failed_builds(output):
    """The object files that failed and the first error line of each."""
    failures = []
    lines = output.splitlines()
    for k, line in enumerate(lines):
        if line.startswith("FAILED: "):
            source = re.search(r"(\w+)_test\.cpp\.o", line)
            first = next((rest.strip() for rest in lines[k + 1:]
                          if "error:" in rest), "-")
            first = first.replace(f"{ROOT}/", "")[:400]
            failures.append(f"{source.group(1) if source else line}: {first}")
    return failures


def run(preset, name):
    header, edits = MUTANTS[name]
    path = CORE / header
    original = path.read_text()
    text = original
    for old, new, occurrences in edits:
        text = mutate(text, old, new, occurrences)
    path.write_text(text)
    try:
        build = timed("build", preset, f"mutant:{name}",
                      ["ninja", "-C", f"build/{preset}", "-k", "0", *TARGETS])
        tests = timed("test", preset, f"mutant:{name}",
                      ["ctest", "--test-dir", f"build/{preset}", "-L",
                       r"^core\.(product|unary)$"])
    finally:
        path.write_text(original)
    failed = re.findall(r"^\s*\d+ - (\S+) \(", tests.stdout, re.MULTILINE)
    return [name, str(build.returncode),
            " | ".join(failed_builds(build.stdout)) or "-",
            " ".join(failed) or "-"]


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    preset = sys.argv[1]
    names = sys.argv[2:] or list(MUTANTS)
    OUT.mkdir(exist_ok=True)
    record = OUT / f"{preset}.tsv"
    # A run of some mutants replaces their rows and keeps the others.
    rows = {}
    if record.exists():
        for line in record.read_text(encoding="utf-8").splitlines()[1:]:
            rows[line.split("\t")[0]] = line.split("\t")
    for name in names:
        rows[name] = run(preset, name)
        print("\t".join(rows[name]), flush=True)
    header = ["mutant", "build_exit", "build_failures", "failed_tests"]
    ordered = [rows[name] for name in MUTANTS if name in rows]
    with open(record, "w", encoding="utf-8") as f:
        f.write("".join("\t".join(r) + "\n" for r in [header, *ordered]))

    # Rebuild the unmutated programs, so that the tree matches the headers.
    rebuilt = timed("build", preset, "mutant:restore",
                    ["ninja", "-C", f"build/{preset}", *TARGETS])
    if rebuilt.returncode != 0:
        sys.exit("the restored headers do not build")


if __name__ == "__main__":
    main()
