"""Mutation runs for the canary core.plan_cost_model and for the cost
model's self-check.

  mutate.py canary <preset> [mutation...]
      Applies one mutation at a time to a generator header or to the
      thresholds registered in test/CMakeLists.txt, reconfigures and builds
      the preset (so that a threshold change reaches CTest), runs only
      core.plan_cost_model, and restores the file. A header mutant adds one
      plain statement that changes nothing but the compiler's count. The
      canary kills a mutant when it fails because a reference unit's count
      moved ("measured threshold", "does not compile at its threshold;", or
      "compiles one below"); a failure because a unit stopped compiling for
      another reason, such as a warning made an error, is recorded as
      "other", which is not a kill.
  mutate.py selfcheck [mutation...]
      Applies one mutation at a time to the committed weights, the figures
      file, or mirror.py, runs cost.py selfcheck for the compiler the
      mutation concerns, and restores the file. The self-check kills a
      mutant when it exits 1.

Each anchor must occur exactly once. CTest is not run when the build fails.
After the last mutant the files are restored and the preset is built again.

Usage, from the worktree root with the job pool exported.
"""

import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
PLAN = ROOT / "include/galoot/core/plan.hpp"
PRODUCT = ROOT / "include/galoot/core/blade_product.hpp"
CMAKE = ROOT / "test/CMakeLists.txt"
SNIPPET = ROOT / "docs/agent/snippets/plan_cost_model"
RECORDS = ROOT / "docs/agent/records/performance/plan-cost-model"
OUT = RECORDS / "out"

# One more statement, which every compiler evaluates and counts, and which
# changes no value.
STEP = "[[maybe_unused]] const std::size_t extra_step = 0;"


def after(anchor, indent):
    """A mutation that inserts STEP on its own line after `anchor`."""
    return anchor, anchor + " " * indent + STEP + "\n"


HEADER_MUTANTS = {
    "typed_scan_step": (PLAN, *after(
        "    while (at < result.size() && result[at].mask != plan[k].out) {\n",
        6)),
    "collect_scan_step": (PLAN, *after(
        "    while (at < size && canonical_less(out[at].mask, t.out)) {\n", 6)),
    "product_term_step": (PRODUCT, *after(
        "    const coefficient_type c = multiply_checked(x.terms[k].coefficient, "
        "scale);\n", 4)),
    "factor_step": (PLAN, *after(
        "  if constexpr (base_operation(Op) == Op) {\n"
        "    return {.blade = a, .coefficient = 1};\n"
        "  } else {\n", 4)),
    "run_term_step": (PLAN, *after(
        "    const term& base_term = terms.terms[terms.size() - 1 - k];\n", 4)),
    "map_term_step": (PLAN, *after(
        "    const term image = unary_term(op, set[a].mask, n);\n", 4)),
    "map_copy_step": (
        PLAN,
        "  for (std::size_t k = 0; k < K; ++k) {\n    out.terms[k] = {\n",
        "  for (std::size_t k = 0; k < K; ++k) {\n    " + STEP +
        "\n    out.terms[k] = {\n"),
    "defects_step": (PLAN, *after(
        "    const plan_term& t = plan[k];\n", 4)),
    "typing_step": (PLAN, *after(
        "    const typed_term& t = terms[k];\n", 4)),
    "capacity_step": (PLAN, *after(
        "      const std::size_t pair = pair_capacity(op, x.mask, y.mask, n);\n",
        6)),
    "map_defects_step": (PLAN, *after(
        "    const map_term& t = map.terms[k];\n", 4)),
}

THRESHOLD = re.compile(r"(plan_cost_\w+\.cpp)=(\d+)")


def threshold_mutants(text):
    """+1 and -1 for each threshold registered for the canary."""
    found = {}
    for m in THRESHOLD.finditer(text):
        source, value = m.group(1), int(m.group(2))
        whole = m.group(0)
        stem = source.removeprefix("plan_cost_").removesuffix(".cpp")
        # The two compilers' blocks register one threshold each per source.
        kind = "gcc" if text.rfind("GNU", 0, m.start()) > \
            text.rfind("Clang", 0, m.start()) else "clang"
        for delta, label in ((1, "up"), (-1, "down")):
            found[f"threshold_{label}_{stem}_{kind}"] = (
                CMAKE, whole, f"{source}={value + delta}")
    return found


def run(args):
    return subprocess.run(args, cwd=ROOT, capture_output=True, text=True)


