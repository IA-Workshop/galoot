#!/usr/bin/env -S python3 -I
"""Mutants of the set-bit loops, written from
the loops and their bounds: reorder_sign in bits.hpp, the contraction of
add_generator_product and the correction of add_blade_product in
blade_product.hpp. Each mutant is applied to the checkout's headers, `all`
is built, the whole suite runs if the build passed, and the headers are
restored. A mutant is killed when the build or any test fails.

With --runtime, every static_assert( of bits_test.cpp, blade_product_test.cpp,
and plan_test.cpp becomes static_assert(true ? true : ..., so the run-time
cases must kill the mutant.

Usage, from the root of a scratch checkout with the job pool exported:
  mutate_review.py <checkout> <preset> [--runtime]
"""
import pathlib
import re
import subprocess
import sys

CT_HEAD = ("    for (mask_t remaining = blade; remaining != 0; "
           "remaining &= remaining - 1) {\n")
CR_HEAD = ("  for (mask_t remaining = rest; remaining != 0; "
           "remaining &= remaining - 1) {\n")
CT_INDEX = ("      const auto j = static_cast<std::size_t>("
            "std::countr_zero(remaining));\n")
CR_INDEX = ("    const auto j = static_cast<std::size_t>("
            "std::countr_zero(remaining));\n")
CT_FLIP = ("        out.add(blade & ~(mask_t{1} << j),\n"
           "                multiply_checked(multiply_checked(sign, entry), c));\n"
           "      }\n"
           "      sign = -sign;\n")
CR_FLIP = ("      add_blade_product(out, g, rest & ~(mask_t{1} << j), b,\n"
           "                        multiply_checked(multiply_checked(sign, entry), scale));\n"
           "    }\n"
           "    sign = -sign;\n")
RS_COUNT = "    swaps += std::popcount(b & ((mask_t{1} << i) - 1U));\n"
RS_INDEX = "    const auto i = static_cast<unsigned>(std::countr_zero(a));\n"


def flip_inside(text):
    """Moves `sign = -sign;` inside the preceding `if (entry != 0)` block."""
    lines = text.split("\n")
    closing, flip = lines[-3], lines[-2]
    return "\n".join(lines[:-3] + [closing.replace("}", "  ") + flip.strip(),
                                    closing, ""])


B, P = "bits", "product"
MUTANTS = [
    ("rv_any_below", B, RS_COUNT,
     "    swaps += (b & ((mask_t{1} << i) - 1U)) != 0 ? 1 : 0;\n"),
    ("rv_only_adjacent", B, RS_COUNT,
     "    swaps += std::popcount(b & ((mask_t{1} << i) >> 1U));\n"),
    ("rv_count_own", B, RS_COUNT,
     "    swaps += std::popcount(a & ((mask_t{1} << i) - 1U));\n"),
    ("rv_index_mod16", B, RS_INDEX,
     "    const auto i = static_cast<unsigned>(std::countr_zero(a)) & 15U;\n"),
    ("rv_index_mod8", B, RS_INDEX,
     "    const auto i = static_cast<unsigned>(std::countr_zero(a)) & 7U;\n"),
    ("ct_flip_on_nonzero", P, CT_FLIP, None),
    ("ct_skip_e_N", P, CT_HEAD,
     "    for (mask_t remaining = blade & ~(mask_t{1} << (N - 1)); "
     "remaining != 0; remaining &= remaining - 1) {\n"),
    ("ct_skip_own_generator", P, CT_HEAD,
     "    for (mask_t remaining = blade & ~generator; remaining != 0; "
     "remaining &= remaining - 1) {\n"),
    ("ct_index_mod16", P, CT_INDEX,
     "      const auto j = static_cast<std::size_t>("
     "std::countr_zero(remaining)) & 15U;\n"),
    ("ct_over_blade_with_i", P, CT_HEAD,
     "    for (mask_t remaining = blade | generator; remaining != 0; "
     "remaining &= remaining - 1) {\n"),
    ("cr_flip_on_nonzero", P, CR_FLIP, None),
    ("cr_skip_e_N", P, CR_HEAD,
     "  for (mask_t remaining = rest & ~(mask_t{1} << (N - 1)); "
     "remaining != 0; remaining &= remaining - 1) {\n"),
    ("cr_over_a", P, CR_HEAD,
     "  for (mask_t remaining = a; remaining != 0; "
     "remaining &= remaining - 1) {\n"),
    ("cr_index_mod16", P, CR_HEAD + CR_INDEX,
     CR_HEAD + "    const auto j = static_cast<std::size_t>("
     "std::countr_zero(remaining)) & 15U;\n"),
    ("cr_sign_starts_plus", P, "  coefficient_type sign = -1;\n",
     "  coefficient_type sign = 1;\n"),
]
STATIC_ASSERT = re.compile(r"([\w./-]+\.(?:cpp|hpp)):(\d+):\d+: error: "
                           r"(?:static assertion|static_assert)")


