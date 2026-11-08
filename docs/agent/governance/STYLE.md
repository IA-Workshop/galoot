# C++ style

Status: ratified by the owner on 2026-11-01 (decision 0094). S7 and S8 amended on 2026-11-01, ratified by the owner: proceed as recommended (decisions 0150 and 0152). S7 amended on 2026-11-08 to extend the weaker-concept pattern to variable templates, ruled by the owner on 2026-11-07 (decision 0196 item 5), its wording confirmed by the owner on 2026-11-08: confirmed the recommendations (decision 0207).

This document sets the style of Galoot's C++ source. It adds detail to the [constitution](CONSTITUTION.md) (cited as C1 to C20), [WORKFLOW.md](WORKFLOW.md), and [VVUQ.md](VVUQ.md), and unlike them it names a language and tools, so it changes when they do. The configuration files it names live at the repository root and are part of the style: where this document and a configuration file disagree, the disagreement is a defect to fix in whichever is wrong.

## Reference and checking

### S1 Reference guide

The reference for C++ practice is the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines), at commit `d1dda82e` (retrieved 2026-10-31). Where this document and the guidelines differ, this document governs. Galoot departs from the guidelines knowingly in two places:

| Guideline | Galoot | Reason |
| --- | --- | --- |
| E.2, throw an exception to signal failure | The core throws no exceptions (S9) | Core functions run at compile time and in builds without exceptions; a returned failure stays visible in the signature |
| NL.10, underscore style for all names | Model object types are CamelCase (S10) | They carry the names of the mathematical objects |

The guideline rules that the static checks exclude are listed, each with its reason, in `.clang-tidy`. Updating the recorded commit is a decision (C18).

### S2 Language edition

The baseline is C++23. A feature of a later edition is used only behind its feature-test macro, in an optional header that the default headers never include, with a fallback or a diagnostic when the macro is absent. Compiler extensions are disabled. An agent never lowers the edition without a decision record that names what requires it.

### S3 Layout

Source layout is whatever `clang-format` produces with the repository's `.clang-format`: LLVM style, 80 columns, and left-aligned pointers and references. Every change passes a check that formatting would leave it unchanged before it merges to `develop` (W5). Vendored code under `include/ext/` keeps its upstream layout.

### S4 Static checks

Every change passes `clang-tidy` with the repository's `.clang-tidy`, warnings treated as errors, before it merges to `develop`. Test builds compile with `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror` on each compiler of the toolchain matrix, at present GCC and Clang. A finding that a change does not fix is suppressed at its location, with the reason beside the suppression.

### S5 Run-time checks

The test suite also runs in a build with address and undefined-behavior sanitizers that stops at the first detection, and with the standard library's precondition checks enabled. Tests that run code in more than one thread also run under a thread sanitizer.

## Structure

### S6 Delivery

Each library is headers alone, under `include/galoot/<module>/`, in namespace `galoot::<module>`, with names in `snake_case.hpp`. Each header has an include guard named from its path (`GALOOT_CORE_BITS_HPP`), includes what it uses, and compiles on its own. The core includes only standard headers and other core headers (C13). Names not meant for users live in a nested `detail` namespace.

### S7 Compile-time generation

A signature is a tag type that carries its value, so diagnostics and symbols name the algebra, and blade sets and other structural data are values passed as class-type template arguments, not encoded as lists of types. Generation is ordinary `constexpr` or `consteval` code over those values. Recursive template instantiation is not used where a loop over values does the same work, because instantiating types costs far more build time than evaluating a loop. Each generated table is computed once per argument set and held in a `static constexpr` variable or variable template. Every template parameter is constrained by a named concept; `std::enable_if` is not used. A class template or a variable template may constrain a parameter by a weaker named concept and check the rest of its requirement with a `static_assert`, in the class's body or in the function that computes the variable's value, so that a misuse gives one error with a literal message. A misuse that a concept or `static_assert` can reject is rejected at compile time, with a message in the terms of the algebra (C9). A constrained catch-all overload that rejects a misuse asserts a condition that is always false, with a literal message (decision 0033); where the help is a number, such as a count of values, its `static_assert` tests the numeric condition itself, so that the compiler prints the values.

Each metaprogramming facility states what it computes and the requirement of each concept it defines. It is followed, in its header or in its test, by `static_assert`s for at least one argument it accepts and, for a constraint, one it rejects, with expected results taken from the stated requirement and not from the implementation.

### S8 Values and types

Multivectors are trivially copyable structural values with public storage, whenever their scalar type is trivially copyable and structural; they have no virtual functions and never allocate. A multivector is constructed from exactly one value per component, in the order its layout lists them: too few or too many values is one compile error whose message prints the two counts, as in "3 == 4". A default-constructed multivector is zero. Descriptors, such as metrics, blade sets, and plans, are aggregates: trivially copyable, without virtual functions, and never allocating. Small values are passed and returned by value; sequences cross interfaces as `std::span` or `std::mdspan`. Conversions between blade sets, and single-argument constructors, are `explicit`; a lossy conversion has its own name (C8). The core performs no dynamic allocation and holds no mutable global state. Owning raw pointers, `new`, and `delete` do not appear outside a resource handle.

