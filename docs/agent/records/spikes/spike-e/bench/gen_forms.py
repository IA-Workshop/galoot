"""Write forms.hpp: forms (A), (B), and (C) of the rigid(3) transforms as
straight-line C++ kernels, from the shared-node programs of ../forms.py.

Run with the math-tools Python environment (sympy), from anywhere:

  <env>/bin/python -B gen_forms.py            # writes forms.hpp beside it
  <env>/bin/python -B gen_forms.py --check    # exit 1 if forms.hpp differs

Each form of each object is a struct of two kernels. `setup(q)` computes
every value of the program that reads only the motor's coefficients and
that the rest of the program reads; `apply(m, q, x)` computes the rest from
those values, the motor, and the object. A call of the form is
apply(setup(q), q, x); a batch under one motor calls setup once. The
program's nodes become `const T` temporaries in node order, which is a
topological order, so each shared value is computed once: the counts of
the kernels are the counts of COUNTING-RULE.md's rule S for that program,
which gen_forms.py prints and checks against ../counts.json.
"""

import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
SPIKE = os.path.dirname(HERE)
sys.path.insert(0, SPIKE)

import dag as D      # noqa: E402
import forms as F    # noqa: E402
import rigid3 as R   # noqa: E402

OBJECTS = (("point", R.POINT, "p", F.form_C_point),
           ("line", R.LINE, "l", F.form_C_line),
           ("plane", R.PLANE, "g", F.form_C_plane))


def programs():
    """(object, form) -> (dag, outputs, layout, suffix)."""
    out = {}
    for name, layout, suffix, form_c in OBJECTS:
        g, o = F.form_A(layout, suffix)
        out[name, "a"] = (g, o, layout, suffix)
        g, o, _ = F.form_B(layout, suffix, grouped=True)
        out[name, "b"] = (g, o, layout, suffix)
        g, o = form_c(True)  # the motor's doubles made once, per motor
        out[name, "c"] = (g, o, layout, suffix)
    return out


def operand(dag, i, layout, suffix, setup_slot):
    """The C++ text that reads node i from apply's point of view."""
    node = dag.nodes[i]
    if node[0] == "var":
        name = node[1]
        motor = [n for n, _, _ in R.MOTOR]
        if name in motor:
            return f"q[{motor.index(name)}]"
        names = [n + suffix for n, _, _ in layout]
        return f"x[{names.index(name)}]"
    if i in setup_slot:
        return f"m[{setup_slot[i]}]"
    return f"t{i}"


def statement(dag, i, read):
    kind, *args = dag.nodes[i]
    if kind == "mul":
        return f"{read(args[0])} * {read(args[1])}"
    if kind == "add":
        return f"{read(args[0])} + {read(args[1])}"
    if kind == "sub":
        return f"{read(args[0])} - {read(args[1])}"
    if kind == "dbl":
        return f"{read(args[0])} + {read(args[0])}"
    raise ValueError(kind)


def kernels(name, form, g, outs, layout, suffix):
    dep = D.depends(g, lambda n: not n.startswith("Q"))
    seen = D.reachable(g, outs)
    uses = {i: 0 for i in seen}
    for i in seen:
        for j in g.nodes[i][1:]:
            if isinstance(j, int):
                uses[j] += 1
    for o in outs:
        uses[o.node] += 1

    # The motor-only values that the object's part reads.
    exported = []
    for i in sorted(seen):
        if dep[i]:
            for j in g.nodes[i][1:]:
                if (isinstance(j, int) and not dep[j]
                        and g.nodes[j][0] != "var" and j not in exported):
                    exported.append(j)
    exported.sort()
    slot = {j: k for k, j in enumerate(exported)}
    for o in outs:
        assert dep[o.node], "an output that reads no object coefficient"

    def setup_read(j):
        node = g.nodes[j]
        if node[0] == "var":
            return operand(g, j, layout, suffix, {})
        return f"t{j}"

    def apply_read(j):
        return operand(g, j, layout, suffix, slot)

    size_m = len(exported)
    size_x = len(layout)
    fn = f"transform_{name}_{form}"
    text = []
    setup_body = [f"  const T t{i} = {statement(g, i, setup_read)};"
                  for i in sorted(seen)
                  if not dep[i] and g.nodes[i][0] != "var"]
    q_name = "q" if setup_body else "/*q*/"
    text.append("template <class T>")
    text.append(f"struct {fn} {{")
    text.append(f"  static constexpr char name[] = \"{form}\";")
    text.append("")
    text.append(f"  [[gnu::always_inline]] static constexpr std::array<T, {size_m}>")
    text.append(f"  setup(const std::array<T, 8>& {q_name}) {{")
    text += ["  " + line for line in setup_body]
    if size_m:
        text.append("    return {" + ", ".join(f"t{j}" for j in exported)
                    + "};")
    else:
        text.append("    return {};")
    text.append("  }")
    text.append("")

    apply_body = []
    for i in sorted(seen):
        if not dep[i] or g.nodes[i][0] == "var":
            continue
        apply_body.append(f"  const T t{i} = {statement(g, i, apply_read)};")
    results = []
    for o in outs:
        node = g.nodes[o.node]
        if node[0] == "var":
            results.append(("" if o.sign > 0 else "-") + apply_read(o.node))
        elif o.sign > 0:
            results.append(f"t{o.node}")
        elif node[0] == "sub" and uses[o.node] == 1:
            # -(a - b) is b - a, at no cost
            k = next(k for k, line in enumerate(apply_body)
                     if line.startswith(f"  const T t{o.node} = "))
            apply_body[k] = (f"  const T t{o.node} = {apply_read(node[2])}"
                             f" - {apply_read(node[1])};")
            results.append(f"t{o.node}")
        else:
            results.append(f"-t{o.node}")
    body = "\n".join(apply_body)
    reads_m = "m[" in body
    reads_q = "q[" in body
    text.append(f"  [[gnu::always_inline]] static constexpr std::array<T, {size_x}>")
    text.append(f"  apply(const std::array<T, {size_m}>& "
                f"{'m' if reads_m else '/*m*/'}, "
                f"const std::array<T, 8>& {'q' if reads_q else '/*q*/'},")
    text.append(f"        const std::array<T, {size_x}>& x) {{")
    text += ["  " + line for line in apply_body]
    text.append("    return {" + ", ".join(results) + "};")
    text.append("  }")
    text.append("};")
    text.append("")

    # Counts from the text itself: one operator per statement.
    def tally(lines):
        c = {"mul": 0, "add": 0, "scl": 0, "neg": 0}
        for line in lines:
            rhs = line.split(" = ", 1)[1].rstrip(";")
            a, op, b = rhs.split(" ")
            if op == "*":
                c["mul"] += 1
            elif a == b:
                c["scl"] += 1
            else:
                c["add"] += 1
        return c
    cs = tally(setup_body)
    ca = tally(apply_body)
    ca["neg"] = sum(1 for r in results if r.startswith("-"))
    return text, cs, ca, size_m


