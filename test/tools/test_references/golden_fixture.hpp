#ifndef GALOOT_TEST_TOOLS_TEST_REFERENCES_GOLDEN_FIXTURE_HPP
#define GALOOT_TEST_TOOLS_TEST_REFERENCES_GOLDEN_FIXTURE_HPP

// Fixture golden header for tools/test_references.py. Not compiled.
namespace golden {

// Values of alpha.
// Fact: the fact of alpha
namespace alpha {
inline constexpr int value = 1;
} // namespace alpha

// Values of beta.
// Fact: e1 ^ e2 = -e2 ^ e1
namespace beta {
inline constexpr int value = 2;
} // namespace beta

// Values of gamma.
// Fact: the fact of gamma
namespace gamma {
inline constexpr int value = 3;
} // namespace gamma

} // namespace golden

#endif // GALOOT_TEST_TOOLS_TEST_REFERENCES_GOLDEN_FIXTURE_HPP
