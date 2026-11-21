# Checks of the documentation design (plan 0049, unit U1)

Written by: Claude (agent, Fable 5.2), 2026-11-21. These files back research note [0028](../../../research-notes/0028-docs-design-transform-and-the-classical-sandwich.md) and the records one directory up. All commands run from the repository root.

| File | What it checks | How it ran | Result |
| --- | --- | --- | --- |
| `transform_sandwich_check.py` | The lemmas and theorems A to D of the note, and that `transform(x, V)` is not the classical action of V, on every triple of basis blades of 20 diagonal forms; the worked values of the note | `python3 -I docs/agent/records/docs/checks/transform_sandwich_check.py`, with CPython 3.14.10 from math-tools' environment and again with the system's; about 2.5 s | every row as expected; output in `transform_sandwich_check.out` |
| `transform_sandwich_probe.cpp` | That the library gives the note's values in R(3, 0, 0), R(2, 0, 0), and rigid(3): 44 `static_assert`s over `int` | `g++ -std=c++23 -Wall -Wextra -I include` and the same with `clang++`, each as `-fsyntax-only` and as a full build that was run; GCC 16.2.1, Clang 23.1.5; under 0.6 s each | compiles and exits 0 on both; three copies with one expected value changed each failed with one failed `static_assert` on both compilers |
| `design_values_probe.cpp` | The test values of `misconceptions.md` and the numbers of the route from rotors to motors: 35 `static_assert`s over `int` | as above | compiles and exits 0 on both |
| `inventory_check.py` | That `concept-inventory.md` has no repeated ID, no missing ID, and no row that rests on a later row | `python3 -I docs/agent/records/docs/checks/inventory_check.py` | "order holds": 19 prerequisites, 100 tutorial concepts in 9 chapters, 20 outside the tutorial, 28 public names; two copies with one dependency changed each failed |

The compiles ran directly, outside the job pool and `tools/timed`, as small probes; none is a test of the suite. The script is written from the definitions of the conventions and shares no code with the library or with `test/reference/conventions_check.py`. Its first version compared unsymmetrized terms of a quadratic form and reported true identities as false; the docstring says how the comparison is made now.

The commit that added the first three files says 28 forms in its message. The count is 20: five Euclidean, three negative definite, six indefinite, and six degenerate.
