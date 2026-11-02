# 0165 WU10 runs in two parts; the antiproducts join `product.hpp`, the involutions and complements get `unary.hpp`; the result layouts follow one rule

- Status: accepted (plan 0019 section 1, departure 1, readings R3, and question 6; recorded by the WU10a implementer)
- Date: 2026-11-02
- Recorded by: Claude (agent, Opus 5.5), implementer of WU10a, cpp-expert role
- Bears on: plan 0001 (WU10), plan 0016 reading 17, decision 0164, decisions 0117 and 0158 (earlier splits)

## Context

Plan 0001 lists WU10 as one unit of size M: involutions, complements, antiproducts, and the De Morgan laws. Plan 0018 moved scaling, sums, and differences into it, and the WU9 review deferred three message fixes to it. That is about twice WU9's surface: seven public functions with four operators, five arithmetic operators with a new sum kernel, a projection, a concept rename, and eleven reworded messages. Decision 0164 left it to the WU10 plan where the new operations live. Plan 0016 reading 17 left open which blade set the unary operations return.

## Options weighed

Split:
1. One unit. One review of the whole surface; the reviewer cannot read it in one sitting (W2).
2. Two parts along the dependency (chosen): WU10a the antiproducts, involutions, complements, their operators, the product messages and the result spelling; WU10b the arithmetic, the projection `part`, and the renames. WU10b changes `mv.hpp`, which WU10a does not, and uses WU10a's message wording, so it branches after WU10a merges. WU7 and WU8 were split for the same reason.

Headers:
1. Everything in `product.hpp`: one file, but `+` and `~` in a file named for products.
2. Sums and scaling in `mv.hpp`: beside the type, but `mv.hpp` is the largest header, and every edit to it rebuilds every multivector test (P8).
3. Three headers, all included by `core.hpp` (chosen): `product.hpp` gains the antiproducts, which share the products' operand checks, catch-alls, and messages; a new `unary.hpp` holds the involutions and complements, which need no operand checks and include only `mv.hpp` and `plan.hpp` (with `blade_set.hpp` and `metric.hpp` for their names); a new `linear.hpp` (WU10b) holds the vector-space operations. The operand checks shared by `product.hpp` and `linear.hpp` move to `mv.hpp`'s `detail` in WU10b, so neither new header includes the other.

Result layouts:
1. Every operation returns the canonical set of the blades it reaches. Uniform, but `~line` would not be a `Line`, and `~~x == x` would need a conversion.
2. Every operation returns its input's layout where it can. The complements reach other blades, so "where it can" becomes a second rule about which blades.
3. One rule by what the operation does to blades (chosen): an operation that sends every blade to itself times a sign keeps the input's layout (the reverse, the antireverse, the grade involution, and in WU10b negation and scaling); every other operation returns the canonical set of the blades it reaches, each with orientation +1, in order of grade and then of index list (the products, the antiproducts, and the complements).

## Decision

1. WU10 runs as WU10a on `feature/core-antiproducts` and WU10b on `feature/core-linear`, the second branched after the first merges.
2. `geometric_antiproduct`, `antiwedge`, `%`, and `&` are in `include/galoot/core/product.hpp`. `reverse`, `antireverse`, `grade_involution`, `right_complement`, `left_complement`, `~`, and `!` are in the new `include/galoot/core/unary.hpp`. `core.hpp` includes both, and both are in `galoot::core`'s header set, which WU9 had left without `product.hpp`; WU10a adds both lines.
3. The layout rule of option 3. The canonical sets are named by public variable templates (decision 0166), so a program can name them: `right_complement_layout<Alg, S>`, `left_complement_layout<Alg, S>`, and the four product layouts.

## Reasons

- The sign maps hold exactly the input's blades, so returning the input's type loses nothing (C6), matches what users of geometric-algebra libraries expect (`~R` has the type of `R`), and lets `~~x == x` compile without a conversion.
- The complements reach other blades. Following the products keeps one rule for every operation whose result has other blades: a canonical set the user converts from, `Plane{!point}`.
- Both sides of each De Morgan law are then the canonical set of the same masks, so they have one type; the tests still compare blade by blade, so they do not rely on it.

## Consequences

- `unary.hpp` costs every includer of `core.hpp` its parse time (measured in the WU10a records, `records/mutations/0019-antiproducts/README.md`).
- WU11 decides whether bulk, weight, and duals join `unary.hpp`. The bulk and the weight keep some blades and drop others; under this rule they return the canonical set of what they keep unless WU11 records otherwise.
- `!!x` is x times (−1)^(gr·ag) by grade, on the canonical layout, not x on its own layout; the comments say so.
