### probe.gcc.s

| operation | instructions | mul | of which mulsd, mulpd | add/sub | of which scalar, packed | neg (xor) | compares | branches | guard branches | memory operands | calls and symbol jumps |
| --- | ---: | ---: | --- | ---: | --- | ---: | ---: | ---: | ---: | ---: | --- |
| wedge_point_point | 46 | 12 | 8, 2 | 6 | 4, 1 | 3 | 0 | 0 | 0 | 15 | none |
| wedge_point_point_point | 70 | 24 | 24, 0 | 14 | 14, 0 | 2 | 0 | 0 | 0 | 16 | none |
| antiwedge_line_plane | 42 | 12 | 12, 0 | 8 | 8, 0 | 1 | 0 | 0 | 0 | 15 | none |
| transform_point | 174 | 48 | 32, 8 | 44 | 22, 11 | 1 | 0 | 0 | 0 | 34 | none |
| transform_line | 225 | 72 | 72, 0 | 58 | 58, 0 | 4 | 0 | 0 | 0 | 44 | none |
| transform_plane | 145 | 48 | 48, 0 | 36 | 36, 0 | 2 | 0 | 0 | 0 | 27 | none |
| compose_motors | 166 | 48 | 48, 0 | 40 | 40, 0 | 7 | 0 | 0 | 0 | 40 | none |

### probe.clang.s

| operation | instructions | mul | of which mulsd, mulpd | add/sub | of which scalar, packed | neg (xor) | compares | branches | guard branches | memory operands | calls and symbol jumps |
| --- | ---: | ---: | --- | ---: | --- | ---: | ---: | ---: | ---: | ---: | --- |
| wedge_point_point | 44 | 12 | 12, 0 | 6 | 6, 0 | 3 | 0 | 0 | 0 | 14 | none |
| wedge_point_point_point | 69 | 24 | 24, 0 | 14 | 14, 0 | 2 | 0 | 0 | 0 | 17 | none |
| antiwedge_line_plane | 41 | 12 | 12, 0 | 8 | 8, 0 | 1 | 0 | 0 | 0 | 15 | none |
| transform_point | 149 | 48 | 48, 0 | 36 | 36, 0 | 1 | 0 | 0 | 0 | 27 | transform_point jmp galoot::core::transform |
| (part) transform_point | 1 | 0 | 0, 0 | 0 | 0, 0 | 0 | 0 | 0 | 0 | 0 |  |
| (part) galoot::core::transform | 148 | 48 | 48, 0 | 36 | 36, 0 | 1 | 0 | 0 | 0 | 27 |  |
| transform_line | 251 | 72 | 62, 5 | 60 | 50, 5 | 4 | 0 | 0 | 1 | 48 | transform_line jmp galoot::core::transform; galoot::core::transform callq galoot::core::detail::run_product<first_kernel>; galoot::core::transform callq __stack_chk_fail |
| (part) transform_line | 1 | 0 | 0, 0 | 0 | 0, 0 | 0 | 0 | 0 | 0 | 0 |  |
| (part) galoot::core::transform | 132 | 36 | 36, 0 | 30 | 30, 0 | 2 | 0 | 0 | 1 | 25 |  |
| (part) galoot::core::detail::run_product<first_kernel> | 118 | 36 | 26, 5 | 30 | 20, 5 | 2 | 0 | 0 | 0 | 23 |  |
| transform_plane | 150 | 48 | 28, 10 | 42 | 20, 11 | 2 | 0 | 0 | 0 | 24 | transform_plane jmp galoot::core::transform |
| (part) transform_plane | 1 | 0 | 0, 0 | 0 | 0, 0 | 0 | 0 | 0 | 0 | 0 |  |
| (part) galoot::core::transform | 149 | 48 | 28, 10 | 42 | 20, 11 | 2 | 0 | 0 | 0 | 24 |  |
| compose_motors | 199 | 49 | 35, 7 | 41 | 29, 6 | 4 | 0 | 0 | 1 | 55 | compose_motors callq galoot::core::detail::run_product<product_kernel>; compose_motors callq __stack_chk_fail |
| (part) compose_motors | 32 | 0 | 0, 0 | 0 | 0, 0 | 1 | 0 | 0 | 1 | 16 |  |
| (part) galoot::core::detail::run_product<product_kernel> | 167 | 49 | 35, 7 | 41 | 29, 6 | 3 | 0 | 0 | 0 | 39 |  |

### probe.clang.inline-2000.s

