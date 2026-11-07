"""Reviewer's mutants of transform's rounding check. Applies one at a time to
the review worktree's include/galoot/core/transform.hpp, builds the judging
targets, runs ctest -L core.transform, restores the header with git."""
import subprocess, sys, pathlib, json, re
ROOT = pathlib.Path(__file__).resolve().parents[5]
H = ROOT / "include/galoot/core/transform.hpp"
M = {
 "k_sum_uncounted": ("out[t.out] = (out[t.out] < term ? term : out[t.out]) + 1;",
                     "out[t.out] = (out[t.out] < term ? term : out[t.out]);"),
 "k_sum_added": ("out[t.out] = (out[t.out] < term ? term : out[t.out]) + 1;",
                 "out[t.out] = out[t.out] + term;"),
 "k_unit_coefficient_counted": ("if (t.coefficient != 1 && t.coefficient != -1) {\n      ++term;",
                                "if (t.coefficient == 1 || t.coefficient == -1) {\n      ++term;"),
 "k_from_first_dropped": ("static_cast<T>(roundings[j] + transform_rounding_margin)",
                          "static_cast<T>(roundings[0] + transform_rounding_margin)"),
 "u_is_epsilon": ("constexpr T u = std::numeric_limits<T>::epsilon() / 2;",
                  "constexpr T u = std::numeric_limits<T>::epsilon();"),
 "abs_x_missing": ("first_kernel<Alg, V, X, true>, abs_v, absolute_values(x));",
                   "first_kernel<Alg, V, X, true>, abs_v, x);"),
 "v_signed_second_stage": ("second_kernel<Alg, V, X, false>, middle, abs_v);",
                           "second_kernel<Alg, V, X, false>, middle, v);"),
 "size_signed": ("const T size = dropped[j] < T{} ? static_cast<T>(-dropped[j]) : dropped[j];",
                 "const T size = dropped[j];"),
 "middle_magnitude_restricted": ("padded_magnitude<T, sandwich_middle_layout<Alg, V, X>.size()>(\n          first_kernel<Alg, V, X, true>",
                                 "padded_magnitude<T, sandwich_middle_layout<Alg, V, X>.size()>(\n          first_kernel<Alg, V, X, false>"),
 "padding_denorm_min": ("constexpr T padding = std::numeric_limits<T>::min();",
                        "constexpr T padding = std::numeric_limits<T>::denorm_min();"),
 "float_padding_zero": ("constexpr T padding = std::numeric_limits<T>::min();",
                        "constexpr T padding = static_cast<T>(std::numeric_limits<double>::min());"),
 "overflow_passes_if_d_finite": ("size <= tolerances[j] && tolerances[j] <= std::numeric_limits<T>::max();",
                                 "size <= tolerances[j] && (tolerances[j] <= std::numeric_limits<T>::max() || size <= std::numeric_limits<T>::max());"),
 "concept_not_exact": ("ring_scalar<T> && std::numeric_limits<T>::is_iec559 &&\n    std::numeric_limits<T>::round_style == std::round_to_nearest;",
                       "ring_scalar<T> && !std::numeric_limits<T>::is_exact && std::numeric_limits<T>::is_specialized;"),
 "gamma_no_denominator": ("const T gamma = static_cast<T>((k * u) / (T{1} - (k * u)));",
                          "const T gamma = static_cast<T>(k * u);"),
 "coefficient_padding_removed": ("term = static_cast<T>(static_cast<T>(c * term) + padding);",
                                 "term = static_cast<T>(c * term);"),
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
            if b.returncode != 0:
                # no stale mutated binary may judge the next mutant
                timed("build", p, "mutant-restore", ["ninja", "-C", f"build/{p}", *TARGETS])
    subprocess.run(["git", "diff", "--exit-code", "--", str(H)], cwd=ROOT, check=True)
main()
