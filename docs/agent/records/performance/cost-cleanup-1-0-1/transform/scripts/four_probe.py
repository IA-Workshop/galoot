# usage: four_probe.py <dir> [split]
# Writes <dir>/build.ninja, which compiles identities_four_test.cpp from a
# git archive of the worktree's HEAD once per form of its list, with that
# form alone, and once with no form, under gcc-debug's exact flags, each
# through tools/timed (selection scratch:four-probe-<k>), so that the run
# log holds each variant's peak memory and instructions. With "split", it
# compiles instead the first 7 and 8 forms and the last 5 and 4
# (scratch:four-split-<first|last>-<count>). Scratch only: the variants are
# not committed; this script and the archive reproduce them.
import os, re, subprocess, sys
W = "/home/brianm/projects/utils/math-core/galoot-wt/cost-cleanup--transform"
d = os.path.abspath(sys.argv[1])
src = f"{d}/src"
os.makedirs(src, exist_ok=True)
subprocess.run(f"git -C {W} archive HEAD include test | tar -x -C {src}", shell=True, check=True)
path = f"{src}/test/core/identities_four_test.cpp"
s = open(path).read()
head = "using of_four =\n    forms<"
start = s.index(head) + len(head)
end = s.index(">;", start)
body = s[start:end]
forms, depth, cur = [], 0, ""
for ch in body:
    if ch in "({<[":
        depth += 1
    elif ch in ")}>]":
        depth -= 1
    if ch == "," and depth == 0:
        forms.append(cur.strip()); cur = ""
    else:
        cur += ch
forms.append(cur.strip())
assert len(forms) == 12, len(forms)
cmds = subprocess.run(["ninja", "-C", f"{W}/build/gcc-debug", "-t", "commands", "core_identities_test"],
                      capture_output=True, text=True, check=True).stdout.splitlines()
comp = next(c for c in cmds if "identities_four_test.cpp.o -c" in c).split(" -- ", 1)[1]
comp = re.sub(r" -MD -MT \S+ -MF \S+", "", comp)
comp = re.sub(r" -o \S+ -c \S+$", "", comp)
comp = comp.replace(f"-I{W}/include", f"-I{src}/include").replace(f"-isystem {W}/include/ext", f"-isystem {src}/include/ext")
split = len(sys.argv) > 2 and sys.argv[2] == "split"
name = "four-split" if split else "four-probe"
lines = [f"rule c\n  command = cd {W} && tools/timed probe gcc-debug scratch:{name}-$k -- {comp} -o $out -c $in\n"]
if split:
    variants = {"first-7": forms[:7], "first-8": forms[:8], "last-5": forms[7:], "last-4": forms[8:]}
    for k, fs in variants.items():
        v = f"{src}/test/core/four_{k}.cpp"
        open(v, "w").write(s[:start] + ", ".join(fs) + s[end:])
        lines.append(f"build {d}/four_{k}.o: c {v}\n  k = {k}\n")
    open(f"{d}/build.ninja", "w").write("".join(lines))
    sys.exit(0)
for k, f in enumerate(forms + [""]):
    v = f"{src}/test/core/four_{k}.cpp"
    open(v, "w").write(s[:start] + f + s[end:])
    lines.append(f"build {d}/four_{k}.o: c {v}\n  k = {k}\n")
open(f"{d}/build.ninja", "w").write("".join(lines))
for k, f in enumerate(forms + ["(none)"]):
    print(k, " ".join(f.split()))
