# 0157 Blade sets in a C array, and blades<Alg, "..."> in place of blades<N, "...">

- Status: accepted by the owner, 2026-11-01 ("proceed as recommended")
- Date: 2026-11-01
- Decided by: the owner, on research note 0019 question 2; recorded by Claude (agent, Opus 5.5)
- Supersedes: decision 0090 item 4 (blade_set.hpp does not include metric.hpp) and its consequence that `blades<rga3, ...>` is not provided; plan 0016 question 11

## Context

Diagnostics name multivector types, and a type's blade set prints as masks and signs; on GCC a `std::array` member also prints libstdc++ internals (`std::__array_traits<...>::_Type`), 194 bytes for a two-blade set against 136 in a C array (note 0019, answering note 0010's open question 4). A user also writes the number of generators twice, `mv<rga3, blades<4, "...">, double>`. Carrying the names as written in the type (`mv<rga3, "e41 ...">`) would print best but brings type identity by text and a second family of types.

## Decision

1. The cheap steps first: `blade_set<K>` holds its blades in a C array, `oriented_blade blades[K]`, with the clang-tidy suppression and its reason beside it. An array of size zero is not C++, so `blade_set<0>` is a specialization holding `std::array<oriented_blade, 0>`; both support range-for, subscripts, and conversion to `std::span`. The set stays a structural, trivially copyable aggregate with a defaulted `==`.
2. `blades<Alg, "...">` takes the algebra tag, constrained by `metric_like`, in place of the generator count; `blade_set.hpp` includes `metric.hpp`. `full<N>` and `of_grade<N, K>` keep the count: they take no names and need no algebra.
3. The names-as-written spelling of a type is not adopted now.

## Consequences

- Code that used the `std::array` interface of `blades` (`.at(i)`, comparing two `blades` members with `==`) is migrated with no change of expected value; the commit lists the migrated lines.
- Constant evaluation reads a C array without the library's checked subscript, so step counts of plans may change. The canary of plan 0015 judges that: the change lands after plan 0015 merges, and a moved threshold is reported with its cause, not silently refitted.
- Every use of `blades<N, ...>` in tests, compile-fail sources, and other branches' new files moves to an algebra tag when it is next merged.

## Owner ruling, 2026-11-01: blade sets keep std::array

- Decided by: the owner; recorded by Claude (agent, Opus 5.5).
- Supersedes: item 1 of the decision above (the C array inside `blade_set`) and the first bullet of its consequences. Items 2 and 3, `blades<Alg, "...">` in place of `blades<N, "...">` and the names-as-written spelling not adopted now, stand.

`blade_set<K>` keeps `std::array<oriented_blade, K>`. The owner prefers consistency, one array type throughout the core and its tests, over the shorter GCC printing, and wants diagnostics improved by other routes, which plan 0016 lists as a WU8a design task and measures for the three likeliest mistakes.

The fact behind item 1 stands as research note 0019 measured it, and so remains the answer to research note 0010's open question 4: GCC prints a two-blade set held in a C array as `blade_set<2>{oriented_blade [2]{...}}`, 136 bytes, against 194 for `std::array` with libstdc++'s `std::__array_traits<...>::_Type` wrapper; Clang prints both compactly. The owner chose consistency over that gain.

Consequences: no `blade_set<0>` specialization, no migration of the tests' `.at(i)` and `blades ==` uses, and no interaction with plan 0015's canary, so WU8a does not wait for plan 0015's merge.