### S9 Failures

The core throws no exceptions. Code that must fail constant evaluation does so by calling a non-`constexpr` function, so that no `throw` appears (decision 0012). A failure the caller is designed to handle, such as unitizing an object of zero weight, is returned in the result type, using `std::expected` or `std::optional`. A precondition that the type system cannot express is checked by `GALOOT_ASSERT` in checking builds and documented at the declaration. Results marked `[[nodiscard]]` include every function whose result is its only effect. An operation whose input can be degenerate, such as a zero norm, the logarithm of a pure translation, or the square root of a half-turn, states the degenerate set and returns a failure value on it. On a rounding scalar, a branch between formulas is decided as V13 requires, never by an exact comparison with zero.

## Source

### S10 Names

| Kind of name | Form |
| --- | --- |
| Variable, function, data member, namespace | `snake_case` |
| Core and library vocabulary type (`metric`, `blade_set`, `mv`) | `snake_case`, as in the standard library |
| Model object type (`rga3::Point`, `cga3::Dipole`) | `CamelCase` |
| Concept, type trait, alias of a trait result | `snake_case`, formed as the standard library forms its own (`is_blade_v`) |
| Other type alias | The form of the kind of type it names |
| Enumeration type | `CamelCase` |
| Enumerator; `constexpr` or `const` variable at namespace scope | `snake_case` |
| Template parameter | `CamelCase`, or one capital letter |
| Macro | `UPPER_CASE` with the prefix `GALOOT_` |

A name fixed by mathematical notation keeps its form: basis names such as `e41` and `e423`, and one-letter names such as `Q` for a motor or `G` for a metric matrix in a small scope. A name that a dependency or the standard library fixes keeps its form. The naming check cannot tell the two type rows apart, so review applies them.

### S11 Comments

Comments state why: intent, invariants, the equation or convention applied and stated in place (`// Right complement: u ^ right(u) = 1`), units, and the published source of a method where one exists. They do not narrate the code. Each declaration in a public header has a `///` comment immediately before it that states what the function requires, guarantees, and preserves, in terms a user of the library can read without the project's records.

All code is written as a human maintainer would write it, for a human reader, and follows the owner's general code style guide (`utils/code-style`). This covers source, comments, tests (including test names and failure messages), build files, scripts, generated files, and user documentation. None of them cites a governance provision (the numbered provisions of the constitution, workflow, VVUQ, performance, and style documents, or the conventions specification) or agent work of any sort: decisions, plans, research notes, reviews, lessons, spikes, records, the workbench, or agent names. A convention is restated where it is applied. The only citations are published sources: papers, books, standards, and the pages of the mathematics. A note that an agent needs for later work is written in the agent documents under `docs/agent/`, where the workflow places it.

### S12 Statements and constants

Statements in a function body are grouped into blocks that each perform one step, separated by one blank line, with a comment at the head of a block whose step is not evident. A numeric constant fixed by a published source or a recorded choice is defined once, by name, with its source. Code is never disabled by commenting it out.

### S13 Operators

Named functions are the interface; each overloaded operator is a one-line alias of a named function, with the meanings the conventions specification gives. Because `^` and `&` bind more loosely than `==`, `+`, and `*`, every expression in code, tests, and documentation that mixes them with other operators is fully parenthesized.

### S14 Tests

Tests use doctest, under `test/`, with one file per component named `<component>_test.cpp`. Each test case names the behavior it fixes and its reference (V2). Compile-time properties are tested with `static_assert`, and rejected uses with tests that fail to compile when the rejection is lost. Randomized tests take their seed from one place and print it on failure (V7). Exact reference values come from the independent scripts in `test/reference/` (V1). A `SUBCASE` never appears inside a loop, at most one `TEST_CASE`, `INFO`, or `CAPTURE` appears on a line, and a test run that executes no test cases fails (research note 0001).

### S15 Floating point

No header relies on options that relax IEEE 754 semantics, and a header whose correctness depends on those semantics, such as a robust predicate, stops compilation with `#error` when the compiler reports such an option. The test build states its floating-point options, including the contraction of multiply-add, so that declared variation (V14) has a recorded cause. Every comparison within a tolerance names its tolerance as a constant with its basis beside it (V12).

### S16 Direct code

Code takes the most direct form that meets its present requirement (C12): a function before a class, a concrete type before a template, and a template parameter only where two present uses differ in it. A helper, wrapper, overload, or option is added only with a present caller, and a short repetition that reads plainly is kept in preference to an abstraction that hides it. A clever construction is replaced by a plain one wherever the plain one costs no more in build or run time, as measured under [PERFORMANCE.md](PERFORMANCE.md).
