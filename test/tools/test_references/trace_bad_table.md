# Fixture table for tools/test_references.py trace: each row after the
# first has one defect; beta's fact, one item's row, and a separator are missing

| Item | Fact | Implemented |
| --- | --- | --- |
| 1.1 | the fact of alpha | yes |
| 1.2 | a fact that no test states | yes |
| 9.9 | a fact of an item the specification lacks | no |
| 2.1 | the fact of gamma | maybe |
| 3.1 |  | no |
| 1.1 | a second fact of 1.1 that no test states | yes |
| 2.1 | a fact | with a pipe | no |
| 1.l | a mistyped item number | yes |
| **1.2** | an item number in bold | yes |

| Item | Fact | Implemented |
| 1.2 | a fact before any separator line | yes |
