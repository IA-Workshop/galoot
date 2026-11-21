# usage: probe_engine.py <component> <driver.cpp> <out.cpp>
# Writes a copy of the driver of core.<component> that prints the engine's
# state after each test case's last draw (in the transform driver, after
# each check of every form; in the identities driver, after the groups of
# each test case), for the equivalence comparison only; never committed.
import sys
component, driver, out = sys.argv[1:]
s = open(driver).read()
probe = '''{
    std::ostringstream engine_state;
    engine_state << source;
    MESSAGE("engine state: ", engine_state.str());
  }
'''
if component == "transform_identities":
    lines = s.splitlines(keepends=True)
    calls = [i for i, l in enumerate(lines) if l.strip() == "check_all_forms(source);"]
    assert len(calls) == 2, calls
    for i in reversed(calls):
        indent = lines[i][: len(lines[i]) - len(lines[i].lstrip())]
        lines.insert(i + 1, indent + probe.replace("\n  ", "\n" + indent))
    s = "".join(lines)
elif component == "identities":
    a = "    check_group(identity, source);\n  }\n"
    assert s.count(a) == 1
    s = s.replace(a, a + "  " + probe)
else:
    raise SystemExit(f"unknown component {component}")
s = s.replace("#include <random>\n", "#include <random>\n#include <sstream>\n", 1)
open(out, "w").write(s)
