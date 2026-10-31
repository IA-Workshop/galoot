# 0008 Spike A: cost and limits of constant-evaluated product plans

Date: 2026-10-31. Checked by: a spike subagent (Claude Opus 5.5) working for the owner. Scope: plan 0001, section 9, Spike A. It measures what constant evaluation costs, and where it stops, when the full geometric-product plan is generated at n = 4 to 8 with a minimal throwaway generator, on GCC 16.2.1 and Clang 23.1.4 (decision 0006). It also compares class-type value template arguments with tag types. Nothing here is library code, and no file under `include/` exists or was touched.

Revision and provenance. The runs used `develop` at several revisions, because the owner committed governance changes while the spike ran. `runs.tsv` records each run's revision (2862040d to 870343ca). None of the commits in that range touch `test/reference/`, `tools/`, or the workbench, so the measured inputs did not change (`git diff --name-only 68eb9813 870343ca`). The correctness checks, the NTTP comparison, and the second grid sample (`main2`) ran at `870343ca45d0e81f07ddd037dd59697644751d2a`. The spike sources and results were afterward copied unchanged from the ignored workbench to `docs/agent/records/spikes/` and committed, so the figures are reproducible from that commit; the runs themselves were made from the workbench copies.

How each claim was checked:

- **Local**: measured on this machine with the scripts named. Most grid points ran twice (sets `main` and `main2`).
- **Docs**: read in the GCC 16 manual installed at `/usr/share/man/man1/g++.1.gz` and `/usr/share/info/gcc.info.gz`.
- **Inferred**: my reasoning, not checked.

## Summary

1. One `constexpr` function generates plans both at compile time and at run time. `runtime_check.cpp` compares a constant-evaluated conformal(3) plan with the same function run on a metric read from the command line: equal, 1024 terms, on both compilers.
2. The generator's tables agree with the independent Python reference (note 0009) on every blade pair of 14 signatures, including conformal(3), degenerate and dense random forms, on both compilers.
3. With default limits, the full gp plan (all 2ⁿ × 2ⁿ blade pairs) compiles up to n = 5 on Clang and n = 7 on GCC for diagonal and conformal metrics. With a dense metric the limits are n = 4 on Clang and n = 5 on GCC. Clang stops at its step limit, raised by `-fconstexpr-steps=N`, where 0 means no limit. GCC stops at its operation limit of 2²⁵, raised by `-fconstexpr-ops-limit=N`. Neither compiler reached its loop or depth limit.
4. Cost of the full plan at n = 8 (euclidean) with raised limits: GCC takes 6.3 s and 1.2 GiB at -O0, or 11.5 s and 2.0 GiB at -O2. Clang takes 48.3 s and 214 to 221 MiB at either level. Clang's instruction count grows about 7× per added generator, GCC's about 4.6×.
5. GCC at -O2 constant-evaluates the plan twice. Making the plan's initializing lambda `consteval` removes the second evaluation: 2.73 s and 574 MiB become 1.47 s and 330 MiB at n = 7. Clang is unaffected.
6. Scratch capacity matters on Clang. The generator's scratch term lists hold 2ⁿ terms, and Clang pays to zero-fill them. Capacity 4 makes Clang 2.7× faster at n = 8, while GCC is unchanged.
7. Value template arguments give long names and, on GCC, misleading diagnostics: GCC prints conformal(3) and R(4,1,0) as if both were diagonal. Tag types give short names and clear diagnostics. A hybrid, with the algebra as a tag and blade sets as values, keeps the clear diagnostics at about 60% of the value-only name length.

Recommendations (Inferred from the measurements):

- **Generator.** Keep the plan generator `constexpr` and run the exhaustive identity proofs (DoD-10, n ≤ 6) at run time with the same function. That needs no raised limits and no build-time cost. Wrap the compile-time use in a `consteval` lambda or function, so GCC -O2 evaluates it once.
- **Practical n.** The full 2ⁿ × 2ⁿ plan is practical at compile time with default limits only for n ≤ 5 on both compilers (n ≤ 4 for dense forms). Past that it needs raised limits, and by n = 8 it costs tens of seconds (Clang) or gigabytes (GCC) per plan. Real programs generate plans only for the blade sets they use (C6), which are far smaller, so the cost per pair is what matters for n up to 16 (decision 0004). With the 2ⁿ scratch of this spike, n = 16 is not viable on Clang (Inferred: each scratch object would be 1 MiB, zero-filled at every recursion level of every pair). WU6 should size the scratch from the input blade sets, for example 2^(max |A ∪ B|), and measure P2 growth at fixed use over n = 4, 8, 16.
- **Limits.** Documented uses should compile under default limits. Where Galoot's own test build raises a limit, the preset states it (C8). Choosing that value is the owner's call, since it is a build option and not a bound (decision 0013).
- **Template arguments.** Use the hybrid: a named tag type that holds the metric value, and blade sets as values. STYLE S7 says signatures are "passed as class-type template arguments", so the hybrid departs from S7 for the signature and needs the owner's decision before WU3 and WU4. A pure tag design would also need computed blade sets as nested types, whose names grow with expression depth.
- **Mask type.** `std::uint32_t` served every measurement and needs no change for n ≤ 16 (decision 0004).

