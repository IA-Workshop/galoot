// Runs the one-pass check, detail::is_valid_form, and the current pair of
// checks, is_symmetric() && entries_in_range(), on every 3 x 3 matrix whose
// entries come from the seven values of verdict_check.cpp (7^9 = 40,353,607
// matrices), and counts the matrices on which they differ. Built against a
// variant tree that defines detail::is_valid_form (single, cached, or
// cached-flat). Prints the number of matrices, the number accepted by each,
// and the number of disagreements; exits 1 if there is any.
#include <array>
#include <climits>
#include <cstddef>
#include <cstdio>

#include <galoot/core/metric.hpp>

namespace core = galoot::core;

int main() {
  constexpr core::entry_type big = core::max_form_entry;
  constexpr std::array<core::entry_type, 7> values{
      INT_MIN, -big - 1, -big, 0, big, big + 1, INT_MAX};
  constexpr std::size_t n = 3;
  constexpr std::size_t entries = n * n;

  std::array<std::size_t, entries> digit{};
  core::metric<n> m{};
  for (std::size_t k = 0; k < entries; ++k) {
    m.gram[k / n][k % n] = values[0];
  }

  long long total = 0;
  long long accepted_current = 0;
  long long accepted_one_pass = 0;
  long long disagreements = 0;
  while (true) {
    const bool current = m.is_symmetric() && m.entries_in_range();
    const bool one_pass = core::detail::is_valid_form(m);
    ++total;
    accepted_current += current ? 1 : 0;
    accepted_one_pass += one_pass ? 1 : 0;
    disagreements += current == one_pass ? 0 : 1;

    // The next matrix, counting in base 7 over the nine entries.
    std::size_t k = 0;
    while (k < entries && digit[k] == values.size() - 1) {
      digit[k] = 0;
      m.gram[k / n][k % n] = values[0];
      ++k;
    }
    if (k == entries) {
      break;
    }
    ++digit[k];
    m.gram[k / n][k % n] = values[digit[k]];
  }
  std::printf("matrices %lld, accepted by the current checks %lld, by the "
              "one pass %lld, disagreements %lld\n",
              total, accepted_current, accepted_one_pass, disagreements);
  return disagreements == 0 ? 0 : 1;
}
