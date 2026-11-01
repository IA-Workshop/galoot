# Stack use of blade_product at run time

`stack_probe.cpp` multiplies the antiscalar of R(16,0,0) by itself at run time with a term list of capacity 2^15, the `product_capacity` of that pair. Each level of the recursion on the grade holds one scratch `term_list` of that capacity, 524,296 bytes, and the recursion is 16 deep.

Measured at `b5a415f` (the header is unchanged since `78680f3`) by the WU6a author, GCC 16.2.1, `-O0 -g -DGALOOT_CHECKING=1`, each run through `tools/timed` (selections `stack-probe:gcc-O0:ulimit-*`):

```bash
g++ -std=c++23 -O0 -g -DGALOOT_CHECKING=1 -Iinclude -o probe docs/agent/records/performance/stack-probe/stack_probe.cpp
for lim in 8192 16384 unlimited; do (ulimit -s $lim; tools/timed measure - "stack-probe:gcc-O0:ulimit-$lim" -- ./probe); done
```

| `ulimit -s` (KiB) | Result |
| --- | --- |
| 8192, this machine's default | segmentation fault (exit 139) |
| 16384 | exit 0, one term, coefficient 1 |
| unlimited | exit 0 |

So a run-time product at the full capacity of n = 16 does not fit the default 8 MiB stack at -O0. No present test or use reaches it: the tests use capacity 32, and the constant evaluations are not on the machine stack. Optimized builds and other compilers were not measured.