## The generator

`docs/agent/records/spikes/spike-a/gen.hpp`, about 230 lines:

- `metric<N>` holds an N × N `int` Gram matrix, and `blade_set<K>` holds K masks (`std::uint32_t`). Both are structural, so they can be class-type template arguments.
- `blade_product(m, a, b)` returns the terms of e_A e_B in the wedge-monomial basis (decision 0003; CONVENTIONS 1.1), using integer arithmetic only. It uses v x = v ⌋ x + v ∧ x for a generator v. For a blade, it uses e_A = e_i e_R − e_i ⌋ e_R, where i is the lowest index of A and R is the rest. The scratch is a fixed-capacity term list, `terms<Cap>`, with Cap = 2ⁿ by default and merging by linear search. No dynamic allocation is used.
- `generate(m, as, bs, out)` writes every term for every pair of `as` × `bs` into the span `out` and returns the count. Called with an empty span it only counts, which is the first of the two passes.
- `plan<M, A, B>` is a variable template. It sizes a `std::array<plan_term, plan_size<M, A, B>>` by the counting pass, then fills it in a lambda. Each `plan_term` is 24 bytes, so the n = 8 full plan puts 1.5 MiB in the object file.
- Metrics: `euclidean<N>` is R(N,0,0); `conformal<N>` is conformal(N − 2) by CONVENTIONS 1.4; `dense<N>` has 2 on the diagonal and 1 everywhere else, the worst case for the contraction recursion.

`plan_tu.cpp` constant-evaluates the full plan for one metric (`-DSPIKE_N`, `-DSPIKE_METRIC`, optional `-DSPIKE_CAP`) and indexes it with `argc`, so the whole table is emitted at every optimization level.

### Correctness of the generator

- **Claim.** Compile-time and run-time results of the one function are equal. Checked: Local. `runtime_check` compares `plan<conformal<5>, all<5>, all<5>>` with `generate` run on the same metric at run time, and prints "equal (1024 terms)" for both compilers. It then builds the table for a metric given on the command line, unknown at compile time.
- **Claim.** The generator agrees with `test/reference/clifford_reference.py` on every blade pair of conformal(3), R(1,3,0), rigid(3), the degenerate form [[0,1,0],[1,0,0],[0,0,0]], and the ten random forms of note 0009 (seed 20261031), on both compilers. Checked: Local, with `diff_reference.py`. The routes are independent: integer recursion in the stored basis against rational LDLᵀ and a change of basis. The author is not independent (plan 0001, risk 5).
- **Claim.** `runtime_check.cpp`, which includes `gen.hpp`, compiles cleanly on both compilers with `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -pedantic-errors`. Checked: Local. clang-format and clang-tidy were not run.

## Measurements

Machine: Intel Core i9-14900K, 24 cores and 32 threads, 125 GiB RAM; Linux 7.3.0-4-cachyos; GCC 16.2.1 20260810, Clang 23.1.4, libstdc++ 16. Every compile was pinned with `taskset -c 6`, to performance core 3. Its other thread is CPU 7, whose busy fraction during each run is in the TSV: 0.00 to 0.04, so effectively idle. Compiles ran one at a time. Load average was 0.4 to 1.2 from other work on the machine. Peak memory comes from `measure.py` (getrusage of children), because GNU time is not installed. Counters come from `perf stat -e ...:u` on the `cpu_core` PMU. Package energy was read from RAPL. The machine was not otherwise idle, so the energy figures in the TSVs are not comparable between runs (P13).

