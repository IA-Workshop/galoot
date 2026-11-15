# 0204 The printed form of a multivector and the names it prints by

- Status: recorded. The form and the names were chosen by the owner on 2026-11-07 ("accept all recommendations", decision 0196 item 5: plan 0022 O1 option B with fallback E, O2 the map form, O6 to O8 the plan's defaults). The wording of CONVENTIONS 7.2, which states them, is a draft awaiting the owner's confirmation. The four readings of item 4 were accepted by the coordinator on 2026-11-08, relaying the parts 1 and 2 report.
- Date: 2026-11-08
- Recorded by: Claude (agent, Opus 5.5), implementer of WU15
- Bears on: `include/galoot/core/format.hpp`; CONVENTIONS 1.1, 1.6, 7.2; plan 0022 R2 to R6, O1, O2, O6 to O8; decision 0196 item 5

## Context

A layout stores a mask and a sign per blade and keeps no names (plan 0022 departure 4), so a formatter must choose a name for each stored oriented blade. Plan 0022 section 10 weighed five name rules (A: a rule from mask and sign that swaps two indices for -1; B: names declared by the algebra tag; C: names kept in the layout value; D: the sign folded into the printed coefficient; E: the canonical name with a leading `-`) and three text forms (the map form, a sum, the coefficients alone).

## Decision

1. **Form.** `std::format("{}", x)` writes `{name: value, ...}`: every component, zero or not, in layout order; each coefficient by `std::formatter<T, char>`; the format specification applied to each coefficient; `{}` for an empty layout. `char` only. No `operator<<`, `wchar_t`, `blade_set` formatter, or doctest `StringMaker` (O8).
2. **Names.** B with fallback E. An algebra tag may declare `names`, a static constant convertible to `std::string_view`, that lists oriented names as `blades<Alg, "...">` reads them. A blade prints by the declared name that stands for it with the layout's orientation. Otherwise it prints by its canonical name, preceded by `-` for orientation -1. The tag's names are checked once per algebra.
3. **Rejections, each one `static_assert` in `format` on an `if constexpr` chain.** These cover a scalar with no formatter, n above 9, a names entry that is not a blade, and a blade named twice. Because the scalar is checked there, `std::formattable` answers yes for a scalar without a formatter (O6). `parse` and `format` have declared return types, so the concept never instantiates their bodies. Derived classes get a formatter by inheriting the base's in one line (O7).
4. **Readings pinned by the tests, accepted:**
   - the scalar with orientation -1 prints `-1`;
   - names may be separated by runs of spaces;
   - a `names` member that does not convert to `std::string_view` is not read;
   - an empty `names` list declares no name and is not an error.
5. **Message texts**, as the test author finalized them:
   - "format: the scalar type has no std::formatter; give it a std::formatter, as double has";
   - "format: names such as e123 have one digit per index, so a multivector prints by name only for n up to 9; format its coefficients instead";
   - "format: an entry of the algebra tag's names is not a blade of the algebra; write 1, or e and distinct indices from 1 to n, as in e41";
   - "format: the algebra tag's names name one blade twice, perhaps in both orientations; name each blade once".

## Consequences

- `format.hpp` is outside `core.hpp`, since `<format>` costs an includer several times the rest of the core (plan 0022 departure 6). `core.format_outside_core` pins that `core.hpp` alone gives no formatter.
- The tag protocol gains one optional member. The rga3 model can declare CONVENTIONS 1.6's names without a core change.
- `get`'s messages keep their text (plan 0022 R9).