def canary(preset, only):
    cmake_text = CMAKE.read_text(encoding="utf-8")
    mutants = {**HEADER_MUTANTS, **threshold_mutants(cmake_text)}
    originals = {path: path.read_text(encoding="utf-8")
                 for path in (PLAN, PRODUCT, CMAKE)}
    print("mutation\tpreset\tbuild_exit\ttest_exit\tverdict\tcanary_output")
    try:
        for name, (path, anchor, replacement) in mutants.items():
            if only and name not in only:
                continue
            if name.startswith("threshold_") and \
                    not name.endswith(preset.split("-")[0]):
                continue
            source = originals[path]
            assert source.count(anchor) == 1, (name, anchor)
            path.write_text(source.replace(anchor, replacement),
                            encoding="utf-8")
            build = run(["tools/timed", "build", preset, f"mutation:{name}",
                         "--", "cmake", "--build", "--preset", preset])
            if build.returncode != 0:
                errors = [line for line in build.stdout.splitlines()
                          if "error" in line]
                print(f"{name}\t{preset}\t{build.returncode}\t-\tbuild "
                      f"fails\t{(errors or ['-'])[0][:200]}", flush=True)
                path.write_text(source, encoding="utf-8")
                continue
            test = run(["tools/timed", "test", preset, f"mutation:{name}",
                        "--", "ctest", "--preset", preset, "-R",
                        r"^core\.plan_cost_model$", "--output-on-failure"])
            lines = [line.strip() for line in test.stdout.splitlines()
                     if "threshold" in line and "recorded;" in line]
            moved = [line for line in lines
                     if "measured threshold" in line
                     or "does not compile at its threshold;" in line
                     or "compiles one below" in line]
            if test.returncode == 0:
                verdict = "SURVIVED"
            elif moved:
                verdict = "killed"
            else:
                verdict = "other"
            print(f"{name}\t{preset}\t0\t{test.returncode}\t{verdict}\t"
                  f"{' | '.join(lines) or '-'}", flush=True)
            path.write_text(source, encoding="utf-8")
    finally:
        for path, text in originals.items():
            path.write_text(text, encoding="utf-8")
        run(["tools/timed", "build", preset, "mutation:restored", "--",
             "cmake", "--build", "--preset", preset])


def selfcheck_command(compiler):
    """The self-check of the README, for one compiler."""
    lines = (RECORDS / "README.md").read_text(encoding="utf-8").splitlines()
    marker = f"selfcheck-{compiler}:"
    command = next(line.split(marker, 1)[1].strip() for line in lines
                   if marker in line)
    return command.strip("`").split()


def selfcheck_mutants():
    found = {}
    for compiler in ("clang", "gcc"):
        weights = OUT / f"weights-{compiler}-galoot-O0.tsv"
        figures = OUT / f"figures-{compiler}.txt"
        text = weights.read_text(encoding="utf-8")
        for phase, event in (("typed", "type.scan"), ("fill", "gen.term"),
                             ("map_typed", "map_typed.term")):
            m = re.search(rf"^{phase}\t{re.escape(event)}\t(-?\d+)$", text,
                          re.MULTILINE)
            if m:
                # Whole lines, since one phase's name ends another's.
                found[f"weight_{phase}_{compiler}"] = (
                    compiler, weights, f"\n{m.group(0)}\n",
                    f"\n{phase}\t{event}\t{int(m.group(1)) + 1}\n")
        lines = figures.read_text(encoding="utf-8").splitlines()
        row = lines[2]
        fields = row.split("\t")
        fields[4] = str(int(fields[4]) + 1)
        found[f"figure_{fields[0]}_{compiler}"] = (
            compiler, figures, row, "\t".join(fields))
    mirror = SNIPPET / "mirror.py"
    for event in ("type.scan", "gen.term", "collect.scan"):
        anchor = f'ev["{event}"] += 1\n'
        for compiler in ("clang", "gcc"):
            found[f"event_{event}_{compiler}"] = (
                compiler, mirror, anchor, "pass\n")
    return found


def selfcheck(only):
    mutants = selfcheck_mutants()
    print("mutation\tselfcheck_exit\tverdict\tfirst_difference")
    for name, (compiler, path, anchor, replacement) in mutants.items():
        if only and name not in only:
            continue
        source = path.read_text(encoding="utf-8")
        assert source.count(anchor) == 1, (name, anchor)
        try:
            path.write_text(source.replace(anchor, replacement),
                            encoding="utf-8")
            result = run(selfcheck_command(compiler))
        finally:
            path.write_text(source, encoding="utf-8")
        first = next((line for line in result.stdout.splitlines()
                      if "differences" not in line), "-")
        verdict = "killed" if result.returncode == 1 else (
            "SURVIVED" if result.returncode == 0 else "error")
        print(f"{name}\t{result.returncode}\t{verdict}\t{first[:200]}",
              flush=True)


def main(argv):
    if argv and argv[0] == "canary" and len(argv) >= 2:
        canary(argv[1], set(argv[2:]))
    elif argv and argv[0] == "selfcheck":
        selfcheck(set(argv[1:]))
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main(sys.argv[1:])
