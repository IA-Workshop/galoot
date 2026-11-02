# 0150 The multivector type and how it is constructed

- Status: accepted by the owner, 2026-11-01 ("proceed as recommended"); the STYLE S8 amendment below is ratified by the same ruling
- Date: 2026-11-01
- Decided by: the owner, on plan 0016 questions 4 and 5 and research note 0019 question 1; recorded by Claude (agent, Opus 5.5)

## Context

Plan 0016 (WU8) proposed `mv<Alg, Blades, T>` as an aggregate holding `std::array<T, K>`, as STYLE S8 required ("Multivectors and descriptors are aggregates"). Research note 0019 found that such an aggregate accepts too few values and fills the rest with zeros, with no warning on either compiler under the project's flags (`Point p{1, 2, 3};` for a four-component point compiles), while a type with a constructor that takes exactly one value per component stays trivially copyable and structural and reports the wrong count as "3 == 4" on both compilers. A silent wrong value is the complaint users of other geometric-algebra libraries file most (note 0019, section 4.3).

## Options weighed

1. Keep the aggregate and document the zero fill. The shortest form a user types stays silently wrong.
2. Keep the aggregate and document a checked factory (`make<Line>(...)`) as the way to construct. The shorter brace form stays available and silent.
3. A constructor that takes exactly one value per component. Chosen.

For the constructor itself (plan 0016, planning probe 2):

- A constructor template converting each value with `static_cast` accepts a `double` variable into a `float` coefficient: an implicit lossy conversion (C8). Rejected.
- A constructor template requiring that each value convert without narrowing (`T{a}`) rejects integer literals such as `Line{0, 0, 1, 0, 0, 0}`, because a function parameter is not a constant expression. Rejected.
- A constructor with K parameters of type T, one per component. Brace initialization then applies the language's narrowing rule exactly as it does for an aggregate: an integer literal is accepted, an `int` or `double` variable that would narrow is an error. Chosen. It is written by inheriting the constructor of a small storage base whose parameter list is `component<T, I>...` for I from 0 to K − 1.

## Decision

1. `mv<Alg, Blades, T>`: the algebra first, then the blade set, then the scalar. No default scalar in the core (C8: the scalar decides rounding, exactness, and what checking mode checks). Models may give their named types a default later, by their own decision.
2. The type is not an aggregate. It is trivially copyable, standard-layout, and structural whenever its scalar type is trivially copyable and structural (`double` is; `std::complex<double>` is trivially copyable but not structural, because its members are private). Its storage is the public member `coefficients`, a `std::array<T, K>`.
3. A value is constructed from exactly K values, in the order the layout lists the blades, by a constructor with K parameters of type T; it is `explicit` when K is 1, so a scalar never converts silently into a one-blade multivector. Any other number of values, other than none, reaches a constructor whose only content is `static_assert(count == component_count, "...")`, so the error prints the two counts ("3 == 4" on GCC, "expression evaluates to '3 == 4'" on Clang). For K = 0 the K-parameter constructor is not declared, because it would be the default constructor.
4. A default-constructed multivector is zero: `Line l;` and `Line l{}` hold T{} in every place (a default member initializer on the storage).
5. STYLE S8 is amended. Its first sentence is replaced by: "Multivectors are trivially copyable structural values with public storage, whenever their scalar type is trivially copyable and structural; they have no virtual functions and never allocate. A multivector is constructed from exactly one value per component, in the order its layout lists them: too few or too many values is one compile error whose message prints the two counts, as in "3 == 4". A default-constructed multivector is zero. Descriptors, such as metrics, blade sets, and plans, are aggregates: trivially copyable, without virtual functions, and never allocating." The rest of S8 is unchanged. The clause "whenever their scalar type is ..." is the recorder's precision of the ruling's wording ("trivially copyable structural values"), which cannot hold for every scalar DoD-7 requires.

## Consequences

- Conversions between blade sets become constructors of the target (decision 0155), so `Motor{line}`, `Motor(line)`, and `static_cast<Motor>(line)` all work; with the aggregate only the last two did.
- PERFORMANCE P5 still says "Values are trivially copyable aggregates (STYLE S8)". That text is not covered by this ruling and is not edited; plan 0016 asks the owner to ratify "trivially copyable" in its place.
- Plan 0016's tests E1 to E3 change accordingly.

