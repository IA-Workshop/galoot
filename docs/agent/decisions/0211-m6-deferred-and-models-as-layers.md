# 0211: Named algebras are convenience layers, and M6 ships after v1

Date: 2026-11-08. Status: recorded. Source: the owner's messages of 2026-11-08.

## Rulings

1. Named algebras (M6's cga3 and others) are convenience layers that build on the general core: a model under `include/galoot/<model>/` in namespace `galoot::<model>`, on the core's public interface only, with zero core diff, as the rga3 model is.
2. D7: shipping M6 is deferred until after v1, so that the other algebras to be considered can be weighed alongside it. M6 is not part of the v1 release candidate. The O6 renormalization helper (decision 0196 item 2) goes with the first model that needs it.
3. The Hestenes paper "Point Groups and Space Groups in Geometric Algebra" is now in the knowledge base references (`crystallography/crystalsymmetry.pdf`, manifest entry committed). It is to be read against the V8 point-group set (a research read, then a note); until that read is recorded, the V8 sufficiency limit of plan 0023's outcome stands.
4. The rga3 tag declares its own oriented `names` in a separate small model unit before the M4 gate, so printing uses the model's published names (hci reading of WU15, S2); WU15 stays core-only.
5. Flector criterion (research of 2026-11-08): the M4 flector distinguishing test is the test case "a reflected point keeps a positive weight" in `test/core/transform_test.cpp` together with "the rga3 Flector e412 reflects (1, 2, 3, 1) to (1, 2, -3, 1) with weight +1" in `test/rga3/objects_test.cpp`. The section 8 "Flector transform" row names both (CONVENTIONS 1.14, decision 0207). The plane hand value (the plane z = 1 reflects to (0, 0, 1, 1)) and the even-sign mutant go into the post-gate test-quality unit.
6. The rotor sign of Colapinto's thesis is opposite to ours; recorded in the knowledge base (`geometric-algebra/rigid-motors-flectors-and-rational-point-groups.md`).
