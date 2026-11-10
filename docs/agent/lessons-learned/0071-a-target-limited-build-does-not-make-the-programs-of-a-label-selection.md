# 0071 A build of a few targets does not make the programs that a `-L` selection of tests runs

- Date: 2026-11-10
- Unit: WU14 follow-ups (plan 0031), found by its blind review (R6); recorded from the M5 gate audit (finding m14)

## What happened

A test author built only the four test programs it had changed, then ran `ctest -L '^core\.(identities|dual_identities|blade_product|plan)$'`. The run exited 8. That label also holds about 40 death and compile-fail tests, such as `core.plan_beyond_n`, whose programs the four-target build had not made. A narrower `-R` run of the changed programs' own tests then passed, so the plan's table of checks named the `-L` selection while the recorded passes used `-R`. The full suite passed at the tip, so nothing was hidden, but the records described the check imprecisely.

## What to do

- Before a `-L` run, build everything the label selects: list it with `ctest --preset <p> -L '<label>' -N` and build the targets those tests name, or build the preset's whole default target. A target-limited build suits a `-R` run of the tests that need only those targets.
- After a target-limited build, a ctest failure in a `-L` run is probably tests whose programs were not built (ctest reports them as not run or failed). Read the first failing test's message before suspecting the code.
- Write in a plan or a record which selection (`-R` or `-L`) a pass used, and which targets were built for it.