def main(argv):
    root = pathlib.Path(argv[0]).resolve()
    preset = argv[1]
    runtime = "--runtime" in argv[2:]
    files = {B: root / "include/galoot/core/bits.hpp",
             P: root / "include/galoot/core/blade_product.hpp"}
    tests = [root / f"test/core/{t}_test.cpp"
             for t in ("bits", "blade_product", "plan")]
    originals = {p: p.read_text(encoding="utf-8")
                 for p in [*files.values(), *tests]}
    mode = "runtime" if runtime else "normal"

    def run(args):
        return subprocess.run(args, cwd=root, capture_output=True, text=True)

    print("mutant\tpreset\tmode\tbuild\ttest\tstatus\tfailed_tests\t"
          "static_asserts\tfirst_error")
    try:
        if runtime:
            for t in tests:
                t.write_text(originals[t].replace(
                    "static_assert(", "static_assert(true ? true : "),
                    encoding="utf-8")
        for name, which, anchor, replacement in MUTANTS:
            text = originals[files[which]]
            assert text.count(anchor) == 1, name
            new = flip_inside(anchor) if replacement is None else replacement
            files[which].write_text(text.replace(anchor, new),
                                    encoding="utf-8")
            build = run(["tools/timed", "build", preset,
                         f"review-mutant:{name}:{mode}", "--", "cmake",
                         "--build", "--preset", preset])
            test_rc, failed = "-", []
            if build.returncode == 0:
                test = run(["tools/timed", "test", preset,
                            f"review-mutant:{name}:{mode}", "--", "ctest",
                            "--preset", preset])
                test_rc = test.returncode
                failed = [ln.split(" - ")[1].split()[0]
                          for ln in test.stdout.splitlines()
                          if re.match(r"\s*\d+ - ", ln)]
            killed = build.returncode != 0 or test_rc != 0
            errors = [ln for ln in build.stdout.splitlines() if "error:" in ln]
            asserts = sorted({f"{m.group(1).split('/')[-1]}:{m.group(2)}"
                              for m in STATIC_ASSERT.finditer(build.stdout)})
            first = errors[0].split(str(root) + "/")[-1][:160] if errors else "-"
            print(f"{name}\t{preset}\t{mode}\t{build.returncode}\t{test_rc}\t"
                  f"{'killed' if killed else 'SURVIVED'}\t"
                  f"{','.join(failed) or '-'}\t{','.join(asserts) or '-'}\t"
                  f"{first}", flush=True)
            files[which].write_text(text, encoding="utf-8")
    finally:
        for p, text in originals.items():
            p.write_text(text, encoding="utf-8")
        run(["tools/timed", "build", preset, "review-mutant:restored", "--",
             "cmake", "--build", "--preset", preset])


if __name__ == "__main__":
    main(sys.argv[1:])
