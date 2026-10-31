// Spike C probe: does a diagnostic pragma around a template with a computed
// static_assert message silence -Wc++26-extensions under -Werror and under
// -pedantic-errors, even when the template is instantiated (-DFK=5, failing)
// outside the pragma region? -DFK=3 passes the assertion.
#include <array>
#include <cstddef>
struct msg_t { std::array<char, 8> b{'h','i'}; constexpr std::size_t size() const { return 2; } constexpr const char* data() const { return b.data(); } };
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++26-extensions"
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++26-extensions"
#endif
template <int K> void f() { static_assert(K < 4, msg_t{}); }
#if defined(__clang__)
#pragma clang diagnostic pop
#else
#pragma GCC diagnostic pop
#endif
template void f<FK>();
int main() {}