HEADER = """\
// Forms of the rigid(3) transforms by a motor, as straight-line kernels over
// any scalar T, for timing against each other. Written by gen_forms.py from
// the programs of the forms; do not edit.
//
// Layouts: a point on e1 e2 e3 e4, a line on e41 e42 e43 e23 e31 e12, a
// plane on e423 e431 e412 e321, and a motor on e41 e42 e43 e1234 e23 e31
// e12 1, with e41 = -e14, e31 = -e13, e321 = -e123.
//
// a  The terms of Q v x v antireverse(Q) that reach x's blades, two stages,
//    as the library's transform computes them, with each sign carried into
//    a subtraction.
// b  The collected form: the transform is linear in x, y = M(Q) x, and each
//    entry of M is a sum of products of two motor coefficients. setup makes
//    each distinct product once and each entry once, applying a factor 2
//    common to an entry's products once to the entry's sum; apply is M x.
//    It equals a for every Q.
// c  A form that assumes the motor is unitized and satisfies its geometric
//    constraint, with v = (Qvx, Qvy, Qvz), s = Qvw, m = (Qmx, Qmy, Qmz),
//    t = Qmw, |v|^2 + s^2 = 1 and v . m + s t = 0, and a = v x u:
//      point  p' = p + 2 (s c + v x c - w t v), c = v x p + w m, w' = w
//      plane  n' = n + 2 (s a + v x a), g_w' = g_w - n' . T
//      line   d' = d + 2 (s a + v x a), k' = R k + T x d'
//    with T = 2 (s m - t v + v x m) and R the rotation of the first line.
//    setup doubles v and s once, and makes T where it is read. It equals a
//    only on such motors.
//
// Each form is a struct of two functions: setup(q) holds the work that
// reads only the motor, apply(m, q, x) the rest, so a batch of objects under
// one motor calls setup once.
// clang-format off
#ifndef SPIKE_E_BENCH_FORMS_HPP
#define SPIKE_E_BENCH_FORMS_HPP

#include <array>

namespace forms {

"""

FOOTER = """\
} // namespace forms

#endif // SPIKE_E_BENCH_FORMS_HPP
"""


def main(argv):
    with open(os.path.join(SPIKE, "counts.json")) as f:
        expected = json.load(f)
    chunks = [HEADER]
    table = []
    ok = True
    keys = {"a": "A", "b": "B", "c": "C-pre"}
    for (name, form), (g, outs, layout, suffix) in programs().items():
        text, cs, ca, size_m = kernels(name, form, g, outs, layout, suffix)
        chunks.append("\n".join(text) + "\n")
        e = expected[name][keys[form]]
        same = (cs["mul"], cs["add"], cs["scl"]) == (
            e["setup"]["mul"], e["setup"]["add"], e["setup"]["scl"]) and (
            ca["mul"], ca["add"], ca["scl"], ca["neg"]) == (
            e["object"]["mul"], e["object"]["add"], e["object"]["scl"],
            e["object"]["neg"])
        ok &= same
        table.append(f"{name:5} {form}  setup mul {cs['mul']:2} add "
                     f"{cs['add']:2} scl {cs['scl']:2} ({size_m:2} values)"
                     f"  apply mul {ca['mul']:2} add {ca['add']:2} scl "
                     f"{ca['scl']:2} neg {ca['neg']}"
                     f"  {'= counts.json' if same else 'DIFFERS from counts.json'}")
    chunks.append(FOOTER)
    text = "\n".join(chunks)
    path = os.path.join(HERE, "forms.hpp")
    print("\n".join(table))
    if "--check" in argv:
        with open(path) as f:
            if f.read() != text:
                print("forms.hpp differs from what gen_forms.py writes")
                return 1
    else:
        with open(path, "w") as f:
            f.write(text)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
