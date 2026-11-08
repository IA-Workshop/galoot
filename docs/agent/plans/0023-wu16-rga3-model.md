# 0023 WU16: the rga3 model, its V8 cases, and the README example

- Status: planned 2026-11-07 (planner: Claude, agent, Opus 5.5, planner role with math-expert, cpp-expert, hci-expert, and numerical-expert judgement; implementers and reviewers to be named by the coordinator at branching). Claimed by this plan's merge into `develop`. Nothing in section 4 after step 0 starts until the owner has ruled on O1 to O9 (section 10).
- Branches, from `develop` at `d68881f` or later, named by W7 as amended (decision 0188):
  - `feature/rga3`, worktree `.claude/worktrees/rga3`: the unit. Only this branch merges into `develop`.
  - `feature/rga3--math`, worktree `.claude/worktrees/rga3--math`: the math-expert's readings, reference script, and conventions checks (step 1), branched from `feature/rga3` at its first commit and merged back into it (W7's subfeature row), since its commits build and pass on their own.
  - `feature/rga3--tests`, worktree `.claude/worktrees/rga3--tests`: the test author's commits, never merged; each group is cherry-picked with `-x` (decision 0185).
- Record numbers: research note 0022 and decisions from 0196, the next free numbers at this commit on every branch. A number taken by another unit in the meantime moves to the next free one, as decision 0193 did.
- Source of scope: plan 0001 (WU16, "`rga3` model and its V8 cases; README example compiled as a test", size M, after WU13 and WU15; M4, "Zero core diff; 1.6 and 5.1 to 5.7 pinned; flector distinguishing test; rigid V8 cases", owner decides "Sufficiency of V8 cases"; DoD-5, DoD-6, DoD-17, DoD-19, DoD-27); CONVENTIONS 1.10 (with the 1.11 draft), items 1.3, 1.6, 5.1 to 5.7, and section 8; decision 0162 item 9 (named model classes deferred to WU16); decision 0187 (the `*_layout` names are public interface, and "WU16's model aliases will spell types through them"); lesson 0066; plan 0019 reading 7 (the scalar conversion, "before WU16's model aliases"); plan 0021 departure 7 (5.4 to 5.6 pinned in the core; 5.1 to 5.3 and 5.7 left to WU16); decisions 0176 to 0195; research notes 0005 and 0021; the design study, "Rigid GA, R(3,0,1)" to "Lengyel versus Hestenes-style conventions" and "6. Operations and naming".
- Goal: the rigid algebra of three dimensions as a model on the core's public interface, with zero diff under `include/galoot/core/`: the algebra tag, the oriented layouts and named types of a point, line, plane, motor, and flector, and the constructors of CONVENTIONS 5.7; tests that pin 1.6 and 5.1 to 5.7 on the model's own types; the V8 cases of DoD-17 (point groups, chains of joint motions against 4×4 matrices, incidence) with reference values computed by a route that shares no code or method with the library; a README whose C++ example is compiled and run as a test on both compilers.

## 1. Departures, ambiguities, and readings (C20)

### Departures and ambiguities

1. **The plan branch.** The brief says to use W7's form for a plan branch, else `records/plan-wu16`. W7's table names no plan branch and no `records/` prefix, so this plan was written on `records/plan-wu16`, as `records/m3-acceptance` and `records/review-wu13a` were. The coordinator merges it into `develop` to claim the unit (W7).

2. **WU15 has not started.** Plan 0001 makes WU16 depend on WU13 and WU15. WU13 is merged (`0fb3a57`, decision 0194). WU15 (`format.hpp` and the review of the DoD-19 catalogue as a whole, decision 0158) has no plan and no branch, and `include/galoot/core/` has no `format.hpp`. Nothing WU16 builds needs `format`: the README example reads coefficients with `get<"e1">(x)`, and `mv.hpp`'s comment already says `std::format("{}", x.coefficients)` prints them. Reading followed: WU16 can run before WU15, if the owner agrees (O1). M4's gate still waits for WU15.

3. **The scalar conversion has not happened either.** Plan 0019 reading 7 placed a unit for a named scalar conversion "after WU10b merges and before WU16's model aliases". It has no plan or decision. The model's types need no conversion between scalars: the mixed-scalar messages of `product.hpp` and `linear.hpp` already tell the user to give both types one scalar type. Reading followed: the order is not needed for WU16, under O1.

