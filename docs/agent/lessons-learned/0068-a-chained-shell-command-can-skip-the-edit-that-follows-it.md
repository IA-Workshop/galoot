# 0068 A shell chain that fails early silently skips the edit script after it

- Date: 2026-11-02
- Unit: WU9 (plan 0018)

## What happened

A command of the form `grep ... && python3 edit.py` ran the `grep`, which matched nothing because of an escaping slip, and so the edit script never ran. The next build failed on the old text and the cause looked like the edit's own fault. The same week, a lint suppression comment placed two lines above its enum did not apply because `NOLINTNEXTLINE` covers only the next line, and a static_assert condition replaced by a named boolean made Clang print the boolean's name and not the two types.

## What to do

- Run the edit and its check as separate steps, or end the chain with a check that the edit is present.
- Put `NOLINTNEXTLINE(check)` directly above the line it covers, with the reason above it.
- To make a compiler print the types, put the comparison itself in the `static_assert`, not a named boolean.