Wall time, peak memory, and retired instructions for the full gp plan, all 2ⁿ × 2ⁿ blade pairs. Limits are raised (`-fconstexpr-ops-limit=2^40`, `-fconstexpr-steps=0`) so that each point measures cost rather than failing. The values are from set `main2`. Plan terms: 4ⁿ for euclidean and conformal (256 to 65,536); for dense, 832, 4,864, 27,136, and 145,408 at n = 4 to 7.

| Metric | n | GCC -O0 | GCC -O2 | Clang -O0 | Clang -O2 |
| --- | --- | --- | --- | --- | --- |
| euclidean | 4 | 0.19 s, 68 MiB, 0.86e9 | 0.13 s, 71 MiB, 1.05e9 | 0.14 s, 110 MiB, 1.31e9 | 0.15 s, 116 MiB, 1.33e9 |
| euclidean | 5 | 0.16 s, 78 MiB, 1.86e9 | 0.27 s, 88 MiB, 2.85e9 | 0.29 s, 111 MiB, 4.73e9 | 0.30 s, 117 MiB, 4.81e9 |
| euclidean | 6 | 0.42 s, 124 MiB, 6.66e9 | 0.59 s, 166 MiB, 11.6e9 | 1.22 s, 116 MiB, 26.3e9 | 1.23 s, 122 MiB, 26.9e9 |
| euclidean | 7 | 1.44 s, 321 MiB, 29.6e9 | 2.49 s, 508 MiB, 53.1e9 | 7.14 s, 135 MiB, 172e9 | 7.24 s, 142 MiB, 173e9 |
| euclidean | 8 | 6.27 s, 1189 MiB, 135e9 | 11.50 s, 2016 MiB, 250e9 | 48.3 s, 214 MiB, 1220e9 | 48.4 s, 221 MiB, 1220e9 |
| conformal | 4 | 0.12 s, 68 MiB, 0.87e9 | 0.15 s, 71 MiB, 1.08e9 | 0.16 s, 110 MiB, 1.35e9 | 0.15 s, 116 MiB, 1.37e9 |
| conformal | 5 | 0.22 s, 78 MiB, 1.93e9 | 0.21 s, 90 MiB, 2.98e9 | 0.31 s, 111 MiB, 4.89e9 | 0.30 s, 117 MiB, 4.96e9 |
| conformal | 6 | 0.39 s, 126 MiB, 6.94e9 | 0.63 s, 172 MiB, 12.1e9 | 1.24 s, 116 MiB, 27.0e9 | 1.28 s, 122 MiB, 27.5e9 |
| conformal | 7 | 1.49 s, 336 MiB, 30.9e9 | 2.64 s, 532 MiB, 55.8e9 | 7.38 s, 136 MiB, 177e9 | 7.36 s, 142 MiB, 176e9 |
| conformal | 8 | 6.53 s, 1247 MiB, 140e9 | 12.01 s, 2111 MiB, 261e9 | 49.0 s, 214 MiB, 1230e9 | 49.6 s, 221 MiB, 1250e9 |
| dense | 4 | 0.15 s, 76 MiB, 1.59e9 | 0.17 s, 86 MiB, 2.35e9 | 0.21 s, 110 MiB, 2.85e9 | 0.22 s, 117 MiB, 2.92e9 |
| dense | 5 | 0.58 s, 184 MiB, 11.1e9 | 0.94 s, 265 MiB, 19.2e9 | 1.32 s, 114 MiB, 25.1e9 | 1.37 s, 121 MiB, 25.9e9 |
| dense | 6 | 5.67 s, 1485 MiB, 125e9 | 9.83 s, 2409 MiB, 217e9 | 13.9 s, 135 MiB, 286e9 | 14.4 s, 141 MiB, 296e9 |
| dense | 7 | 68.3 s, 17.7 GiB, 1534e9 | not run | 167.1 s, 239 MiB, 3496e9 | not run |

The dense rows come from sets `dense` and `dense7`, one sample each. Repeatability over the 40 points that ran twice (`main` and `main2`): at n ≥ 6, instruction counts differ by at most 1.6%, peak memory by 0.2%, and wall time by 12.9%. Below n = 6, wall times are dominated by start-up and vary by up to a factor of 2. Instructions per cycle ran from 2.5 at n = 4 to 3.7 (GCC) and 4.3 (Clang) at n = 8.