4. **CONVENTIONS fixes no component order for a motor or a flector.** Sections 5.4 to 5.6 say how a motor and a flector act, not where their coefficients are stored. The core's examples and tests use two motor orders: `blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">` in `mv.hpp`'s head comment, `bare_user.cpp`, and `product_test.cpp`, and `blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">` in `transform.hpp`'s head comment and `transform_test.cpp`; a flector is `"e1 e2 e3 e4 e423 e431 e412 e321"` in `transform_test.cpp`. A model's layout is public interface (decision 0187's reason applies to its layout names), so the order is a convention (C10) and needs the owner (O5). Zero core diff means the core's comments keep their own examples; section 12 notes the mismatch for a later documentation pass.

5. **CONVENTIONS 5.7's `translation(d)` does not say what d is.** A probe (departure 9) shows that a function of the model whose arguments are plain scalars is not found by argument-dependent lookup: `translation(std::int64_t{2}, ...)` from a user's namespace is "not declared in this scope" on GCC 16 and an "undeclared identifier" on Clang 23, while a function of the model taking a `Line` is found unqualified, because the algebra tag is a template argument of `mv`. So the spelling of d decides whether a user must qualify the call. O6 asks.

6. **DoD-17 says hexagonal and trigonal point groups need √3.** A planning probe (departure 9) builds all 32 crystallographic point groups as versors with rational coefficients, up to scale, by putting the three- and sixfold axes along [111] rather than along z: a turn by 60° about [111] is e41 + e42 + e43 + 3𝟙 (half angle 30°, so cos/sin = √3, and √3 times the axis (1, 1, 1)/√3 is rational), and a turn by 120° is e41 + e42 + e43 + 𝟙. A threefold axis along z does need √3: the ratio of the coefficients on 𝟙 and e43 of a rotor by 120° about z is cos 60° / sin 60° = 1/√3 under any scaling. Reading followed: the V8 set proposes all 32 groups, exactly over the test rational; the owner rules on it with the sufficiency of the V8 cases (O8).

7. **5.1 to 5.3 and 1.6 are partly pinned already.** The trace table has 1.6 at `yes` (an oriented name lists generators in the order of their product) and 5.4 to 5.6 at `yes` from WU13a's tests on layouts written in the test (plan 0021 departure 7). 5.1, 5.2, 5.3, and 5.7 are `no`. M4 asks for 1.6 and 5.1 to 5.7 "pinned"; reading followed: WU16 adds trace rows that state the facts on the model's own types (its layouts and aliases), sets 5.1 to 5.3 and 5.7 to `yes`, and adds tests of 5.4 to 5.6 on the model's types beside the core's, so that a change to a model layout cannot leave them passing.

8. **DoD-5's test-only model.** DoD-5 asks for the model, a test-only exact rational scalar, and one test-only model to be added with no core diff. The rational exists (`test/support/rational.hpp`), and the core's tests already define algebra tags and layouts outside the core (`test/core/signatures.hpp`). Reading followed: WU16 shows the zero core diff for `rga3` in its review record; a named test-only model such as STA is not part of WU16 and is left to WU14 or the release unit, which the M4 gate report states.

9. **Planning probes, not reproducible as committed (V10).** The statements marked "probe" rest on two scripts and one C++ source in the planner's scratch area, against the headers of `d68881f`, on g++ 16.2.1, clang++ 23.1.4, and Python 3.14:
   - `rga3_probe.py`: exact, over `Fraction`, with an independent exact Clifford implementation outside the repository (the Chevalley route, sharing no code with `test/reference/`), the transform of CONVENTIONS 4.4, and seed 20261107. It checks that the antiproducts of two motors, of two flectors, and of a motor and a flector in either order stay on the motor and flector layouts of departure 4 (50 random pairs each); that ½d on e23, e31, e12 plus 𝟙 moves points by d and leaves a direction (weight 0) unchanged (20 cases); that s e41 + c 𝟙, s e42 + c 𝟙, and s e43 + c 𝟙 with (s, c) = (3/5, 4/5), (5/13, 12/13), (8/17, 15/17), (7/25, 24/25), and (4/5, −3/5) turn points as the right-handed 3×3 rotation matrix with cos θ = c² − s² and sin θ = 2sc; that a chain of four joints, each Rz(θ) Tz(d) Tx(a) Rx(α) with rational half-angle sines and cosines, gives exactly the 4×4 matrix product on ten points, moves lines as the points that span them, and stays unitized; and that the 32 crystallographic point groups, generated by rational versors normalized up to scale, close under the antiproduct with orders 1, 2, 2, 2, 4, 4, 8, 4, 16, 8, 12, 24, 24, 24, 48 (triclinic to cubic), 3, 6, 6, 6, 12 (trigonal, axis [111]), 6, 6, 12, 12, 12, 12, 24 (hexagonal, axis [111]), and 4, 8, 8, 8, 4 (−4, 4/m, 4mm, −42m, mm2), every element an isometry fixing the origin with V ⟇ V̰ = λ𝟙, λ > 0. The expected orders are the classical ones as the planner knows them; no table was read (V9). All checks pass, in 0.9 s and 13 MiB.
   - `incidence_probe.py`: the same route, 200 random cases, seed 7. For points with weights w_p and w_q, p ∧ q has direction w_p q − w_q p and moment p × q on e23, e31, e12; the plane p ∧ q ∧ r of unitized points is (n, −n · p) with n = (q − p) × (r − p); the line p ∧ q of unitized points meets a plane g = (g_n, g_w) in (m × g_n + g_w v, −v · g_n), with v = q − p and m = p × q; and two planes meet in the line with direction g2_n × g1_n and moment g2_w g1_n − g1_w g2_n. All pass.
   - `model_probe.cpp`: a model namespace `galoot::rga3` written only on the core's public interface (the algebra tag, five layouts, alias templates `Point<T>` to `Flector<T>`), compiled with `-std=c++23 -pedantic-errors -Wall -Wextra -Wconversion -Wshadow -Werror -fsyntax-only` on both compilers, with `static_assert`s over `std::int64_t`: `Line{p ^ q}` has direction q − p; `Plane{(p ^ q) ^ r}` contains p; a translation built as above moves (1, 2, 3, 1) to (3, 6, −3, 1); the scaled rotor 3 e43 + 4 𝟙 turns (1, 0, 0, 1) into (7, 24, 0, 25); `compose(t, turn)` applies `turn` first; the mirror z = 0 on the flector layout reflects (1, 2, 3) to (1, 2, −3) with weight +1; `transform(line, mirror)` is a `Line`; `Flector{compose(turn, mirror)}` converts; `sizeof(Motor<double>)` is 64; and the unqualified-lookup facts of departure 5. Both compile; each compile took under 0.5 s (rows in a scratch log through `tools/timed --log`).

   The math-expert and the test author repeat each fact as a committed check or test before anything relies on it.

### Readings

R1. **Where the model lives.** STYLE S6: headers under `include/galoot/<module>/`, namespace `galoot::<module>`. So the model is `include/galoot/rga3/` in namespace `galoot::rga3`, with an umbrella `include/galoot/rga3.hpp` as `core.hpp` is for the core. Proposed headers, for the implementer and the reviewer to confirm (S16): `rga3/objects.hpp` (the tag, the layouts, the aliases; includes `<galoot/core.hpp>`) and `rga3/motions.hpp` (`rotation` and `translation`; includes `objects.hpp` and `<cmath>`), so that a user of the objects alone does not parse `<cmath>` (P3). One header is the alternative if the reviewer judges the split not worth its file. The model includes only standard headers and the core's public headers, and uses nothing from `galoot::core::detail` (C13, "extensions reach the core only through its public interface").

R2. **The model's public interface, proposed (O2, O4).**

| Name | What it is | Source |
| --- | --- | --- |
| `galoot::rga3::algebra` | the tag, `value = galoot::core::rigid<3>()`, as `metric.hpp`'s comment shows | 1.3 |
| `point_layout` | `blades<algebra, "e1 e2 e3 e4">` | 5.1 |
| `line_layout` | `blades<algebra, "e41 e42 e43 e23 e31 e12">` | 1.6, 5.2 |
| `plane_layout` | `blades<algebra, "e423 e431 e412 e321">` | 1.6, 5.3 |
| `motor_layout` | `blades<algebra, "e41 e42 e43 e1234 e23 e31 e12 1">`, after O5 | 5.4, 5.5, O5 |
| `flector_layout` | `blades<algebra, "e1 e2 e3 e4 e423 e431 e412 e321">`, after O5 | 5.6, O5 |
| `Point<T>`, `Line<T>`, `Plane<T>`, `Motor<T>`, `Flector<T>` | alias templates of `galoot::core::mv<algebra, ..._layout, T>`, no default scalar | S10 (CamelCase model types), O2 |
| `rotation(l, theta)` | `Motor<T>` holding l sin(θ/2) + 𝟙 cos(θ/2) | 5.5, 5.7 |
| `translation(...)` | `Motor<T>` holding ½d on e23, e31, e12 and 1 on 𝟙; argument form after O6 | 5.5, 5.7 |

   Everything else a user does with these types is the core's: `wedge` and `^` (join), `antiwedge` and `&` (meet), `transform`, `compose`, `bulk`, `weight`, the duals, `dot`, `antidot`, `project`, `antiproject`, `get`, `truncate`, `part`, and the explicit conversions between layouts (`Line<double>{p ^ q}`). Argument-dependent lookup finds both the core's functions and the model's for arguments of the model's types (probe). Not in the model: a `Magnitude` type, norms, `unitize`, `exp`, `log`, a motor from screw parameters, a motor-to-matrix conversion, and named concepts per object class such as `versor<T>` (plan 0001 section 3 puts square-root norms, `unitize`, `exp`, and `log` out of v1; the others have no present user, C12).

R3. **Alias templates, not derived classes (O2).** Decision 0162 item 9 deferred "named model classes deriving from `mv`" (HCI F6) to WU16. The aliases are recommended because: a derived class loses `explicit` from the inherited constructor on GCC 16 (lesson 0066), so `Point<double> p = 2.0;` would compile there; `truncate` and `part` take the mv type itself and reject a derived class (`mv.hpp`), so a derived class would need a core change, against "zero core diff"; and every product, transform, and conversion returns `mv`, so a derived class needs a conversion back at every step. What the aliases give up is diagnostic length on GCC: plan 0016 measured the lossy-assignment line at 811 characters with an alias and 98 with a derived class, and `get`'s at 499 against 257. Clang prints the alias name in its "aka" text (plan 0016, route 6).

R4. **`rotation(l, theta)` (5.7, O6, O7).** For a scalar type with `std::sin` and `std::cos`: `std::floating_point<T>`, with float and double tested and `long double` neither tested nor claimed (CONVENTIONS 4.5's reason). theta in radians, of type T exactly, as the core's scaling requires the multivector's scalar type exactly. l is a `Line<T>`; the comment states that l is assumed unitized (direction of length 1) and a line (direction · moment = 0), that neither is checked, and what a violation gives: a motor that is not unitized, which `transform` does not correct (C8 is met by the statement, as for `transform`'s λ, plan 0021 R3). Not `constexpr` in the sense of a constant evaluation: `std::sin` is not usable in a constant expression in C++23 on Clang 23, though GCC 16 evaluates it as an extension (knowledge base, constexpr `<cmath>` note), so a `constexpr` `rotation` would be constant on one compiler only, which C8's "behavior is declared rather than left to ... the environment" rules out. The recommendation is a plain `inline` function, `[[nodiscard]]`, documented as run time only. Exact tests use rotors written by their coefficients (as WU13's V11 rotor 3/5, 4/5).

