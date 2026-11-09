# hci-expert reading: WU12 (plan 0030), the change of basis on `feature/core-basis-change` at `37f1729`

- Reader: hci-expert, Claude Opus 5.5 (`claude-opus-5-5`), high effort, 2026-11-09. Agent work, reviewer role. This reading is advice only and independent of the designer (decision 0220), the test author, and the implementer (decision 0221). Mathematical correctness is the math-expert's and the blind reviewer's job, not mine.
- Read as: a C++23 user who knows geometric algebra, in particular the conformal model and Witt bases, and has not seen Galoot's internals.
- Subject: `include/galoot/core/basis_change.hpp` at `37f1729`, decision 0220 (the interface) with the owner's rulings in decision 0222, decision 0221, the 17 change-of-basis messages in `records/diagnostics/catalogue/scan-output.md` and `misuse-catalogue.md`, and the compile-fail sources `test/core/basis_change_*_fail.cpp` and `test/core/change_basis*_fail.cpp` with their registrations in `test/CMakeLists.txt` (lines 2309 to 2388). I did not edit the unit's branch.
- Work: branch `records/wu12-hci-reading` from `develop` at `2b733f4`, worktree `.claude/worktrees/wu12-hci-reading`. I wrote the programs and ran them in the session's scratch directory, with the unit's `include/` on the include path.
- Compilers: g++ 16.2.1 and clang++ 23.1.4 with libstdc++ 16, using the test build's flags: `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS -O0 -fno-diagnostics-color`. The two task programs were linked and run. The mistake cases were compiled with `-fsyntax-only`.
- Verdict: **nothing blocks the merge.**
  - The header's example and both of my task programs compile and run on both compilers, and they print what the comments say.
  - Of the 17 library messages, I reached the 13 that a user can hit by plain mistakes. Each came out as one error with the library's sentence on the first error line, on both compilers, except in the two cases below.
  - Should-fix S1: a tag with the wrong `D`, as in `conformal_change<2>()` between cga3 and R(4, 1, 0), gives two errors. The source's check misses and so does the target's. The code's own comment promises one error per tag.
  - Should-fix S2: the operand message for `change_basis` advises giving x "the source's algebra tag". For the usual cause, a value already in the target, following that advice gives a wrong answer with no error.
  - Two message rewordings, three comment edits, and a catch-all for a value passed as the template argument should also be done (S3 to S7). Ten notes follow.
- Job pool: running, 23 of 23 slots free before my first compile and after my last. I neither started nor stopped it.

## Departures, skipped checks, and ambiguities (C20)

1. **Programs and run rows are not committed.** The brief limits my commits to this record. Section 3 describes each case well enough to rewrite it, and the scratch paths are in my report. I ran 79 compiles through `tools/timed --log` into a scratch log (selection `hci-wu12:<case>:<compiler>`, step `probe`, preset `-`). They took 15.4 s of wall time in all, none longer than 1.04 s, with a peak resident set of 230,620 KiB. Because `tools/timed` ran from the main checkout, the rows' revision and branch columns read `2b733f4` and `develop`, but every compile used the unit's headers at `37f1729`. The rows are not in `runs.tsv`.
2. **No CMake build or CTest ran.** I read the compile-fail tests and their registrations, and I did not run them. The catalogue scan, the mutants, and the test runs belong to the authors and the blind reviewer.
3. **Four messages were not reached by my own programs.** The width check (`basis_change: n is larger than ...`) needs 33 rows, and the layout checks G1 and G2 need a hand-written blade set given to `change_basis_layout` directly. I read those three in their tests and did not compile them myself. The narrow-scalar message U3 needs a coefficient above 2^24 under `float`, and I read it in its test too. The two self-check messages were left alone because no user input reaches them.
4. **Task first was not pure.** The brief had me read the header and decision 0220 before I wrote the programs, so my first-try code was informed by the header's example. The mistakes in section 3 are the ones I judge a newcomer likely to make. Nobody was observed.
5. **One proposal was prototyped outside the repository.** S6's catch-all was added in a scratch translation unit by reopening `galoot::core`, not in the header. A cpp-expert should confirm it before it is adopted.
6. **The names were already confirmed by the owner** (decision 0222 item 1). Where I question a name (N1), I do not ask for a change. I put the cost on record and list it as an owner item.
7. **Rewordings change pinned literals.** S2 and S3 change messages that `test/CMakeLists.txt` pins: `not_isometry` at line 2341, and `not_in_source` and `not_in_target` at lines 2368 and 2369. Each rewording needs its test commit before its header commit (W9). S1, S4, S5, and S7 change no pinned text. S6 adds two messages and needs two new tests.
8. **Knowledge base.** I commit nothing there, per the brief. I wrote no note there. Two candidates are listed at the end.

