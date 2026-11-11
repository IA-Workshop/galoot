# Claim trace of the documentation pages

- Unit: plan 0041, `--pages` part (section 4.1: "the author keeps a trace (claim, page, source line) in the mutation record directory, not in the page"). By Claude (agent, Opus 5.5), hci-expert role, on `feature/documentation-lane--pages`.
- Sources: header lines at the branch point `818eb4c`, where no file under `include/` differs from `77cd947`; CONVENTIONS at version 1.17; the build files and presets at the branch point. "Probe" marks a claim checked by a scratch compile of this run (logged in the author's scratch directory, not the run log), and "Example" a claim that a compiled example of the same page also shows.
- Scope: the factual claims a page makes in prose that no example of it shows, one row each, plus a few that an example shows only in part. Restatements of a convention in words (S11) cite the convention.

## docs/guide/README.md (the first page)

| Claim | Source |
| --- | --- |
| A product's result is on the canonical blades it reaches, orientation +1 | `product.hpp` 25-29; CONVENTIONS 1.7 |
| `const Motor m = compose(b, a);` does not compile; conversions are explicit | `mv.hpp` 604-613 (`explicit`, line 618); Probe d.cpp, GCC and Clang |
| `2 * line` over `double` is rejected with a message that says 2.0; `a * s`, `a / s` alike | `linear.hpp` 44-49, 438-478; Example (rejected block) |
| `translation(1.0, 0, 2)` and `rotation(line, 1)` are rejected | `motions.hpp` 178-189, 113-124; Probe e.cpp, f.cpp |
| Integer literals are accepted in braces for a floating scalar | `mv.hpp` 593-601 |
| `%` is the antiproduct, its identity is 𝟙; `compose(b, a)` is `b % a` | `product.hpp` 328-332; `transform.hpp` 645; Example |
| `!x` is a multivector; `if (!x)` does not compile; `!!x` is x times (−1)^(gr ag); GCC warns on `!x == y` | `unary.hpp` 120-123; Probe c.cpp |
| `p ^ q == l` means `p ^ (q == l)` and does not compile | C++ precedence; `product.hpp` 32; Probe b.cpp |
| Values of different layouts have no `==`, so `(p ^ q) == l` for a `Line` l does not compile either | `mv.hpp` 502; Probe a.cpp (the plan's 4.3 item 4 text "Write `(p ^ q) == l`" holds only for an l of the join's layout) |
| `get<"e14">(line)` is rejected with the message to negate | `mv.hpp` 1274-1285, 1337-1340; Example (rejected block) |
| On a canonical layout the name with increasing indices reads it; the other order gets its own message | `mv.hpp` 1278-1279, 1332-1336; Probe g.cpp |
| Up to 16 generators; a larger `MaxN` up to 32, untested | `metric.hpp` 51-55, 168-170; `bits.hpp` 37-38 |
| Names, `get`, printing up to 9 generators; masks and positions above | `blade_set.hpp` 81-84; `mv.hpp` 1281-1283; `format.hpp` 174 |
| On Clang the scalar times full<8> fails at the default steps | `plan.hpp` 624-625; `mv.hpp` 891-892 |
| `transform` multiplies the weight by λ; no `unitize` in this release | `transform.hpp` 41-45, 694-695; CONVENTIONS 4.4; plan 0001 section 2 (`unitize` waits) |
| Printing needs `format.hpp`, which the umbrellas leave out because `<format>` is costly | `format.hpp` 26-27; `core.hpp` 8-19 (no `format.hpp`) |
| Unqualified `transform` and `reverse` under `using namespace std::ranges` or `std::views` do not compile | `transform.hpp` 59-64; `unary.hpp` 64-69; `objects.hpp` 30-33 |
| `translation` must be qualified | `motions.hpp` 17-19, 162-164 |

## docs/guide/core.md

| Claim | Source |
| --- | --- |
| Tag with `static constexpr` member `value`; the tag's name appears in diagnostics | `metric.hpp` 7-16, 291-299 |
| The four factories and their conventions | `metric.hpp` 164-173, 195-199, 205-213, 233-243; CONVENTIONS 1.2-1.4 |
| `from_gram` entries within `max_form_entry`, symmetric, checked with a message | `metric.hpp` 239-242 |
| A tag in a header goes in a named namespace | decision 0229 item 1, as the README said before the move |
| A coefficient on s e_A multiplies s e_A; mask bit i − 1 is e_i; 0b1011 is e124 | `blade_set.hpp` 4-13; CONVENTIONS 1.1, 1.6 |
| `blades` rules (1, digits, n ≤ 9, no repeats, each checked with a message) | `blade_set.hpp` 619-636 |
| Canonical order, e14 before e23 | `blade_set.hpp` 21-25, 90-95; CONVENTIONS 1.7 |
| `merge` keeps signs; opposite signs rejected | `blade_set.hpp` 593-599 |
| A layout keeps no algebra | `mv.hpp` 483-485; `blade_set.hpp` 625-626 |
| `mv` is trivially copyable, sized blades × sizeof(T), never allocates; `coefficients` public | `mv.hpp` 494-498, 386 |
| Wrong count is one error printing the counts | `mv.hpp` 662-667; STYLE S8; Example (rejected block, the literal) |
| Braces reject narrowing variables; parentheses may narrow; one-blade constructor explicit | `mv.hpp` 593-601 |
| `==` is exact, IEEE for floats; different layouts do not compare; no output operator | `mv.hpp` 499-505 |
| `get` returns a reference; rvalue on a temporary; found by ADL; each rejection has its own message | `mv.hpp` 1269-1289 |
| Conversions explicit, negate on opposite orientation; other algebra or scalar rejected | `mv.hpp` 604-613, 640-646 |
| `truncate` checks exact scalars only, never rounding; `part` never checks; neither rounds | `mv.hpp` 1029-1050, 1094-1111; CONVENTIONS 4.5 |
| `ring_scalar` and `exact_scalar` requirements | `mv.hpp` 90-115 |
| Strict scalar rule for scaling; `2.5 * x` over int64 rejected | `linear.hpp` 44-49 |
| Product coefficients exact; a scalar that cannot hold one is rejected | `product.hpp` 225-229 |
| `x / s` uses the scalar's division, never a reciprocal; int truncates toward zero; zero divisor checked | `linear.hpp` 383-396 |
| Definitions of the four products, metric-independence of the wedge and antiwedge, the meet | `product.hpp` 220-223, 260-261, 328-332, 390-394; CONVENTIONS 4.1, 4.2 |
| Associativity and De Morgan laws | CONVENTIONS 4.2 |
| Mismatched operands rejected with the product named as written | `product.hpp` 35-40, 81-85 |
| Operator table and precedence, `%` binds as `*` | CONVENTIONS 7.1; `product.hpp` 32-33, 416; `unary.hpp` 120-123 |
| No operator for the antireverse and the rest | CONVENTIONS 7.1 |
| Same-type sums keep the type; different layouts give canonical; no number in a sum | `linear.hpp` 27-42; Example (rejected block) |
| `s * x` left, `x * s` right; `-x` makes −0.0; one scalar operation per coefficient | `linear.hpp` 54-56, 353-355, 369-370, 426-428 |
| Involution signs; complement definitions; left = (−1)^(gr ag) right; double complement | `unary.hpp` 61-107; `bits.hpp` 126-147; CONVENTIONS 2.1, 3.1 |
| Complements read only n | `unary.hpp` 28-30 |
| Bulk, weight, and dual definitions; rigid(3) split; other metrics | `dual.hpp` 25-58; CONVENTIONS 3.2, 3.3 |
| Double dual zero in a degenerate metric | CONVENTIONS 3.4; `dual.hpp` 433-434 |
| Duals and the bulk and weight check nothing | `dual.hpp` 71-73 |
| Lengyel's dot; squared norms; empty layout when nothing pairs | `dual.hpp` 520-533, 570-575, 65-68 |
| Projection and antiprojection definitions and rigid(3) meaning; w² factor | `dual.hpp` 607-613, 646-653; CONVENTIONS 4.3 |
| Parity, even and odd forms, n even or odd | `transform.hpp` 24-40; CONVENTIONS 4.4 |
| Outermorphism with λ^(k−1); no unitize | `transform.hpp` 41-45; CONVENTIONS 4.4 |
| `compose` order and closure | `transform.hpp` 645-658; CONVENTIONS 4.4, 5.8 |
| Mixed parity rejected | `transform.hpp` 772-776; Example (rejected block) |
| Checking mode: macro, default 0, constant evaluation always, abort message form, no exceptions | `assert.hpp` 6-9, 25-31, 42-45, 61-64 |
| The list of what is checked | `mv.hpp` 1040-1044; `transform.hpp` 704-718, 720-735; `linear.hpp` 392-396; `blade_product.hpp` 48-57; `motions.hpp` 63-68 |
| `-ffast-math` and `-ffinite-math-only` checking builds do not compile | `transform.hpp` 100-113 |
| `GALOOT_ASSERT` message rules | `assert.hpp` 55-64 |
| Printing format, names member, canonical names with `-`, −0 prints as -0, width per coefficient, no printing above 9 | `format.hpp` 147-180; CONVENTIONS 7.2 |
| Derived classes need the base's formatter | `format.hpp` 182-187 |
| Change of basis: tag, rows over denominators, checks at first use, inverse computed | `basis_change.hpp` 35-49, 144-160, 603-623; CONVENTIONS 1.8 |
| No `==` on `basis_change`; `same_rows` compares as written | `basis_change.hpp` 154-156, 180-183 |
| Integer scalar only where no division; double rounds once per sum and division | `basis_change.hpp` 93-101, 1149-1158 |
| `conformal_change` maps and determinant −1 | `basis_change.hpp` 222-227; CONVENTIONS 1.4 |
| What a change preserves, with factors; bulk −e5 against −e5/2 | `basis_change.hpp` 51-81; CONVENTIONS 1.8 |
| ADL, the three cases needing a namespace, `std::execution::bulk` | `dual.hpp` 97-102; `mv.hpp` 1102-1106; `transform.hpp` 59-64; `unary.hpp` 64-69; `motions.hpp` 17-19 |
| Rejecting overloads answer yes to a requires-expression; `has_name`; truncate's deduced type | `product.hpp` 37-40; `linear.hpp` 438-442; `mv.hpp` 1077-1080, 1283-1285, 1252-1263 |
| Which results keep the input's layout | `unary.hpp` 18-20; `linear.hpp` 27-29; `transform.hpp` 47; `mv.hpp` 1029 |
| Reference entries | each entry's `///` comment, at the lines listed in the inventory's draft and above |

## docs/guide/rga3.md

| Claim | Source |
| --- | --- |
| R(3, 0, 1), names, e41 = −e14, antiscalar e1234, the tag names the blades for printing | `objects.hpp` 4-23, 41-62 |
| Point, line, plane layouts and meanings; join direction and moment | `objects.hpp` 64-86; CONVENTIONS 5.1-5.3 |
| Motor layout, transform form, half angles and distances | `objects.hpp` 88-100; CONVENTIONS 5.4, 5.5, 5.8 |
| Flector layout; a plane written as a flector reflects in it | `objects.hpp` 102-114; CONVENTIONS 5.6, 5.8, section 8 |
| Lengyel's Flector page writes the single form; the preset uses the grade involution | CONVENTIONS section 8, "Flector transform" |
| A negative weight is the same point | CONVENTIONS 5.1 (homogeneous point) |
| `rotation`: full angle, ccw, any positive length, not checked to be a line, zero direction checked, floating only, exact angle type, run time only | `motions.hpp` 28-75, 92-124; CONVENTIONS 5.7 |
| `translation`: halves, a point of weight w moves by w d, one type, integers rejected, unitized, qualified | `motions.hpp` 146-189; CONVENTIONS 5.7 |
| Incidence by join and meet | `objects.hpp` 78-80; `product.hpp` 393-394; Example |
| No `unitize`, norms, exp, or log | plan 0001 section 2; `include/` has none |

## docs/guide/limits.md

| Claim | Source |
| --- | --- |
| Generator bound, `MaxN`, the three rejections, untested above 16 | `metric.hpp` 51-55, 140-153, 168-170 |
| Identities on sparse layouts of 8, 12, 16 generators in four signatures | `test/core/identities_sparse_test.cpp` line 1 and its forms, as plan 0041 section 1 item 8 reads them |
| Names to 9; each name-reading function rejects above 9 | `blade_set.hpp` 81-84, 504-506; `mv.hpp` 1281-1283, 1311-1314; `format.hpp` 221-223; `has_name` returns false (`mv.hpp` 1258-1260), which the page groups with the rejections: a defect of wording to fix, below |
| `max_form_entry` definition, 32,767 | `metric.hpp` 57-62; Example (derived from `INT_MAX`) |
| `coefficient_type` overflow message and behavior | `blade_product.hpp` 48-57, 75, 94 |
| Products and plans: every figure | `plan.hpp` 615-667 (the region the core pass removes, record `limits-removed.md`); `product.hpp` 44-52, 337-360; `mv.hpp` 486-493, 883-905, 951-954 |
| Steps and operations of each plan | `plan.hpp` 645-667, 974-981; checked by `cost.py comments --section` against the figures files, 0 unknown counts |
| Blade sets and sums | `blade_set.hpp` 574-577, 586-588, 600-605; `linear.hpp` 58-63, 102-114 |
| Duals | `dual.hpp` 75-95 |
| Transform and compose | `transform.hpp` 66-82 |
| Change of basis | `basis_change.hpp` 103-124 |
| Raising the limits: defaults and flags | `plan.hpp` 625-640; `product.hpp` 354; `plan.hpp` 621-623 |
| Rounding check: bound, padding, types checked, floor, assumptions, chains | `transform.hpp` 720-766, 100-113, 416-419, 545-548; CONVENTIONS 4.5 |
| Contraction, L1 and L2 | `records/numerical/0040-contraction-check.md` section 7.2, verbatim, at the coordinator's instruction (decision 0243) |
| Presets build baseline x86-64 with `-ffp-contract=off` | `CMakePresets.json` 46, 54, 62 (no `-march` in any preset) |
| Toolchains; bare and `-fno-exceptions` compiles; CMake 4.4 | `CMakePresets.json`; the tests `core.bare_compile*`, `rga3.bare_compile*`; `CMakeLists.txt` 5 |
| rga3 objects far below the limits; transform drops zero in rigid(3) for the five objects | `transform.hpp` 704-707, 754-756; `product.hpp` 46-47 |
| `rotation` length range | `motions.hpp` 52-56 |

## README.md

| Claim | Source |
| --- | --- |
| Status: core close to a stable release, preset versioned separately and below its first stable release; no version | plan 0041 section 4.2; decision 0240 items 3 and 5 |
| No `install()`, no `find_package`; CMake 4.4 | decision 0240 item 5; decision 0241 item 5; `CMakeLists.txt` 5 |
| The target carries the include path and C++23, and Galoot's tests are not built as a subproject | `CMakeLists.txt` 16-35, 41-49 (`PROJECT_IS_TOP_LEVEL`) |
| Compiles with `-fno-exceptions` | `core.bare_compile_no_exceptions`, `rga3.bare_compile_no_exceptions` |

## Defects of wording found while tracing

- limits.md, "Blade names": `has_name` is listed among the functions "rejected with a message" above 9 generators, but it answers false and never errs. Fixed in the page in the same commit as this trace.
