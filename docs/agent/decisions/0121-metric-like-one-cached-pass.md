# 0121 metric_like checks the form in one cached pass (proposed)

- Status: proposed; awaits the owner's ruling on the questions at the end. Nothing in `include/` has changed.
- Date: 2026-11-01
- Proposed by: Claude (agent, Opus 5.5), cpp-expert in the researcher role, from research note 0015 (branch `research/metric-like-proof`).
- Bears on: decision 0060 item 5 and its consequence ("the concept could trust the factories and check only the type"), decision 0061 item 4 (the guard), decision 0105's option for the owner, decision 0107 item 4 (the P2 question).

## Context

`metric_like` reads all n² Gram entries three times per evaluation, and Clang evaluates it a second time when a unit also evaluates the concept as an expression. Decision 0107 measured the result as about 18 M instructions on GCC and 20 to 24 M on Clang of growth from n = 4 to 16 in every unit that uses an algebra, and asked the owner whether that falls under PERFORMANCE P2's exception. Note 0015 asked whether a cheaper mechanism can be proved as safe.

## Options weighed

Each is stated with its invariant and proof in note 0015, where G1 to G5 are listed (the type is a metric, the concept is false rather than an error for an unreadable value, n ≥ 1, symmetry, entries in range); costs are one check at n = 16, GCC / Clang, and the decision-0107 growth from n = 4 to 16 without and with a `static_assert` in the unit.

| Option | One check | Growth, no `static_assert` | Growth, with it | Keeps G1 to G5 | Interface |
| --- | --- | --- | --- | --- | --- |
| Today | 21.4 / 25.6 M | +17.7 / +20.8 M | +18.5 / +42.8 M | yes | — |
| A. One pass, cached per tag, reading a plain copy | 5.2 / 6.2 M | +2.5 / +5.2 M | +3.0 / +2.1 M | yes (Theorems 1 to 4) | none |
| A′. As A, reading through `std::array` | 14.5 / 16.4 M | +11.4 / +11.5 M | +11.5 / +14.5 M | yes (Theorems 1, 2, 4) | none |
| B. Trust the factories; concept tests the type | about 0.2 / 0.8 M | about 0 | about 0 | only if `metric` is sealed (Theorems 5, 6) | `metric` no longer an aggregate, const C-array matrix, an extra pointer member; departs from STYLE S8 |
| C. Tags derive from a validating `signature<M>` | as A′ | as A′ (inferred) | as A′ (inferred) | yes, short of a specialization (Theorem 7) | every tag is written differently |

B was rejected because its invariant is false for the aggregate `metric` (a hand-written aggregate, a changed copy, and a `std::bit_cast` each pass a type test, compiled on both compilers), and making it true costs three stated properties (aggregate, trivially copyable on Clang with `std::array` rows, the plain layout) to save the last 2 to 5 M, and nothing for algebras built by `from_gram`, whose own checks run in every unit anyway. C was rejected because it changes every tag for no saving over A′.

## Proposal

Option A: in `metric.hpp`,

```cpp
namespace detail {

template <std::size_t N>
struct flat_gram {
  // NOLINTNEXTLINE(*-avoid-c-arrays): built-in subscripts, read without a call.
  entry_type e[N][N];
};

// Whether m is symmetric with every entry within max_form_entry. Each pair
// g_ij, g_ji is read once, for j <= i; when the two are equal, the range of
// one is the range of both. The matrix is read through a copy whose reads
// are built-in subscripts, which costs a third of reading it through
// std::array at n = 16.
template <std::size_t N>
constexpr bool is_valid_form(const metric<N>& m) noexcept {
  static_assert(sizeof(flat_gram<N>) == sizeof(m.gram));
  const flat_gram<N> g = std::bit_cast<flat_gram<N>>(m.gram);
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j <= i; ++j) {
      const entry_type a = g.e[i][j];
      const entry_type b = g.e[j][i];
      if (a != b || a < -max_form_entry || a > max_form_entry) {
        return false;
      }
    }
  }
  return true;
}

// One evaluation per tag and translation unit; later checks read the result.
template <class Alg>
inline constexpr bool valid_form_v = is_valid_form(Alg::value);

} // namespace detail

template <class Alg>
concept metric_like =
    detail::is_metric_v<std::remove_cvref_t<decltype(Alg::value)>> &&
    (std::remove_cvref_t<decltype(Alg::value)>::generator_count >= 1) &&
    requires {
      typename std::integral_constant<entry_type, Alg::value.gram[0][0]>;
    } && detail::valid_form_v<Alg>;
```

with `#include <bit>`, and the `///` comment's last sentence changed to say that the guard reads one entry, which decides whether the value can be read at compile time at all. The public members `is_symmetric()` and `entries_in_range()` stay; `from_gram` keeps its two checks and their separate messages.

This is the `cached-flat` tree of `docs/agent/records/performance/metric-like-check/variants.py`, which passed, on both compilers: `metric_test.cpp` and `plan_test.cpp` (compiled), the `plan_not_metric_like` compile-fail expectation, the exhaustive verdict at n = 1 and 2, the exhaustive comparison at n = 3, and the hole probes. It was not put through the full suite, the sanitizer preset, clang-tidy, or clang-format; an implementing unit does that, with a W5 review.

## What the owner must rule on

1. Whether to adopt A, which keeps every current guarantee and changes no public interface. (Recommended.)
2. Whether the `std::bit_cast` copy is acceptable under STYLE S16, which prefers the plain construction where it costs no more. The plain one (A′) costs about three times as much at n = 16 and keeps about 11 M of growth on each compiler; the copy is guarded by a `static_assert` that fails to compile if a library lays out `std::array` with padding (note 0015, Theorem 3). If not, A′.
3. Decision 0107 item 4, restated: after A, the growth of a fixed use from n = 4 to 16 that remains from the check is about 2.5 M on GCC and 5 M on Clang (within about twice the spread of the floor on Clang). Whether that is classified under P2's exception for growth that follows from the signature data, as one pass over the form's own entries, once per algebra and unit.
4. Whether to record, in the user documentation, that a program which specializes Galoot's templates (such as `metric<N>`) is outside its guarantees. Today's concept and every option are defeated by such a specialization (`holes/specialized.cpp`, `specialized_data.cpp`); saying so states the threat model the checks are built for, as the standard library does for its own templates.

## Consequences if adopted

- A unit of work (plan, W5 review) changes `metric.hpp` and the header's comment, adds the cost of one evaluation at three sizes to the P2 record, and keeps `metric_test.cpp`'s `static_assert`s unchanged; a test that the concept rejects a forged `std::bit_cast` metric and a mutated copy (now only probes in the records) can be added there.
- The per-plan figures of decisions 0105 and 0107 change: their units would show about 2 to 5 M of growth instead of 18 to 43 M.
- Option B stays recorded as rejected, with its proof of what sealing would take, should a later need (for example a module build) change the balance.
