"""A stand-in for a compiler, for the tests of plan_cost_model.py.

Usage:
  python3 -I fake_compiler.py <threshold> [evades=<limit>] [other-below]
      <compiler flags>... <source>

Compiles (exits 0) exactly when the limit given by -fconstexpr-steps= or
-fconstexpr-ops-limit= is at least <threshold>, and otherwise exits 1 with
Clang's or GCC's message for the limit, as a unit whose largest constant
evaluation has that threshold would. With evades=<limit> it also compiles
at that one limit below the threshold, as GCC can when its count runs out
inside the argument of a built-in function. With other-below it fails below
the threshold with some other error. The source is not read.
"""

import sys

threshold = int(sys.argv[1])
evades = None
other_below = False
limit = None
message = None
for arg in sys.argv[2:]:
    if arg.startswith("evades="):
        evades = int(arg.removeprefix("evades="))
    elif arg == "other-below":
        other_below = True
    elif arg.startswith("-fconstexpr-steps="):
        limit = int(arg.removeprefix("-fconstexpr-steps="))
        message = "error: constexpr evaluation hit maximum step limit"
    elif arg.startswith("-fconstexpr-ops-limit="):
        limit = int(arg.removeprefix("-fconstexpr-ops-limit="))
        message = "error: 'constexpr' evaluation operation count exceeds limit"

if limit is None:
    sys.exit("fake_compiler.py: no limit given")
if limit >= threshold or limit == evades:
    sys.exit(0)
print("error: some other error" if other_below else message)
sys.exit(1)
