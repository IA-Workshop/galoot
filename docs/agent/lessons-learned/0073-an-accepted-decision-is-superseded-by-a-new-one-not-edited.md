# 0073 An accepted decision is superseded by a new decision, not edited

- Date: 2026-11-15
- Unit: plan 0042 (M7 gate and release), found by the M7 audit (MA2) and repeated in the release audit (MA3)

## What happened

After decision 0245 reached `develop`, its item 1 was rewritten in place to point at decision 0246, which superseded it. The owner's M5 ruling (decision 0228 item 6(a)) says accepted records are not edited. The M7 audit flagged the edit. The fix, decision 0248, reaffirmed the rule but did not quote the options it accepted, so the release audit raised the same finding about decisions 0248 and 0249.

## Cause

The change felt like tidying, because the old item pointed at a ruling that no longer stood. Nothing checks a decision file against its first committed text, and the rule to quote the options lives in a decision, not in the template an author starts from.

## What now detects it

Decision 0250 quotes the options of decisions 0248 and 0249 in an addendum, and says where it reconstructed them. A superseded decision gets a new decision that names it, and the old file stays as written. The auditor compares each decision file with its first commit (`git log --follow -p`) and quotes the options for every item.
