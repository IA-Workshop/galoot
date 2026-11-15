"""Compile each compile-fail source of one preset with -c and with
-fsyntax-only, pinned to one CPU through tools/measure, and compare exit
status and diagnostics. Usage: compare_modes.py <worktree> <preset> <cc.json> <outdir> <cpu>"""
import json
import os
import shlex
import subprocess
import sys

root, preset, cc_json, outdir, cpu = sys.argv[1:6]
entries = [e for e in json.load(open(cc_json))
           if "compile_fail" in e["output"] and e["file"].endswith("_fail.cpp")]
os.makedirs(outdir, exist_ok=True)
compiler_id = "GNU" if "g++" in entries[0]["command"].split()[0] else "Clang"
summary = []
for e in entries:
    args = shlex.split(e["command"])
    name = os.path.basename(os.path.dirname(os.path.dirname(e["output"])))
    name = name.removesuffix("_compile_fail.dir").removesuffix(".dir")
    o = args.index("-o")
    c = args.index("-c")
    source = args[c + 1]
    base = [a for i, a in enumerate(args) if i not in (o, o + 1, c, c + 1)]
    commands = {
        "c": base + ["-o", f"{outdir}/{name}.o", "-c", source],
        "s": base + ["-fsyntax-only", source],
    }
    results = {}
    for mode, command in commands.items():
        record = f"{outdir}/{mode}.tsv"
        run = subprocess.run(["taskset", "-c", cpu, f"{root}/tools/measure", "tu",
                              preset, compiler_id, record, "--", *command],
                             cwd=e["directory"], capture_output=True, text=True)
        text = run.stdout + run.stderr
        text = "\n".join(l for l in text.splitlines()
                         if not l.startswith("measure:"))
        open(f"{outdir}/{name}.{mode}.out", "w").write(text)
        results[mode] = (run.returncode, text)
    same = results["c"] == results["s"]
    summary.append(f"{name}\t{results['c'][0]}\t{results['s'][0]}\t"
                   f"{'same' if same else 'DIFFERENT'}")
open(f"{outdir}/summary.tsv", "w").write(
    "test\texit_c\texit_syntax_only\toutput\n" + "\n".join(summary) + "\n")
print(f"{preset}: {len(summary)} sources, "
      f"{sum('DIFFERENT' in s for s in summary)} differ")
