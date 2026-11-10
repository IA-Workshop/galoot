#!/usr/bin/env python3
"""The two quotients that typed_change forms while it puts the terms of one
result blade over their least common denominator, q / gcd(q, own) and
q_B / own, as written and reduced, for e1 = (3 e'1 + 3 e'2) / 6 and
e2 = (2 e'1 - 2 e'2) / 6, whose rows reduce by 3 and 2 to (1, 1) / 2 and
(1, -1) / 3. Each quotient is bounded by its dividend, which shrinks, but is
not itself the written value divided by a positive integer.

Usage: python3 -I quotient_probe.py
"""

import math


def quotients(dens):
    # The images of e1 and e2 both reach e'1, with own denominators d0, d1.
    own = list(dens)
    q = 1
    steps = []
    for o in own:
        steps.append(("q / gcd(q, own)", q // math.gcd(q, o)))
        q = q // math.gcd(q, o) * o
    for o in own:
        steps.append(("q_B / own", q // o))
    return q, steps


for name, dens in (("as written", [6, 6]), ("reduced", [2, 3])):
    q, steps = quotients(dens)
    print(f"{name}: denominators {dens}, q_B = {q}")
    for label, v in steps:
        print(f"  {label} = {v}")
