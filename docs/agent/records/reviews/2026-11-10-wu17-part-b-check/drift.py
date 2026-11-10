import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sympy as sp
from formb import results, Q, objs
vx, vy, vz, vw, mx, my, mz, mw = Q
v = sp.Matrix([vx, vy, vz]); m = sp.Matrix([mx, my, mz]); s, t = vw, mw
px, py, pz, pw = objs["point"][1]; P = sp.Matrix([px, py, pz])
c = v.cross(P) + pw * m
C = list(P + 2 * (s * c + v.cross(c) - pw * t * v)) + [pw]
e = sp.symbols("e")
# unit rotor about z by 90 degrees (half angle 45: use rational 3/5, 4/5), translation 0, scaled by (1+e)
num = {vx: 0, vy: 0, vz: sp.Rational(3, 5) * (1 + e), vw: sp.Rational(4, 5) * (1 + e), mx: 0, my: 0, mz: 0, mw: 0}
pt = {px: 1, py: 0, pz: 0, pw: 1}
S = [sp.simplify(x.subs(num).subs(pt)) for x in results["point"][0]]
Cv = [sp.simplify(x.subs(num).subs(pt)) for x in C]
print("sandwich, unitized:", [sp.simplify(a / S[3]) for a in S])
print("form C            :", [sp.series(a / Cv[3], e, 0, 2).removeO() for a in Cv])
