# usage: probe_build.py <tag> <preset>...
# Writes <scratch>/probe-<tag>/build.ninja, which compiles the engine-state
# copy of the driver with each preset's exact flags, against a git archive of
# the worktree's HEAD, and links it with the preset tree's other objects.
import os, re, subprocess, sys
S = os.path.dirname(os.path.abspath(__file__))
W = "/home/brianm/projects/utils/math-core/galoot-wt/cost-cleanup--rounding"
tag, presets = sys.argv[1], sys.argv[2:]
maker = os.environ.get("PROBE_MAKER", f"{S}/probe_engine.py")
d = f"{S}/probe-{tag}"
src = f"{d}/src"
os.makedirs(src, exist_ok=True)
# A frozen copy of the sources at HEAD, so a mutation run cannot reach it.
subprocess.run(f"git -C {W} archive HEAD include test | tar -x -C {src}", shell=True, check=True)
subprocess.run([sys.executable, maker, f"{src}/test/core/identities_rounding_test.cpp",
                f"{src}/test/core/identities_rounding_probe.cpp"], check=True)
lines = []
for p in presets:
    cmds = subprocess.run(["ninja", "-C", f"{W}/build/{p}", "-t", "commands", "core_identities_rounding_test"],
                          capture_output=True, text=True, check=True).stdout.splitlines()
    comp = next(c for c in cmds if "core/identities_rounding_test.cpp.o -c" in c)
    comp = comp.split(" -- ", 1)[1]
    comp = re.sub(r" -MD -MT \S+ -MF \S+", "", comp)
    comp = re.sub(r" -o \S+ -c \S+$", "", comp)
    comp = comp.replace(f"-I{W}/include", f"-I{src}/include").replace(f"-I{W}/test", f"-I{src}/test") \
               .replace(f"-isystem {W}/include/ext", f"-isystem {src}/include/ext")
    link = cmds[-1]
    link = re.sub(r"^: && ", "", link); link = re.sub(r"\s+&& :$", "", link)
    link = re.sub(r" (-Wl,|-Xlinker )--dependency-file=\S+", "", link)
    link = link.replace("test/CMakeFiles/core_identities_rounding_test.dir/core/identities_rounding_test.cpp.o", f"{d}/{p}.o")
    link = re.sub(r" -o test/core_identities_rounding_test", f" -o {d}/{p}.exe", link)
    lines += [f"rule c_{p}\n  command = {comp} -o $out -c $in\n",
              f"rule l_{p}\n  command = cd {W}/build/{p} && {link}\n",
              f"build {d}/{p}.o: c_{p} {src}/test/core/identities_rounding_probe.cpp\n",
              f"build {d}/{p}.exe: l_{p} {d}/{p}.o\n"]
open(f"{d}/build.ninja", "w").write("".join(lines))
print(d)