| operation | instructions | mul | of which mulsd, mulpd | add/sub | of which scalar, packed | neg (xor) | compares | branches | guard branches | memory operands | calls and symbol jumps |
| --- | ---: | ---: | --- | ---: | --- | ---: | ---: | ---: | ---: | ---: | --- |
| wedge_point_point | 44 | 12 | 12, 0 | 6 | 6, 0 | 3 | 0 | 0 | 0 | 14 | none |
| wedge_point_point_point | 69 | 24 | 24, 0 | 14 | 14, 0 | 2 | 0 | 0 | 0 | 17 | none |
| antiwedge_line_plane | 41 | 12 | 12, 0 | 8 | 8, 0 | 1 | 0 | 0 | 0 | 15 | none |
| transform_point | 148 | 48 | 48, 0 | 36 | 36, 0 | 1 | 0 | 0 | 0 | 27 | none |
| transform_line | 236 | 72 | 72, 0 | 58 | 58, 0 | 4 | 0 | 0 | 0 | 42 | none |
| transform_plane | 149 | 48 | 28, 10 | 42 | 20, 11 | 2 | 0 | 0 | 0 | 24 | none |
| compose_motors | 166 | 48 | 48, 0 | 40 | 40, 0 | 6 | 0 | 0 | 0 | 34 | none |

### probe.clang.no-copyable.s

| operation | instructions | mul | of which mulsd, mulpd | add/sub | of which scalar, packed | neg (xor) | compares | branches | guard branches | memory operands | calls and symbol jumps |
| --- | ---: | ---: | --- | ---: | --- | ---: | ---: | ---: | ---: | ---: | --- |
| wedge_point_point | 44 | 12 | 12, 0 | 6 | 6, 0 | 3 | 0 | 0 | 0 | 14 | none |
| wedge_point_point_point | 69 | 24 | 24, 0 | 14 | 14, 0 | 2 | 0 | 0 | 0 | 17 | none |
| antiwedge_line_plane | 41 | 12 | 12, 0 | 8 | 8, 0 | 1 | 0 | 0 | 0 | 15 | none |
| transform_point | 149 | 48 | 48, 0 | 36 | 36, 0 | 1 | 0 | 0 | 0 | 27 | transform_point jmp galoot::core::transform |
| (part) transform_point | 1 | 0 | 0, 0 | 0 | 0, 0 | 0 | 0 | 0 | 0 | 0 |  |
| (part) galoot::core::transform | 148 | 48 | 48, 0 | 36 | 36, 0 | 1 | 0 | 0 | 0 | 27 |  |
| transform_line | 251 | 72 | 62, 5 | 60 | 50, 5 | 4 | 0 | 0 | 1 | 48 | transform_line jmp galoot::core::transform; galoot::core::transform callq galoot::core::detail::run_product<first_kernel>; galoot::core::transform callq __stack_chk_fail |
| (part) transform_line | 1 | 0 | 0, 0 | 0 | 0, 0 | 0 | 0 | 0 | 0 | 0 |  |
| (part) galoot::core::transform | 132 | 36 | 36, 0 | 30 | 30, 0 | 2 | 0 | 0 | 1 | 25 |  |
| (part) galoot::core::detail::run_product<first_kernel> | 118 | 36 | 26, 5 | 30 | 20, 5 | 2 | 0 | 0 | 0 | 23 |  |
| transform_plane | 150 | 48 | 28, 10 | 42 | 20, 11 | 2 | 0 | 0 | 0 | 24 | transform_plane jmp galoot::core::transform |
| (part) transform_plane | 1 | 0 | 0, 0 | 0 | 0, 0 | 0 | 0 | 0 | 0 | 0 |  |
| (part) galoot::core::transform | 149 | 48 | 28, 10 | 42 | 20, 11 | 2 | 0 | 0 | 0 | 24 |  |
| compose_motors | 188 | 48 | 48, 0 | 40 | 40, 0 | 4 | 0 | 0 | 1 | 47 | compose_motors callq galoot::core::detail::run_product<product_kernel>; compose_motors callq __stack_chk_fail |
| (part) compose_motors | 32 | 0 | 0, 0 | 0 | 0, 0 | 1 | 0 | 0 | 1 | 16 |  |
| (part) galoot::core::detail::run_product<product_kernel> | 156 | 48 | 48, 0 | 40 | 40, 0 | 3 | 0 | 0 | 0 | 31 |  |

### probe.clang.no-slp.s

