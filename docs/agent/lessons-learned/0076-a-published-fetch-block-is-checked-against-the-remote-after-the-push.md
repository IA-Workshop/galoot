# 0076 A published fetch block is checked against the remote after the push

- Date: 2026-11-21
- Unit: plan 0045 (release 1.0.1), units R5 and R12

## What happened

The README released with 1.0.0 showed a `FetchContent` block with a placeholder for the repository and the tag `1.0.0`, while no remote held the tag. A reader who copied the block could not fetch the library. The tag was pushed six days later.

## Cause

The documentation tests are offline by design: they build each CMake block against the local source tree, so they cannot tell whether the URL and the tag exist anywhere. Publishing is the owner's step, after every check has run, and nothing ran after it.

## What now detects it

The release commit writes the real URL and tag into the README, and the blind review of that commit clones and builds the block against the newest tag the remote already holds. After the owner's push, the coordinator runs an unauthenticated `git ls-remote` of the README's URL for the branch and both tags (unit R12), and the release is not reported as published until all three are listed.