- **Claim.** Growth per added generator, from instruction counts. Clang: 3.6×, 5.6×, 6.7×, 7.0× from n = 4 to 8, approaching 8×, which matches 4ⁿ pairs times a 2ⁿ zero-fill (Inferred). GCC -O0: 2.2×, 3.6×, 4.4×, 4.6×, approaching 4ⁿ pairs times a factor of order n (Inferred). Checked: Local.
- **Claim.** GCC's peak memory grows with the work of the evaluation: 1.2 GiB at n = 8, 1.5 GiB for dense n = 6, and 17.7 GiB for dense n = 7. Clang stays between 110 and 240 MiB throughout. Checked: Local. Inferred: GCC keeps what one evaluation allocates until the evaluation ends, so memory, not time, is GCC's practical limit, and Clang's limit is time.
- **Not run: dense n = 8.** Extrapolating dense n = 6 to 7 (×12 in instructions, ×12 in memory on GCC) gives about 200 GiB on GCC, more than the machine has, so P11 forbids starting it. On Clang it would take about 35 minutes on one core at about 250 MiB. I judged that impractical by any standard and did not run it (Inferred).

### GCC at -O2

- **Claim.** GCC at -O2 spends about twice as long in "constant expression evaluation" as at -O0 (`-ftime-report` at n = 7: 2.62 s and 832 MB against 1.41 s and 450 MB); parsing and code generation stay negligible. Checked: Local.
- **Claim.** Declaring the plan's initializing lambda `consteval` (`-DSPIKE_PLAN_LAMBDA=consteval`) brings -O2 down to the -O0 cost: 1.47 s and 330 MiB, against 2.73 s and 574 MiB with the plain lambda. On Clang it changes nothing (7.42 s and 7.43 s). Checked: Local, one sample each. Inferred cause: at -O2, GCC also compiles the lambda as an ordinary function and tries to fold the `generate` call in its body, which repeats the whole evaluation before giving up; a `consteval` lambda has no run-time body to fold.

### Scratch capacity

`-DSPIKE_CAP=4` (set `cap`; the Cap 2ⁿ columns are from set `main`) replaces the 2ⁿ-term scratch with 4 terms, which is enough for the euclidean and conformal metrics (at most 2 terms per product). A product that overruns the capacity fails constant evaluation through the bounds check, so a too-small capacity cannot pass silently.

| n | Clang -O0, Cap 2ⁿ | Clang -O0, Cap 4 | GCC -O0, Cap 2ⁿ | GCC -O0, Cap 4 |
| --- | --- | --- | --- | --- |
| 6 (euclidean) | 1.21 s, 26.2e9 | 0.82 s, 15.5e9 | 0.38 s, 6.67e9 | 0.43 s, 6.68e9 |
| 7 (euclidean) | 7.30 s, 175e9 | 3.57 s, 73.6e9 | 1.45 s, 29.5e9 | 1.44 s, 29.4e9 |
| 8 (euclidean) | 48.5 s, 1225e9 | 17.6 s, 354e9 | 6.39 s, 135e9 | 6.35 s, 135e9 |
| 8 (conformal) | 48.7 s, 1224e9 | 18.1 s, 363e9 | 6.56 s, 140e9 | 6.60 s, 140e9 |

- **Claim.** Capacity sets most of Clang's cost and none of GCC's. Checked: Local. Inferred: Clang materializes every element of a zero-initialized array during evaluation, while GCC represents the zero fill compactly.
- **Claim.** With capacity 4, Clang's growth falls to about 4.8× per generator. Checked: Local.

## Constant-evaluation limits

Default limits, -O0 (set `limits`). "Pass" means the full gp plan compiled.

| Metric | n = 4 | n = 5 | n = 6 | n = 7 | n = 8 |
| --- | --- | --- | --- | --- | --- |
| euclidean, GCC | pass | pass | pass | pass | ops limit |
| euclidean, Clang | pass | pass | step limit | step limit | step limit |
| conformal, GCC | pass | pass | pass | pass | ops limit |
| conformal, Clang | pass | pass | step limit | step limit | step limit |
| dense, GCC | pass | pass | ops limit | ops limit | ops limit |
| dense, Clang | pass | step limit | step limit | step limit | step limit |

Smallest power of two that passes (set `sweep`, -O0). The binding evaluation is the fill pass, which repeats the counting pass and adds the writes:

