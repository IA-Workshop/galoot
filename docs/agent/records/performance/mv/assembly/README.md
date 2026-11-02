# The first assembly read of the code that applies a plan (plan 0016, criterion 24)

Recorded by Claude (agent, Opus 5.5), cpp-expert role, implementing WU8b, with the performance-engineer role file for the reading. A reading of generated code, as PERFORMANCE P4 asks for representative operations; DoD-24 in full (every operation, a benchmark against the hand-written form) stays with WU17.

## What was read

- `kernels.cpp`: rigid(3) point ⟑ point, point ∧ point, and motor ⟑ point over `double`, each through `detail::apply` (functions `library_*`) and written out by hand in the same order of terms (`hand_*`), and a point made zero by default and then written in every place (`written_point`). The hand-written motor ⟑ point was derived from the blade masks, the reordering sign, and the squares of rigid(3), by a few lines of Python that share no code with the library; the other two are the textbook formulas.
- `read.py`: compiles the unit with each compiler at `-O2` and the test flags (`-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off`), Intel syntax, and counts each function's instructions by kind; a packed instruction counts once per lane of two doubles. The texts are kept beside the counts.
- Compilers: GCC 16.2.1 and Clang 23.1.4, libstdc++ 16, x86-64 with no `-march` (SSE2).
- Command, from the worktree root: `tools/timed measure - asm-read:<revision> -- python3 -I docs/agent/records/performance/mv/assembly/read.py docs/agent/records/performance/mv/assembly/out-<revision>`.

Two reads are kept: `out-942a2d7/` (the kernel as first committed) and `out-be1b209/` (after the change the first read called for).

## Result at `be1b209`

| Function | Terms / outputs | GCC: mul, add/sub, stores | Clang: mul, add/sub, stores | Hand-written, GCC; Clang |
| --- | --- | --- | --- | --- |
| point ⟑ point | 15 / 7 | 15, 10, 4 | 15, 8, 4 | 15, 10, 4; 15, 8, 4 |
| point ∧ point | 12 / 6 | 12, 6, 3 | 12, 6, 3 | 12, 6, 3; 12, 6, 3 |
| motor ⟑ point | 28 / 8 | 28, 20, 8 | 28, 20, 8 | 28, 20, 8; 28, 20, 8 |

In every library function on both compilers: no negation, no integer-to-floating conversion, no branch, no call (each product is inlined into its caller), no reference to constant data (so no load of plan data), and no store of zero. The other instructions are loads, register moves, and shuffles, within one instruction of the hand-written function's count (GCC's point ⟑ point 33 against 32, Clang's point ∧ point 23 against 22; the difference is the order of shuffles the vectorizer chose). The criterion's points:

- One multiplication per term: met on both compilers, and no multiplication by ±1 (no plan here has another coefficient).
- terms − outputs additions and subtractions: met on Clang (8, 6, 20); on GCC 10 for point ⟑ point, where the vectorizer packed the sums so that two lanes of its packed additions are not needed. The hand-written function has the same 10, so this is GCC's choice for this arithmetic, not the library's.
- No integer-to-floating conversion, no load of plan data, no branch on blades: met.
- Inlining: both compilers inline all three products into the caller at `-O2`, as they inline the hand-written functions.
- The zero fill of the result is not emitted, for the kernel (stores equal the outputs, as a packed store or a scalar one; no zero store) and for `written_point` (GCC 4 stores, Clang 3 with one packed, no zero store).

## What the first read found (`out-942a2d7/`)

The kernel then wrote each term into the result's coefficients through a reference. GCC inlined all three products. Clang inlined the two point products but kept motor ⟑ point out of line in a function `detail::run_product` (the `call` in `library_motor_gp_point`), and since a write through that reference might change an input, it stored the output after each of the 28 terms (28 stores against 8). `be1b209` builds the result in a local array returned by value; with it, Clang inlines motor ⟑ point too, and every count matches the hand-written function. The planning probe of plan 0016 (section 11, item 1) had also seen Clang call an expanded lambda out of line; the by-value result is the remedy measured here, with no vendor attribute.

## Not read

`-O0` code (about one call per term, as the planning probe saw), the other operations and maps, `std::int64_t` and the other scalars, `-march` settings with fused multiply-add, and run time: a benchmark against the hand-written form is DoD-24's, with WU17.
