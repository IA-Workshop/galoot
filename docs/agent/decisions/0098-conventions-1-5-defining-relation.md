# 0098 Conventions 1.5: item 4.1 states the defining relation of the geometric product

- Status: accepted
- Date: 2026-11-01
- Recorded by: Claude (agent, Sonnet 5.5), on the owner's instruction "proceed with items 1 and 2 as recommended", answering the question raised by the WU6a review (finding F12) and the implementer's departure 3.

## Context

The convention-trace row for item 4.1 cites the fact e_i e_j + e_j e_i = 2 g_ij, which CONVENTIONS 4.1 did not state. The geometric product is fixed by that relation for a given metric, and no other item stated it.

## Decision

Item 4.1 now opens with: the geometric product is the associative product fixed by the defining relation e_i ⟑ e_j + e_j ⟑ e_i = 2 g_ij on the generators, so that e_i ⟑ e_i = g_ii, where g is the metric. The rest of the item is unchanged. The specification moves to version 1.5; the owner's instruction ratifies it (constitution C10).

## Consequences

The trace row for 4.1 now names a fact the specification states. No code or test changes. Tests that cite the fact keep their markers.