| operation | instructions | mul | of which mulsd, mulpd | add/sub | of which scalar, packed | neg (xor) | compares | branches | guard branches | memory operands | calls and symbol jumps |
| --- | ---: | ---: | --- | ---: | --- | ---: | ---: | ---: | ---: | ---: | --- |
| wedge_point_point | 44 | 12 | 12, 0 | 6 | 6, 0 | 3 | 0 | 0 | 0 | 14 | none |
| wedge_point_point_point | 69 | 24 | 24, 0 | 14 | 14, 0 | 2 | 0 | 0 | 0 | 17 | none |
| antiwedge_line_plane | 41 | 12 | 12, 0 | 8 | 8, 0 | 1 | 0 | 0 | 0 | 15 | none |
| transform_point | 149 | 48 | 48, 0 | 36 | 36, 0 | 1 | 0 | 0 | 0 | 27 | transform_point jmp galoot::core::transform |
| (part) transform_point | 1 | 0 | 0, 0 | 0 | 0, 0 | 0 | 0 | 0 | 0 | 0 |  |
| (part) galoot::core::transform | 148 | 48 | 48, 0 | 36 | 36, 0 | 1 | 0 | 0 | 0 | 27 |  |
| transform_line | 247 | 72 | 72, 0 | 58 | 58, 0 | 4 | 0 | 0 | 1 | 48 | transform_line jmp galoot::core::transform; galoot::core::transform callq galoot::core::detail::run_product<first_kernel>; galoot::core::transform callq __stack_chk_fail |
| (part) transform_line | 1 | 0 | 0, 0 | 0 | 0, 0 | 0 | 0 | 0 | 0 | 0 |  |
| (part) galoot::core::transform | 132 | 36 | 36, 0 | 30 | 30, 0 | 2 | 0 | 0 | 1 | 25 |  |
| (part) galoot::core::detail::run_product<first_kernel> | 114 | 36 | 36, 0 | 28 | 28, 0 | 2 | 0 | 0 | 0 | 23 |  |
| transform_plane | 142 | 48 | 48, 0 | 36 | 36, 0 | 2 | 0 | 0 | 0 | 26 | transform_plane jmp galoot::core::transform |
| (part) transform_plane | 1 | 0 | 0, 0 | 0 | 0, 0 | 0 | 0 | 0 | 0 | 0 |  |
| (part) galoot::core::transform | 141 | 48 | 48, 0 | 36 | 36, 0 | 2 | 0 | 0 | 0 | 26 |  |
| compose_motors | 191 | 48 | 48, 0 | 40 | 40, 0 | 3 | 0 | 0 | 1 | 50 | compose_motors callq galoot::core::detail::run_product<product_kernel>; compose_motors callq __stack_chk_fail |
| (part) compose_motors | 35 | 0 | 0, 0 | 0 | 0, 0 | 0 | 0 | 0 | 1 | 19 |  |
| (part) galoot::core::detail::run_product<product_kernel> | 156 | 48 | 48, 0 | 40 | 40, 0 | 3 | 0 | 0 | 0 | 31 |  |

### Benchmark programs

