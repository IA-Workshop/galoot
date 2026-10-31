"""Spike C: record the diagnostic a user sees for each misuse case.

Compiles diag.cpp once per (compiler, standard, form, case), serially and
under tools/timed (PERFORMANCE P13), with the S4 warnings, -Werror, and
-ffp-contract=off. Diagnostics are kept verbatim in out/diag/, and one row per
compile goes to out/diag/summary.tsv: exit status, line and byte counts, the
first line, the first error line, the number of error lines, the line at which
the expected message first appears, and whether the user's call site and the
algebra or blade names appear. Wall times are not measurements (not pinned);
they go to runs.tsv only.

It then compiles case 9 once per form and writes out/diag/detect.tsv: what a
requires-expression or std::is_convertible_v reports for each misuse, and how
many hard errors the probing itself raised.

It also compiles the control with bare -std=c++23 -pedantic-errors in forms R
and P (out/diag/pedantic.tsv).

Usage, from anywhere:  python3 -I run_diag.py [--detect-only | --pedantic-only]
"""

import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(subprocess.run(["git", "-C", str(HERE), "rev-parse",
                            "--show-toplevel"], capture_output=True,
                           text=True, check=True).stdout.strip())
SPIKE_A = ROOT / "docs/agent/records/spikes/spike-a"
OUT = HERE / "out/diag"
WARN = ["-Wall", "-Wextra", "-Wpedantic", "-Wconversion", "-Wshadow",
        "-Werror", "-ffp-contract=off", "-fno-diagnostics-color"]
FORMS = {1: "R", 2: "K", 3: "D", 4: "G", 5: "A", 6: "P"}
CASES = {10: "control", 1: "different algebras", 21: "blade not in set",
         22: "name not valid in algebra", 23: "opposite orientation",
         31: "implicit lossy conversion", 32: "explicit lossy conversion",
         4: "mixed-parity versor", 5: "scalar without *"}
# Substrings that identify the library's own message, per form and case.
MESSAGE = {
    "K": {1: "different algebras", 21: "get<name>", 23: "get<name>",
          31: "lossy conversion", 32: "lossy conversion",
          4: "both parities", 5: "the scalar type must"},
    "G": {1: "different algebras, rga3 and r400", 21: "not in the blade set",
          23: "not in the blade set", 31: "conversion drops",
          32: "conversion drops", 4: "both parities", 5: "the scalar type must"},
}
MESSAGE["A"] = dict(MESSAGE["K"])
MESSAGE["P"] = dict(MESSAGE["K"])
MESSAGE["R"] = {}
MESSAGE["D"] = {}
for form in MESSAGE:
    MESSAGE[form][22] = "blade_name_is_not_valid_in_this_algebra"
CALL_LINE = {10: "\0", 1: "gp(p, v)", 21: "get<\"e1\">(l)", 22: "\"e5\"",
             23: "get<\"e14\">(l)", 31: "const Point r = q",
             32: "static_cast<Point>(q)", 4: "transform(p, v)",
             5: "gp(a, a)"}
NAMES = {1: ("rga3", "r400"), 4: ("Mixed",)}