## 1. Task first

1. **Take a conformal(3) point to R(4, 1, 0) and back** (`t1_conformal.cpp`, run on both compilers with the same output). I followed the header's example. The unqualified call `change_basis<cga3_to_r410>(p)` works by argument-dependent lookup, as decision 0220 says. The point (1, 2, 3, 1, 7) printed as `{e1: 1, e2: 2, e3: 3, e4: 6.5, e5: 7.5}` in R(4, 1, 0), and it came back equal. The origin went to `{..., e4: -0.5, e5: 0.5}` and came back as `{..., e4: 1, e5: 0}`, as the header says it does. Naming the result type as `mv<r410, change_basis_layout<cga3_to_r410, point_layout>, double>` worked the first time.
   - Surprise: the example wraps the round trip in `truncate<Point>`, but for the point's layout the round trip already returns exactly `Point` (`m27_same_type`, a `static_assert` on `std::is_same_v`). Writing `Point p2 = change_basis_back<cga3_to_r410>(q);` compiles. `truncate` is needed only when the image grows, as it does for the origin alone (S4).
   - Surprise: the target's values print with the names e1 to e5, the same names as the source's. The header writes them e'1 to e'5. While debugging a round trip I could not tell from the printed values which algebra a value was in (N8).
2. **Compute in the diagonal basis and bring the result back.** `q * q` in R(4, 1, 0), mapped back, equaled `p * p` in conformal(3). Nothing in the interface got in the way.
3. **Define a Witt change of my own** (`t2_witt.cpp`, two fermion modes, run on both compilers with the same output). The form had to be scaled by 2, `from_gram({{0, 1, 0, 0}, {1, 0, 0, 0}, ...})`, because a form's entries are integers. Nothing in `basis_change.hpp` says so; I learned it from decision 0219 (N4). The rows, e1 = e'1 + e'3 and e2 = (e'1 − e'3) / 2 in R(2, 2, 0), had to be worked out by hand: the library checks a change, and it does not find one (N5). With the rows in hand, `from_rows(rows, {1, 2, 1, 2})` worked on the first try. The anticommutator f1 f1† + f1† f1 came out as 2, the same in both bases, and the antiscalar went to −1 times the target's.
   - Needed a second look: which way the rows go. The comment says it clearly, "row i writes the source's generator e_i in the target's generators". But many linear algebra texts put the new basis vectors in the columns of the change-of-basis matrix. When I wrote the matrix the column way on purpose (`m16_transposed`), the isometry message caught it and told me to "check the rows", not which way they go (S3).
4. **Make the mistakes the messages exist for**, plus some that have no message (section 3).

## 2. The checklist (`api-design/interface-review-checklist.md`), in brief

