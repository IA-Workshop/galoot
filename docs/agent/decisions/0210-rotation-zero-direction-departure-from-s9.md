# 0210: `rotation` keeps its checking-build assert on a zero direction; the unchecked NaN is a departure from STYLE S9's failure-value sentence

Date: 2026-11-08. Status: recorded. Source: the owner's reply "confirmed the recommendations" of 2026-11-08, to the open items of plan 0024 after its blind review and delta review. Recorded by Claude (agent, Opus 5.5), implementer of plan 0024. Number: 0204 and 0205 are on `feature/core-format` (WU15), which may take 0206 onward, so this record takes 0210, above the highest number on `develop` and `feature/core-format`, leaving a gap.

Bears on: STYLE S9; CONVENTIONS 5.7 and its version 1.13; decision 0203 items 2, 3, and 6; `records/reviews/2026-11-08-rotation-normalize-review.md` M1; `records/reviews/2026-11-08-rotation-normalize-delta-review.md` D1, L1, L2.

## Rulings and their resolution

1. **The zero direction (delta review D1).** The owner confirmed the recommendation: `rotation(l, theta)` keeps `GALOOT_ASSERT(std::isnormal(length), ...)` in checking builds and its plain `Motor<T>` result. It does not return `std::optional<Motor<T>>`. In unchecked builds a zero direction keeps giving the documented infinite or NaN coefficients, and a direction whose squares overflow a motor holding only cos(θ/2). This is a ratified departure from STYLE S9's sentence "An operation whose input can be degenerate, such as a zero norm, ... states the degenerate set and returns a failure value on it", which the delta review read as applying here. The reasons:
   - No change to the public interface: the return type of `rotation` stays `Motor<T>`, so no caller and no example changes.
   - No cost at release: an unchecked, optimized build compiles to the same code as before the assert (checked by the delta review on g++ and clang++ at `-O2`).
   - No precedent: no function of the library returns `std::optional` or `std::expected` yet, and the first one should be a function whose degenerate input the caller is meant to handle, not a misuse.
   S9's assert sentence ("A precondition that the type system cannot express is checked by `GALOOT_ASSERT` in checking builds and documented at the declaration") is applied. The `rotation` comment states the departure and its reason, without naming the rule. D1 is resolved by this ruling.
2. **CONVENTIONS 5.7's wording in 1.13** (decision 0203 item 2) is confirmed. The status line and the 1.13 row of CONVENTIONS say so; no other text of CONVENTIONS changed.
3. **Delta review L1.** The `rotation` comment's Cost line now adds the `std::isnormal` test and its branch in checking builds, and the unchecked sentence says the check is dropped by an optimizing build, while without optimization a call and a branch remain. Comment only.
4. **Delta review L2.** The claim that a length whose squares are subnormal is not caught is reworded to say it passes the check and that no test covers the case. No test was added: a test that such a length is accepted would guard a limitation rather than a contract. Comment only.

Items 3 and 4 and the departure note of item 1 change only comments in `include/galoot/rga3/motions.hpp`, so under W5 they need no delta review; they are listed here as their resolution.

## Consequences

- Decision 0203 item 6's owner item (the failure-value sentence) is closed by item 1.
- Should the library later adopt failure values for degenerate inputs, `rotation` is a candidate to revisit with them, as a change to its return type made before a release.
