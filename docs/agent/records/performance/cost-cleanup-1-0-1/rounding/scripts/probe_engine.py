# usage: probe_engine.py <driver.cpp> <out.cpp>
# Writes a copy of the driver that prints the engine's state after the test
# case's last draw, for the equivalence comparison only; never committed.
import sys
s = open(sys.argv[1]).read()
anchor = '  MESSAGE(std::format("{} coefficients, {} of them with at most one rounding "'
assert s.count(anchor) == 1
s = s.replace(anchor, '''  {
    std::ostringstream engine_state;
    engine_state << source;
    MESSAGE("engine state: ", engine_state.str());
  }
''' + anchor)
s = s.replace('#include <random>\n', '#include <random>\n#include <sstream>\n', 1)
open(sys.argv[2], 'w').write(s)