R5. **`translation` (5.7, O6).** For a scalar type whose halving is exact: a floating type (exact barring underflow of a subnormal d, which the comment states) or an exact type whose division is exact, such as the test rational. A built-in integer type is rejected at compile time with its own message, since ½d would truncate. Returns a `Motor<T>`, which is unitized for every d.

R6. **Messages (C9; decisions 0166, 0173, 0179).** Proposed, for the test author to finalize: `rotation` over an integer or other non-floating scalar ("rotation: the angle needs std::sin and std::cos, which this scalar type lacks; build the motor from its coefficients, l sin(θ/2) + 𝟙 cos(θ/2)"); `rotation` with an axis that is not a `Line<T>` ("rotation: the axis must be an rga3 Line; convert it with Line<T>{x}"); `rotation` with theta of another type than the line's scalar; `translation` over an integer scalar ("translation: halving d would truncate in an integer scalar type; use a floating or exact rational type"). Each pinned by a compile-fail test on both compilers, with `detail::` and `apply:` forbidden on the first error line. The catch-alls follow decision 0033's form P (a requires-expression asking a catch-all is told yes).

R7. **Build and tooling (build-expert role).** A new INTERFACE target `galoot_rga3` (alias `galoot::rga3`) with its own header set, linking `galoot_core`, with `VERIFY_INTERFACE_HEADER_SETS` so each model header compiles alone (S6); the root `CMakeLists.txt` check that every header under `include/galoot` belongs to a header set is extended to the union of both sets; `format-check` reads both sets. Tests in `test/rga3/<component>_test.cpp`, registered by `galoot_add_test(MODULE rga3 COMPONENT ...)` with `galoot::rga3` linked beside it, labels `rga3;rga3.<component>`. `tools/include_scan.py` gains the model's rules (standard headers, the core's public headers, and the model's own; guards and names as for the core; the core still includes nothing of the model) with a rejecting fixture each in `test/tools/include_scan/`. `rga3.bare_compile` compiles a user file with only `-std=c++23 -pedantic-errors -I include` (DoD-6). `tests.citations` gains `README.md` (W12: no process records in user documentation).

R8. **The README example (DoD-27, O9).** `README.md` is empty at `d68881f`. Recommended mechanism: a small script that extracts each fenced `cpp` block of `README.md` into the build tree, and a CTest per block that compiles it with the test flags and runs it, on each preset, so both compilers build it; the check fails when the README has no block, so a lost fence cannot pass. The alternative, an example kept as a test source with a check that the README's block equals it byte for byte, needs no generator but keeps the text twice. The README's text is user documentation (W12): what Galoot is, how to use it (headers only, C++23), the rga3 example, the license.

