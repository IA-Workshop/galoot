"""Which guard site of blade_product.hpp is the first to see an overflow?
A line-by-line port of the recursion with exact Python integers; each guarded
operation checks |result| <= 2^63 - 1 and records its site on the first miss."""
import random, sys, itertools
MAX = 2**63 - 1

class Overflow(Exception):
    pass

def key(m):
    return (bin(m).count("1"), [i for i in range(32) if m >> i & 1])

def reorder_sign(a, b):
    s = 0
    a >>= 1
    while a:
        s += bin(a & b).count("1"); a >>= 1
    return -1 if s & 1 else 1

def chk(v, site):
    if abs(v) > MAX:
        raise Overflow(site)
    return v

def add(lst, blade, c):
    if c == 0:
        return
    at = 0
    while at < len(lst) and key(lst[at][0]) < key(blade):
        at += 1
    if at < len(lst) and lst[at][0] == blade:
        v = chk(lst[at][1] + c, "sum")
        if v == 0:
            del lst[at]
        else:
            lst[at] = (blade, v)
        return
    lst.insert(at, (blade, c))

def gen_product(out, g, n, i, x, scale):
    gen = 1 << i
    for blade, coef in list(x):
        c = chk(coef * scale, "scale")
        if blade & gen == 0:
            add(out, blade | gen, chk(reorder_sign(gen, blade) * c, "wedge"))
        sign = 1
        for j in range(n):
            if not blade >> j & 1:
                continue
            e = g[i][j]
            if e != 0:
                add(out, blade & ~(1 << j), chk(chk(sign * e, "inner") * c, "contraction"))
            sign = -sign

def blade_prod(out, g, n, a, b, scale):
    if a == 0:
        add(out, b, scale); return
    low = (a & -a).bit_length() - 1
    rest = a & (a - 1)
    rp = []
    blade_prod(rp, g, n, rest, b, 1)
    gen_product(out, g, n, low, rp, scale)
    sign = -1
    for j in range(n):
        if not rest >> j & 1:
            continue
        e = g[low][j]
        if e != 0:
            blade_prod(out, g, n, rest & ~(1 << j), b, chk(chk(sign * e, "inner") * scale, "correction"))
        sign = -sign

def survey(g, n, counts):
    for a in range(1 << n):
        for b in range(1 << n):
            try:
                blade_prod([], g, n, a, b, 1)
            except Overflow as e:
                counts[e.args[0]] = counts.get(e.args[0], 0) + 1

K = 32767
rng = random.Random(int(sys.argv[1]) if len(sys.argv) > 1 else 1)
counts = {}
forms = 0
for n in (5, 6):
    for trial in range(int(sys.argv[2]) if len(sys.argv) > 2 else 6):
        lo = rng.choice([1, K // 4, K // 2, K - 100])
        g = [[0] * n for _ in range(n)]
        for i in range(n):
            for j in range(i, n):
                v = rng.randint(lo, K) * rng.choice([1, -1])
                if rng.random() < 0.3:
                    v = rng.choice([0, 1, -1, K, -K])
                g[i][j] = g[j][i] = v
        survey(g, n, counts); forms += 1
    # the overflow test's form, and its sign variants
    for s in (1, -1):
        g = [[K if i == j else s for j in range(n)] for i in range(n)]
        survey(g, n, counts); forms += 1
print("forms", forms, "first overflow by site:", counts)
