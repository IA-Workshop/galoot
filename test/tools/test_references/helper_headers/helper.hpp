#ifndef GALOOT_TEST_TOOLS_TEST_REFERENCES_HELPER_HEADERS_HELPER_HPP
#define GALOOT_TEST_TOOLS_TEST_REFERENCES_HELPER_HEADERS_HELPER_HPP

// Fixture for tools/test_references.py golden: a helper header that reads
// golden values itself instead of taking them from the test case. Not
// compiled.
constexpr int twice_alpha() { return 2 * golden::alpha::value; }

#endif // GALOOT_TEST_TOOLS_TEST_REFERENCES_HELPER_HEADERS_HELPER_HPP