R9. **Reference values (V1 kind 3, S14).** The V8 referents are computed by plain linear algebra over `Fraction`, sharing no code and no method with the library or with the GA routes of `conventions_check.py`: 3×3 rational orthogonal matrices for the point groups (signed permutation matrices, and Rodrigues' formula R = cos θ I + sin θ [u]× + (1 − cos θ) u uᵀ where sin θ · u and (1 − cos θ) u uᵀ are rational), 4×4 homogeneous matrices for the chains, and cross products and determinants for the incidences. A new module `test/reference/rigid_motions.py` holds them; `golden.py` imports it and writes new namespaces of `golden.hpp`, each with its `// Fact:` line and trace row, and, as `golden.py` already does for its own tables, writes a value only where the linear-algebra route and the GA route of `conventions_check.py` agree. One golden header is kept, so the W5 trace command in WORKFLOW is unchanged; the cost is that every core test that links `galoot_golden` rebuilds once (P8, "a reference script or data in `test/reference/`").

R10. **What the C++ tests compute.** Point groups: the closure of the generators under `compose`, at run time over the test rational, each element normalized by its first nonzero coefficient in canonical order, counted, and matched one to one with the golden matrices by its action on the four points (1, 0, 0), (0, 1, 0), (0, 0, 1), (1, 2, 3); four antiproduct plans (motor and flector in each order) serve every group. Chains: `compose` of the joint motors and `transform` of golden points, compared exactly. Incidence: `wedge` and `antiwedge` on the model's types against golden values. Run time keeps the build cost to the plans (a closure of 48 elements in a constant evaluation would cost far more); `static_assert`s pin the hand values of 5.x.

R11. **V12 for `rotation`.** Over double and float, `rotation(l, θ)` equals bit for bit the motor whose coefficients the test computes from the definition, l_i · std::sin(θ/2) and std::cos(θ/2), which pins the half angle, the assignment of sine and cosine, and the order of the layout. That shares `std::sin` with the library, so it verifies the construction, not the accuracy of the C library. A geometric test of "turns by theta" at double (for example a quarter turn about z) needs a tolerance that includes the C library's error for sin and cos; the numerical-expert derives it from `transform`'s forward bound (decision 0180) and the C library's published accuracy, read under V9. If no such source can be read, the test is not written and the report says that the angle's accuracy at double is not validated. "Turns by theta" itself is pinned exactly by rotors with rational half-angle sines and cosines (5.5, 5.7 facts).

### The W2 question

As in plans 0020 and 0021, with decision 0195 item 6: the test author commits each group on `feature/rga3--tests`, first compiling it against a scratch stand-in (such as the planner's `model_probe.cpp`) and running the `tests.reference_markers`, `tests.golden_facts`, and `tests.citations` checks on it (plan 0021 departure 16); the implementer cherry-picks each group with `-x` immediately before the code it judges; each such commit is the single failing commit W2 allows and says so. A test author's build failure may be combined with its fix before anyone has fetched it (decision 0195 item 6), and the brief tells the reviewer.

## 2. Definition of done

- `include/galoot/rga3.hpp` and the headers of R1, in `galoot::rga3`, with the names of R2 as ruled; every declaration with its `///` comment (S11), restating the conventions it applies (the layouts and their oriented names, the half angles, what `rotation` assumes and does not check, that `rotation` is not usable in a constant expression, that a call of `translation` with scalar arguments needs its namespace if O6's form (a) is ruled), with no citation of records (S11).
- The build and tooling of R7, and the README mechanism of R8 as ruled.
- `README.md`: the text of R8, its example compiled and run as a test on both compilers.
- CONVENTIONS version 1.12 (or folded into 1.11 if the owner has not yet confirmed 1.11's wording): the motor and flector layouts (O5), 5.7's argument form (O6), the line's moment p × q in 5.2, each with its label after step 1's checks, and the revision row; a decision recording the text as committed.
- `test/reference/conventions_check.py`: checks for the new 5.x facts (layout closure under the antiproduct, the moment, `translation` and `rotation` from their definitions with rational half angles, a direction unmoved by a translation). `test/reference/rigid_motions.py` and `golden.py`'s new namespaces (R9). `reference.golden` passes.
- Research note 0022: Lengyel's Motor and Flector pages read directly as raw wikitext under V9 (the layouts and the constructors, as note 0021 read the Projections page); the source of the point-group orders (the International Tables for Crystallography, volume A, if it can be read, otherwise a named substitute with how it may differ); and the source of the joint-chain form (Denavit and Hartenberg's paper if it can be read; otherwise the chain is described as the product Rz(θ) Tz(d) Tx(a) Rx(α) without a claim of conformity to a named convention).
- Tests (doctest-expert role) in `test/rga3/`: the objects (layouts, aliases, sizes, 1.6 and 5.1 to 5.3 facts, 5.4 to 5.6 on the model's types), the motions (5.7, R11), the compile-fail tests of R6, and the V8 cases (R10), each `TEST_CASE` with its reference marker.
- Trace table: rows for the model's facts of 1.6 and 5.1 to 5.7 and for each new golden namespace; 5.1, 5.2, 5.3, and 5.7 set to `yes`.
- Decisions: the model's interface and its headers (with O2, O4 to O7 as ruled); the README mechanism; the V8 set (with O8).
- Records: mutants (`records/mutations/0023-rga3/`); diagnostics (`records/diagnostics/wu16-messages.md`); P1 build cost of the new units on both compilers (no W10 before-and-after for the core, whose diff is empty; one includer of `core.hpp` measured before and after to show it unchanged); the zero core diff shown by `git diff --stat` of `develop` against the branch for `include/galoot/core/` and `include/galoot/core.hpp`, in the review record (DoD-5); the plan's outcome.
- A W5 blind review with the usability checklist, an hci-expert reading before it, and the merge into `develop` after P9's checks (section 7).

## 3. Acceptance criteria

1. **Zero core diff.** `git diff develop...feature/rga3 -- include/galoot/core include/galoot/core.hpp` is empty at the merge.
2. **Layouts and orientations (1.6).** Each model layout equals the oriented set written in the test from CONVENTIONS 1.6's names (masks and signs: e41 = −e14, e31 = −e13, e321 = −e123, and so on), never one read from the header; `sizeof` of each type over double and float is its component count times the scalar's size (P5); each type is trivially copyable and a structural type over double.
3. **5.1 to 5.3 on the model's types,** exactly over `std::int64_t` and the test rational, as `static_assert`s where the scalar allows: `weight(p)` is w on e4; `Line<T>{p ^ q}` of unitized points has direction q − p and moment p × q; `antiwedge(p, g)` is g_x x + g_y y + g_z z + g_w w on the scalar; points counterclockwise about the tip of a normal give that normal; seeded cases against the golden incidence values.
4. **5.4 to 5.6 on the model's types:** the composition order with non-commuting motors, the translator moves by 2t, the rotor turns by 2φ counterclockwise, the mirror e412 reflects (1, 2, 3, 1) to (1, 2, −3, 1) with weight +1, lines and planes move as the points that span them, for motors and flectors on `motor_layout` and `flector_layout`.
5. **5.7.** `translation` moves points by d and leaves a direction unchanged, exactly over double and the test rational; `rotation` meets R11 over double and float; rotations about the x, y, and z axes and about a line not through the origin, built from rational half angles, turn as the right-handed matrices of the golden values.
6. **Point groups (V8).** For each of the 32 classes (O8), the closure of its rational generators under `compose` has the published order, and its elements act as the golden matrices, one to one.
7. **Chains of joint motions (V8).** At least three seeded chains of three to six joints: `transform` of golden points by the composed motor equals the golden 4×4 product exactly, and the composed motor is unitized.
8. **Incidence (V8).** The join and meet values of criterion 3, the meet of two planes, of a line and a plane, and of three planes against the golden values; a point on a plane, a point on a line, and two coplanar lines give exact zeros where the linear-algebra route says so.
9. **Diagnostics.** Each message of R6 is one error with its message on the first error line on GCC and Clang, also as `auto r = ...;`, with the forbidden names absent there.
10. **README.** Each fenced C++ block of `README.md` compiles under the test flags and runs to exit 0 on every preset; removing the fence fails the check.
11. **Tooling.** The include scan rejects each new fixture with its finding; `rga3.bare_compile` passes; `tests.citations` reads `README.md`.
12. **Mutants** of section 6 are killed on gcc-debug and clang-debug by a test, never by a warning; an equivalent survivor is recorded with its reason.
13. **Checks** of section 7 pass on the final revision, from new, empty build trees, on a clean tree.

## 4. Approach: parts and order

Tests come before the code they judge (section 1, "The W2 question"). Expected-value changes to existing tests, if any, are separate listed commits (W9). Small commits.

0. Claim: this plan merged into `develop`. The owner rules on O1 to O9.
1. **Math-expert** (`feature/rga3--math`): research note 0022 (the V9 readings of section 2); `conventions_check.py`'s new checks; `rigid_motions.py` and `golden.py`'s namespaces; the regenerated `golden.hpp`; the probes of departure 9 repeated as committed checks. The conventions check and `reference.golden` run. If a reading disagrees with the layouts of departure 4 or with a probe, section 8 of CONVENTIONS gains a row and the owner decides (C11) before step 2.
2. **CONVENTIONS** text as ruled, its decision, and the new trace rows set to `no` (implementer). The labels "Checked" rest on step 1.
3. **Test group 1, objects** (test author): criteria 2 to 4, and the trace facts of 1.6 and 5.1 to 5.6.
4. **Build lane and objects** (implementer, with build-expert judgement): R7's target, header-set check, include scan and fixtures, bare compile, test registration; `rga3.hpp` and `objects.hpp`. P8 for a change to the build description: the full suite once in gcc-debug at this commit, then narrowed.
5. **Test group 2, motions** (test author): criterion 5, the compile-fail tests of R6, R11.
6. **`motions.hpp`** (implementer).
7. **Test group 3, V8 cases** (test author): criteria 6 to 8. They judge code that already exists, so they pass when committed, as WU13's identities did.
8. **README and its check** (implementer, with hci-expert and build-expert judgement): R8, criterion 10.
9. Trace rows set to `yes`, with the trace gate run.
10. Records, decisions, mutants, costs, the outcome. The hci-expert reading (names, messages, README), then the blind review; fixes; a delta review as section 8 says.
11. The coordinator merges `develop` into `feature/rga3`, runs P9's `develop`-level checks (`gate:all`), and merges it into `develop`.

## 5. Evidence and reference values (VVUQ)

| Claim | Reference | Independent check |
| --- | --- | --- |
| Layouts and oriented names (1.6, 5.1 to 5.3) | CONVENTIONS 1.6 and 5.1 to 5.3, Confirmed in note 0005 (F1) | golden `oriented_names` (exists); the test writes each set from the names, not from the header |
| Motor and flector layouts | Lengyel's Motor and Flector pages, read directly (note 0022) | closure of the layouts under the antiproduct in `conventions_check.py` and in `compose_layout` comparisons in the tests |
| Line moment, plane offset, meets | `rigid_motions.py` (cross products, determinants) | the GA route of `conventions_check.py`; `golden.py` writes a value only where both agree |
| `translation`, `rotation` | 5.5 and 5.7 as definitions, with rational half angles | right-handed rotation matrices in `rigid_motions.py`; R11 for double |
| Point groups | orders: the International Tables, volume A, or a named substitute (V9); elements: rational matrices closed in `rigid_motions.py` | C++ closure over versors, matched one to one with the matrices |
| Chains | 4×4 rational matrix products in `rigid_motions.py` | C++ `compose` and `transform`, exact |
| Messages | the test author's catalogue | compile-fail tests on both compilers |

Exact scalars compare with `==` (V11). The only rounding comparison is R11's, whose tolerance, if the test is written, is derived before the result is seen (V12). Common-mode risk (plan 0001 risk 5): the reference scripts and the code are written by agents of one kind; the linear-algebra route, the separate math-expert, the blind reviewer's reading of `rigid_motions.py`, and the owner's ruling on the V8 set mitigate it.

V10, to fill in at the outcome: verification as above; validation (V8) by the point groups, the chains, and the incidences, against the sources of note 0022. Not validated: `rotation`'s accuracy at double unless R11's source is read; the Terathon side of section 8 (C15, C16); any setting of a point group other than the one tested (the orders do not depend on it).

## 6. Mutants

Each on gcc-debug and clang-debug:
- each layout with one blade's orientation flipped (e14 for e41, e123 for e321, e13 for e31);
- the motor layout with 1 and e1234 swapped, and with the halves e41 e42 e43 e1234 and e23 e31 e12 1 swapped; the flector layout's halves swapped;
- `rotation` with θ in place of θ/2, with sine and cosine swapped, with −θ;
- `translation` with d in place of ½d, with −½d, with the components of d permuted;
- each catch-all's constraint removed;
- `rotation` and `translation` given the wrong layout (a mutant of the return type).

The reference scripts are not mutated; a wrong golden value is caught by `golden.py`'s two-route agreement and by `reference.golden`.

## 7. Checks and presets (P8, P9, P13)

- Every build and test through `tools/timed` after `eval "$(tools/jobpool env)"`; the job pool's status read before and after each heavy run; the pool never started twice and never stopped by an agent.
- During work (P8, one configuration, gcc-debug): `ctest -L '^rga3'`, the `tools.include_scan_rejects` and `core.include_scan` checks when the scan changes, `reference.golden` and the tests that link `galoot_golden` when `test/reference/` changes, `tests.reference_markers`, `tests.golden_facts`, and `tests.citations` when a test or the README changes; `python3 -I test/reference/conventions_check.py` when it changes. The build-lane commit (step 4) changes the root `CMakeLists.txt`, so the full suite runs once there.
- Merge of `feature/rga3--math` into `feature/rga3` (P9): the selection of P8 for what it changed, in gcc-debug and clang-debug.
- Merge into `develop` (P9): the full suite on gcc-debug, gcc-release, clang-debug, clang-release, gcc-asan and clang-asan (sanitizers, S5), `lint` (clang-tidy, S4), the full suite under `clang-measure` (decision 0184), `format-check` (S3), the trace gate of W5, `tools/runlog check` (and `prune` if it reports removed rows) since `runs.tsv` changes, and the compile-record copy of decision 0080. All from new, empty build trees at the final revision of the build inputs, on a clean tree. The merge message carries P13's line.
- Cost (C14, P1): per-unit wall time and peak memory of the new test units on both compilers from `compile.tsv`, the `gcc-measure` and `clang-measure` rows of an includer of `rga3.hpp`, and an includer of `core.hpp` before and after (unchanged expected, since the core is). No bound (C14).

## 8. Review plan (W5)

- An hci-expert reading after step 9, independent of the implementer and the test author: the names of R2, the messages of R6, the README, and the diagnostics a first user meets (a wrong count for `Motor<double>`, a lossy conversion, a missing blade name, `rotation` over an integer scalar), on both compilers. Its findings are resolved before the blind review.
- A blind review (the `blind-reviewer` agent, Opus, a context that shares nothing with the authors), with the usability checklist, since the unit adds a public interface (decision 0160). It gets the diff of `feature/rga3` against `develop`, sections 2 and 3, the authors' departures, and the list of commits for W9 (the cherry-picked tests, and any combined build fix, decision 0195 item 6); not the authors' view that the change is correct. It checks C13 (only public core interface used), the zero core diff, the independence of `rigid_motions.py` from the library and from `conventions_check.py`'s GA route, the V9 records of note 0022, and the governance documents. The reviewer's report is committed as written (decision 0182 item 2).
- W5's delta-review rule names core code; the model is an extension (C2, C19). Recommended anyway: a delta review of any fix after the review that changes behavior in `include/galoot/rga3/`, and none for a change to comments only, listed in the review record (decision 0183).
- The M4 gate (WU13, WU15, WU16) gets an auditor run on `develop`, as plan 0001 section 6 requires; not part of this unit.

## 9. Risks

1. **Sources.** The International Tables are not freely available, and Denavit and Hartenberg's paper may not be either. Substitutes are named as such (V9); the orders are also derived by closure (kind 1), so a substitute changes only the validation's referent, not the verification.
2. **Lengyel's pages disagree with the layouts of departure 4.** Then section 8 gains a row and the owner decides (C11), and O5's recommendation changes with the reading.
3. **Two public names for one convention.** The core's examples keep their own motor order (departure 4). A user who copies `mv.hpp`'s example and then the model's `Motor` meets two orders. Noted for a documentation pass outside this unit (section 12).
4. **Unqualified `transform` under `using namespace std::ranges`** finds `std::ranges::transform` (stated in `transform.hpp`). The README example qualifies its calls or does not use that directive.
5. **Shared files.** `test/CMakeLists.txt`, `CMakeLists.txt`, `golden.py`, `golden.hpp`, `conventions_check.py`, `tools/include_scan.py`, the trace table, CONVENTIONS, and `runs.tsv`. Whichever branch merges second merges its parent first (W7). WU15, the O6 chain-limit research unit (decision 0195 item 1), and WU12 may touch some of them.
6. **Compile cost** of the V8 tests: four antiproduct plans and the point and line plans of rigid(3), far below the limits `transform.hpp` states; measured, not bounded.

## 10. Open questions for the owner

Each has a recommendation. Those marked **owner ruling** change a convention, the roadmap, or a public interface, or were deferred by the owner to this unit.

- **O1. Order of units (owner ruling).** WU16 before WU15 and before the scalar-conversion unit (departures 2 and 3). Tradeoffs: running now keeps the critical path (WU13 → WU16 → WU18) moving and needs nothing from either unit; the cost is that WU15's `format` may later change the README example, and the scalar conversion's names may later be added to the model's messages. Recommended: proceed; M4's gate still waits for WU15.
- **O2. Named types as alias templates, not derived classes (owner ruling; deferred to WU16 by decision 0162 item 9).** Tradeoffs in R3: aliases need no core change and keep `explicit` on both compilers and `truncate`, `part`, and every result type working unchanged; derived classes would shorten GCC's longest diagnostic lines several times over but need a core change, a GCC workaround, and a conversion after every operation. Recommended: aliases. A later switch would be an incompatible change, announced at release (W13).
- **O3. No default scalar (with O4).** `Point<double>`, not `Point<>` or a `Point` fixed to double, as `mv` has none (plan 0016) and C8 asks for declared behavior over defaults. Recommended: yes. The alternative, a default of double, saves typing in the common case.
- **O4. The model's public names (owner ruling).** R2's table: the namespace `galoot::rga3`, the tag `algebra`, the five `*_layout` names, the five aliases, `rotation`, `translation`, the headers of R1, and no `Magnitude` type. Tradeoffs: `algebra` reads well in diagnostics (`mv<galoot::rga3::algebra, ...>`) where a tag named `rga3` would print `galoot::rga3::rga3`; leaving out `Magnitude` and `unitize` keeps to v1's scope (plan 0001 section 3) at the cost of a model with no norms beyond `dot` and `antidot`. Recommended: as in R2.
- **O5. The motor and flector layouts (owner ruling; CONVENTIONS, C10).** Proposed CONVENTIONS text, after step 1's reading confirms it (a new item 5.8, so that 5.4 to 5.7 keep their numbers): "A motor stores (r_x, r_y, r_z, r_w) on e41, e42, e43, 𝟙 and (u_x, u_y, u_z, u_w) on e23, e31, e12, 1, and a flector stores (s_x, s_y, s_z, s_w) on e1, e2, e3, e4 and (h_x, h_y, h_z, h_w) on e423, e431, e412, e321." Tradeoffs: this is the order `transform.hpp`'s example and `transform_test.cpp` use, and, if note 0022 confirms it, the order of Lengyel's published formulas, so coefficients copy over with no reordering (1.6's purpose); the other order in the core's examples ("1 e41 … e1234", grade order) reads naturally to a reader of the core but matches no published formula. Recommended: the text above, labeled Confirmed only after note 0022, and its closure Checked by `conventions_check.py`.
- **O6. 5.7's `translation(d)` and the constructors' scalars (owner ruling; CONVENTIONS 5.7's wording).** Options for d: (a) three scalars, `galoot::rga3::translation(dx, dy, dz)`: nothing discarded or assumed, but the call needs its namespace (departure 5); (b) a `Point<T>` taken as a direction, which ADL finds, but which discards or must check its weight (C8); (c) a new direction type on e1, e2, e3, which ADL finds and which `transform` already handles correctly (a translation leaves it unchanged, probe), at the cost of a sixth type whose only present user is `translation` (C12). Recommended: (a), with 5.7 reworded "`translation(dx, dy, dz)` moves by d = (dx, dy, dz)". Also for the owner: `rotation` for floating scalars only and `translation` rejecting integer scalars (R4, R5). Recommended: yes.
- **O7. `rotation` at compile time.** Not usable in a constant expression on any compiler, rather than on GCC only (R4). Recommended: yes. The alternative marks it `constexpr` and documents that only GCC evaluates it at compile time.
- **O8. The V8 cases and their sufficiency (owner ruling; plan 0001 M4 "owner decides").** Proposed: every Checked value of section 5 on the model's types; all 32 crystallographic point classes with the three- and sixfold axes along [111] (departure 6), against a published table of orders and against rational matrices; at least three seeded chains of three to six joints against 4×4 matrices; the incidences of criterion 8. This amends DoD-17's reading ("families realizable over ℚ only; hexagonal and trigonal need √3 and are deferred or run under V12"): no family needs √3 in this setting, so none is deferred and none runs under V12. Tradeoffs: the [111] setting is not the standard setting of the hexagonal and trigonal classes in the International Tables, which put the principal axis along z with a hexagonal basis; the orders and the kinds of element do not depend on the setting, but a reader comparing coordinates with the tables must conjugate. Recommended: this set, as sufficient for M4.
- **O9. The README (owner ruling on content; tooling by decision).** Recommended: WU16 writes a short README (R8) and the extraction check; the full quickstart, core guide, and model guide stay with DoD-27 in M8. The alternative is that the owner writes the README text and WU16 adds only the check and one example.

Proposed changes to governance documents: none. Proposed changes to CONVENTIONS: O5, O6, and 5.2's added sentence "Its moment is p × q for points of any weight" (labeled Checked after step 1), in version 1.12, or in 1.11 if the owner prefers to confirm both drafts at once. Proposed change to plan 0001: DoD-17's point-group sentence, under O8, recorded as a decision with the owner's ruling (plan 0001's status line says a decision record governs where it and the plan differ).

## 11. Agent cost estimate

Recorded in `docs/agent/records/agent-cost.tsv` as each agent finishes. Comparisons from that file: WU13a's math-expert used 311k tokens, its test author 464k, its implementer and coordinator 546k, its blind reviewer 349k (review and delta).

| Role | Model | Estimate |
| --- | --- | --- |
| math-expert: note 0022, `conventions_check.py`, `rigid_motions.py`, golden namespaces | Opus, high | 300k to 450k tokens |
| test author (doctest-expert), three groups and the compile-fail tests | Opus, high | 350k to 500k |
| implementer (cpp-expert role, with build-expert judgement): headers, build lane, README check, records | Opus, high | 350k to 550k |
| numerical-expert, R11's tolerance and source, if written | Opus, high, or Sonnet for the reading | 60k to 120k |
| hci-expert reading | Opus, high | 100k to 180k |
| blind reviewer with the usability checklist | Opus, high | 300k to 400k |
| delta reviewer, if needed | Opus, high | 150k to 250k |

Size M, as plan 0001 has it: about 1.6M to 2.5M tokens in all, less than WU13 (2.6M to 3.6M estimated), since the model adds little code and the core is unchanged. This planner's own cost is recorded by the coordinator from the harness report.

## 12. Notes for later units

- The core's examples (`mv.hpp`'s head comment, `bare_user.cpp`) use the motor order "1 e41 e42 e43 e23 e31 e12 e1234"; once O5 is ruled, a documentation pass could align them with the model, as a change to comments only.
- A test-only named model (DoD-5's "for example STA") is still to be placed (departure 8).
- A rational versor table of the 32 point classes is a natural fixture for M7's cost measurements (many compositions of four plan pairs) and for a black-box comparison later (V1 kind 4).
- DoD-24's rga3 run-cost items (point ∧ point, line ∨ plane, transform of a point and a line by a motor) stay with M7; the model's types are the types they name.

## 13. Outcome

In progress. 2026-11-07: `feature/rga3` branched from `develop` at `88c9484` after the owner's rulings on O1 to O9 (decision 0196); step 1 (the math-expert) started on `feature/rga3--math`. Research note 0022 was taken by the flat-plane chain rounding note in the meantime, so this unit's research note is 0023, as section 1's record-number rule says.

### Steps 1 to 10

Implemented on `feature/rga3` (coordinator: Claude, agent, Opus 5.5; step 1: math-expert role on `feature/rga3--math`; tests: doctest-expert role on `feature/rga3--tests`, three groups; implementers in the cpp-expert role, with build-expert judgement for step 4 and hci-expert and build-expert judgement for step 8; R11's tolerance: numerical-expert role on `records/rotation-tolerance`; steps 9 and 10: an implementer, Claude, agent, Opus 5.5). Not reviewed yet (W5) at step 10: the hci-expert reading and the blind review of section 8 came next (the last two sections of this outcome). Not merged.

Commits, in the order of section 4. Each test group was committed on `feature/rga3--tests` and cherry-picked with `-x`; groups 1 and 2 immediately before the code they judge, each the single failing commit W2 allows, its message saying so; group 3 judges existing code and passed when committed.

| Step | Commit | What |
| --- | --- | --- |
| | `cfde578` | the branch point in this outcome |
| 1 | `60f6e74` (merge of `feature/rga3--math`: `a00b8f7`, `09bff5b`, `7155eaa`, `48e7cd6`, `c475123`, `b4287de`) | `conventions_check.py`'s rigid checks; `rigid_motions.py`; seven golden namespaces; research note 0023 |
| 2 | `1c8e350`, `691d645` | CONVENTIONS 1.12, decision 0197, trace rows at `no` |
| | `ab72934` | the `rigid_planes` fact written with commas (a golden fact's text, regenerated) |
| 3 | `8cedcaf` (from `0733aa6`) | test group 1, objects |
| 4 | `9900399`, `88c32b3`, `3d8f5d8`, `e139baf`, `6400b3f`, `8d1f883` | `objects.hpp`, `rga3.hpp`, the target and header set, `rga3.bare_compile`; the include scan and fixtures; `README.md` in `tests.citations`; trace rows; the full gcc-debug suite at the build lane (412 of 412) |
| 5 | `333769d` (from `330377a`) | test group 2, motions and eight compile-fail sources |
| 6 | `9813525`, `d44ec64`, `bf3cbbb`, `61c6b17`, `62ef7b9`, `e6a5499`, `986e80d` | `motions.hpp` and the registration of the compile-fail tests; the fifth catch-all and its test; trace rows; the pointer probes; the mutants |
| 7 | `79abffd`, `ffc4066` (from `b711850`, `ebb11ee`) | test group 3: point groups, chains, incidence, and R11's geometric test of `rotation` |
| 8 | `b6d8d04`, `27ba27a`, `593b094` | the README; its extraction check |
| 9 | `1b1fa1b` | the five rows of group 3 at `yes` |
| 10 | `554d3ba` | the model headers export the core to include-cleaner (comments only) |
| | `e18be9c` | `motions_test.cpp`: trailing commas and `std::numbers::pi`, as the lint preset asks (a test not judging its author's change) |
| | `339e154`, `797c5c6`, `c799821`, `030dbb5`, `8fdabb1`, and this commit | the diagnostics record; run rows; the cost script, its results, and the compile rows; decisions 0198 to 0201; agent costs and this outcome |

Departures from sections 1 to 10 (C20):

1. **Record numbers.** Research note 0023, not 0022 (taken by the flat-plane chain rounding note); decisions 0197 to 0201, not from 0196 (0196 holds the owner's rulings). Section 1's record-number rule allows both.
2. **Five messages, not four** (decision 0198): `translation` over scalars of two types gained a catch-all (`d44ec64`), written by the implementer and tested after it by the same author (`bf3cbbb`). `62ef7b9` adds probes that a pointer is no scalar of `translation`, which killed two surviving mutants. The texts differ from R6's drafts (`records/diagnostics/wu16-messages.md`).
3. **5.8's wording** uses Lengyel's coefficient letters in place of O5's r, u, s, h (decision 0197 item 3); it awaits the owner's confirmation.
4. **R11's test was written** (decision 0201), at k = 9 ulps, the only figure glibc 2.44's manual states; it is a testsuite ceiling, not a proved bound. The derivation is on `records/rotation-tolerance` (`3acd5d0`), since merged into `develop` (`141021a`) and from there into this branch (`038c81f`).
5. **Point-group orders and the chain form** rest on named substitutes (research note 0023, G1, G2, J1, J2): the International Tables and Denavit and Hartenberg's paper could not be read (HTTP 403).
6. **Mutants: at first only `motions.hpp` had a committed record** (`records/mutations/0023-rga3/`, 23 mutants, 17 killed on both compilers by a test, 6 equivalent survivors), and section 6's layout mutants of `objects.hpp` had been run by the test author against a scratch stand-in only (nine killed, `8cedcaf`'s message). Resolved by `05c1fa9`: 20 layout and alias mutants of `objects.hpp` run on the real header at `613cab0`, gcc-debug and clang-debug, all 20 killed on both compilers, each by a test, recorded in the same directory.
7. **The lint preset fails on two test units** at `e18be9c`: building the rga3 targets under `lint` reports 69 findings in `objects_test.cpp` and 11 in `point_groups_test.cpp` (`modernize-use-designated-initializers`, `readability-trailing-comma`, `readability-use-anyofallof`, `performance-inefficient-vector-operation`), none from `misc-include-cleaner`. The model headers, the README examples, `motions_test.cpp`, `chains_test.cpp`, and `incidence_test.cpp` are clean. Not fixed in step 10, whose brief named only `motions_test.cpp`. Resolved by `bf630e7` (`objects_test.cpp`) and `613cab0` (`point_groups_test.cpp`), style only, no expected value moved; the lint preset on the rga3 targets passed at `9b792a4`.
8. **The README examples are exempt from `bugprone-exception-escape`** under `lint` (decision 0199): a user's `main` may let `std::format_error` escape.
9. **The README** holds two examples (a composed motion over double and an exact join and meet at compile time over `int`) and checks their output through `// prints:` lines, more than R8 asked.
10. **The full suites ran twice** in each debug configuration in step 10: the first compile record was refused by decision 0080's step because a later `ctest -L '^rga3'` had recompiled the compile-fail sources, so both trees were rebuilt from empty and the suite rerun before copying.
11. **One trace-gate row** of step 9 is named `trace none convention-trace` where earlier rows say `check - trace-gate`.

Evidence (V10):

- **Verification** (V1 kinds 1 to 3): the layouts against masks and signs written in the tests from CONVENTIONS 1.6's names; 5.1 to 5.3 as `static_assert`s over `std::int64_t` and the test rational and against seeded golden joins and meets; 5.4 to 5.6 on the model's types; `translation` exactly over float, double, and the test rational; `rotation` bit for bit against l sin(θ/2) and cos(θ/2) over float and double; rotors with rational half angles against Rodrigues' matrices; the 32 point-group closures, four seeded chains of 3 to 6 joints, and eight cases each of eight incidence kinds, exactly, against values that `golden.py` writes only where the geometric route of `conventions_check.py` and the linear-algebra route of `rigid_motions.py` agree (seed 20261107). The quarter turn by `rotation` within its derived bound over float and double. Nine compile-fail tests, each one error with its message. All pass.
- **Validation** (V8): the motor and flector layouts against Lengyel's Motor and Flector pages, read directly (note 0023, M1, F1); the point-group orders and the abelian, cyclic, centrosymmetric, enantiomorphic, and polar properties against Wikipedia's table of the 32 classes, a secondary substitute (G2, G3); the chain's product and closed form against a secondary account of the Denavit-Hartenberg matrix (J2). Passed.
- **Not validated:** `rotation`'s accuracy beyond the one derived case (a quarter turn about z, from (1, 0, 0, 1)), and on any library but glibc 2.44 on x86-64 (k = 9 is that library's stated ceiling); the point-group abstract-group and property columns against any primary source (they come from Wikipedia plus computation; Hestenes prints orders only, which were read in his Table 4 and matched after rebuilding all 32 groups from his Table 2 [checked-self]; the International Tables could not be read) and the chain against Denavit and Hartenberg's paper; the elements of the trigonal and hexagonal groups in the Tables' standard setting (the [111] setting is conjugate to it); the Terathon side of CONVENTIONS section 8 (C15, C16). (The objects' layout mutants, at first listed here, ran on the real header in `05c1fa9`; departure 6.)
- **Zero core diff:** `git diff --stat develop...feature/rga3 -- include/galoot/core include/galoot/core.hpp` prints nothing at `8fdabb1` (`develop` at `88c9484`).
- **Trace gate** at `1b1fa1b`: passes; 5.1, 5.2, 5.3, 5.4 to 5.8 all `yes`; the only items with no test are 6.1 to 6.4.
- **Diagnostics** (`records/diagnostics/wu16-messages.md`, `wu16/sizes.tsv`): the five messages on the first error line on g++ 16.2.1 and clang++ 23.1.4, one error line per source; Clang prints a non-`Line` axis by its blade masks (up to 462 characters on the first line), GCC's longest line is its "required from" line (up to 1,091 characters).
- **Build cost** (`records/performance/rga3-cost/`, pinned, three repeats, under the timing lock): an includer of `core.hpp` 913.6 M instructions before and after on GCC, 974.0 M and 972.6 M on Clang (the same text; the difference is inferred to come from the extracted tree's path); an includer of `rga3.hpp` 1,396.0 M and 1,357.0 M; the test units from 3.6 G (`incidence_test.cpp`, Clang) to 12.7 G (`motions_test.cpp`, GCC), each under 1.2 s and 340 MiB. `compile.tsv` holds the 301 and 303 rows of fresh gcc-debug and clang-debug builds at `c799821`.
- **Final runs** of step 10, in new, empty gcc-debug and clang-debug trees at `c799821` (the build inputs last changed at `e18be9c`; later commits change records only), with a clean tree: build and the full suite, 434 of 434 on each; `ctest -L '^rga3'` 19 of 19; `tests.reference_markers`, `tests.golden_facts`, and `tests.citations` 3 of 3; `format-check` exit 0, on each. The lint preset on the rga3 targets only (departure 7). Not run here, left to the merge into `develop` (P9, section 7): gcc-release, clang-release, the sanitizer presets, the full `lint` preset, `clang-measure`'s full suite, `tools/runlog check`. The job pool read 23 of 23 slots free before and after each heavy run.

Listed for review (W9): the cherry-picked tests `8cedcaf`, `333769d`, `79abffd` (which also adds R11's test to `motions_test.cpp` after the code it judges); code commits that also register or add tests, `9900399` (`rga3.bare_compile` and `bare_user.cpp`), `88c32b3` (the include-scan fixtures), `9813525` (the registration of group 2's compile-fail tests), `27ba27a` (the README check and its fixtures, after the README of `b6d8d04`); tests written after the code they judge by its author, `bf3cbbb` (of `d44ec64`) and `62ef7b9`; the golden fact's text, `ab72934`; and `e18be9c`, a style-only change to a test. Added after step 10: `bf630e7` and `613cab0`, style-only changes to `objects_test.cpp` and `point_groups_test.cpp` for the lint preset; `05c1fa9`, the objects' mutants (a record and `mutate.py`, no test changed); the hci-resolution commits `2a2db45` (test literals, committed before the header change), `62fd480` (the header), `dd1f2a9` (a file mode), `d128394` (comments), and `fad0c10` (the README, with two `static_assert`s in a compiled example); and `f008617`, the blind review's record (cherry-picked from `86eefdd`).

Open, for the owner or the next step: the 5.8 wording (decision 0197 item 2); WU15 before M4's gate (O1). Departures 4, 6, and 7, and the hci-expert reading and the blind review of section 8, listed here at step 10, are done (departures 4, 6, 7 above; the two sections below).

### The hci-expert reading and its fixes

The hci-expert reading of section 8 (`reviews/2026-11-07-wu16-hci-reading.md`, cherry-picked as `e0d260b` from `records/review-wu16-hci`) found nothing blocking: seven findings to act on (S1 to S7), three of them owner questions, and seven that could wait (C1 to C7). Resolved on `feature/rga3` by an implementer (Claude, agent, Opus 5.5, cpp-expert and hci-expert judgement):

| Finding | Resolution | Commits |
| --- | --- | --- |
| S5, S6, S7: three of `rotation`'s messages | Reworded as the reading proposed; the axis catch-all asserts `false` with no condition, so Clang's first line falls from 462 to 180 characters; its two tests also forbid `blade_set` on the first error line. One error, message on the first error line, on both compilers | `2a2db45` (tests, failing alone), `62fd480`, `9b792a4` (the diagnostics record) |
| S1, documentation half | `rotation`'s comment and the README say the axis needs a direction of length 1, how to unitize a join, and what a direction of length L does (a turn by 2 atan(L tan(θ/2)), weights times L² sin²(θ/2) + cos²(θ/2), checked numerically) | `d128394`, `fad0c10` |
| S2, README half; S4 | The README explains homogeneous points, e4 as the generator often called e0, the braces of `Motor{compose(a, b)}`, and the order and halves of a motor's coefficients, with two integer motors checked by `static_assert` in example 2 | `fad0c10` |

Also: `mutate.py` made executable (`dd1f2a9`); the three axis mutants run again on both presets after the assertion lost its condition, with the same results (`f2fca74`).

Not resolved, for the owner: S1's behavior (normalize the axis in `rotation`, or keep the documented precondition); S2's core half (whether `compose` of two operands of one layout returns that layout); S3 (`transform(motor, point)` compiles and returns a motor; the reading's fix is documentation, held back with S1 and S2). Not done, could wait: C1 to C7.

Listed for review (W9): `2a2db45`, the changed literals of the six rotation compile-fail tests and the new `blade_set` guard, committed before the header change; `fad0c10`, which adds two `static_assert`s to README example 2, a compiled test, in the same commit as the text they illustrate.

Checks at `9b792a4`, on gcc-debug and clang-debug: build, `ctest -L '^rga3'` 19 of 19, `tests.*` 5 of 5, `format-check`; the trace gate passes; the lint preset on the rga3 targets passes. The job pool read 23 of 23 slots free before and after.

### The blind review and its resolution

The blind review of section 8 (`reviews/2026-11-07-wu16-review.md`, blind-reviewer role, Claude, agent, Opus 5.5, with the usability checklist; cherry-picked as `f008617` from `86eefdd` on `records/review-wu16`, which is not merged) reviewed `feature/rga3` at `6f47dbe` against `develop` at `141021a`. Outcome: approve, no blocking or major findings, two minor and two for information. Resolved on `feature/rga3` by the coordinator (Claude, agent, Opus 5.5). Every fix changes comments or records only, so none needs a delta review (W5, decision 0183); each is listed here with its resolution.

| Finding | Resolution | Commits |
| --- | --- | --- |
| 1, minor: this outcome stale (departures 6 and 7, "Not validated", "Open", and the W9 list) | Brought up to date: departure 6 records the 20 layout and alias mutants of `05c1fa9`, all killed on both compilers; departure 7 the lint fixes `bf630e7` and `613cab0`; departure 4 the merge of `records/rotation-tolerance` into `develop` (`141021a`) and into this branch (`038c81f`); "Not validated" and "Open" no longer list them; the W9 list adds `bf630e7`, `613cab0`, `05c1fa9`, the hci-resolution commits `2a2db45`, `62fd480`, `dd1f2a9`, `d128394`, `fad0c10`, and `f008617` | `ebfe9ca` |
| 2, minor: the meet signs of `rigid_motions.py` matched, not derived | Stated in `rigid_motions.py`'s docstring and as open item 5 of research note 0023: the signs of the line–plane, plane–plane, and three-plane meets were chosen to match the library's convention and are checked only up to scale by `planes_meet`, so a sign error shared by both routes would pass; the incidence zeros are unaffected, and the plane's orientation is pinned by `self_check`. No logic or golden value changed (`golden.py --check` matches) | `1ecd03f` |
| 3, info: `objects.hpp`'s head comment lacks the `std::ranges` caveat | Added in the wording of `transform.hpp` and the README | `55316b9` |
| 4, info: C13 holds by inspection only; the include scan checks includes, not the use of `core::detail` | Recorded, no code: a possible later check is a scan of the model headers (and of any later model) for the token `detail` qualified by `core`, run with the include scan. Not in this unit's scope (section 12) | none |

The usability items the review lists as not met (S1's behavior, S2's core half, S3, GCC's long `mv` types) are the owner items of the hci reading's section above and of O2; nothing new.

### Checks for the merge into `develop`

Section 7's checks (P9), at `ebfe9ca`, the last commit that changes a build input or a checked file (later commits add run rows and this record only), after merging `develop` at `141021a` (already contained: nothing to merge), on a clean tree, each preset from a new, empty build tree, every run through `tools/timed` as `gate:<selection>`. The job pool read 23 of 23 slots free before and after each preset.

| Preset | Full suite | Build, test wall (s) |
| --- | --- | --- |
| gcc-debug | 434 of 434 | 33.0, 7.0 |
| gcc-release | 434 of 434 | 40.8, 6.7 |
| clang-debug | 434 of 434 | 27.0, 4.5 |
| clang-release | 434 of 434 | 45.3, 4.2 |
| gcc-asan | 426 of 426 | 139.5, 3.9 |
| clang-asan | 434 of 434 | 122.6, 4.2 |
| lint | 434 of 434 (clang-tidy clean) | 161.0, 4.1 |
| clang-measure | 426 of 426 | 46.8, 3.7 |

The two counts of 426 are as `test/CMakeLists.txt` registers the tests: the eight `core.plan_cost_model` cases need `_GLIBCXX_ASSERTIONS`, which `clang-measure` does not define, and on GCC are registered only without the sanitizers, so `gcc-asan` lacks them. (The GCC and Clang sets of 434 differ too: `core.plan_evaluated_once` and `core.plan_map_evaluated_once` are Clang only, and two `bfloat16` cases of the transform rounding check GCC only.) `format-check` exit 0; the trace gate passes (items with no test: 6.1 to 6.4); `tools/runlog check` reports no removed row, so no prune. About 662 s of wall time in all. Deferred to the M4 gate: the compile-record copy of decision 0080.

