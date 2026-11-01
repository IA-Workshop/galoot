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
