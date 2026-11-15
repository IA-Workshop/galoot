"""Reviewer's own mutants of transform.hpp and dual.hpp (WU13a review).

  python3 -I mut.py <worktree> [mutant...]

Each mutant is applied to the worktree's header, the judging programs are
built on gcc-debug and clang-debug (ninja -k 0), the core.transform and
core.dual labels run, and the header is restored.
"""
import pathlib, re, subprocess, sys, concurrent.futures as cf

ROOT = pathlib.Path(sys.argv[1]).resolve()
OUT = pathlib.Path(__file__).resolve().parent / "mutants.tsv"
TARGETS = ["core_transform_test", "core_transform_identities_test",
           "core_transform_mode_1", "core_transform_mode_0", "core_dual_test",
           "core_dual_identities_test"]
LABELS = r"^core\.(transform|dual)$"
PRESETS = ["gcc-debug", "clang-debug"]
T, D = "transform.hpp", "dual.hpp"

MUTANTS = {
    # first stage: involution sign read from V's blade instead of x's
    "inv_from_v_blade": (T, [("odd && antigrade(X.blades[t.b].mask, n) % 2 != 0 ? -1 : 1;",
                              "odd && antigrade(V.blades[t.a].mask, n) % 2 != 0 ? -1 : 1;")]),
    # involution applied for an even V and not for an odd one
    "inv_for_even_not_odd": (T, [("odd && antigrade(X.blades[t.b].mask, n) % 2 != 0 ? -1 : 1;",
                                  "!odd && antigrade(X.blades[t.b].mask, n) % 2 != 0 ? -1 : 1;")]),
    # antireverse sign computed with n - 1 generators (off by one)
    "antireverse_n_minus_1": (T, [("return odd * antireverse_sign(V.blades[b].mask, n);",
                                   "return odd * antireverse_sign(V.blades[b].mask, n - 1);")]),
    # the odd sign chosen by grade parity of V (flips for both parities in odd n)
    "odd_sign_by_grade_parity": (T, [("const coefficient_type odd = antigrade_parity<Alg, V> == Parity::odd ? -1 : 1;",
                                      "const coefficient_type odd = (antigrade_parity<Alg, V> == Parity::odd) != (n % 2 == 1) ? -1 : 1;")]),
    # restricted first stage keeps every term (cost only)
    "first_keeps_all": (T, [("count += All || read[t.out] ? 1 : 0;", "count += (static_cast<void>(read[t.out]), 1);"),
                            ("if (!All && !read[t.out]) {", "if (false && !All && !read[t.out]) {")]),
    # checking path computes the dropped part from the restricted middle
    "dropped_from_restricted_middle": (T, [("const auto middle = detail::transform_middle<Alg, V, X, true>(",
                                            "const auto middle = detail::transform_middle<Alg, V, X, false>(")]),
    # the check skips the last dropped coefficient
    "check_skips_last": (T, [("  for (const T& d : dropped) {\n",
                              "  for (std::size_t i = 0; i + 1 < D; ++i) {\n    const T& d = dropped[i];\n")]),
    # the check runs only for built-in integers
    "check_only_integral": (T, [("if constexpr (exact_scalar<T> && std::equality_comparable<T>) {",
                                 "if constexpr (std::integral<T>) {")]),
    # compose accepts only even operands
    "compose_rejects_odd": (T, [("detail::antigrade_parity<Alg, A> != detail::Parity::mixed &&\n      detail::antigrade_parity<Alg, B> != detail::Parity::mixed;",
                                 "detail::antigrade_parity<Alg, A> == detail::Parity::even &&\n      detail::antigrade_parity<Alg, B> == detail::Parity::even;")]),
    # parity computed with n + 1 generators: every parity flipped
    "parity_n_plus_1": (T, [("parity_of(S, std::remove_cvref_t<decltype(Alg::value)>::generator_count);",
                             "parity_of(S, std::remove_cvref_t<decltype(Alg::value)>::generator_count + 1);")]),
    # every dropped term written to the first dropped place
    "dropped_to_place_0": (T, [(": dropped_places[t.out];", ": dropped_places[t.out] * 0;")]),
    # kept orientation read from V's blade instead of x's
    "orientation_from_v": (T, [("const coefficient_type orientation = Kept ? X.blades[out].sign : 1;",
                                "const coefficient_type orientation = Kept ? V.blades[t.b].sign : 1;")]),
    # antiproject: outer wedge operands swapped
    "antiproject_outer_swapped": (D, [("return wedge(b, antiwedge(a, weight_dual(b)));",
                                       "return wedge(antiwedge(a, weight_dual(b)), b);")]),
    # antiproject: inner antiwedge operands swapped
    "antiproject_inner_swapped": (D, [("return wedge(b, antiwedge(a, weight_dual(b)));",
                                       "return wedge(b, antiwedge(weight_dual(b), a));")]),
    # antiproject: left weight dual
    "antiproject_left_weight_dual": (D, [("return wedge(b, antiwedge(a, weight_dual(b)));",
                                          "return wedge(b, antiwedge(a, left_weight_dual(b)));")]),
}


def timed(step, preset, sel, cmd):
    return subprocess.run(["tools/timed", step, preset, sel, "--", *cmd],
                          cwd=ROOT, capture_output=True, text=True)


def first_errors(out):
    errs = []
    lines = out.splitlines()
    for k, line in enumerate(lines):
        if line.startswith("FAILED: "):
            m = re.search(r"(\w+)\.cpp\.o", line)
            first = next((r.strip() for r in lines[k + 1:] if "error:" in r), "-")
            errs.append(f"{m.group(1) if m else '?'}: {first.replace(str(ROOT) + '/', '')[:220]}")
    return errs


def one(preset, name):
    b = timed("build", preset, f"mutant:{name}",
              ["ninja", "-C", f"build/{preset}", "-k", "0", *TARGETS])
    t = timed("test", preset, f"mutant:{name}",
              ["ctest", "--test-dir", f"build/{preset}", "-L", LABELS])
    failed = re.findall(r"^\s*\d+ - (\S+) \(", t.stdout, re.MULTILINE)
    return [name, preset, str(b.returncode), " | ".join(first_errors(b.stdout)) or "-",
            " ".join(failed) or "-"]


def main():
    names = sys.argv[2:] or list(MUTANTS)
    rows = []
    for name in names:
        header, edits = MUTANTS[name]
        path = ROOT / "include/galoot/core" / header
        orig = path.read_text()
        text = orig
        for old, new in edits:
            assert text.count(old) == 1, (name, old)
            text = text.replace(old, new)
        path.write_text(text)
        try:
            with cf.ThreadPoolExecutor(2) as ex:
                res = list(ex.map(lambda p: one(p, name), PRESETS))
        finally:
            path.write_text(orig)
        for r in res:
            print("\t".join(r), flush=True)
            rows.append(r)
    with open(OUT, "a") as f:
        for r in rows:
            f.write("\t".join(r) + "\n")
    for p in PRESETS:
        r = timed("build", p, "mutant:restore", ["ninja", "-C", f"build/{p}", *TARGETS])
        print("restore", p, r.returncode)


main()
