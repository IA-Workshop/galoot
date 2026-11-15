# 0017 Certifying that a product plan evaluates within a compiler's limits

- Date: 2026-11-01
- Checked by: Claude (agent, Opus 5.5), performance-engineer role, researcher, on branch `research/constexpr-ceiling-certification` from `develop` `e1af2ac`. Headers measured: `include/` of `e1af2ac`, unchanged. GCC 16.2.1 20260810, Clang 23.1.4, libstdc++ 16, on the development machine.
- Scripts and data: `docs/agent/records/performance/constexpr-certification/` (`mirror.py`, `probe.py`, `harness.py`, `cost.py`, and `out/`). Every compile ran through `tools/timed measure` (rows `ceiling-cert:*` in `runs.tsv`) holding a slot of the shared job pool.
- Stale for the current `develop`: while this ran, `develop` merged WU7b (`6324af8`), which changes `generate_plan` and `plan_capacity` (the antiproducts' `factor_of` and `run_term`, and an argument n), and the single-pass `metric_like` (`890e72c`). Every figure here is for the headers of `e1af2ac`. The count, fill, and capacity weights, and `mirror.py`'s copy of those functions, must be redone before the model is applied to `develop`; collect, typed, defects, and typing are unchanged in that diff only if the reader confirms it (not checked). This is the recalibration that section 5's test exists to force.
- Later, 2026-11-01 (plan 0015): the four scripts moved to `docs/agent/snippets/plan_cost_model/` and were changed there for `develop`; the versions that produced this note's figures are those of `3bd9904` under `records/performance/constexpr-certification/`. The refit against `develop` at `4895a40` is in `records/performance/plan-cost-model/`; it leaves this note's weights of `defects`, `collect`, `typed`, and `typing` unchanged. The text below is not edited.
- How each claim was checked: **Read** (the source named, read in this session), **Measured** (compiles on this machine, by the scripts named), **Derived** (my own derivation, given in full), **Inferred** (my reasoning, not checked).

## The question

Decision 0113 records, for `product_plan`, the largest `full<n>` × `full<n>` that builds at the default limits, found by building every n until one failed. The owner asks whether a formal specification or proof lets the library certify, for a given operation, algebra, and blade sets, that the plan will evaluate on a given compiler, without building each case.

## Answer in brief

1. **No compiler-independent specification exists.** The C++ standard leaves every limit on constant evaluation to the implementation and does not say how anything is counted. GCC and Clang document only the name and default of each limit. What each counts is defined only by its source, differs between the two compilers, and on Clang differs between two evaluators shipped in the same release behind the same flag (section 1).
2. **For one compiler build it is exactly predictable.** A constant evaluation is deterministic, and both counters charge a fixed amount for each pass along each path through the code. So the count of every phase of the generator is a weighted sum of how often each path is taken, with weights that depend on the compiler and its options but not on the input. A Python copy of the generator (`mirror.py`) counts the paths; the weights are fitted from compiles. The fit is exact, in integers, on both compilers (GCC with its call cache off), and predicts every held-out phase probe (98 to 99 per configuration) and every whole plan tested (up to 63 plus 56 per configuration, n up to 16) with zero error. One Clang weight was also derived by hand from Clang's counting rule before it was measured, and agrees. One GCC event had to be found by its residual: GCC charges for reading a Gram entry that the metric's factory never wrote (section 3).
3. **A certificate "predicted count < limit" is trustworthy for the calibrated compiler build, one-sided, and only for it.** The evidence supports a margin of zero on both compilers, given a check that the compiler is the one calibrated. On GCC the certified count is the cold one (cache off), a proven upper bound on the real count, which is 1 % to 4.2 times lower; so some plans that build (the dense `full<5>` product) are not certified. GCC's limit has a defect that makes pass and fail non-monotone in the limit, which does not affect the certificate's direction but makes bisected thresholds unsafe without a check (section 3.3).
4. On Clang the model reproduces decision 0113's table exactly. On GCC the cold certificate agrees with it except for the dense form (it certifies n = 4 where 5 builds), and the measured real counts agree with every entry. The model also predicts where cheaper typing would move the ceilings (section 4).
5. Section 5 proposes a certified statement for the `///` comment, a CTest that guards the calibration, and the predictor as the tool; decision 0125 (proposed) lists what the owner rules on.

## 1. What is specified

### 1.1 The C++ standard

Read: the C++23 text as tagged `n4950` in the `cplusplus/draft` sources (`source/intro.tex`, `source/expressions.tex`, `source/limits.tex`), and the current working draft at eel.is (`[implimits]`, `[intro.compliance.general]`, `[expr.const]`), retrieved 2026-11-01. N4950 is the working draft closest to C++23 and a substitute for ISO/IEC 14882:2024, which was not read.

- [intro.compliance] (N4950): a conforming implementation shall, "within its resource limits as described in [implimits], accept and correctly execute" a well-formed program (quotation).
- [expr.const] (N4950, the list of what makes an expression not a core constant expression): "an expression that would exceed the implementation-defined limits (see [implimits])" (quotation). So a constant evaluation that exceeds a limit is not a constant expression, and a `static_assert` or `constexpr` variable that needs it is ill-formed: a hard error, not undefined behavior.
- Annex [implimits] (N4950, a normative annex): "Every implementation shall document those limitations where known. This documentation may cite fixed limits where they exist, say how to compute variable limits as a function of available resources, or say that fixed limits do not exist or are unknown." And: "The bracketed number following each quantity is recommended as the minimum for that quantity. However, these quantities are only guidelines and do not determine compliance." (quotations). The two quantities for constant evaluation are "Recursive constexpr function invocations [512]" and "Full-expressions evaluated within a core constant expression [1 048 576]".
- The current draft makes the annex informative, calls the numbers "a potential minimum value", and moves the documentation duty to a recommended practice in [intro.compliance.general] ("An implementation is encouraged to document its limitations...") (read).

What the standard guarantees: that exceeding a limit is diagnosed (the program is ill-formed). What it does not: any minimum (the numbers "do not determine compliance"), any rule for counting (the annex names "full-expressions evaluated" as a quantity, not as a unit any implementation must count), or any relation between limits of two implementations. A certificate cannot rest on the standard.

### 1.2 GCC 16

Documentation read: the GCC 16 manual installed with the compiler (`gcc.info`, C++ Dialect Options), quoted:

- `-fconstexpr-ops-limit=N`: "Set the maximum number of operations during a single constexpr evaluation. ... The default is 33554432 (1<<25)." "Operation" is not defined.
- `-fconstexpr-loop-limit=N`: "Set the maximum number of iterations for a loop in C++14 constexpr functions to N. ... The default is 262144 (1<<18)."
- `-fconstexpr-depth=N`: "Set the maximum nested evaluation depth for C++11 constexpr functions to N. ... The minimum specified by the standard is 512." (The standard specifies no minimum, see 1.1.)
- `-fconstexpr-cache-depth=N`: "Set the maximum level of nested evaluation depth for C++11 constexpr functions that will be cached to N. ... The default is 8."
- There is no option for memory, and no `-fconstexpr-backtrace-limit` (that option is Clang's).

Source read: `gcc/cp/constexpr.cc` of the `releases/gcc-16` branch of the GCC mirror, retrieved 2026-11-01. A substitute: the installed 16.2.1 is a distribution build of that branch dated 2026-08-10, and the branch may have moved since. What it says:

- The operation count is "Number of cxx_eval_constant_expression calls (except skipped ones, on simple constants or location wrappers) encountered during current cxx_eval_outermost_constant_expr call" (comment on `constexpr_ops_count`). The count is per outermost evaluation: each `static constexpr` local of `make_product_plan` is one evaluation with its own budget. It is incremented before evaluating any tree node that is not a constant, and the evaluation fails when the count reaches the limit (`if (++count >= limit)`), so an evaluation of C operations compiles under limit L exactly when C < L.
- The loop count is per execution of one loop statement, `++count >= constexpr_loop_limit`.
- The call cache: a call whose arguments are all constants (and hold no address of a variable still being built or of the heap) is looked up in `constexpr_call_table`, and stored there when the call's nesting depth is below `-fconstexpr-cache-depth`. The table is created once and cleared only in `fini_constexpr`, after parsing, so it serves every later evaluation in the translation unit. A hit still binds (evaluates) the arguments and skips the body.
- Built-in functions: their arguments are evaluated with `quiet = true` and with throwaway non-constant flags (`cxx_eval_builtin_function_call`). When the operation count reaches the limit inside such an argument, the error is not issued, and the count is set to its most negative value (`INTTYPE_MINIMUM (HOST_WIDE_INT)`), which removes the limit for the rest of that evaluation. Measured consequence in 3.3.

### 1.3 Clang 23

Documentation read: the Clang 23.1.0 User's Manual and Constant Interpreter pages at releases.llvm.org and the in-progress manual at clang.llvm.org, and `clang++ --help-hidden` of the installed 23.1.4, retrieved 2026-11-01. There is no 23.1.4 page (404).

- `-fconstexpr-steps=N`: "Sets the limit for the number of full-expressions evaluated in a single constant expression evaluation. This also controls the maximum size of array and dynamic array allocation that can be constant evaluated. The default is 1048576, and the limit can be disabled with -fconstexpr-steps=0." (quotation; `--help-hidden`: "Set the maximum number of steps in constexpr function evaluation (0 = no limit)").
- `-fconstexpr-depth=N`: "Sets the limit for constexpr function invocations to N. The default is 512."
- `-fconstexpr-backtrace-limit=N`: "Set the maximum number of entries to print in a constexpr evaluation backtrace (0 = no limit)". It changes only the diagnostic, not what evaluates.
- Constant Interpreter page: "The interpreter is activated by passing -fexperimental-new-constant-interpreter to clang. Since Clang 23, the bytecode interpreter can also be enabled by default by passing -DCLANG_USE_EXPERIMENTAL_CONST_INTERP=ON to cmake." (quotation). So which evaluator a `clang++` runs is a choice of whoever built it.

Source read: `clang/lib/AST/ExprConstant.cpp` and `clang/lib/AST/ByteCode/{Interp.cpp,InterpState.cpp,InterpState.h}` at tag `llvmorg-23.1.4`, retrieved 2026-11-01.

- Classic evaluator: `EvaluateStmt` calls `Info.nextStep(S)` once for every statement it evaluates, compound statements and declarations included; `nextStep` fails when no step is left and otherwise decrements. Expressions take no step. So the count is the number of statements executed, not of full-expressions as the manual says, and an evaluation of C steps compiles exactly when C <= L. `CheckArraySize` also refuses an array of more elements than the limit.
- Bytecode interpreter: `noteStep` is called only by the jump opcodes `Jmp`, and `Jt` and `Jf` when taken, and fails when the count reaches zero: one step per jump taken.
- Clang has no call cache in either evaluator (inferred from the absence of one in these files, and consistent with every measurement here).

Measured, a loop `for (int i = 0; i < K; ++i) { s += i; }` in a `constexpr` function read by a `static_assert`, K = 1000 and 2000, bare `-std=c++23 -O2`: the smallest passing `-fconstexpr-steps` is 2K + 5 with `-fno-experimental-new-constant-interpreter` (two statements per iteration, five around the loop), K + 2 with `-fexperimental-new-constant-interpreter`, and 2K + 5 with neither flag, so the installed Clang runs the classic evaluator by default. GCC's smallest passing `-fconstexpr-ops-limit` is 17K + 17: seventeen operations per iteration of the same loop.

### 1.4 What this means

There is no formal specification to certify against. The standard gives none; the compilers give a name and a default; the rule each compiler applies is an implementation detail of one version, which differs between GCC and Clang by a factor of five to seven on this generator, and between Clang's two evaluators by about two on a plain loop. Any certificate is therefore a statement about one compiler build, and must carry a check that the compiler in use is that build and counts as calibrated.

What the source does give is a rule simple enough to model: Clang charges one step per statement executed; GCC one operation per expression node evaluated, less what its call cache saves.

## 2. The cost model of the generator

### 2.1 Why a weighted path count is exact

Take one function body and fix the path through it: which branch each `if` takes and how many times each loop runs. Along a fixed path the compiler evaluates the same statements and the same expression nodes every time, whatever the values flowing through them. Clang charges one step per statement and GCC one operation per non-constant node, so each pass along the path costs a fixed number, the same on every call. A call to another function adds the cost of the path taken through the callee. So the count of a whole evaluation is

  cost = w0 + Σ_e w_e · n_e,

where e ranges over the paths ("events": one more iteration of this loop, this branch taken), n_e is how often the evaluation takes it, and w_e is the fixed cost of one pass. Each w_e is a whole number (a count of statements or nodes). It depends on the compiler, its version, the standard library (whose `operator[]`, `size()`, and assertion macros are on the paths), and the options that change those (`-O`, `_GLIBCXX_ASSERTIONS`); it does not depend on the input. The input enters only through the n_e, and those are what a line-for-line copy of the generator can count without a compiler.

Two things can break the exactness. A path whose cost depends on a value and not only on the branches taken. GCC has two: a cached call costs less than an uncached one, and reading an element of a constant aggregate costs one operation more when the element was never written (the metric factories write only the nonzero entries of the Gram matrix, so whether `g.gram[i][j]` was written depends on how the algebra was built, not only on the entry's value). Both are found below; the second only from the residual of a fit that left it out. And a fit that confuses two events always taken together, which leaves their weights individually unknown but their sum known; a prediction is then exact only for inputs whose event counts lie in the span of the training inputs, which `cost.py` checks for every prediction.

GCC's cache is the first kind, and it is why the model for GCC is fitted with the cache off (`-fconstexpr-cache-depth=1`, under which nothing is stored, since every call is at depth 1 or more). Call that count the cold count. The real count is never higher. Proof, from the source in 1.2: with the cache on, each call either misses, and is evaluated exactly as with the cache off, or hits, and then its arguments are bound as they would be and its body is skipped; a cacheable call has only constant arguments and so no effect on the rest of the evaluation, which therefore continues along the same paths. Every operation counted with the cache on is counted with it off. So the cold count is an upper bound on the real count of the same evaluation, whatever else the translation unit evaluated before it, while the real count depends on that history (a plan evaluated after another that shares its calls costs less).

### 2.2 The phases

`make_product_plan` evaluates, as separate constant evaluations in this order: `in_range` and `valid_signs` (one pass over each set), `capacity` (`plan_capacity`), `count` (`generate_plan` with an empty output), `generated` (the fill: `filled_plan`), `defects` (`defects_of`), `found` (`collected`, which runs `collect_result`), `plan` (`typed`, which copies the result and runs `type_terms`), and `typing` (`typed_defects_of`), then the `static_assert`s on the stored flags. A unit compiles when each of these, and every other evaluation in it, is under the limit; its threshold is the largest.

Notation: A and B the two sets, p = |A| and q = |B| their sizes, P = pq the number of pairs; T the number of terms; R the number of result blades; n the number of generators. For the terms in plan order, s_k is the size of the result list when term k is collected and π_k the position at which its blade is found or inserted.

What each phase does and what it costs, with the events of `mirror.py`:

| Phase | Events (how often) | Order of the count |
| --- | --- | --- |
| capacity | per set A blade (p), per pair (P), per pair of the product (P), per raise of the capacity (at most n) | Θ(pq) |
| count | per blade of A (p), per pair (P), the pair's own product (see below), per term (T) | Θ(P + T) plus the products |
| fill | as count, plus one write per term (T) | as count |
| defects | per term (T), split by which ordering test decides | Θ(T) |
| collect | per term (T), split by how its search ends; per step of the scan (Σ π_k, split by the path of `canonical_less`); per place shifted (Σ over new blades of s_k − π_k) | Θ(T + Σ π_k + shifts) ≤ Θ(T·R) |
| typed | per result blade copied (R), per term (T), per step of the position search (Σ π_k) | Θ(T + R + Σ π_k) ≤ Θ(T·R) |
| typing | per result blade (R), per term (T), split by ordering test | Θ(T + R) |

The product of one pair (`pair_terms`) is, for the wedge, a constant, plus, when the blades are disjoint, one insertion into an empty list and one step of `reorder_sign` per generator of the first blade. For the geometric product it is the recursion of `blade_product`: one level per generator of the first blade, each with a term-list initialization, a call of `add_generator_product` over the terms so far (one step per generator of each, and an insertion per nonzero Gram entry met), and a contraction loop over the remaining generators of the first blade that recurses once per nonzero off-diagonal entry. Its events are those of `mirror.py`; for a diagonal form, where no off-diagonal entry is nonzero and every product of two blades is one blade, the cost of a pair is linear in the grade of the first blade times the grade of the intermediate products, so at most a constant times n² (derived from the code; the counts are in `mirror.py`).

### 2.3 Proofs of the orders

Each proof counts loop iterations; the weights only multiply them.

**Lemma 1 (the scans).** In `collect_result`, Σ π_k ≤ T (R − 1), and the shifts total at most R (R − 1)/2.

*Proof.* The list holds distinct blades of the result, so s_k ≤ R throughout. The scan for term k stops at the first place whose blade is not less than the term's, after π_k steps. If the term's blade is already listed, the scan stops on it, so π_k ≤ s_k − 1 ≤ R − 1. If it is new, the list does not yet hold it, so s_k ≤ R − 1, and π_k ≤ s_k ≤ R − 1. Either way π_k ≤ R − 1. Summing over the T terms gives the first bound. A shift happens only when a new blade is inserted, at most once per blade of the result, and the insertion at place π into a list of s moves s − π ≤ s blades; the j-th new blade meets a list of j − 1, so the total is at most 0 + 1 + ... + (R − 1) = R (R − 1)/2. ∎

So collect costs at most a constant times T·R (the shifts are fewer, since R ≤ T), and typed the same, since its search for term k stops at the same π_k in the final list.

**Lemma 2 (the bound is reached, to a factor of two).** For `full<n>` × `full<n>` under the geometric product in R(n,0,0), Σ π_k = T (R − 1)/2 exactly, with T = 4^n and R = 2^n, and no blade is ever shifted.

*Proof.* In R(n,0,0) the product of e_A and e_B is ±e_(A xor B), one term for every pair. The plan lists pairs by a, then b, both in the canonical order of `full<n>`, whose first blade is the scalar. So the first row (a = scalar) produces e_B for every B in canonical order. Each is greater than every blade before it, so the scan for the j-th runs over the j − 1 blades already listed (π = j − 1), the blade is appended at the end, and nothing is shifted. After that row the list is complete and never changes. In every later row the map b ↦ A xor B sends the 2^n masks one-to-one onto themselves, so the row meets every blade of the list exactly once, and the positions it finds sum to 0 + 1 + ... + (R − 1) = R (R − 1)/2. The first row sums to the same. With 2^n rows, Σ π_k = 2^n R (R − 1)/2 = T (R − 1)/2, since T = 2^n · R. ∎

For the other forms `mirror.py` gives the counts directly; the ratio Σ π_k / (T (R − 1)) for n = 3 to 7 is exactly 1/2 for conformal(n − 2) (not proved; a row there is not one-to-one, since e4 e5 has two terms and e4 e4 none, but the positions still balance), between 0.583 and 0.543 for R(n−1,0,1), whose rows miss the blades of pairs that share the null generator, and between 0.437 and 0.433 for the dense form. No form shifts a blade, because every first row is complete and in order.

So for full sets the collect and typed phases grow as T·R = 8^n, against T times the cost of a pair for count and fill (4^n · O(n²) in a diagonal form), and they become the largest phase from n = 5 or 6 on (section 3 has the numbers). Each added generator multiplies typing by about 8 and generation by about 4 to 5.

**Lemma 3 (the checks).** defects and typing cost a constant times T + R.

*Proof.* Each is one pass over the terms and, for typing, one or two passes over the R result blades, with a bounded amount of work per element (a constant number of comparisons, at most one `canonical_less`). ∎

**Hard limits that the counts do not show.** GCC's loop limit applies to each loop execution: collect, typed, defects, and typing each run one loop over all T terms, so GCC refuses any plan with T ≥ 2^18 = 262,144 whatever its operation count; for full gp sets that is n = 9 (T = 4^9). Clang refuses an array of more than L elements, which never binds here, since every term costs more than one step in the fill. The recursion depth of `blade_product` is the grade of the first blade plus a fixed number of frames, at most n + 10 ≤ 42, far below 512 on both compilers.

### 2.4 The fitted weights, and one checked by hand

Clang 23.1.4, Galoot's test flags at -O0 (`out/weights-clang-galoot-O0.tsv`; a basic solution: where two events always occur together, one carries the sum and the other zero). In steps:

| Phase | Constant | Per event |
| --- | --- | --- |
| collect | 39 | each step of the scan: 39 when `canonical_less` decides by grade, 34 when by index; a term whose blade is found: 53; a new blade at the end: 38, inside the list: 45, plus 20 per blade shifted |
| typed | 31 | 16 per result blade copied, 70 per term, 24 per step of the position search |
| defects | 39 | per term, by which test decides its order: 37 (a or b moved on), 65 or 60 (same pair, `canonical_less` by grade or index) |
| typing | 17 | 52 per result blade, 39 per term, 5 more per grade comparison |
| count, fill | 25, 31 | 62 per pair, 11 per blade of A, and the product's own events (22 per recursion level, 25 per generator visited, 41 per insertion into a term list, 32 to 37 per comparison inside it, 13 to 26 per generator of `reorder_sign`, and so on) |

So for large full sets the typing costs about 36 steps per scan step, Σ π_k ≈ T (R − 1)/2 of them, and generation about 62 + (cost of a product) per pair.

Check by hand (Derived, then Measured). Clang takes one step per statement. One iteration of `type_terms`'s search, `while (at < result.size() && result[at].mask != plan[k].out) { ++at; }`, executes: the loop body (a compound statement and `++at;`: 2 steps); `result.size()` in the condition (`span::size` is `{ return _M_extent._M_extent(); }`, and `_M_extent()` is again a body and a return: 4 steps); and two `span::operator[]` calls, each a body (1), libstdc++'s assertion `do { if (...) ...; } while (false)` with `_GLIBCXX_ASSERTIONS` (the `do`, its body, the `if`: 3), the `size()` call inside the assertion (4), and the `return` (1): 9 steps each. Total 2 + 4 + 9 + 9 = 24, the fitted weight. Without `_GLIBCXX_ASSERTIONS`, libstdc++ 16 writes the assertion as `if (std::__is_constant_evaluated() && !bool(cond))`, and `__is_constant_evaluated()` is a function whose body is an `if consteval` with a `return` in a compound statement: 4 more steps per `operator[]`, so 24 + 8 = 32 predicted for the bare -O2 configuration before it was measured. The fit of that configuration, made afterwards from its own compiles, gives 32 (`out/weights-clang-bare-O2.tsv`). The other changes of the row, read after the fit, fit the same accounting of 4 steps per indexing call: 16 to 24 per blade copied (two `std::array::operator[]`), 70 to 94 per term (six indexing calls).

This is also why the bare -O2 configuration costs Clang more than Galoot's test flags (about 10 % to 17 % on the full plans), although it has fewer library checks: the remaining check calls a function, the full one calls a built-in, and built-ins take no step.

## 3. Calibration, and how well the model predicts

### 3.1 Method

Measured (`probe.py`, `harness.py`, `cost.py`):

- **Phase probes.** For each phase, a unit holds the phase's inputs as literals (the sets; for the later phases, the plan, the result, or the typed plan, written out by `mirror.py`) and evaluates that phase once, as its own `constexpr` variable, as `make_product_plan` does. Each probe has a baseline: the same unit without that evaluation. A probe whose threshold does not exceed its baseline's measures something else (the literals, the metric, the header) and is left out as hidden.
- **Why a search.** Neither compiler reports its count. GCC's error names the limit, not the count reached; Clang's says "constexpr evaluation hit maximum step limit; possible infinite loop?"; Clang's `-ftime-trace` gives durations of evaluations, not steps; and the installed compilers are stripped, so a debugger cannot read the counters. The only exact reading of a count is the smallest limit that admits it.
- **Threshold.** The smallest limit under which the unit compiles (`-fsyntax-only`), found by a widening bracket and bisection, then confirmed by compiling at a dozen limits just above it (+1 to +127 and +0.01 % to +10 %); a failure there restarts the search above it (3.3). For Clang the threshold is the step count; for GCC it is the operation count plus one.
- **Inputs.** Random, seed 20261101 and successors (`probe.py cases`): n from 3 to 6, forms R(n,0,0), R(n−1,0,1), conformal(n − 2), the dense form, and random symmetric integer forms with entries in {−1, 0, 1, 2}; sets of 2 to 24 random blades with random signs; both operations. Training: 398 probes (280 over all phases, 118 more of the product's count and fill). Held out: 112 probes from a different seed, 98 of them not hidden. Whole plans: 63 (`full<n>` for n = 3 to 7 in the four forms and both operations, and 24 random pairs of up to 96 blades at n = 4 to 9) and 66 more of the sets programs write (the rigid(3) pairs embedded with e4 as e_n, n = 4, 8, 12, 16; the sparse pairs; random sets of 2 to 12 blades at n = 10 to 16).
- **Fit.** Least squares over the rationals (`cost.py fit`), phase by phase. A prediction is flagged when its event counts are not a rational combination of the training rows'. That check catches an extrapolation in the events the mirror counts; it cannot catch an event the mirror does not count, which only a fit that is not exact reveals (3.2).
- **Configurations.** Clang: Galoot's test flags at -O0 (full calibration) and bare `-std=c++23 -pedantic-errors -O2` (full calibration), and Galoot's flags at -O2 (whole plans only, with the -O0 weights). GCC: Galoot's flags at -O0, cold (`-fconstexpr-cache-depth=1`) for the calibration, and both cold and warm (default) for whole plans. GCC's other configurations were not calibrated (cost).

### 3.2 Results

| Compiler, configuration | Training: rank and largest residual | Held-out phase probes | Whole plans |
| --- | --- | --- | --- |
| Clang, Galoot -O0 | every phase exact, integer weights | 98 of 98 exact | 63 of 63 exact (`out/check-plans-clang-galoot-O0.txt`); wide: 57 of 57 exact, and 9 whose largest evaluation is the algebra's own (`out/check-wide-clang-galoot-O0.txt`) |
| Clang, bare -O2 | every phase exact, integer weights | 98 of 98 exact | 63 of 63 exact (`out/check-plans-clang-bare-O2.txt`) |
| Clang, Galoot -O2 | not fitted | not run | 39 of 39 `full<n>` plans exact with the -O0 weights (`out/check-plans-clang-galoot-O2.txt`); the random plans not run |
| GCC, Galoot -O0, cold | every phase exact, integer weights (after the event below) | 99 of 99 exact | 61 of 61 exact (`out/check-plans-gcc-galoot-O0-cold.txt`): 39 `full<n>` and 22 random; of the two largest random plans (2.1 × 10⁹ and 1.7 × 10⁹ operations), one compiled at its prediction with no failure above it, its compile one below not reached, and the other's search was stopped with its count bracketed between 2,088,578,105 and 2,088,839,751, around the prediction 2,088,732,631 (`out/check-plans-gcc-galoot-O0-cold-random.txt`; both stopped after about an hour of 2-minute compiles); wide: 55 of 55 exact, and 9 whose largest evaluation is the algebra's own (`out/check-wide-gcc-galoot-O0-cold.txt`); 2 random wide cases not run (below) |

"Exact" means a predicted threshold equal to the measured one. The largest case, a random form at n = 15 (`wide-4`, 1,578,530,785 steps), compiled at its prediction and failed one step below it, in compiles of 15 and 19 minutes; the confirming compiles above it were not run (I emptied its unit after the second, so those rows of `runs.tsv`, 0.02 s each, are void). Galoot's flags at -O2 count exactly as at -O0 on Clang: constant evaluation always checks `GALOOT_ASSERT`, whatever `GALOOT_CHECKING` says, and both define `_GLIBCXX_ASSERTIONS`. On Clang, every prediction made from the weights alone, on inputs the fit never saw, including n = 7 and n = 16 where it saw at most 6, came out exact. The model reproduces decision 0113's Clang table: `full<5>` × `full<5>` predicted 655,239 steps in R(5,0,0) and 695,423 in conformal(3) (under 2^20 = 1,048,576), `full<6>` 5,110,503 (over); the dense form 680,927 at n = 4 and 8,318,542 at n = 5; the wedge 216,396 at n = 5 and 1,309,448 at n = 6.

Two random wide cases were not run on GCC: their predicted cold counts were 3.7 × 10⁸ and 8.3 × 10⁹ operations, and GCC's memory grows with the work of an evaluation (recorded peaks of 9.3 GB for the dense `full<6>` at 5.7 × 10⁸ operations and 35 GB for a random plan at 2.1 × 10⁹, against 0.9 GB at the limit of 3.4 × 10⁷), so the second could not have been served on this machine and the first was not needed.

GCC needed one event more than Clang. With the events of the first fit, GCC's count and fill kept a remainder of up to 167 operations in training (4.5 × 10⁻⁴), and on the wide plans up to 5,566 under the count (0.4 %, a random from_gram form at n = 12; the missing event distorted the other weights, so the error reached forms that have no unwritten entry): the error grew with n, beyond the 0.2 % margin first proposed. The rigid form, whose null generator has a row of the Gram matrix with no entry written, had the largest remainders, all underpredictions. Adding as events the reads of Gram entries that the factory did not write (`mirror.Form`, `mirror.entry`) made every GCC phase exact: one operation per read of an entry in a row with no written entry, none otherwise. Clang's weights for these events are zero. Inferred mechanism, not read in GCC's source: GCC keeps a constant aggregate's written elements and supplies the others by value-initialization when read, which is one more evaluation. Candidates tried before it and rejected, as scratch fits not kept in `out/`: the capacity of the term lists, the recursion depth, the lengths of the lists, the signs and sizes of coefficients, and the written elements of a term list.

GCC warm (the real count) against cold, `full<n>` × `full<n>`, as follows (`out/plans-gcc-galoot-O0-{cold,warm}.tsv`; warm is GCC's default, the threshold in millions of operations; the limit is 33.55 M).

| n | R(n,0,0) cold / warm | R(n−1,0,1) cold / warm | dense form cold / warm | wedge, every form, cold / warm |
| --- | --- | --- | --- | --- |
| 3 | 0.134 / 0.070 | 0.117 / 0.058 | 0.331 / 0.138 | 0.038 / 0.036 |
| 4 | 0.702 / 0.453 | 0.607 / 0.372 | 3.65 / 1.33 | 0.186 / 0.183 |
| 5 | 3.59 / 3.19 | 3.08 / 2.62 | **44.5** / 13.4 | 1.09 / 1.02 |
| 6 | 25.9 / 23.8 | 21.2 / 19.5 | 569 / 139 | 6.56 / 5.97 |
| 7 | 203 / 184 | 165 / 149 | | 39.7 / 35.6 |

conformal(n − 2) as R(n,0,0) at n = 6 and 7, and 0.153 / 0.070, 0.781 / 0.453, 3.91 / 3.19 at n = 3 to 5. The warm counts reproduce every entry of decision 0113's GCC table (products to n = 6, the dense form to 5, the wedge to 6). The largest warm phase is `typed` in every row: its position search compares masks with `!=` and calls nothing that GCC can cache, while `collect_result`'s `canonical_less` calls are cached.

The cold model is the bound; the real count is lower by 1 % to a factor of 4.2, most for the dense form, whose recursion repeats the same small products. So the cold certificate, sound, does not certify the dense `full<5>` (cold 44,454,415 against 33,554,432), which builds (warm 13,448,041).

A by-product for growth at fixed use: the predicted (and, where the plan is the largest evaluation, measured) count of the largest phase of each rigid(3) pair and of the sparse pairs is the same number at n = 4, 8, 12, and 16 on both compilers, for example motor times motor 26,555 steps and 152,250 operations (`out/check-wide-*.txt`). The plan's constant evaluation does not grow with n at fixed use; what grows is the algebra's own check, whose evaluation is the unit's largest at n = 16 for the smallest sets (3,652 steps on Clang, 22,760 operations on GCC).

### 3.3 GCC's limit is not monotone

Read (1.2) and measured. A phase probe of `type_terms` with GCC, Galoot's flags at -O0, cache off: it compiles at limit 262,144, fails at 262,145 and at every limit tried up to 6,000,000, and compiles from 8,000,000; its count, from the checked search, is 6,406,542 (probe `train3-typed`, `out/train-gcc-galoot-O0-cold.tsv`). The passes below the count are limits at which the count runs out inside the argument of `__builtin_expect` in libstdc++'s assertion: the error is dropped, the counter goes to its most negative value, and the evaluation finishes unlimited. A plain bisection, the first run of the GCC training (`out/first-search-gcc-galoot-O0-cold.tsv`), landed on such a limit in 20 of 280 probes and reported thresholds too low by under 1 % (five), by 2 % to 35 % (eleven), and by factors of 3.5, 5, 24, and 27 (four). The fit of that run had residuals up to 8 × 10⁵. With the check of 3.1 the same probes fit as in 3.2. The check also caught one in a whole plan: the largest random plan compiled at 2,088,578,104 and failed at the limits one below and one above it; its count is near 2,088,732,631.

Consequences. (a) A certificate "count < limit" is sound on GCC: below the limit nothing is evaded. (b) "count ≥ limit, so it fails" is not certain: a plan whose count exceeds the limit may build at that one limit. Such a pass is deterministic for that compiler and source, so a trial build that passed was a real pass; but the next edit anywhere in the generator can turn it into a failure. (c) The error message is then often "`__builtin_expect(...)` is not a constant expression" in `<span>`, which decision 0113 quotes for R(6,0,1) at n = 7: a symptom of the same mechanism, though there the build failed.

### 3.4 Is "predicted ≤ c · limit" trustworthy, and with what margin?

- **Clang, classic evaluator, calibrated configuration: yes, with c = 1.** The model has no observed error on 98 held-out probes in each calibrated configuration and on 63 + 57 held-out plans (Galoot -O0), its weights are integers, and one was confirmed from the counting rule by hand. What it rests on, and what the certificate must check: the Clang version and that the classic evaluator is the default (a build with `CLANG_USE_EXPERIMENTAL_CONST_INTERP=ON`, or `-fexperimental-new-constant-interpreter`, counts jumps, not statements); the libstdc++ version (its `span`, `array`, and assertion code are on every path); the configuration's assertion macros and `-O` level; the generator's own source; and, for GCC, how the metric constant is built (2.1). A change to any of them invalidates the weights, silently. Hence the canary of section 5.
- **GCC, cold model: yes as an upper bound, with c = 1.** After the event of 3.2 it has no observed error on 99 held-out probes and every whole plan run. Before that event a margin of 0.2 % plus 500 operations looked ample and was not (the wide plans went to 0.4 %): the lesson is that a margin fitted to the observed error protects only against the error of inputs like those observed, while an exact fit with integer weights is a check that no path is missing. It is sound against the real (cached) count by the argument of 2.1. It certifies the product to n = 6 (cold 25,853,002, 77 % of the limit) and the wedge to n = 6, which is what builds; it does not certify the dense `full<5>`, which builds.
- **Not trustworthy: any c applied across compilers, versions, or evaluators,** and any certificate of failure on GCC (3.3).
- **Outside the counters:** memory (GCC grows with the work of the evaluation: about 0.9 GB at the n = 7 failures of decision 0113, up to 35 GB in the cold searches here, and no option limits it), GCC's per-loop limit (T < 262,144), and the recursion depth (n + 10 frames). The first is not certified; the last two are checked structurally.
- **Other evaluations in the unit.** The certificate covers the plan's phases. A unit also evaluates the algebra's `metric_like` check (n² entries; it is the largest evaluation of the unit for the small sets at n = 16, 3,652 steps on Clang) and the sets themselves (`full<n>`); these are small against the limits for every case here, but a certificate for a whole unit adds them.

## 4. Where cheaper typing would move the ceilings

Predicted, not measured (`cost.py ceilings`, `out/ceilings-*.txt`). The two typing phases are replaced in the model by a binary search: a term's place in a list of s blades takes ⌊log₂ s⌋ + 1 comparisons, each costing between one present scan step and one and a half (the midpoint and the two bounds); everything else is kept at its fitted weight. The generation phases, which set the ceilings once the typing is cheap, are the exactly fitted ones, so only the typing estimate is uncertain, and it is not the largest phase in any row below. Merging the per-pair runs instead would not help the full sets: in a diagonal form each pair has one term, so each run is one blade and a merge is an insertion.

| `full<n>` × `full<n>` | Clang 23, now | Clang 23, binary search | GCC 16 (cold certificate), now | GCC 16 (cold certificate), binary search |
| --- | --- | --- | --- | --- |
| Product, R(n,0,0), R(n−1,0,1), conformal(n − 2) | 5 | 5 (n = 6: fill 3.16 M steps, over 1.05 M) | 6 | 6 (n = 7: fill 87.7 M cold, over 33.5 M) |
| Product, dense form | 4 | 4 (fill) | 4 (builds 5) | 4 (builds 5) |
| Wedge, every form | 5 | 6 (n = 7: fill 1.22 M) | 6 | 8 (n = 8: fill 29.5 M; n = 9: fill 114.8 M) |

So a cheaper typing changes no product ceiling on either compiler's certificate; on GCC the real (cached) count of the generation was measured below the limit at n = 7 before typing existed (decision 0103: `gp_plan` built `full<7>`), so the product might build to 7 there, uncertified (inferred: the generator has changed since, by the set-bit loops of decision 0104, which made it cheaper, and the real count of its generation at n = 7 was not measured here). It moves the wedge by one generator on Clang and two on GCC. The typed and sparse pairs that programs use cost at most 2.5 % of Clang's limit now (motor times motor, 48 terms: 26,555 steps) and 0.45 % of GCC's (152,250 operations cold), at every n from 4 to 16.

## 5. A certified statement in place of a measured table

Proposed for the owner's ruling (decision 0125).

**In the documentation.** `product_plan`'s `///` comment would replace "Measured at the default limits ... builds for n up to 5 on Clang 23 and up to 6 on GCC 16" with a statement of the law and certified examples, in the user's terms and with no reference to the project's records. A draft:

> The plan is a constant evaluation, so the compiler's limits on constant evaluation bound the sets in practice. Its cost is a fixed function of the sets: generating it takes a fixed amount of work per pair of blades and per term, and placing its terms on the result set takes work proportional to the number of terms times the number of result blades, which dominates for large sets. Certified at the default limits, by an exact model of the compiler's count checked against it: with Clang 23 (its classic constant evaluator) and libstdc++ 16, `full<5>` with `full<5>` costs 655,239 of the 1,048,576 steps allowed in R(5,0,0) and 695,423 in conformal(3); `full<6>` costs 5,110,503 and does not build. With GCC 16 at most 25.9 million of the 33.6 million operations allowed for `full<6>` with `full<6>`. Four blades times three at n = 16 cost a few thousand on either.

**In a test.** `core.plan_cost_model`, registered for the Clang presets and one GCC preset: compile two or three reference plans with the limit set to their committed predicted thresholds (expected to build) and to one less (expected to fail; on GCC with `-fconstexpr-cache-depth=1`, and the build at one less checked to fail with the operation-count message, since a pass below the count is possible there, 3.3), and on a mismatch print the measured threshold. The committed thresholds are values ported with their derivation (the predictor's output at a recorded revision), not a call of the predictor. It would catch a new compiler, library, evaluator, or generator change that invalidates the certified figures, which the measured table cannot: a stale table goes on reading as true. Cost: about six compiles of under a second each.

**The tool.** `cost.py certify <weights> <limit> <relative margin> <absolute margin> <op> <form> <n> <a> <b>` (margins 0 0 for an exact model) prints each phase's predicted count, whether every prediction lies inside the span of the fit, and the verdict, with GCC's per-loop limit checked structurally. Refitting for a new compiler is `probe.py run` over the training cases (about 800 units; on the shared pool about ten minutes on Clang and thirty on GCC with the confirmation of 3.1) and `cost.py fit`. It belongs with the snippets (`docs/agent/snippets/`) if the owner accepts the test, since it produces the test's values.

## 6. Reproduction

From the worktree root, with the pool exported (`eval "$(tools/jobpool env)"`); `R=docs/agent/records/performance/constexpr-certification`:

```bash
python3 -I $R/probe.py cases train $R/out/train-cases.tsv          # and train-gp, test, plans, wide
python3 -I $R/probe.py run $R/out/all-train-cases.tsv $R/out/all-train-clang-galoot-O0.tsv clang galoot-O0 warm 12
python3 -I $R/cost.py fit $R/out/all-train-cases.tsv $R/out/all-train-clang-galoot-O0.tsv $R/out/weights-clang-galoot-O0.tsv
python3 -I $R/probe.py run $R/out/plans-cases.tsv $R/out/plans-clang-galoot-O0.tsv clang galoot-O0 warm 12 $R/out/weights-clang-galoot-O0.tsv
python3 -I $R/cost.py check $R/out/plans-cases.tsv $R/out/plans-clang-galoot-O0.tsv $R/out/weights-clang-galoot-O0.tsv
python3 -I $R/cost.py ceilings $R/out/weights-clang-galoot-O0.tsv 1048576 9
python3 -I $R/cost.py certify $R/out/weights-clang-galoot-O0.tsv 1048576 0 0 gp r 5 full full
```

GCC as the same with `gcc galoot-O0 cold` (the calibration) or `warm`. The units are regenerated from the case files, so `out/*.units/` are not kept. Every compile is a `ceiling-cert:<case>:<cache>:<compiler>:<config>:L<limit>` row of `runs.tsv`.

## 7. Open questions

1. The mechanism of GCC's charge for unwritten aggregate elements (3.2) is inferred from the fit, not read in its source.
2. A model of GCC's call cache, which would certify what the cold model leaves out (the dense `full<5>`). It needs, for every call in the generator, whether its arguments are constants and its nesting depth below 8.
3. The bytecode interpreter: its weights were not fitted. Its counts are about half the classic evaluator's on a plain loop; since Clang 23 a distribution may build it as the default.
4. GCC's other configurations (Galoot -O2, bare -O2) were not calibrated; decision 0113 found the same ceilings in all three.
5. Memory is not modeled.
