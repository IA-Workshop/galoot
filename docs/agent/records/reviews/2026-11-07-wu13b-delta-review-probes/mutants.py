"""Mutants of transform's rounding check: the sign of the dropped part, the
middle blades of the magnitude, the margin gate, the finite-math
guard, and the padding. Applies one at a time, builds the judging targets,
runs ctest -L core.transform, and restores the header with git."""
import subprocess, sys, pathlib, json, re
ROOT = pathlib.Path("/home/brianm/projects/utils/math-core/galoot/.claude/worktrees/review-wu13b-delta")
H = ROOT / "include/galoot/core/transform.hpp"
M = {
 "size_signed": (
  "const T size = dropped[j] < T{} ? static_cast<T>(-dropped[j]) : dropped[j];",
  "const T size = dropped[j];"),
 "middle_magnitude_restricted": (
  "padded_magnitude<T, sandwich_middle_layout<Alg, V, X>.size()>(\n          first_kernel<Alg, V, X, true>",
  "padded_magnitude<T, sandwich_middle_layout<Alg, V, X>.size()>(\n          first_kernel<Alg, V, X, false>"),
 "margin_always_holds": (
  "return digits >= 62 || product < (std::uint64_t{1} << digits);",
  "return true;"),
 "margin_product_squared": (
  "const std::uint64_t product = (count + 1) * ((2 * count) + 1);",
  "const std::uint64_t product = (count + 1) * (count + 1);"),
 "gate_removed": (
  "                  detail::transform_rounding_checked<Alg, V, X, T>) {\n      if (detail::checking_now()) {",
  "                  detail::rounding_checked_scalar<T>) {\n      if (detail::checking_now()) {"),
 "finite_math_guard_removed": (
  "#elif defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ == 1 &&",
  "#elif 0 && defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ == 1 &&"),
 "finite_math_guard_ignores_checking": (
  "#elif defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ == 1 &&            \\\n    GALOOT_CHECKING == 1",
  "#elif defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ == 1"),
 "padding_back_to_lambda": (
  "  constexpr T padding =\n      std::numeric_limits<T>::min() / (std::numeric_limits<T>::epsilon() / 2);",
  "  constexpr T padding = std::numeric_limits<T>::min();"),
}
TARGETS = ["core_transform_rounding_test", "core_transform_rounding_mode_1",
           "core_transform_rounding_mode_0", "core_transform_test"]
def timed(step, preset, sel, cmd):
    return subprocess.run([str(ROOT / "tools/timed"), step, preset, sel, "--", *cmd],
                          cwd=ROOT, capture_output=True, text=True)
def main():
    presets = sys.argv[1:]
    names = list(M)
    results = []
    orig = H.read_text()
    for name in names:
        old, new = M[name]
        assert orig.count(old) == 1, name
        for p in presets:
            H.write_text(orig.replace(old, new))
            try:
                b = timed("build", p, f"mutant:{name}", ["ninja", "-C", f"build/{p}", "-k", "0", *TARGETS])
                errs = sorted(set(re.findall(r"^(\S+:\d+:\d+: error: [^\n]{0,90})", b.stdout + b.stderr, re.M)))
                t = timed("test", p, f"mutant:{name}", ["ctest", "--test-dir", f"build/{p}", "-L", "core.transform", "--output-on-failure"])
                failed = re.findall(r"\d+ - (\S+) \(", t.stdout)
                results.append({"mutant": name, "preset": p, "build_exit": b.returncode,
                                "errors": errs[:3], "failed": failed})
                print(json.dumps(results[-1]), flush=True)
            finally:
                H.write_text(orig)
            if True:
                # no stale mutated binary may judge the next mutant
                timed("build", p, "mutant-restore", ["ninja", "-C", f"build/{p}", *TARGETS])
    subprocess.run(["git", "diff", "--exit-code", "--", str(H)], cwd=ROOT, check=True)
main()