- `wedge_point_point_bench.gcc.s`: bench::drawn_pool<> call operator new; bench::main<> call bench::drawn_pool<>; bench::main<> call bench::main<>::{lambda#1}::operator; bench::main<> call operator new; bench::main<>::{lambda#1}::operator call bench::run<>; main call bench::main<>
- `wedge_point_point_point_bench.gcc.s`: bench::drawn_pool<> call operator new; bench::main<> call bench::drawn_pool<>; bench::main<> call bench::main<>::{lambda#1}::operator; bench::main<> call operator new; bench::main<>::{lambda#1}::operator call bench::run<>; main call bench::main<>
- `antiwedge_line_plane_bench.gcc.s`: bench::drawn_pool<> call operator new; bench::main<> call bench::drawn_pool<>; bench::main<> call bench::main<>::{lambda#1}::operator; bench::main<> call operator new; bench::main<>::{lambda#1}::operator call bench::run<>; main call bench::main<>
- `transform_point_bench.gcc.s`: bench::drawn_pool<> call operator new; bench::main<> call bench::drawn_pool<>; bench::main<> call bench::main<>::{lambda#1}::operator; bench::main<> call operator new; bench::main<>::{lambda#1}::operator call reference_kernels::transform_point<>; main call bench::main<>
  - out of line: reference_kernels::transform_point<>, 157 instructions with what it calls, 48 mul (32 mulsd, 8 mulpd), 44 add/sub, 0 branches, 0 guard branches
- `transform_line_bench.gcc.s`: bench::drawn_pool<> call operator new; bench::main<> call bench::drawn_pool<>; bench::main<> call bench::main<>::{lambda#1}::operator; bench::main<> call operator new; bench::main<>::{lambda#1}::operator call reference_kernels::transform_line<>; main call bench::main<>
  - out of line: reference_kernels::transform_line<>, 219 instructions with what it calls, 72 mul (72 mulsd, 0 mulpd), 58 add/sub, 0 branches, 0 guard branches
- `transform_plane_bench.gcc.s`: bench::drawn_pool<> call operator new; bench::main<> call bench::drawn_pool<>; bench::main<> call bench::main<>::{lambda#1}::operator; bench::main<> call operator new; bench::main<>::{lambda#1}::operator call reference_kernels::transform_plane<>; main call bench::main<>
  - out of line: reference_kernels::transform_plane<>, 145 instructions with what it calls, 48 mul (48 mulsd, 0 mulpd), 36 add/sub, 0 branches, 0 guard branches
- `compose_motors_bench.gcc.s`: bench::drawn_pool<> call bench::drawn<>; bench::drawn_pool<> call operator new; bench::main<> call bench::drawn_pool<>; bench::main<> call bench::main<>::{lambda#1}::operator; bench::main<> call operator new; bench::main<>::{lambda#1}::operator call galoot::core::detail::run_product<product_kernel>; bench::main<>::{lambda#1}::operator call reference_kernels::compose_motors<>; main call bench::main<>
  - out of line: galoot::core::detail::run_product<product_kernel>, 159 instructions with what it calls, 48 mul (48 mulsd, 0 mulpd), 40 add/sub, 0 branches, 0 guard branches
  - out of line: reference_kernels::compose_motors<>, 159 instructions with what it calls, 48 mul (48 mulsd, 0 mulpd), 40 add/sub, 0 branches, 0 guard branches
- `wedge_point_point_bench.clang.s`: bench::drawn_pool<> call operator new; main call bench::drawn_pool<>; main call bench::main<>::{lambda#1}::operator; main call operator new
- `wedge_point_point_point_bench.clang.s`: bench::drawn_pool<> call operator new; main call bench::drawn_pool<>; main call bench::main<>::{lambda#1}::operator; main call operator new
- `antiwedge_line_plane_bench.clang.s`: bench::drawn_pool<> call operator new; main call bench::drawn_pool<>; main call bench::main<>::{lambda#1}::operator; main call operator new
- `transform_point_bench.clang.s`: bench::drawn_pool<> call operator new; bench::main<>::{lambda#1}::operator call galoot::core::transform; bench::main<>::{lambda#1}::operator call reference_kernels::transform_point<>; main call bench::drawn<>; main call bench::drawn_pool<>; main call bench::main<>::{lambda#1}::operator; main call operator new
  - out of line: galoot::core::transform, 148 instructions with what it calls, 48 mul (48 mulsd, 0 mulpd), 36 add/sub, 0 branches, 0 guard branches
  - out of line: reference_kernels::transform_point<>, 156 instructions with what it calls, 48 mul (48 mulsd, 0 mulpd), 36 add/sub, 0 branches, 0 guard branches
- `transform_line_bench.clang.s`: bench::drawn_pool<> call operator new; bench::main<>::{lambda#1}::operator call galoot::core::transform; bench::main<>::{lambda#1}::operator call reference_kernels::transform_line<>; galoot::core::transform call galoot::core::detail::run_product<first_kernel>; main call bench::drawn<>; main call bench::drawn_pool<>; main call bench::main<>::{lambda#1}::operator; main call operator new
  - out of line: galoot::core::transform, 250 instructions with what it calls, 72 mul (62 mulsd, 5 mulpd), 60 add/sub, 0 branches, 1 guard branches
  - out of line: galoot::core::detail::run_product<first_kernel>, 118 instructions with what it calls, 36 mul (26 mulsd, 5 mulpd), 30 add/sub, 0 branches, 0 guard branches
  - out of line: reference_kernels::transform_line<>, 240 instructions with what it calls, 72 mul (72 mulsd, 0 mulpd), 58 add/sub, 0 branches, 0 guard branches
- `transform_plane_bench.clang.s`: bench::drawn_pool<> call operator new; bench::main<>::{lambda#1}::operator call galoot::core::transform; bench::main<>::{lambda#1}::operator call reference_kernels::transform_plane<>; main call bench::drawn<>; main call bench::drawn_pool<>; main call bench::main<>::{lambda#1}::operator; main call operator new
  - out of line: galoot::core::transform, 149 instructions with what it calls, 48 mul (28 mulsd, 10 mulpd), 42 add/sub, 0 branches, 0 guard branches
  - out of line: reference_kernels::transform_plane<>, 148 instructions with what it calls, 48 mul (28 mulsd, 10 mulpd), 42 add/sub, 0 branches, 0 guard branches
- `compose_motors_bench.clang.s`: bench::main<>::{lambda#1}::operator call galoot::core::detail::run_product<product_kernel>; bench::main<>::{lambda#1}::operator call reference_kernels::compose_motors<>; main call bench::drawn<>; main call bench::main<>::{lambda#1}::operator; main call operator new
  - out of line: galoot::core::detail::run_product<product_kernel>, 167 instructions with what it calls, 49 mul (35 mulsd, 7 mulpd), 41 add/sub, 0 branches, 0 guard branches
  - out of line: reference_kernels::compose_motors<>, 137 instructions with what it calls, 48 mul (0 mulsd, 24 mulpd), 52 add/sub, 0 branches, 0 guard branches

