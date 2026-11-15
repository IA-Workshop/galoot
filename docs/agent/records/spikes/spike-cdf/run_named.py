"""Spike F: build cost of named component access.

For each compiler, -O0 and -O2, spelling (string get<"e41">, index get_at<0>,
subscript x["e41"]), and REPS in 1, 16, 64, 256, generates a unit with
gen_named.py and compiles it to an object, pinned and measured (pinned.py),
serially. REPS = 0 (the fixture and main alone) is the baseline. Each unit
has 18 distinct (type, name) accesses and 18 * REPS uses. From the object,
it records the text and rodata size and, for the accessor instantiations that
-O0 emits out of line, their number and mangled lengths. Rows go to
out/named/named.tsv; one demangled string-form symbol goes to
out/named/symbols.txt.

With `diag`, it instead compiles the `missing` variant of each spelling
(-fsyntax-only, S4 flags; the string spelling under fixture forms R and P)
named_detect.cpp, and named_check.cpp (which must compile: exit 0), and
writes their diagnostics and line counts to out/named/diag/. It also counts
the run-time calls of index_of in the -O0 assembly of the string spelling,
with and without SPIKE_GET_INDEX_AT_RUN_TIME, and compiles named_check.cpp
and the string unit with bare -std=c++23 -pedantic-errors.

Usage, from anywhere:  python3 -I run_named.py [diag]
"""

import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from pinned import FIELDS, ROOT, measure  # noqa: E402

SPIKE_A = ROOT / "docs/agent/records/spikes/spike-a"
OUT = HERE / "out/named"
GEN = OUT / "gen"
FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", f"-I{SPIKE_A}",
         f"-I{HERE}"]
ACCESSOR = {"string": "3get", "index": "6get_at", "subscript": "ixE"}


def sizes(obj):
    out = subprocess.run(["size", "-A", str(obj)], capture_output=True,
                         text=True).stdout
    sec = {ln.split()[0]: int(ln.split()[1]) for ln in out.splitlines()
           if len(ln.split()) >= 2 and ln.split()[1].isdigit()}
    return sec.get(".text", 0), sec.get(".rodata", 0)


def accessor_symbols(obj, form):
    out = subprocess.run(["nm", str(obj)], capture_output=True,
                         text=True).stdout
    syms = [ln.split()[2] for ln in out.splitlines()
            if len(ln.split()) == 3 and ln.split()[1] in "TW"
            and ACCESSOR.get(form, "\0") in ln.split()[2]]
    return syms


