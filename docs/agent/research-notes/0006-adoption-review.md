# 0006 What to adopt from notes 0001 to 0005

- Subject: a review of research notes 0001 (doctest), 0002 (CMake), 0003 (Colapinto's thesis), 0004 (Romero et al.), and 0005 (Lengyel's sources) against the governance documents and the design study, for what Galoot should adopt or plan for.
- Written by: Claude (agent), 2026-10-31. Every item cites the note that holds its evidence; this note adds no new evidence.
- Source ranking: the design study first, Lengyel's wikis for conventions, Colapinto for generic-programming technique (CONVENTIONS.md).

Items marked Done are already in the governance documents and await ratification with them. Items marked Proposed need the owner's word before they enter a governance document. Items marked Plan are notes for the roadmap (C12) and change no document now.

## Conventions

1. Done: CONVENTIONS.md 1.2 adds the Confirmed label from note 0005, and every entry formerly labeled Study, except 6.2, 6.4, and the operator table of 7.1, now rests on Lengyel's own pages. The flector row now records that Lengyel states −F ⟇ x ⟇ F̰ at every grade on one page and the positive form for points on another (0005). No convention changed.
2. Done: 6.1 now says that a round point is not the dual of its sphere; the sphere is its container (0005 C3). This answers the sign question that note 0003 raised.
3. Done: a conformal-duals disagreement is recorded. The wiki's Duals table negates the wiki's own definition, and the poster agrees with the definition (0005 C5). The owner confirms the choice when phase 3 begins.
4. Proposed, for phase 2: specify exp and log of rigid bivectors, following Lengyel, with the principal branch, the results at angles 0 and π, and any other branch under its own name. Specify the two motor interpolations under distinct names, because they give different results (0003, §5.2.3).
5. Plan: the Versor-to-Galoot translation table of note 0003 goes into user documentation once there is any. Versor orders components within a grade by mask value, so its coefficient arrays do not carry over to 1.7's order.

## Style

6. Proposed, as a sentence in STYLE S9: "An operation whose input can be degenerate, such as a zero norm, the logarithm of a pure translation, or the square root of a half-turn, states the degenerate set and returns a failure value on it. On a rounding scalar, a branch between formulas is decided as V13 requires, never by an exact comparison with zero." Source: the thesis's formulas branch on exact equality and divide by zero at these points (0003).
7. Proposed, as sentences in STYLE S14: "A `SUBCASE` never appears inside a loop. At most one `TEST_CASE`, `INFO`, or `CAPTURE` appears on a line. A test run that executes no test cases fails." Sources: doctest issue #1208 and measurements; the Clang 22 change; the zero-match exit code (0001). These rules are now in the doctest-expert agent; the proposal makes them binding on people too.
8. Plan: a plan formatter that names the blades and lists the terms of a generated table, kept from Versor's introspection, for debugging and for error messages (C9; 0003, App. A.4).

## Process

9. Proposed, as a sentence in VVUQ V9: "A formula or table from a published source is a reference only after it is recomputed from the definitions; an erratum found this way is recorded in a research note." Sources: errata found in the thesis (0003), a reversed moment sign on the wiki's Flector page, and the negated conformal Duals table (0005).
10. Plan: the validation cases (V8) with independent answers, by phase:
    - Phase 2 (rigid): crystallographic point and space groups built from mirror flectors, with known group orders (0003, §3.2 to 3.3); exp and log round trips; Denavit–Hartenberg chains against matrices (0003, §4.3).
    - Phase 3 (conformal): the line-to-circle boost (0003, §5.3.1); Bennett-linkage closure, once a primary source for the angle relation is read (0003).
    - Mesh: Romero's seven-point example, the 12 lattice points on x² + y² = 25, grids, and random points checked by Euler count and empty circles (0004).
11. Proposed: the conformal checks from note 0005's scratch scripts move into `test/reference/` when phase 3 begins, rather than now, because no conformal code exists yet (C12).
12. Owner decision: the size of the machine's shared pool of job slots (PERFORMANCE P11, C14). The recommendation is 32, one per hardware thread. It prevents oversubscription and limits no workstream below what the machine provides (0002, F6).
13. Owner decision: whether to report the two wiki errata to Lengyel, after a person looks at the live pages (0005).

## Planning

14. Core operations beyond products: exp, log, square root, inverse, ratio, and interpolation of versors, as named operations. The simple-bivector forms come in phases 2 and 3, and the general bivector split later (0003). The inverse checks that A Ã is a scalar (C8).
15. Scalar concepts: keep the minimal ring concept for products, and add a second concept with square root, atan2, and the trigonometric and hyperbolic functions for exp, log, and norms (0003).
16. Robust predicates in the mesh and robust modules: each classical predicate is one wedge with a fixed constant (incircle, insphere, orient2d, orient3d; 0004). Evaluate with a filter and an exact fallback (V13). Break cocircular ties by symbolic perturbation. Take a sign only after multiplying by the sign of the weight. Do not port Romero's Algorithm 2, which compares unnormalized values (0004). Use conformal(2) for planar diagrams, and keep Voronoi vertices rational (0004).
17. Build cost: measure from the first header (P1, P2). Appendix A of the thesis reports no build or run measurements, so Galoot has no baseline to inherit (0003).
18. Build decisions that agents record under C18 when the build is written: CMake 4.4 as the minimum, one test program per component, `DOCTEST_CONFIG_SUPER_FAST_ASSERTS`, unity builds and precompiled headers off, modules deferred (0001, 0002).

## The design study

19. The design study is canonical, so these corrections are for the owner to make or approve: its reference list still marks Romero et al. "abstract only", and the least-squares circle fits it attributes to that paper come from the paper's precursors (0004); its flector formula is the one that 5.6 corrects (CONVENTIONS section 8).

## Open questions

- Lengyel's pages do not state the order of motor composition (0005); 5.4 rests on Checked alone.
- The thesis's experimental claims (the direction rule of eq. 6.21, the Hopf commutation) are hypotheses under V9 until proved (0003).
