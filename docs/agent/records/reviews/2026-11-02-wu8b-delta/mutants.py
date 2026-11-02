"""Delta review mutants of WU8b: apply one edit to mv.hpp in the throwaway
worktree, build all on gcc-debug, and, when it builds, run CTest on the
core.mv label. Each run goes through tools/timed. Restores the header after
each mutant."""
import pathlib, subprocess, sys

W = "/home/brianm/projects/utils/math-core/galoot/.claude/worktrees/delta-wu8b-mut"
OUT = pathlib.Path(sys.argv[1])
MV = pathlib.Path(W, "include/galoot/core/mv.hpp")
ORIG = MV.read_text()

M = [
 ("odd_part_not_divided",
  "            : std::bit_width(magnitude >> static_cast<unsigned>(\n"
  "                                              std::countr_zero(magnitude)));\n",
  "            : std::bit_width(magnitude >> static_cast<unsigned>(\n"
  "                                              0 * std::countr_zero(magnitude)));\n"),
 ("exponent_strict",
  "           std::bit_width(magnitude) <= limits::max_exponent;\n",
  "           std::bit_width(magnitude) < limits::max_exponent;\n"),
 ("odd_bits_one_more",
  "    return odd_bits <= limits::digits &&\n",
  "    return odd_bits <= limits::digits + 1 &&\n"),
 ("signed_guard_too_low",
  "      return limits::digits >= 63 ||\n",
  "      return limits::digits >= 15 ||\n"),
 ("signed_as_magnitude",
  "    if constexpr (limits::is_signed) {\n",
  "    if constexpr (false && limits::is_signed) {\n"),
 ("coefficient_by_convertibility",
  "    !multivector_or_derived<Value> && !std::constructible_from<T, Value>;\n",
  "    !multivector_or_derived<Value> && !std::convertible_to<Value, T>;\n"),
 ("pointer_not_several",
  "    std::is_pointer_v<std::remove_cvref_t<Value>> ||\n",
  "    false ||\n"),
 ("braced_catch_all_explicit_again",
  "  constexpr mv(const std::remove_cvref_t<T> (& /*values*/)[N]) {\n",
  "  constexpr explicit mv(const std::remove_cvref_t<T> (& /*values*/)[N]) {\n"),
 ("copyable_fault_skipped",
  "  } else if constexpr (!std::copyable<T>) {\n",
  "  } else if constexpr (false && !std::copyable<T>) {\n"),
 ("truncate_catch_all_returns_x",
  "  if constexpr (std::default_initializable<std::remove_cvref_t<Target>>) {\n",
  "  if constexpr (false) {\n"),
 ("opposite_ordered_not_told",
  "              ? NameLookup::other_orientation_in_ordered_layout\n",
  "              ? NameLookup::other_orientation\n"),
 ("truncate_skips_last_place",
  "    GALOOT_ASSERT(c[at] == T{}, \"truncate: a dropped coefficient is not zero\");\n",
  "    GALOOT_ASSERT(at == dropped.back() || c[at] == T{}, \"truncate: a dropped coefficient is not zero\");\n"),
]

def run(step, sel, cmd, log):
    r = subprocess.run(["tools/timed", step, "gcc-debug", sel, "--"] + cmd,
                       cwd=W, capture_output=True, text=True)
    log.write_text(r.stdout + r.stderr)
    return r.returncode, r.stdout + r.stderr

OUT.mkdir(parents=True, exist_ok=True)
only = sys.argv[2:]
for name, old, new in M:
    if only and name not in only:
        continue
    assert ORIG.count(old) == 1, name
    MV.write_text(ORIG.replace(old, new))
    try:
        b, text = run("build", f"delta-wu8b-mut:{name}",
                      ["cmake", "--build", "--preset", "gcc-debug"],
                      OUT / f"{name}.build.txt")
        errs = [l for l in text.splitlines() if " error: " in l]
        warn = [l for l in errs if "-Werror" in l]
        line = f"{name}\tbuild={b}"
        if b != 0:
            line += f"\tfirst_error={errs[0][-170:] if errs else '?'}\twerror_lines={len(warn)}"
        else:
            t, ttext = run("test", f"delta-wu8b-mut:{name}:core.mv",
                           ["ctest", "--preset", "gcc-debug", "-L", "core\\.mv"],
                           OUT / f"{name}.test.txt")
            failed = [l.split()[-1] if False else l.strip() for l in ttext.splitlines()
                      if "(Failed)" in l or "***Failed" in l or "(Subprocess aborted)" in l]
            line += f"\ttest={t}\tfailed={'; '.join(failed)[:600]}"
        print(line, flush=True)
    finally:
        MV.write_text(ORIG)
