"""Clifford algebra by explicit matrices over Q (python-flint fmpq_mat).

Route: congruence-diagonalize the Gram matrix over Q, g = M D M^T; represent the
orthogonal generators f_k by Jordan-Wigner matrices on (Q^2)^(x)n
(f_k = Z x .. x Z x A_k x I x .. x I, A_k = [[0, D_k], [1, 0]]); put
e_i = sum_k M_ik f_k; define the blade of a mask as the antisymmetrized product
of its generators (the wedge, valid in characteristic 0 for any form); read
coefficients back by acting on the vacuum vector.  No bit-mask sign rule is used
for the geometric product.
"""
from fractions import Fraction as Fr
from itertools import combinations, permutations
import flint

Q = flint.fmpq


def perm_sign(seq):
    s = 1
    seq = list(seq)
    for i in range(len(seq)):
        for j in range(i + 1, len(seq)):
            if seq[i] > seq[j]:
                s = -s
    return s


def congruence_diag(g):
    """Return (M, D) with g = M diag(D) M^T, M rational invertible."""
    n = len(g)
    A = [[Q(x) for x in row] for row in g]
    T = [[Q(int(i == j)) for j in range(n)] for i in range(n)]  # T g T^T = current A

    def addrow(i, j, c):  # row_i += c row_j, and same on columns
        for k in range(n):
            A[i][k] += c * A[j][k]
        for k in range(n):
            A[k][i] += c * A[k][j]
        for k in range(n):
            T[i][k] += c * T[j][k]

    def swap(i, j):
        A[i], A[j] = A[j], A[i]
        for row in A:
            row[i], row[j] = row[j], row[i]
        T[i], T[j] = T[j], T[i]

    for p in range(n):
        if A[p][p] == 0:
            q = next((q for q in range(p + 1, n) if A[q][q] != 0), None)
            if q is not None:
                swap(p, q)
            else:
                q = next((q for q in range(p + 1, n) if A[p][q] != 0), None)
                if q is None:
                    continue
                addrow(p, q, Q(1))
        if A[p][p] == 0:
            continue
        for q in range(p + 1, n):
            if A[q][p] != 0:
                addrow(q, p, -A[q][p] / A[p][p])
    D = [A[i][i] for i in range(n)]
    Tm = flint.fmpq_mat(T)
    M = Tm.inv()
    # check
    Gm = flint.fmpq_mat([[Q(x) for x in r] for r in g])
    Dm = flint.fmpq_mat([[D[i] if i == j else Q(0) for j in range(n)] for i in range(n)])
    assert M * Dm * M.transpose() == Gm
    return M, D


def kron(a, b):
    ra, ca, rb, cb = a.nrows(), a.ncols(), b.nrows(), b.ncols()
    out = flint.fmpq_mat(ra * rb, ca * cb)
    for i in range(ra):
        for j in range(ca):
            if a[i, j] != 0:
                for k in range(rb):
                    for l in range(cb):
                        out[i * rb + k, j * cb + l] = a[i, j] * b[k, l]
    return out


