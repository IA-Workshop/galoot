# Mutants of blade_product.hpp

The mutation record of plan 0011 and its follow-ups.

- `mutate.py` and the `results-*.tsv` files are the original driver and its runs. `survivors.md` explains each survivor.
- `mutants.toml` is the port of `mutate.py` to `tools/mutate` (plan 0026), with its rows in `out/`. It is kept unchanged. Two of its mutants, `generator_scale_dropped` and `range_widest_shifted`, are stopped only by a warning made an error, so the tool records them `unjudged` (`../0026-tools/README.md`, criterion 9).
- `../0029-identities/blade_product_rewrites.toml` rewrites those two to build cleanly: `generator_scale_dropped_used` and `range_widest_shifted_mod`. A test kills both on gcc-debug and clang-debug, in default and run-time mode (`../0029-identities/README.md`, "The two warning-only blade_product mutants").
