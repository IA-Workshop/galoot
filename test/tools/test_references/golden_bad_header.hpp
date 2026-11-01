#ifndef GALOOT_TEST_TOOLS_TEST_REFERENCES_GOLDEN_BAD_HEADER_HPP
#define GALOOT_TEST_TOOLS_TEST_REFERENCES_GOLDEN_BAD_HEADER_HPP

// Fixture golden header for tools/test_references.py golden: alpha is
// defined twice and delta states no fact. Not compiled.
namespace golden {

// Fact: the fact of alpha
namespace alpha {
inline constexpr int value = 1;
} // namespace alpha

// Fact: e1 ^ e2 = -e2 ^ e1
namespace beta {
inline constexpr int value = 2;
} // namespace beta

// Fact: the fact of gamma
namespace gamma {
inline constexpr int value = 3;
} // namespace gamma

// Fact: the fact of alpha
namespace alpha {
inline constexpr int other = 4;
} // namespace alpha

// Values with no fact.
namespace delta {
inline constexpr int value = 5;
} // namespace delta

} // namespace golden

#endif // GALOOT_TEST_TOOLS_TEST_REFERENCES_GOLDEN_BAD_HEADER_HPP