| Item | Verdict | Evidence |
| --- | --- | --- |
| Field's terms, full words | Met | change of basis, source, target, rows, denominators; "congruent over the rationals" at line 5 is standard linear algebra |
| One meaning; no std clash | Partly | `basis_change` and `change_basis` differ only in word order (N1); no standard clash |
| Similar things look alike | Met | `from_rows` beside `from_gram`, `conformal_change<D>()` beside `conformal<D>()`, the tag beside an algebra tag, `_layout` names |
| Shortest form is correct | Met, with one nuance | the example's `truncate` is not needed for its own layout (S4) |
| Lossy conversions named | Met | `Point{...}` from a grown round trip is rejected with "mv: the conversion drops blades of its input; ... truncate<Type>(x)" (`m26`) |
| Compile-fail test per catalogued misuse, both compilers | Met (read, not run) | 17 messages, 15 pinned in both listings, 2 self-checks listed as internal |
| One error, message on the first error line | Met except S1 and the overflow | wrong `D` gives two (S1); overflow gives 6 on GCC and 9 on Clang by design (N7) |
| Operation first, mistake, fix | Mostly met | `basis_change:` prefix for tag faults, as ruled (decision 0222 item 3); G1 and G2 start with "basis change map:", which is no public name (N6) |
| The fix is safe to follow | Not met for U2 | "give x the source's algebra tag" (S2) |
| Context lines in the user's terms | Met on Clang, known on GCC | Clang names the tag (`make_checked_change<c>`) and the call; GCC's chain spells blade sets in full, with lines up to about 770 characters |
| Distinct causes, distinct messages | Met | the merge of an integer scalar and a scalar without `/` was ruled (decision 0222 item 4) |
| Probes documented | Met | each catch-all's `///` says a requires-expression is told yes (`m24` confirms) |
| Assumptions stated | Mostly met | that integer acceptance depends on direction is left unsaid (N3) |
| Examples compile | Met | the header's example (also tested at `404f2fc`) and `from_rows`'s Witt example |
| Maintainer finds each behavior | Met | tag rules in `detail::make_checked_change`, scalar rules in the two functions, generation in `detail::make_change_map` |

## 3. The programs

### 3.1 Mistakes the library catches

The first error line, with the path shortened. "1 / 1" means one error on GCC and one on Clang.

| Case | Mistake, as written | Errors | First error line |
| --- | --- | --- | --- |
| `m03` | `change_basis<r410>(p)`, an algebra tag | 1 / 1 | `change_basis: the template argument is not a change of basis; give a tag with source, target, and value, as in struct cga3_to_r410 { ... }` |
| `m04` | a tag with `using from = ...; using to = ...;` | 1 / 1 | the same |
| `m18` | `static const basis_change<5> value;`, not constexpr | 1 / 1 | the same |
| `m28` | a tag whose `value` is `R<4, 1, 0>()`, a metric | 1 / 1 | the same |
| `m05` | `value = conformal_change<3>;`, no parentheses | 2 / 3 | the compiler's "constant evaluation returns address of immediate function" (GCC) or "constexpr variable 'value' must be initialized by a constant expression" (Clang), at the user's line, then the library's message. Good enough: the first line points at the typo |
| `m06` | `conformal_change<2>()` between cga3 and R(4, 1, 0) | **2 / 2** | the source message, then the target message, each "(5 == 4)" (S1) |
| `m22` | `source = rga3` with `conformal_change<3>()` | 1 / 1 | `basis_change: the source algebra has a different number of generators than the change has rows; ...` with "(4 == 5)" |
| `m07` | source and target swapped | 1 / 1 | `basis_change: the rows do not carry the target's form to the source's (P G' P^T is not G); check the rows, and that source and target are not swapped` |
| `m16` | the Witt matrix written by columns | 1 / 1 | the same (S3) |
| `m17` | a denominator of −2 | 1 / 1 | `basis_change: every denominator must be positive; put a row's sign in its entries` |
| `m21` | dependent rows `{{1, 2}, {2, 4}}` | 1 / 1 | `basis_change: the rows are linearly dependent, so the change has no inverse; ...` |
| `m29` | one bad tag used three times in a file | 1 / 1 | the isometry message once; the per-tag check runs once |
| `m10` | `change_basis` applied to its own result | 1 / 1 | `change_basis: x is not in the change's source algebra; give x the source's algebra tag, or use change_basis_back to map from the target` (S2) |
| `m19` | x in a third algebra | 1 / 1 | the same |
| `m20` | `change_basis_back` on a source value | 1 / 1 | the `change_basis_back` counterpart |
| `m11`, `m12` | `int64_t` on the origin; `int` on the whole point | 1 / 1 | `change_basis: the change divides by an integer on this layout, which the scalar type cannot do exactly; use a rational or floating scalar type` |
| `m31` | `int` on e5: forward compiles, back is rejected | 1 / 1 | the `change_basis_back` counterpart (N3) |
| `m25` | `O o2 = change_basis_back<C>(change_basis<C>(o))` for the origin layout | 1 / 1 | the compiler's "conversion from ‘mv<...>’ to non-scalar type" (GCC) and "no viable conversion" (Clang), the core's usual behavior for implicit conversion |
| `m26` | `O{...}` from the same | 1 / 1 | `mv: the conversion drops blades of its input; it is lossy, so request it with truncate<Type>(x)`, which teaches exactly the fix |
| `m35` | the overflow test's source | 6 / 9 | first line from the core's assertion machinery; the message is in the quoted source line, 37 lines into GCC's 102 and 26 into Clang's 145 (N7) |
| `m36` | `conformal_change<6>` on `full<8>` under Clang, past the stated limit | 3 (Clang) | "constexpr variable 'found' must be initialized by a constant expression" ... "constexpr evaluation hit maximum step limit", as the header says, naming no change |

