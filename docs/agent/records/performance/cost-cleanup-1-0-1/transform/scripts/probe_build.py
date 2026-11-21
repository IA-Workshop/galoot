# usage: probe_build.py <tag> <component> <preset>...
# Writes <scratch>/probe-<tag>-<component>/build.ninja, which compiles the
# engine-state copy of the driver of core.<component> with each preset's
# exact flags, against a git archive of the worktree's HEAD, and links it
# with the preset tree's other objects of that program.
import os, re, subprocess, sys
S = os.path.dirname(os.path.abspath(__file__))
W = "/home/brianm/projects/utils/math-core/galoot-wt/cost-cleanup--transform"
tag, component, presets = sys.argv[1], sys.argv[2], sys.argv[3:]
maker = os.environ.get("PROBE_MAKER", f"{S}/probe_engine.py")
target = f"core_{component}_test"
driver = f"test/core/{component}_test.cpp"
d = f"/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/probe-{tag}-{component}"
src = f"{d}/src"
os.makedirs(src, exist_ok=True)
# A frozen copy of the sources at HEAD, so a mutation run cannot reach it.
subprocess.run(f"git -C {W} archive HEAD include test | tar -x -C {src}", shell=True, check=True)
subprocess.run([sys.executable, maker, component, f"{src}/{driver}",
                f"{src}/test/core/{component}_probe.cpp"], check=True)
lines = []
for p in presets:
    cmds = subprocess.run(["ninja", "-C", f"{W}/build/{p}", "-t", "commands", target],
                          capture_output=True, text=True, check=True).stdout.splitlines()
    obj = f"test/CMakeFiles/{target}.dir/core/{component}_test.cpp.o"
    comp = next(c for c in cmds if f"{obj} -c" in c)
    comp = comp.split(" -- ", 1)[1]
    comp = re.sub(r" -MD -MT \S+ -MF \S+", "", comp)
    comp = re.sub(r" -o \S+ -c \S+$", "", comp)
    comp = comp.replace(f"-I{W}/include", f"-I{src}/include").replace(f"-I{W}/test", f"-I{src}/test") \
               .replace(f"-isystem {W}/include/ext", f"-isystem {src}/include/ext")
    link = cmds[-1]
    link = re.sub(r"^: && ", "", link); link = re.sub(r"\s+&& :$", "", link)
    link = re.sub(r" (-Wl,|-Xlinker )--dependency-file=\S+", "", link)
    assert obj in link
    link = link.replace(obj, f"{d}/{p}.o")
    link = re.sub(rf" -o test/{target}\b", f" -o {d}/{p}.exe", link)
    lines += [f"rule c_{p}\n  command = {comp} -o $out -c $in\n",
              f"rule l_{p}\n  command = cd {W}/build/{p} && {link}\n",
              f"build {d}/{p}.o: c_{p} {src}/test/core/{component}_probe.cpp\n",
              f"build {d}/{p}.exe: l_{p} {d}/{p}.o\n"]
open(f"{d}/build.ninja", "w").write("".join(lines))
print(d)
