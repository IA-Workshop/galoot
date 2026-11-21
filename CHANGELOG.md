# Changelog

Each release of the core is listed here, newest first, with what changed for
a user of the library. A release that removes or changes a public name says
so under its own heading. The rga3 preset is below 1.0, and its interface may
still change.

## 1.0.1 (2026-11-21)

A patch release. No public name is added, removed, or changed, and a program
that compiled with 1.0.0 compiles with 1.0.1.

### Headers

- Including a Galoot header no longer leaves the four helper macros
  `GALOOT_DETAIL_PASTE_`, `GALOOT_DETAIL_PASTE`,
  `GALOOT_DETAIL_FORCE_INLINE_IS_0` and `GALOOT_DETAIL_FORCE_INLINE_IS_1`
  defined. They are undefined as soon as the check that uses them is done.
- Defining `GALOOT_FORCE_INLINE` with no value (`-DGALOOT_FORCE_INLINE=`) now
  stops the compile with one error, "GALOOT_FORCE_INLINE must be 0 or 1",
  where 1.0.0 printed two. A value that does not paste into one token, such as
  `-1`, still gets the preprocessor's own single error. The guide says which
  spellings are accepted and what each rejected one gives.
- Two local names in `core/basis_change.hpp` and `core/bits.hpp`, and three
  more in `core/metric.hpp` and the rga3 preset, are renamed. A linter that
  checks for confusable identifiers, with Galoot's headers not marked as
  system headers, no longer reports them against a name of yours such as `I`
  or `rn`. A parameter's name is not part of a C++ interface, so no call
  changes.
- Two overloads of `galoot::core::detail::apply` that nothing calls are
  removed. Nothing in `detail` is public.

### Build

- Configuring Galoot's own build with `GALOOT_FORCE_INLINE` set to anything
  but `ON` or `OFF` now stops with an error that names the value, as
  `GALOOT_CHECKING` already did. Values such as `TRUE`, `YES` or `1`, which
  1.0.0 accepted as ON, are rejected. Only a build of Galoot's own tests sees
  this option: a project that adds Galoot with `add_subdirectory` or
  `FetchContent` is not affected.
- Galoot's own tests compile in smaller pieces. The largest single compile of
  the test suite drops from 5.9 GiB to 3.0 GiB with GCC in a debug build, and
  the Clang optimized builds of the tests take about a fifth of the time they
  took. This changes nothing for a program that uses the library. Compile
  time and memory for a program with very many operations are as in 1.0.0;
  `GALOOT_FORCE_INLINE` defined as 0 remains the remedy.

### Documentation

- The README's `FetchContent` block names the repository,
  `https://github.com/IA-Workshop/galoot.git`, the tag `1.0.1`, and
  `GIT_SHALLOW TRUE`, which fetches only the tagged commit.
- The README's copy route says to copy `include/galoot`, so a copy no longer
  brings the vendored test framework with it.
- The guide says that `==` and `!=` on two basis changes do not compile, and
  that the compiler shows the header line ending in `// use same_rows(a, b)`
  for the comparison to write instead.
- The limits page says "build presets" where it means CMake configurations,
  so that "preset" alone names the algebra presets.

## 1.0.0 (2026-11-15)

The first release of the core.

- A header-only C++23 library that generates a geometric algebra from a
  compact declaration: a metric, given as the squares of the generators or as
  the matrix of their inner products, and the sets of blades that each type
  stores.
- Every product is worked out at compile time into a flat table of
  multiply-add terms, and the same code runs in a constant expression and at
  run time, over any scalar type that has ring arithmetic.
- The exact change of basis between two algebras, and the other operations of
  the core: products, duals, complements, projections, `transform` and
  `compose`, and printing through `<galoot/core/format.hpp>`.
- Checks of preconditions at run time when `GALOOT_CHECKING` is 1, and
  inlining that `GALOOT_FORCE_INLINE` set to 0 leaves to the compiler.
- Misuse is rejected at compile time with a message that says what to write
  instead.
- The rga3 preset, the rigid geometric algebra of three dimensions, in the
  layouts of Eric Lengyel's published formulas. It is below 1.0.
- A guide, [docs/guide](docs/guide/README.md), whose examples are compiled and
  run as tests.
- Tested with GCC 16 and Clang 23 on Linux with libstdc++. It is released under
  the MIT License.