### 3.2 Mistakes only the compiler catches

| Case | Mistake | Errors | What the user reads |
| --- | --- | --- | --- |
| `m01` | `core::basis_change<cga3_to_r410>(p)`, the near-homonym | 1 / 1 | GCC: "type/value mismatch at argument 1 in template parameter list for ‘template<long unsigned int N> struct galoot::core::basis_change’ ... expected a constant of type ‘long unsigned int’, got ‘cga3_to_r410’". Clang: "template argument for non-type template parameter must be an expression", with a note at `template <std::size_t N>`. Neither says `change_basis` (N1) |
| `m02` | `change_basis<conformal_change<3>()>(p)`, the value instead of a tag | 4 / 1 | GCC: "no matching function for call to ‘change_basis<galoot::core::conformal_change<3>()>(const Point&)’" with three nested "type/value mismatch" errors; Clang: "no matching function" with three "must be a type" notes (S6) |
| `m13` | `from_rows(rows)` without denominators | 1 / 1 | "candidate expects 2 arguments, 1 provided"; clear |
| `m14` | `from_rows({{1, 1}, {0.5, -0.5}}, ...)` | 2 / 4 | narrowing from `double` to `entry_type` (aka `int`); clear enough, and the `///` says why the entries are integers |
| `m15` | 2 rows of 3 entries | 1 / 1 | "no matching function for call to ‘from_rows’"; the candidate shows `[N][N]` |

### 3.3 Accepted, as documented

