// No Galoot code. An insertion of K distinct, already ordered keys into a
// sorted list through std::span, as collect_result does: every index read is
// in range, and the scan takes K(K-1)/2 steps. K comes from -DKEYS=...
#include <array>
#include <cstddef>
#include <span>

#ifndef KEYS
#define KEYS 1024
#endif

constexpr std::size_t insert_all(std::span<const unsigned> keys,
                                 std::span<unsigned> out) {
  std::size_t size = 0;
  for (const unsigned key : keys) {
    std::size_t at = 0;
    while (at < size && out[at] < key) {
      ++at;
    }
    for (std::size_t j = size; j > at; --j) {
      out[j] = out[j - 1];
    }
    out[at] = key;
    ++size;
  }
  return size;
}

constexpr std::array<unsigned, KEYS> ordered() {
  std::array<unsigned, KEYS> keys{};
  for (std::size_t k = 0; k < KEYS; ++k) {
    keys[k] = static_cast<unsigned>(k);
  }
  return keys;
}

struct sorted {
  std::array<unsigned, KEYS> keys;
  std::size_t size;
};

constexpr sorted collect() {
  constexpr std::array<unsigned, KEYS> keys = ordered();
  sorted s{};
  s.size = insert_all(keys, s.keys);
  return s;
}

constexpr sorted result = collect();
static_assert(result.size == KEYS);

int main() { return 0; }