class Alg:
    def __init__(self, g):
        self.g = [[Q(x) for x in r] for r in g]
        n = self.n = len(g)
        self.N = 1 << n
        M, D = congruence_diag(g)
        Z = flint.fmpq_mat([[1, 0], [0, -1]])
        I2 = flint.fmpq_mat([[1, 0], [0, 1]])
        f = []
        for k in range(n):
            A = flint.fmpq_mat([[0, D[k]], [1, 0]])
            m = flint.fmpq_mat([[1]])
            for j in range(n):
                m = kron(m, Z if j < k else (A if j == k else I2))
            f.append(m)
        dim = 1 << n
        self.e = []
        for i in range(n):
            m = flint.fmpq_mat(dim, dim)
            for k in range(n):
                if M[i, k] != 0:
                    m += M[i, k] * f[k]
            self.e.append(m)
        # generator relations
        for i in range(n):
            for j in range(n):
                assert self.e[i] * self.e[j] + self.e[j] * self.e[i] == 2 * self.g[i][j] * flint.fmpq_mat([[int(a == b) for b in range(dim)] for a in range(dim)])
        self.Id = flint.fmpq_mat([[int(a == b) for b in range(dim)] for a in range(dim)])
        self.blade = {}
        for mask in range(dim):
            idx = [i for i in range(n) if mask >> i & 1]
            k = len(idx)
            if k == 0:
                self.blade[mask] = self.Id
                continue
            acc = flint.fmpq_mat(dim, dim)
            fact = 1
            for t in range(2, k + 1):
                fact *= t
            for p in permutations(range(k)):
                prod = self.Id
                for t in p:
                    prod = prod * self.e[idx[t]]
                acc += perm_sign(p) * prod
            self.blade[mask] = acc * Q(1, fact)
        # coefficient readout through the vacuum vector
        cols = [[self.blade[m][r, 0] for m in range(dim)] for r in range(dim)]
        self.B = flint.fmpq_mat(cols)
        self.Binv = self.B.inv()

    # multivectors are lists of fmpq indexed by mask
    def zero(self):
        return [Q(0)] * self.N

    def mat(self, a):
        m = flint.fmpq_mat(self.N, self.N)
        for mask, c in enumerate(a):
            if c != 0:
                m += c * self.blade[mask]
        return m

    def coef(self, m):
        v = flint.fmpq_mat([[m[r, 0]] for r in range(self.N)])
        c = self.Binv * v
        out = [c[i, 0] for i in range(self.N)]
        assert self.mat(out) == m, "not in the algebra"
        return out

    def gp(self, a, b):
        return self.coef(self.mat(a) * self.mat(b))

    def grade(self, mask):
        return bin(mask).count("1")

    def basis(self, mask, c=1):
        v = self.zero()
        v[mask] = Q(c)
        return v

    def vec(self, cs):
        v = self.zero()
        for i, c in enumerate(cs):
            v[1 << i] = Q(c)
        return v

    def add(self, *xs):
        out = self.zero()
        for x in xs:
            out = [p + q for p, q in zip(out, x)]
        return out

    def scale(self, s, a):
        return [Q(s) * c for c in a]

    def gradepart(self, a, k):
        return [c if self.grade(m) == k else Q(0) for m, c in enumerate(a)]

    # metric-free wedge: sign of the reordering of the concatenated index lists
    def wedge_blades(self, A, B):
        if A & B:
            return 0, 0
        la = [i for i in range(self.n) if A >> i & 1]
        lb = [i for i in range(self.n) if B >> i & 1]
        return perm_sign(la + lb), A | B

    def wedge(self, a, b):
        out = self.zero()
        for A, ca in enumerate(a):
            if ca == 0:
                continue
            for B, cb in enumerate(b):
                if cb == 0:
                    continue
                s, C = self.wedge_blades(A, B)
                if s:
                    out[C] += s * ca * cb
        return out

    def full(self):
        return self.N - 1

    def right(self, a):
        out = self.zero()
        for A, c in enumerate(a):
            if c != 0:
                s, _ = self.wedge_blades(A, self.full() ^ A)  # e_A ^ s e_~A = 1
                out[self.full() ^ A] += s * c
        return out

    def left(self, a):
        out = self.zero()
        for A, c in enumerate(a):
            if c != 0:
                s, _ = self.wedge_blades(self.full() ^ A, A)
                out[self.full() ^ A] += s * c
        return out

    def by_grade_sign(self, a, f):
        return [c * f(self.grade(m)) for m, c in enumerate(a)]

    def rev(self, a):
        return self.by_grade_sign(a, lambda k: (-1) ** (k * (k - 1) // 2))

    def antirev(self, a):
        n = self.n
        return self.by_grade_sign(a, lambda k: (-1) ** ((n - k) * (n - k - 1) // 2))

    def ginv(self, a):
        return self.by_grade_sign(a, lambda k: (-1) ** k)

    def aginv(self, a):
        n = self.n
        return self.by_grade_sign(a, lambda k: (-1) ** (n - k))

    # metric exomorphism: wedge of the images g(e_i) = sum_j g_ij e_j
    def G(self, a):
        out = self.zero()
        for A, c in enumerate(a):
            if c == 0:
                continue
            img = self.basis(0)
            for i in range(self.n):
                if A >> i & 1:
                    img = self.wedge(img, self.vec(self.g[i]))
            out = self.add(out, self.scale(c, img))
        return out

    def AG(self, a):  # antiexomorphism, taken as left o G o right
        return self.left(self.G(self.right(a)))

    def bulk(self, a):
        return self.G(a)

    def weight(self, a):
        return self.AG(a)

    def bdual(self, a):
        return self.right(self.G(a))

    def wdual(self, a):
        return self.right(self.AG(a))

    def dot(self, a, b):
        Gb = self.G(b)
        return sum((x * y for x, y in zip(a, Gb)), Q(0))

    def antidot(self, a, b):
        Gb = self.AG(b)
        return sum((x * y for x, y in zip(a, Gb)), Q(0))

    def agp(self, a, b):
        return self.left(self.gp(self.right(a), self.right(b)))

    def antiwedge(self, a, b):
        return self.left(self.wedge(self.right(a), self.right(b)))

    def parity_antigrade(self, a):
        ps = {(self.n - self.grade(m)) % 2 for m, c in enumerate(a) if c != 0}
        assert len(ps) == 1
        return ps.pop()

    def transform(self, x, V):
        if self.parity_antigrade(V) == 0:
            return self.agp(self.agp(V, x), self.antirev(V))
        return self.scale(-1, self.agp(self.agp(V, self.aginv(x)), self.antirev(V)))

    def compose(self, Q2, Q1):
        return self.agp(Q2, Q1)

    def project(self, a, b):
        return self.antiwedge(b, self.wedge(a, self.wdual(b)))

    def antiproject(self, a, b):
        return self.wedge(b, self.antiwedge(a, self.wdual(b)))

    def name(self, mask):
        return "e" + "".join(str(i + 1) for i in range(self.n) if mask >> i & 1) if mask else "1"

    def show(self, a):
        return " + ".join(f"{c}*{self.name(m)}" for m, c in enumerate(a) if c != 0) or "0"


def diag(*d):
    return [[d[i] if i == j else 0 for j in range(len(d))] for i in range(len(d))]


def conformal(d):
    n = d + 2
    g = diag(*([1] * d + [0, 0]))
    g[d][d + 1] = g[d + 1][d] = -1
    return g