`m23` (`float` on the point's layout), `m30` (`int` on e1, e2, e3, e12, checked by a `static_assert` at compile time), `m33` (a bad tag that is never used compiles, as `basis_change_like`'s comment says), `m34` (`basis_change_like` holds for that bad tag and fails for an algebra tag), and `m24` (a requires-expression says yes for `change_basis<r410>(p)`, as the catch-all's `///` warns).

## 4. Findings

Should-fix findings are ranked by the number of users they reach and the cost of changing them later. Every one of them is cheap now.

### Should-fix

**S1. A tag with the wrong `D` gives two errors, and the code says one.** `basis_change.hpp:548` and `:552` assert the source's and the target's counts unconditionally. With `conformal_change<2>()` between conformal(3) and R(4, 1, 0), the most likely way to get a count wrong, both fire on both compilers (`m06`). The comment at lines 539 to 541 says "Each fault stops the checks after it, so a tag gives one error", and decision 0220 section 3.4 says "a tag with several faults gives one error". The test `core.basis_change_source_count` has only the source wrong, so its `ONE_ERROR` does not cover this.
- Before: two `static_assert`s in sequence.
- After: the target's assertion inside `if constexpr (C::source::value.generator_count == n)`, so one error names the first count to fix. The condition stays written as the comparison, so GCC still prints "(5 == 4)". Add a compile-fail case with both counts wrong, `ONE_ERROR`, requiring the source message. No pinned literal changes.

**S2. The operand message's first suggestion is unsafe for the usual cause.** `basis_change.hpp:1046` to `:1048` reads "change_basis: x is not in the change's source algebra; give x the source's algebra tag, or use change_basis_back to map from the target". The common way to hit this is direction: x is already in the target, or the user applied the change twice (`m10`). Retagging a target value as the source compiles and gives coordinates that mean something else, with no error. For a product, "give both types the same algebra tag" is right, because mixed algebras there are a typo. Here the two algebras are both real and distinct.
- After: "change_basis: x is not in the change's source algebra; use change_basis_back to map from the target, or check that the tag's source is x's algebra". The same for `change_basis_back` at `:1058` to `:1060`: "change_basis_back: y is not in the change's target algebra; use change_basis to map from the source, or check that the tag's target is y's algebra". Pinned at `test/CMakeLists.txt:2368` and `:2369`.

**S3. The isometry message is hard to parse and does not mention row direction.** `basis_change.hpp:572` to `:574` reads "the rows do not carry the target's form to the source's (P G' P^T is not G)". The header and every `///` say the change must "keep every inner product", and the message should say it in the same words. The written-by-columns mistake (`m16`) lands here, so the message should name the row convention.
- After: "basis_change: the change does not keep the inner products (P G' P^T is not G); check that row i writes e_i in the target's generators, and that source and target are not swapped". Pinned at `test/CMakeLists.txt:2341` (`not_isometry`), which three tests use.

**S4. The header's example teaches a step its own layout does not need.** Lines 24 and 25 write `Point p2 = galoot::core::truncate<Point>(galoot::core::change_basis_back<cga3_to_r410>(q));`. For `point_layout` the round trip returns `Point` itself (`m27`), so a reader copies `truncate` into every round trip. On an exact scalar, `truncate` checks what it drops, so the extra call is harmless, but it hides the case where it is needed. Lines 79 to 82 explain that case in prose.
- After: `Point p2 = galoot::core::change_basis_back<cga3_to_r410>(q);  // p2 == p`, then the origin case, which needs `truncate`:
  ```cpp
  //   using Origin = galoot::core::mv<cga3, galoot::core::blades<cga3, "e4">, double>;
  //   auto o = galoot::core::change_basis<cga3_to_r410>(Origin{1});  // e4' and e5'
  //   Origin o2 = galoot::core::truncate<Origin>(
  //       galoot::core::change_basis_back<cga3_to_r410>(o));  // e5 is 0
  ```
  The test of the example, `404f2fc`, changes with it.

**S5. One letter means three things in the comments.** `d` is det(P) (lines 44 to 62). `D` is the dimension of `conformal_change<D>` (lines 202 to 215), and the letter is fixed by `conformal<D>()`. `D` is also "the diagonal D of the denominators" (lines 451 and 510 to 511, and the `M G' M^T = D G D` formula). On line 510 the reader has just met `D` as a dimension. Writing the diagonal as Δ, as decision 0220 section 4.1 already does, and det(P) as `det P` (or δ) makes each formula readable alone. Comment-only.

**S6. Passing the change's value in place of a tag gets no library message.** `change_basis<conformal_change<3>()>(p)` (`m02`) is the spelling that decision 0220 section 7 weighed and rejected, so other users will try it too. GCC gives four errors, and neither compiler mentions a tag. A catch-all overload for a value template argument catches it with one error on each compiler, and it leaves the normal calls unaffected. I prototyped it in scratch, `p01` and `p02`:
  ```cpp
  template <auto V, multivector X>
  constexpr std::remove_cvref_t<X> change_basis(const X& /*x*/) {
    static_assert(false, "change_basis: the template argument is a value, not a "
                         "change tag; put it in a tag, as in struct cga3_to_r410 "
                         "{ using source = cga3; using target = r410; static "
                         "constexpr auto value = conformal_change<3>(); }");
    return {};
  }
  ```
  `change_basis_back` would get the same. Each needs a `///` comment and a compile-fail test, which is two messages and about 1 s of slot time per compiler. A cpp-expert should confirm it (departure 5). The near-homonym `basis_change<C>(x)` (N1) cannot be caught this way, because a class template has no catch-all.

**S7. The `///` of `basis_change` should name the function that applies it.** A user who remembers the header's name, `basis_change.hpp`, writes `basis_change<C>(x)` and gets only the compiler's text (`m01`). One sentence at line 143 helps a reader of the declaration: "The function that applies a change is change_basis, a verb, and basis_change is the value that a tag holds." Comment-only.

### Notes

**N1. The near-homonym, on record.** `basis_change` (the value type, the header, and the concept's stem) and `change_basis` (the function) differ only in word order. The confirmed names were ruled twice (decision 0219 item 3 and decision 0222 item 1), and decision 0220 section 5 already measured the compiler's errors. Most users never type the value type, since `from_rows` and `conformal_change` return it, so the cost falls on the header name and on diagnostics. S7 is the cheap mitigation. A rename stays possible until M5 ships (owner item 1).

**N2. The tag pattern is discoverable from the places a user looks.** The header comment opens with the tag, the catch-all message shows a whole tag (`m03`, `m04`, `m18`, `m28`), and the tag matches the algebra tags users already write. Outside the header nothing introduces it, which is N9.

**N3. Integer acceptance depends on the direction and the layout, and the comment says only the layout.** `int` on e5 maps forward, since infinity = e'4 + e'5, but not back (`m31`). Lines 963 to 970 should add: "The inverse has its own denominators, so a layout that needs no division one way can need one the other way." The message is right as it is.

**N4. The Witt example should say why the form is scaled.** At line 175, "For the form [[0, 1], [1, 0]]" is the Witt form of one mode scaled by 2, because a form's entries are integers. A physicist reading for f f† + f† f = 1 will look for 1/2 and not find it. "For the Witt form scaled by 2, [[0, 1], [1, 0]], since a form's entries are integers" says it.

**N5. A change is checked, never found.** A user with their own form has to work out P by hand. The core checks it exactly and inverts it, which is what O3 ruled. The test-side `test/support/diagonalize.hpp` shows that finding P is mechanical. Whether a user-facing factory, such as one for Witt forms of m modes or an exact LDLᵀ diagonalization, belongs in the core later is an owner question (owner item 4). For v1, a second example in the header with two modes (section 1, task 3) shows the block pattern.

**N6. "basis change map:" is not a name the user wrote.** G1 and G2 (lines 808 to 810) start with a phrase that names no public entity. They are reached only through `change_basis_layout` and `change_basis_back_layout` given a hand-written set, so "change_basis_layout:" would be truer for most callers. Low reach. This is a pinned literal, so it needs its own test commit.

**N7. The overflow and the size limits are not one error, and the header says so.** The overflow gives 6 errors on GCC and 9 on Clang, with the sentence only in the quoted source line (`m35`). The limit failure names no change (`m36`). Both are stated at lines 113 to 116 and in decision 0220, and they match the rest of the core's overflow behavior. The fix is the core-wide one, a `static_assert` on a computed verdict, and not this unit's.

**N8. Values in the two algebras print alike.** `std::format` writes e1 to e5 for both cga3 and R(4, 1, 0). The header writes e'_i for the target, which helps on paper but not in a debugger. The fix belongs to `format.hpp` (an optional algebra name in the output, or a tag's own `names`, which a user can already give R(4, 1, 0)). The change of basis is the first feature where two algebras of one dimension meet in one program, so this is where users will notice.

**N9. A user needs a page outside the header.** The README shows only the rga3 preset. Nothing in it declares an algebra tag, and the change of basis is the first core feature that cannot be used without declaring two of them. The header comment is good reference text, but it is 116 lines of mathematics before the first declaration. A README section, "Your own algebra, and a change of basis", with the conformal round trip and the Witt tag, compiled as the README's other blocks are, would give users the task-first page (owner item 3).

**N10. Smaller wording points in the header comment.**
- Line 50: "phi(a ∨ b) = phi(a) ∨ phi(b) / d" reads more surely as "(phi(a) ∨ phi(b)) / d". The same holds at line 53.
- Lines 56 and 62 to 64: "transform, project, and antiproject then with 1 / d^2" and "A transform agrees across bases as stated where it drops nothing" each needed a second read. The first could be "under the same condition, transform, project, and antiproject scale by 1 / d^2". The second could be "this holds for a transform whose result keeps every blade it computes, as for a versor applied to an object of whole grades."
- Line 205: "e(D+1)(D+2) maps to -e'(D+1)(D+2)" is clearer with its instance: "for D = 3, e45 maps to -e'45".
- Line 154: "n is larger than the number of bits in a blade mask" says `n` where the parameter is `N`. This follows `metric`'s message, so it is consistent, and I would leave it.

## 5. What reads well

- The tag reads like the algebra tags, and its three members are the words of the field.
- `from_rows(rows, denominators)` keeps every number exact and visible, and wrong shapes and fractional entries fail at the user's line.
- Every tag fault is caught once per tag, not once per call (`m29`), and on Clang the context names the tag.
- The operand and template-argument catch-alls are exactly one error, and the template-argument message shows a whole tag to copy.
- The comment states what the change preserves, with its factors, and what it does not, with a concrete counterexample: the bulk of the origin, −e5 against −e5 / 2.
- The lossy round trip is handled by existing machinery whose message teaches `truncate` (`m26`).

## 6. Open items for the owner

1. **The near-homonym `basis_change` and `change_basis` (N1).** The names are confirmed (decision 0222 item 1).
   - Options: (a) keep them and add S7's sentence; (b) rename the value type before M5, to `basis_matrix<N>` or similar, with the header renamed to match.
   - Tradeoff: (b) removes the confusion at its source, but it reverses a twice-ruled name, touches the 35 files under `include/` and `test/` that name it, and the catalogue, and `basis_matrix` hides that a value is a change of basis, as decision 0220 section 7 says of `basis_map`. Most users never type the value type.
   - Recommended: (a).
2. **The rewordings S2 and S3, and N6.** The texts came from decision 0220 as proposals, and decision 0222 ruled the prefix, not the wording.
   - Options: (a) reword in this unit, with a test commit first (W9); (b) leave them for the M5 gate's message pass.
   - Tradeoff: (a) costs two small commits now. (b) keeps the unsafe advice of S2 in the merged core until then.
   - Recommended: (a) for S2 and S3, (b) for N6.
3. **User documentation beyond the header (N9).**
   - Options: (a) a README section with the conformal round trip and a Witt tag, compiled as a test, in this unit or at the M5 gate; (b) the header comment alone until v1.
   - Tradeoff: (a) adds a README test of about 1 s per compiler and some README length. (b) leaves the core's only multi-algebra feature without a task-first page, against W12's "changes in the same unit".
   - Recommended: (a) at the M5 gate, together with a short "declare an algebra" passage, which the README lacks today as well.
4. **A factory that finds a change (N5).**
   - Options: (a) none in v1, with examples in the header; (b) `witt_change<M>()` for the scaled Witt form of M modes against R(M, M, 0); (c) a general exact diagonalization.
   - Tradeoff: (b) is small and covers the physics case in the design study. (c) is real machinery that the test side already has, but it would need its own plan.
   - Recommended: (a) now, with (b) considered after v1 if a user asks.
5. **The value catch-all S6.**
   - Options: (a) add it, with its two tests; (b) leave the compiler's text.
   - Tradeoff: (a) is two overloads and two tests. (b) leaves GCC's four errors for a spelling a decision record already shows people consider.
   - Recommended: (a), after a cpp-expert confirms the overload set.

## Knowledge-base candidates (not written)

- In `api-design/library-diagnostics-in-cpp.md`: a function template whose first parameter is a type can also have a catch-all whose first parameter is `auto`. That catches a value passed where a tag belongs, with one error and no effect on valid calls (GCC 16.2, Clang 23.1, checked here only in scratch).
- In `api-design/error-message-design.md`: a message's suggested fix must be safe to follow for the most likely cause, not only true for some cause. "Give x the same tag" is right for a product of two algebras and wrong for a change between them.
