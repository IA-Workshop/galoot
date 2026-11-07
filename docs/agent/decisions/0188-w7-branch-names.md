# 0188 W7 branch names: subfeatures `feature/<name>--<part>`, and rows for test author, research, review, and gate branches

- Status: accepted and ratified (owner answer 2026-11-07, relayed by coordinator: "proceed with recommendations", M3 gate report owner decision 11). WORKFLOW W7 amended by this record.
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), records clerk role
- Bears on: WORKFLOW W7; plan 0020 departure 5; plan 0021 (branches, departure 4); the M3 gate's m13 and departure 1; decision 0185

## Context

W7 named a subfeature `feature/<name>/<part>`. Git refuses that name while `feature/<name>` exists ("cannot lock ref ... exists"; plan 0020 departure 5), so no unit could use it. Units also used branch forms W7 does not name: the test author's branch (`feature/core-duals-tests`, never merged, its commits cherry-picked), `research/<name>`, `review/<name>`, and `gate/m3`.

## Options weighed

1. Amend W7: subfeatures `feature/<name>--<part>`, merged only into `feature/<name>`, and rows for the test author's branch and the `research/`, `review/`, and `gate/` prefixes (recommended).
2. Amend the subfeature form and retire the other prefixes.

## Decision

Option 1. W7's branch table becomes:

| Branch | Holds | Merged into it by |
| --- | --- | --- |
| `master` | Releases only, each with its tag (W13) | The owner |
| `develop` | Integrated work that has passed W5 | Any contributor, agents included |
| `feature/<name>` | One unit of work or a group of related units, branched from `develop` | Any contributor, agents included |
| `feature/<name>--<part>` | A subfeature: one part of a feature, branched from its feature | Any contributor, agents included |
| `feature/<name>--tests` | A test author's commits for `feature/<name>`, branched from it; never merged, its commits are cherry-picked into it (decision 0185) | Nobody |
| `research/<name>`, `review/<name>`, `gate/<milestone>` | A research note or spike, a review record (W5), or a gate report and its records, branched from `develop` and merged into it | Any contributor, agents included |

## Consequences

- `<name>` of a test author's branch may itself be a subfeature's, as in `feature/core-transform--rounding--tests`.
- Branches named before this record keep their names: `feature/core-duals-tests` (a test author's branch) and `feature/core-rounding-wu11` (a subfeature named as a feature).
- Plan 0021's branch names follow the new forms.
