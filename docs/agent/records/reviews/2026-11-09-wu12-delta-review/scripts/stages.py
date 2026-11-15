import importlib.util, sys
spec = importlib.util.spec_from_file_location("n", sys.argv[1])
src = open(sys.argv[1]).read().replace("\nmain()\n", "\n")
ns = {}
exec(compile(src, "noting", "exec"), ns)
Z3 = [[0]*3 for _ in range(3)]
m = 2**31 - 1
cases = {
 "inverse_rows witness, null forms": ([[0, 1048576, 0], [0, -16777216, 46341], [2, -378, 0]], [1, 1073741824, 2147483647], Z3, Z3),
 "inverts witness, null forms": ([[65536, 0, 0], [1, -2, 0], [0, 1073741824, 1048576]], [2124569019, 2, 2], Z3, Z3),
 "2x2 elimination, 2I against I": ([[m, m], [-m, m]], [m, m], [[2,0],[0,2]], [[1,0],[0,1]]),
}
for k, (r, d, G, Gp) in cases.items():
    print(k, "->", ns["stage"](r, d, G, Gp), "elim:", ns["eliminate"](r)[0:3:2])
