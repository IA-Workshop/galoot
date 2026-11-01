"""For each preset: each compile-fail test's compiler and arguments against
the old object library's command in compile_commands.json at the baseline."""
import json, shlex, subprocess, sys
cc_dir = sys.argv[1]
for preset in sys.argv[2:]:
    before = {}
    for e in json.load(open(f"{cc_dir}/{preset}.json")):
        if "_compile_fail.dir" in e["output"]:
            a = shlex.split(e["command"])
            o, c = a.index("-o"), a.index("-c")
            before[a[c + 1]] = [x for i, x in enumerate(a) if i not in (o, o + 1, c, c + 1)]
    tests = json.loads(subprocess.run(["ctest", "--test-dir", f"build/{preset}", "--show-only=json-v1"],
                                      capture_output=True, text=True, check=True).stdout)["tests"]
    same = differ = 0
    for t in tests:
        cmd = t.get("command", [])
        if not any(x.endswith("_compile_fail.o") for x in cmd):
            continue
        rest = cmd[cmd.index("--", cmd.index("--") + 1) + 1:]
        if "tu" in rest[:2]:
            rest = rest[rest.index("--") + 1:]
        o, c = rest.index("-o"), rest.index("-c")
        src = rest[c + 1]
        args = [x for i, x in enumerate(rest) if i not in (o, o + 1, c, c + 1)]
        if before.get(src) == args:
            same += 1
        else:
            differ += 1
            print(preset, t["name"], args, before.get(src))
    print(f"{preset}: {same} same, {differ} different, {len(before)} sources before")
