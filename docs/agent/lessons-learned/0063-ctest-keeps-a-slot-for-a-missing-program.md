# 0063 CTest under the shared pool can keep a slot for a test whose program is missing

- Date: 2026-11-01
- Unit: WU6a (plan 0011), Claude (agent, Opus 5.5); found while explaining the pool losses of the day

## What happened

The job pool lost slots on three occasions on 2026-11-01 (2, then 23, then 23, then 10 of 23), each time with no client holding it and each time after a mutation run. The mutation driver deleted the test programs before each build and then ran CTest even when the build failed. CTest 4.4, running tests in parallel under the shared pool, kept the slot it had taken for a test whose program was missing ("Not Run").

## Evidence

A build-breaking mutant lost one slot per run (two runs, 09:45:00Z and 09:45:01Z). Six label runs under that mutant lost five slots. Three label runs with the original header and the programs deleted lost one slot each. Mutants whose build succeeded lost nothing. A simple failing or killed Ninja build under the pool, tried separately, lost none. The link between the earlier all-slots losses and this cause is inferred from times and counts; no other client held the pool.

## Cause

CTest takes a job slot for a test before it finds the program missing and does not return it. The driver ran CTest after a failed build.

## What detects it now

Mutation drivers skip CTest after a failed build and rebuild the programs at the end (mutate.py in the WU6a mutation records). Check `tools/jobpool status` before and after every driver run. The reviewer's `mutate_review.py` copies the old driver and has the same flaw. A restart is the recovery (`tools/jobpool stop && tools/jobpool start 23`) and is logged when it happens.
