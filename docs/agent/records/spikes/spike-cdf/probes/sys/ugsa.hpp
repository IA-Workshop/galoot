// Spike C probe: a header with a computed static_assert message, included
// by sys_use.cpp either with -I or with -isystem (system-header suppression).
#include <array>
#include <cstddef>
struct msg_t { std::array<char, 8> b{'h','i'}; constexpr std::size_t size() const { return 2; } constexpr const char* data() const { return b.data(); } };
template <int K> void f() { static_assert(K < 4, msg_t{}); }