| Point | Smallest passing limit |
| --- | --- |
| Clang, euclidean or conformal, n = 6 | `-fconstexpr-steps=4194304` (2²²) |
| Clang, euclidean or conformal, n = 7 | `-fconstexpr-steps=16777216` (2²⁴) |
| Clang, euclidean or conformal, n = 8 | `-fconstexpr-steps=67108864` (2²⁶) |
| Clang, dense, n = 5 / n = 6 | 2²² / 2²⁶ |
| GCC, euclidean or conformal, n = 8 | `-fconstexpr-ops-limit=134217728` (2²⁷) |
| GCC, dense, n = 6 | 2²⁷ |

- **Claim.** GCC 16's defaults are `-fconstexpr-ops-limit` 33554432 (2²⁵), `-fconstexpr-loop-limit` 262144 (2¹⁸), and `-fconstexpr-depth` 512. Checked: Docs (gcc.info and g++(1)), and Local: the error text "operation count exceeds limit of 33554432 (use '-fconstexpr-ops-limit=' to increase the limit)" names both the value and the flag. I had expected 2³³ from memory; this GCC's default is 2²⁵.
- **Claim.** Clang's step limit is raised by `-fconstexpr-steps=N`, with 0 meaning no limit. Checked: Local (`clang++ --help-hidden`). Clang's diagnostic says "constexpr evaluation hit maximum step limit; possible infinite loop?" and does not name the flag. The default is not printed by `--help`. Clang's documentation gives 1048576, but I did not read it in this session, and no local copy is installed. The sweep is consistent with it: n = 6 fails both at the default and at an explicit 2²⁰.
- **Claim.** Neither the loop limit nor the depth limit was reached; every loop in the generator runs at most 2ⁿ ≤ 256 times. Checked: Local. Inferred: a design that flattens all pairs into one loop of 4ⁿ iterations would reach GCC's loop limit at n = 9.
- **Gotcha.** For dense n = 7 at the default limit, GCC's first error reads "`__builtin_expect((__n >= ...size()), 0)` is not a constant expression", inside libstdc++'s `operator[]`. The same code passes with a raised `-fconstexpr-ops-limit`, and an index at that line cannot overrun unless an earlier write already failed. So the message is the operation limit, misreported. Checked: Local.

## Value template arguments against tag types

`nttp.cpp` defines `mv` and `gp` three ways over the same generator: value (`mv<metric M, blade_set B>`), tag (`mv<class Algebra, class Blades>`, each with `::value`), and hybrid (`mv<class Algebra, blade_set B>`). The deliberate misuse multiplies a conformal(3) vector by an R(4,1,0) vector: both have n = 5, but they are different algebras. `run_nttp.sh` compiles each variant at -O0, reads gp's mangled names with `nm`, and keeps the misuse diagnostic.

| Compiler | Style | Mangled length, v·v | Demangled length | Mangled lengths for (v v), ((v v) v), (((v v) v) v) | Diagnostic lines / bytes / longest line |
| --- | --- | --- | --- | --- | --- |
| GCC | value | 350 (390 alias) | 1913 | 350, 398, 418 (aliases 390, 438, 458) | 15 / 2042 / 874 |
| GCC | tag | 111 | 239 | 111, 124, 137 | 15 / 1237 / 174 |
| GCC | hybrid | 235 | 739 | 235, 283, 303 | 15 / 1238 / 207 |
| Clang | value | 418 | 2070 | 418, 466, 486 | 8 / 857 / 447 |
| Clang | tag | 111 | 239 | 111, 124, 137 | 8 / 607 / 210 |
| Clang | hybrid | 256 | 746 | 256, 304, 324 | 8 / 566 / 215 |

- **Claim.** With value arguments, the name encodes the whole metric. It grows with n², so a 16 × 16 metric would add hundreds of characters to every symbol and every type in a diagnostic (Inferred from the encoding). Tag names stay constant in n but grow by 13 characters per nested product, because computed blade sets become nested types (`gp_blades<A, gp_blades<…>, …>`). The hybrid stays flat in expression depth and grows only with the blade masks. Checked: Local.
- **Claim.** GCC emits each value-argument instantiation under two mangled names in the same section, for example 350 and 390 characters for v·v. Checked: Local (`readelf -sW`, same section index). Inferred: an ABI-compatibility alias.
- **Claim.** GNU `c++filt` 2.47 does not demangle Clang's value-argument symbols (it prints them unchanged); `llvm-cxxfilt` does. Checked: Local.
- **Claim.** GCC's value-argument diagnostic misprints the metrics. It reads "deduced conflicting values for non-type parameter 'M' ('spike::metric<5>{…{1}}, …{1}}, …{1}}, …{-1}}, …{-1}}}' and '…{1}}, …{1}}, …{1}}, …{1}}, …{-1}}}')": each row is printed with its leading zeros dropped and no positions. Conformal(3)'s rows [0,0,0,0,−1] and [0,0,0,−1,0] become {−1} and {−1}, so it reads as diag(1,1,1,−1,−1) and R(4,1,0) as diag(1,1,1,1,−1). That misleads the user (C9). GCC's demangled symbol shows the rows correctly. Clang prints both metrics in full and correctly. Checked: Local; the full texts are in `out/nttp/*.misuse.txt`.
- **Claim.** With tags and with the hybrid, both compilers name the algebras: "deduced conflicting types for parameter 'Algebra' ('hybrid::conformal3' and 'hybrid::r410')" on GCC, and "('hybrid::conformal3' vs. 'hybrid::r410')" on Clang. That is the C9 form, in terms of the algebra. Checked: Local.