def compile_one(cxx, std, form, case):
    tag = f"{cxx.rstrip('+')}-{std}-{FORMS[form]}-case{case}"
    path = OUT / f"{tag}.txt"
    cmd = [str(ROOT / "tools/timed"), "build", "-",
           f"spike-c:diag:{cxx}:{std}:{FORMS[form]}:case{case}", "--",
           cxx, f"-std={std}", *WARN, f"-I{SPIKE_A}",
           f"-DSPIKE_FORM={form}", f"-DSPIKE_CASE={case}", "-fsyntax-only",
           str(HERE / "diag.cpp")]
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=HERE)
    # tools/timed adds its own line at the end; it is not the compiler's.
    text = "\n".join(ln for ln in r.stderr.splitlines()
                     if not ln.startswith("timed: ")) + "\n"
    path.write_text(text)
    text = text if text.strip() else ""
    lines = text.splitlines()
    first = lines[0] if lines else ""
    errors = [ln for ln in lines if " error: " in ln or ln.startswith("error:")]
    want = MESSAGE[FORMS[form]].get(case)
    at = next((i + 1 for i, ln in enumerate(lines) if want and want in ln), "")
    call = "yes" if any(CALL_LINE[case] in ln for ln in lines) else "no"
    names = ",".join(n for n in NAMES.get(case, ()) if n in text) or "-"
    return [cxx, std, FORMS[form], str(case), CASES[case], str(r.returncode),
            str(len(lines)), str(len(text.encode())), str(len(errors)),
            str(at), call, names, first, errors[0] if errors else ""]


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    if "--detect-only" in sys.argv:
        detect()
        return
    if "--pedantic-only" in sys.argv:
        pedantic()
        return
    rows = [["compiler", "std", "form", "case", "what", "exit", "lines",
             "bytes", "error_lines", "message_at_line", "call_site_shown",
             "names_shown", "first_line", "first_error_line"]]
    for cxx in ("g++", "clang++"):
        for form in (1, 2, 5, 6, 3, 4):
            stds = ("c++23", "c++26") if form == 4 else ("c++23",)
            for std in stds:
                for case in CASES:
                    rows.append(compile_one(cxx, std, form, case))
                    print("\t".join(rows[-1][:12]), flush=True)
    (OUT / "summary.tsv").write_text(
        "\n".join("\t".join(r) for r in rows) + "\n")
    pedantic()
    detect()


def pedantic():
    """The control (case 10) with bare -std=c++23 -pedantic-errors."""
    rows = [["compiler", "form", "exit", "stderr_lines"]]
    for cxx in ("g++", "clang++"):
        for form in (1, 6):
            r = subprocess.run(
                [str(ROOT / "tools/timed"), "build", "-",
                 f"spike-c:pedantic:{cxx}:{FORMS[form]}", "--", cxx,
                 "-std=c++23", "-pedantic-errors", f"-I{SPIKE_A}",
                 f"-DSPIKE_FORM={form}", "-DSPIKE_CASE=10", "-fsyntax-only",
                 str(HERE / "diag.cpp")], capture_output=True, text=True)
            lines = [ln for ln in r.stderr.splitlines()
                     if not ln.startswith("timed: ")]
            rows.append([cxx, FORMS[form], str(r.returncode),
                         str(len(lines))])
            print("\t".join(rows[-1]), flush=True)
    (OUT / "pedantic.tsv").write_text(
        "\n".join("\t".join(r) for r in rows) + "\n")


# Case 9 packs six requires-expression answers into one number, printed by a
# deliberately incomplete type; bit k set means "reported well-formed".
BITS = ["gp_different_algebras", "gp_scalar_without_mul", "get_missing_e1",
        "convertible_motor_to_point", "transform_mixed_parity",
        "gp_point_point_correct"]


def detect():
    rows = [["compiler", "std", "form", "answers", *BITS, "hard_errors"]]
    for cxx in ("g++", "clang++"):
        for form in (1, 2, 5, 6, 3, 4):
            std = "c++26" if form == 4 else "c++23"
            cmd = [str(ROOT / "tools/timed"), "build", "-",
                   f"spike-c:detect:{cxx}:{std}:{FORMS[form]}", "--", cxx,
                   f"-std={std}", *WARN, f"-I{SPIKE_A}",
                   f"-DSPIKE_FORM={form}", "-DSPIKE_CASE=9", "-fsyntax-only",
                   str(HERE / "diag.cpp")]
            r = subprocess.run(cmd, capture_output=True, text=True, cwd=HERE)
            (OUT / f"detect-{cxx.rstrip('+')}-{FORMS[form]}.txt").write_text(
                r.stderr)
            m = re.search(r"detection_answers<(\d+)>", r.stderr)
            v = int(m.group(1)) if m else -1
            other = sum(1 for ln in r.stderr.splitlines()
                        if " error: " in ln and "detection_answers" not in ln)
            rows.append([cxx, std, FORMS[form], str(v),
                         *[("well-formed" if v >> k & 1 else "ill-formed")
                           if v >= 0 else "?" for k in range(len(BITS))],
                         str(other)])
            print("\t".join(rows[-1]), flush=True)
    (OUT / "detect.tsv").write_text(
        "\n".join("\t".join(r) for r in rows) + "\n")


if __name__ == "__main__":
    main()
