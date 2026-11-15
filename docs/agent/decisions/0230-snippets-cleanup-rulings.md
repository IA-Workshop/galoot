# 0230 Snippets cleanup rulings

Date: 2026-11-10. The owner replied "Accept all recommendations" to the report on the snippets cleanup (plan 0037). Each item quotes the options and the recommendation accepted.

1. **`cost.py comments` fails on `plan.hpp`** on four counts (1,024 twice, 4,096, 134217728) that are sizes or a flag value from the scalar-full10-gcc research record, not costs the model predicts. Options: (a) leave it failing; (b) give `comments` a list of counts that are not model costs, each with its source, as it has for limits, with a test first, which could also close the check's miss on "about 89 million"; (c) reword the comment in `plan.hpp`, which touches `include/` and needs the full P9 matrix. Accepted: (b), at WU17 when `cost.py` is next touched.
2. **Stray rows in `verify-train-gen-{gcc,clang}.tsv`** (`traingen47-count` and `-fill`, not in the case file; both files arrived in `cd7dcf2`). No weight is affected. Options: (a) leave as recorded; (b) regenerate the verify files at the next refit. Accepted: (b), at WU17's refit.