## Implementation, 2026-11-01 (WU8a)

Recorded by Claude (agent, Opus 5.5), cpp-expert role, implementing WU8a of plan 0016.

1. The checks of the layout, in `mv`'s body, each a `static_assert` with a literal message, each reading a variable template evaluated once for each set (and, for the range, each number of generators) that a program uses:
   - every blade names only e1 to en (`detail::blades_in_algebra<n, Blades>`, through `plan.hpp`'s `all_in_algebra`): "mv: the blade set names a generator beyond e_n";
   - every sign is +1 or -1 (`detail::signs_valid<Blades>`, the variable `index_of` already reads): "mv: the blade set holds a sign other than +1 or -1";
   - no mask appears twice, in either orientation, by the direct comparison of every pair (`detail::distinct_masks<Blades>`, O(K²) steps): "mv: the blade set holds one blade twice, perhaps in both orientations".

   The canonical-order fast path stays unbuilt (plan 0016 reading 16); the measured limit is in decision 0156.
2. The storage base is `detail::coefficient_storage<Multivector, T, std::index_sequence<I...>>`, whose first parameter is the multivector type itself. Two multivector types with the same scalar and number of blades would otherwise share the base, and its `==` would compare a Line with any other six-blade type of any algebra. With the type in the base, values of different layouts, algebras, or scalars do not compare, as plan 0016 reading 13 requires.
3. `==` is written out (`coefficients == other.coefficients`) and constrained by `std::equality_comparable<T>`, rather than defaulted. With a defaulted body, GCC 16.2.1 failed to compile `std::equality_comparable<mv<..., T>>` for a scalar without `==` (inside `std::array`'s `==`), where the plan wants a multivector of such a scalar to exist without `==`.
4. The zero initializer is written once per place, `coefficients{zero_at(I)...}`, not `coefficients{}`: GCC 16.2.1 miscompiles the latter after a constant evaluation in the same translation unit writes into a zero value of the type (lesson 0065; the reproducer, which needs no Galoot code, is under `docs/agent/records/spikes/gcc-shared-array-initializer/`).
5. Members: `algebra_type`, `layout`, and `component_count` (the last named so that Clang prints "3 == 6" and not the layout). `scalar_type` has no caller in WU8a and is left for the part that needs it (STYLE S16).
6. The count catch-all is constrained to any number of values other than none, the component count, and a single multivector, so that copies and, in WU8b, conversions are not caught.

## Review fixes, 2026-11-01 (WU8a)

Recorded by Claude (agent, Opus 5.5), cpp-expert role, after the W5 blind review and the hci-expert reading of WU8a. They change items 1, 2, 4, and 5 of the implementation section above:

1. The storage base is `detail::coefficient_storage<Multivector, T, Ts...>`, with Ts the scalar type once per component (built by `detail::storage_of` from an index pack through `template <std::size_t> using same = T;`). Its constructor's parameters are then T itself, so a narrowing error names the scalar ("from 'int' to 'double'") and no helper of the library. The zero initializer is `coefficients{Ts{}...}`, still one initializer per place.
2. The three layout checks read private static members named for their rules (`layout_names_only_generators_of_the_algebra`, `layout_signs_are_plus_or_minus_one`, `layout_holds_each_blade_once`), so that Clang's "due to requirement" names the rule instead of printing the layout's masks. Their messages say "layout" and give a fix after a semicolon:
   - "mv: the layout names a generator that the algebra does not have; a layout written for a larger algebra cannot be used here";
   - "mv: the layout holds a sign other than +1 or -1; give each blade of a layout written out by hand the sign +1 or -1";
   - "mv: the layout holds one blade twice, perhaps in both orientations; list each blade once".
3. `scalar_type` is provided: the tests' helpers use it, which is its present caller.
4. `ring_scalar<bool>` stays true and is documented: bool meets the requirements through integer promotion though its + is not a ring's. Rejecting it would need a special case and a message that the scalar message does not fit.
5. A scalar that is not `std::semiregular` is still rejected by `mv`'s template head with the compiler's account of the concept (`core.mv_scalar_without_default` records each compiler's wording). Constraining by a weaker concept and asserting `std::semiregular` in the body is the alternative the blind review names; it was not tried here, because the base class is formed from T before the body's assertions are read, so a reference scalar would likely fail there first (inferred). It is left for WU8b with the hci reading's F8 (references, const, and bool).

### Addendum, 2026-11-01: the storage base's parameters (`af447ae`)

Item 1 of the review fixes above names the base `detail::coefficient_storage<Multivector, T, Ts...>`. At `af447ae` its first parameter, the multivector type, was replaced by the multivector's algebra and layout: the base is `detail::coefficient_storage<Alg, Blades, T, Ts...>`, built by `detail::storage_of<Alg, Blades, T>`. Reason: clang-tidy's `bugprone-crtp-constructor-accessibility` read a base whose first parameter is the derived class as a CRTP base with public constructors, which the inherited constructors need. The parameters still differ for every multivector type, so no two types share a base and none compares with another through it; the test that `a == b` is not a valid expression across algebras, layouts, and scalars is in core.mv (`b2edac9`). The text above is kept as written.

## WU8b, 2026-11-01: scalars checked in the body, single arguments, derived classes

Recorded by Claude (agent, Opus 5.5), cpp-expert role, implementing WU8b of plan 0016, for the items the WU8a reviews deferred to it (HCI reading F8 and F10, blind review F5). They change items 4 and 5 of the review-fixes section above.

1. Scalars (HCI F8, decided with the usability checklist's "each misuse gives one error, and the message is on the first error line"). `mv`'s template head constrains T by `std::destructible` (it was `std::semiregular`), and the body checks, in this order, reporting only the first fault: T is not a reference and not const or volatile; T is not `bool`; T is `std::semiregular`; T is a `ring_scalar`. Each check reads a private named member, so Clang names the rule. Messages:
   - "mv: the scalar type is a reference or is const or volatile; give the type itself, as in double for const double";
   - "mv: bool cannot be the scalar type, since true + true is true; use an integer type such as int";
   - "mv: the scalar type must be default constructible, copyable, and assignable; give it a default constructor that makes its zero";
   - the ring-operations message, unchanged.

   The storage holds T without reference or cv-qualification, so that the base class is valid until the body's check, and for a scalar that is not semiregular it holds `unsigned char`, so that making a default value of the rejected type raises no second error on GCC. A use of a member of such a rejected type's coefficients can still fail after the message (the WU8b diagnostics record, case 14).
2. `ring_scalar` excludes `bool` (item 4 of the review fixes is reversed): its arithmetic meets the other requirements through integer promotion, but its sums are not a ring's, and a silent wrong result is what the counted constructor was adopted to prevent. Generic code asking `ring_scalar<bool>` is now told no.
3. Consequence for generic code: `requires { typename mv<A, B, X>; }` is now true for any destructible X, including a scalar without a default constructor, which the template head used to reject (the test of the template head changed accordingly, in `fdd72ae`, listed there).
4. A single argument that is neither a coefficient nor a multivector, such as a `std::array` of the coefficients, and a braced list in a second pair of braces, `Point{{1, 2, 3, 1}}`, which the aggregate took (HCI F10), each give one error: "mv: give the coefficients one by one, not as one array or braced list; write one value for each component, in the order the layout lists them". The braced list is caught by an `explicit` constructor template from `const T (&)[N]`, the only parameter a braced list deduces, and the array by an `explicit` single-argument template; the count catch-all no longer takes either. A constructor from an array was not added: it would bring back the silent zero fill for `Point{{1, 2, 3}}`.
5. A value of a class derived from an `mv` (blind F5) is excluded from the count catch-all, so `Line l(derived)` is the copy constructor and `Motor{derived}` the conversion, as for the base. The constructors of `mv` are not inherited for this purpose: a constructor of the base taking the base is excluded from a derived class's candidates when constructing from one argument (C++ [over.match.funcs]), so a class derived from `mv` with `using mv::mv;` is made from coefficients, not from a value of the base.
