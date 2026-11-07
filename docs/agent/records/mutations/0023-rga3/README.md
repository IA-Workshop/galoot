# Mutants of the rga3 model (WU16)

## Mutants of rotation and translation

Plan 0023 section 6, the mutants that concern `include/galoot/rga3/motions.hpp`. `mutate.py` applies one mutant at a time to the header, builds `rga3_motions_test` with `ninja -k 0`, runs every test labeled `rga3.motions` (the program and the nine compile-fail tests), and restores the header. Results: `out/gcc-debug.tsv` and `out/clang-debug.tsv` (mutant, build exit, each failed build with its first error line, the failed tests); every build and test is a `mutant:<name>` row in `runs.tsv`.

Run by the WU16 implementer (Claude, agent, Opus 5.5) in `.claude/worktrees/rga3`, one preset after the other, so no two runs edited the header at once. All 23 ran at `61c6b17` (the header of `d44ec64`). Eight survived on both compilers; six of them are equivalent (below), and the other two, `integer_catch_all_any_type` and `mixed_catch_all_any_type`, were killed after `62ef7b9` added three requires-probes to the overload test case. Those two were run again with that edit in the tree before it was committed, unchanged, as `62ef7b9`; their rows replace the first ones.

The layout and alias mutants of `include/galoot/rga3/objects.hpp` are in the second half of this record, [Mutants of the objects](#mutants-of-the-objects-wu16).

### Result

23 mutants, each on gcc-debug and clang-debug: 17 killed on both compilers, each by a test, and six equivalent survivors.

| Mutant | Killed by (both compilers) |
| --- | --- |
| `rotation_full_angle` (theta in place of theta/2) | `rga3.motions` (the bit-for-bit rotation case) |
| `rotation_negative_angle` (-theta/2) | `rga3.motions` |
| `rotation_sin_cos_swapped` | `rga3.motions` |
| `rotation_layout` (the result on the core's grade order "1 e41 ... e1234") | the `decltype` static_asserts of `motions_test.cpp` |
| `rotation_any_scalar` (the valid overload for any T) | `rga3.rotation_integer_scalar`, `rga3.rotation_rational_scalar` |
| `translation_full_d` (d in place of d/2) | `rga3.motions` |
| `translation_negative_half_d` | `rga3.motions` |
| `translation_d_permuted` (dy, dz, dx) | `rga3.motions` |
| `translation_layout` | the motor comparison in `motions_test.cpp`, which has no conversion from the other layout |
| `translation_accepts_integers` (the valid overload without `!std::integral<T>`) | the overload static_asserts (an ambiguous call); `rga3.translation_int`, `_int64` |
| `integer_catch_all_any_type` (`class T` for `std::integral T`) | the probe `!can_call_translation<const char*, const char*, const char*>`: the catch-all becomes viable and its `Motor<const char*>` result fails mv's scalar check, so the test does not build |
| `mixed_catch_all_any_type` (no `ring_scalar` on X, Y, Z, and no requires-clause) | the probe `!can_call_translation<const char*, int, int>`, the same way |
| `non_floating_catch_all_removed` | the overload static_asserts; `rga3.rotation_integer_scalar`, `_rational_scalar` |
| `angle_type_catch_all_removed` | the overload static_asserts; `rga3.rotation_angle_int`, `_double_for_float` |
| `axis_catch_all_removed` | the overload static_asserts; `rga3.rotation_axis_point`, `_axis_join` |
| `integer_catch_all_removed` | the overload static_asserts; `rga3.translation_int`, `_int64` |
| `mixed_catch_all_removed` | `rga3.translation_mixed` |

**Equivalent survivors: each catch-all's requires-clause removed** (`non_floating_catch_all_unconstrained`, `angle_type_catch_all_unconstrained`, `angle_type_catch_all_any_type`, `axis_catch_all_unconstrained`, `axis_catch_all_any_type`, `mixed_catch_all_unconstrained`). The stand-in's survivors, judged on the real header: removing a constraint makes a catch-all viable for calls that another overload already takes, and that overload still wins by the standard's ordering, so no call resolves differently.

- Against the valid overloads, partial ordering ([temp.func.order]) decides before constraints. `rotation(const Line<T>&, T)` is more specialized than `(const Line<T>&, const S&)` and than `(const X&, const S&)`. `translation(T, T, T)` is more specialized than `(const X&, const Y&, const Z&)`.
- Between catch-alls with the same parameter types, such as the unconstrained non-floating catch-all and the angle-type catch-all on `(Line<float>, double)`, the more constrained one wins (the tie-breaker of [temp.func.order]/6, where template parameters are compared without their type-constraints).
- `axis_catch_all_any_type`: for a non-multivector X, `typename X::scalar_type` in the result type is a substitution failure, so the overload is not viable, as before; `!can_call_rotation<double, double>` holds either way.

Both compilers agree with this reading: every compile-fail test still gave its one message. So the requires-clauses do not change overload resolution. They are kept because each states the case its catch-all rejects, and the result does not then rest on the ordering rules above.

The axis catch-all's assertion later lost its condition (`62fd480`, after the hci-expert reading's S6: `static_assert(false, ...)`, so that Clang's first error line no longer prints the axis's type). Its three mutants were run again at that commit, gcc-debug and then clang-debug, with the same results: `axis_catch_all_unconstrained` and `axis_catch_all_any_type` survive as before, and `axis_catch_all_removed` is killed by the same static_assert and the same two compile-fail tests. Their rows replace the first ones; the static_assert's line in `motions_test.cpp` is now 393, moved by later edits of the file, and the other rows keep the line numbers of their run.

## Mutants of the objects (WU16)

Plan 0023 section 6, the mutants that concern `include/galoot/rga3/objects.hpp`: each layout with one blade's orientation flipped, the motor layout with 1 and e1234 swapped and with its halves swapped, the flector layout with its halves swapped, and each alias with its scalar fixed to double or with the layout of another type of the same size. Every rga3 program includes the header, so `mutate.py` builds all seven (`rga3_objects_test`, `_motions_test`, `_point_groups_test`, `_chains_test`, `_incidence_test`, `rga3_readme_example_1`, `_2`) with `ninja -k 0` and runs every test whose label starts with `rga3`. The rows follow the motions rows in `out/gcc-debug.tsv` and `out/clang-debug.tsv`; the motions rows were not run again.

Run by a WU16 implementer (Claude, agent, Opus 5.5) at `613cab0`, whose `objects.hpp` is that of `554d3ba`, gcc-debug and then clang-debug. A point has no flipped-blade mutant: its blades are the generators, which have one name each.

### Result

20 mutants, each on gcc-debug and clang-debug: all 20 killed on both compilers, each by a test, and none by a warning (no build failure names a `-W` flag). Every first error line is a static_assert of a test, a call in a test's own code that has no overload, or a static_assert of the core that a test's conversion or product reaches.

| Mutant | Killed by (both compilers) |
| --- | --- |
| `line_e14_for_e41`, `line_e13_for_e31` | the layout static_asserts of `objects_test.cpp` (`line_layout == set_of(line_names)`); `rga3.motions`, `rga3.incidence` |
| `plane_e123_for_e321`, `plane_e134_for_e431` | the layout static_asserts; the static_assert of README example 2; `rga3.incidence` |
| `motor_e14_for_e41` | the layout static_asserts; `rga3.motions`, `rga3.point_groups`, `rga3.chains` |
| `motor_e13_for_e31` | the layout static_asserts; `rga3.motions`, `rga3.readme_example_1` |
| `flector_e123_for_e321` | the layout static_asserts only: no run-time test fails |
| `flector_e134_for_e431`, `flector_halves_swapped` | the layout static_asserts; `rga3.point_groups` |
| `motor_1_e1234_swapped`, `motor_halves_swapped` | the layout static_asserts; `rga3.motions`, `rga3.point_groups`, `rga3.chains`, `rga3.readme_example_1` |
| `point_alias_double`, `line_alias_double`, `plane_alias_double`, `motor_alias_double`, `flector_alias_double` | calls with no overload in the tests over `std::int64_t`, `int`, and the test rational (a braced construction from those scalars, or a product of mixed scalars); for Motor also the `scalar_type` static_assert of `objects_test.cpp`, and for Flector the static_asserts on what compose converts to; `line_alias_double` also fails the four rotation compile-fail tests (`rga3.rotation_integer_scalar`, `_rational_scalar`, `_angle_int`, `_angle_double_for_float`) |
| `point_alias_on_plane_layout`, `plane_alias_on_point_layout`, `motor_alias_on_flector_layout`, `flector_alias_on_motor_layout` | the static_asserts of `objects_test.cpp` that each alias is the multivector on its own layout; the core's lossy-conversion static_assert reached from the tests' conversions (`Line{p ^ q}` and the like); for all but `plane_alias_on_point_layout` also run-time tests (`rga3.motions`, `rga3.point_groups`, `rga3.chains`, `rga3.bare_compile`) |

`flector_e123_for_e321` fails no run-time test because the one program whose checks reach a flector's offset does not build: `objects_test.cpp` reflects (5, 7, -1) in the mirror with offset -2 on e321, which the flipped blade would move, but its layout static_assert stops the build first. The other programs use flectors through point groups, whose mirrors pass through the origin, so their offset is zero and the flip leaves them unchanged. The mutant is killed by a test either way; it is not a gap.