## Reproduction

From the repository root, serially. No job-slot pool exists on the machine (plan 0002, "Current state: there is no pool"), so every command is a single pinned compile, as plan 0002 prescribes for measurement runs.

```bash
S=docs/agent/records/spikes/spike-a
for c in g++ clang++; do                                   # one function, two uses
  tools/timed build - spike-a:runtime_check:$c -- $c -std=c++23 -O2 -Wall -Wextra \
    -Wpedantic -Wconversion -Wshadow -Werror -pedantic-errors \
    -o $S/out/runtime_check_$([ $c = g++ ] && echo gcc || echo clang) $S/runtime_check.cpp
done
$S/out/runtime_check_gcc && $S/out/runtime_check_clang     # "equal (1024 terms)"
tools/timed test - spike-a:diff_reference -- python3 -I $S/diff_reference.py gcc clang

python3 -I $S/run_grid.py limits main dense cap            # about 17 minutes
python3 -I $S/run_grid.py dense7                           # about 4 minutes; GCC peaks at 17.7 GiB
python3 -I $S/run_grid.py sweep                            # about 5 minutes
python3 -I $S/run_grid.py main2                            # second sample with every P13 counter
bash $S/run_nttp.sh                                        # NTTP table; outputs in $S/out/nttp/

# GCC -O2 attribution and the consteval lambda (n = 7, conformal)
g++ -std=c++23 -O2 -ftime-report -c -DSPIKE_N=7 -DSPIKE_METRIC=conformal -o /dev/null $S/plan_tu.cpp
g++ -std=c++23 -O2 -DSPIKE_PLAN_LAMBDA=consteval -c -DSPIKE_N=7 -DSPIKE_METRIC=conformal \
  -o /dev/null $S/plan_tu.cpp
```

Each grid point runs `tools/timed build - spike-a:<set>:<compiler>:<opt>:<metric>:n<n> -- taskset -c 6 python3 -I measure.py -- perf stat -x, -e <events> -- <compiler> ...`. The results are in `$S/out/<set>.tsv`, one row per point. Sets `main2` and `sweep` record cycles, cache references and misses at L1d, L2, and LLC, and package energy. The earlier sets record instructions only, because the P13 extension (decision 0018) was ratified while they ran.

## Open questions

1. Scratch sizing for WU6: 2^(max |A ∪ B|) from the input blade sets, or a counting pass that finds the largest product first? Either can be a constant expression. Measure at n = 16 with sparse use.
2. Hybrid template arguments depart from STYLE S7's wording ("passed as class-type template arguments") for the signature. Owner decision.
3. Should Galoot's test build raise Clang's step limit so that some exhaustive proofs can be `static_assert`s, or run them all at run time? I recommend run time (C12): one mechanism, no limit flags.
4. The GCC diagnostic misprint and the double evaluation at -O2 may be worth reporting upstream; neither was searched for in GCC's bug tracker.
5. Not measured: the rga3 typed pairs that plan 0001 section 9 lists for Spike A. Only full plans and vector × vector were measured; typed pairs belong with WU6's P2 grid.

## Correction from Spike D (2026-10-31)

The finding that the full plan compiles at default limits up to n = 5 on Clang was measured at -O0 only. Note 0011 (D5) shows that Clang at -O1 and above fails the n = 5 plan at its default step limit (2^20) and passes at 2^21, because libstdc++ 16 enables `_GLIBCXX_ASSERTIONS` only without `__OPTIMIZE__` and an optimized build calls `__is_constant_evaluated()` on every checked access. Default-limit claims in this note hold for -O0.