def diag():
    d = OUT / "diag"
    d.mkdir(parents=True, exist_ok=True)
    rows = [["compiler", "spelling", "fixture_form", "exit", "lines",
             "error_lines", "first_error_line"]]
    jobs = [("string", 1), ("string", 6), ("subscript", 1), ("detect", 1),
            ("check", 1)]
    for cxx in ("g++", "clang++"):
        for form, fixture_form in jobs:
            if form in ("detect", "check"):
                src = HERE / f"named_{form}.cpp"
            else:
                src = d / f"{form}-missing.cpp"
                src.write_text(subprocess.run(
                    [sys.executable, "-I", str(HERE / "gen_named.py"), form,
                     "1", "missing"], capture_output=True, text=True,
                    check=True).stdout)
            sel = f"spike-f:diag:{cxx}:{form}:form{fixture_form}"
            r = subprocess.run(
                [str(ROOT / "tools/timed"), "build", "-", sel, "--", cxx,
                 *FLAGS, "-fno-diagnostics-color",
                 f"-DSPIKE_FORM={fixture_form}", "-fsyntax-only", str(src)],
                capture_output=True, text=True)
            text = "\n".join(ln for ln in r.stderr.splitlines()
                             if not ln.startswith("timed: "))
            (d / f"{cxx.rstrip('+')}-{form}-form{fixture_form}.txt"
             ).write_text(text + "\n")
            lines = text.splitlines()
            errs = [ln for ln in lines if " error: " in ln]
            rows.append([cxx, form, str(fixture_form), str(r.returncode),
                         str(len(lines)), str(len(errs)),
                         errs[0] if errs else ""])
            print("\t".join(rows[-1][:6]), flush=True)
    # A user's build with no warning flags (DoD-6): bare -pedantic-errors.
    for cxx in ("g++", "clang++"):
        for src in (HERE / "named_check.cpp", d / "string-1.cpp"):
            if not src.exists():
                src.write_text(subprocess.run(
                    [sys.executable, "-I", str(HERE / "gen_named.py"),
                     "string", "1"], capture_output=True, text=True,
                    check=True).stdout)
            r = subprocess.run(
                [str(ROOT / "tools/timed"), "build", "-",
                 f"spike-f:pedantic:{cxx}:{src.stem}", "--", cxx,
                 "-std=c++23", "-pedantic-errors", f"-I{SPIKE_A}", f"-I{HERE}",
                 "-fsyntax-only", str(src)], capture_output=True, text=True)
            lines = [ln for ln in r.stderr.splitlines()
                     if not ln.startswith("timed: ")]
            rows.append([cxx, f"pedantic-errors:{src.stem}", "1",
                         str(r.returncode), str(len(lines)), "", ""])
            print("\t".join(rows[-1]), flush=True)

    # Run-time blade search at -O0: count calls of index_of in the assembly
    # of the string spelling, with the index bound to a constant (default)
    # and without (SPIKE_GET_INDEX_AT_RUN_TIME).
    src = d / "string-1.cpp"
    src.write_text(subprocess.run(
        [sys.executable, "-I", str(HERE / "gen_named.py"), "string", "1"],
        capture_output=True, text=True, check=True).stdout)
    for cxx in ("g++", "clang++"):
        for extra in ([], ["-DSPIKE_GET_INDEX_AT_RUN_TIME"]):
            asm = d / "get.s"
            sel = f"spike-f:asm:{cxx}:{'runtime' if extra else 'constant'}"
            subprocess.run([str(ROOT / "tools/timed"), "build", "-", sel,
                            "--", cxx, *FLAGS, "-O0", *extra, "-S", "-o",
                            str(asm), str(src)], capture_output=True)
            calls = sum(1 for ln in asm.read_text().splitlines()
                        if "call" in ln and "index_of" in ln)
            asm.unlink()
            rows.append([cxx, "string-O0-asm", " ".join(extra) or "constant",
                         "0", "", "", f"{calls} calls of index_of"])
            print("\t".join(rows[-1]), flush=True)
    (d / "summary.tsv").write_text("\n".join("\t".join(r) for r in rows)
                                   + "\n")


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    if sys.argv[1:] == ["diag"]:
        diag()
        return
    GEN.mkdir(exist_ok=True)
    head = ["compiler", "opt", "form", "reps", "distinct", "uses", *FIELDS,
            "text_bytes", "rodata_bytes", "accessor_syms",
            "accessor_mangled_min", "accessor_mangled_max"]
    rows = [head]
    demangled = []
    points = [("none", 0)] + [(f, r) for f in ("string", "index", "subscript")
                              for r in (1, 16, 64, 256)]
    for cxx in ("g++", "clang++"):
        for opt in ("O0", "O2"):
            for form, reps in points:
                src = GEN / f"{form}-{reps}.cpp"
                src.write_text(subprocess.run(
                    [sys.executable, "-I", str(HERE / "gen_named.py"),
                     "string" if form == "none" else form, str(reps)],
                    capture_output=True, text=True, check=True).stdout)
                obj = OUT / f"{cxx.rstrip('+')}-{opt}-{form}-{reps}.o"
                sel = f"spike-f:named:{cxx}:{opt}:{form}:{reps}"
                fields, err = measure(sel, [cxx, *FLAGS, f"-{opt}", "-c",
                                            "-o", str(obj), str(src)])
                if err:
                    print(err, file=sys.stderr)
                text, rodata = sizes(obj) if obj.exists() else (0, 0)
                syms = accessor_symbols(obj, form) if obj.exists() else []
                lens = [len(s) for s in syms]
                if syms and form == "string" and opt == "O0" and reps == 1:
                    dem = subprocess.run(["llvm-cxxfilt", syms[0]],
                                         capture_output=True,
                                         text=True).stdout.strip()
                    demangled.append(f"{cxx}\t{syms[0]}\t{dem}")
                rows.append([cxx, opt, form, str(reps),
                             str(18 if reps else 0), str(18 * reps),
                             *[fields[k] for k in FIELDS], str(text),
                             str(rodata), str(len(syms)),
                             str(min(lens)) if lens else "",
                             str(max(lens)) if lens else ""])
                print("\t".join(rows[-1][:11]), flush=True)
                if obj.exists():
                    obj.unlink()
    (OUT / "named.tsv").write_text("\n".join("\t".join(r) for r in rows)
                                   + "\n")
    (OUT / "symbols.txt").write_text("\n".join(demangled) + "\n")


if __name__ == "__main__":
    main()
