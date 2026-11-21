# usage: sentinel.py <component> <driver.cpp> <out.cpp>
# A copy of the split driver of core.<component> with one group of forms
# left out (the forms of four given by Gram matrices in the transform
# driver; the other forms of four in the identities driver), and the
# engine-state messages of probe_engine.py, to show that the comparison of
# the per-seed output and of the engine states can fail; never committed.
import os, subprocess, sys
component, driver, out = sys.argv[1:]
s = open(driver).read()
if component == "transform_identities":
    line = "  identities::gram_forms_of_four(source);\n"
    assert s.count(line) == 1
    s = s.replace(line, "")
elif component == "identities":
    line = "  for (const auto check_group : groups) {\n"
    assert s.count(line) == 1
    s = s.replace(line, line + """    if (check_group == &galoot_test::identities::other_forms_of_four) {
      continue;
    }
""")
else:
    raise SystemExit(f"unknown component {component}")
open(out, "w").write(s)
here = os.path.dirname(os.path.abspath(__file__))
subprocess.run([sys.executable, f"{here}/probe_engine.py", component, out, out], check=True)
