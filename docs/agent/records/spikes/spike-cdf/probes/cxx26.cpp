// Spike C probe: C++26 diagnostic features under -std=c++23. One feature per
// compile, selected by -DPROBE_<NAME>; the _OK variants are correct programs
// that only declare the feature. Run by run_probes.sh with the S4 flags.
//   UGSA         static_assert with a computed (user-generated) message, P2741
//   DELETE_REASON  `= delete("reason")`, P2573
//   SA_FALSE     static_assert(false, "literal") in a template, P2593 (a DR)
#include <array>
#include <cstddef>
#include <string_view>

struct msg_t {
  std::array<char, 32> buf{};
  std::size_t n = 0;
  constexpr std::size_t size() const { return n; }
  constexpr const char* data() const { return buf.data(); }
};
constexpr msg_t make_msg(int k) {
  msg_t m{};
  const char* s = "blade e";
  while (*s) m.buf[m.n++] = *s++;
  m.buf[m.n++] = static_cast<char>('0' + k);
  const char* t = " not in set";
  while (*t) m.buf[m.n++] = *t++;
  return m;
}

#if defined(PROBE_UGSA)
template <int K> void f() { static_assert(K < 4, make_msg(K)); }
template void f<5>();
#elif defined(PROBE_UGSA_OK)
template <int K> void f() { static_assert(K < 4, make_msg(K)); }
template void f<3>();
#elif defined(PROBE_DELETE_REASON)
void g(int) = delete("g: integers are not accepted");
void h() { g(1); }
#elif defined(PROBE_DELETE_REASON_OK)
void g(int) = delete("g: integers are not accepted");
#elif defined(PROBE_SA_FALSE)
template <class T> void f() { static_assert(false, "f: never valid"); }
template void f<int>();
#elif defined(PROBE_SA_FALSE_OK)
template <class T> void f() { static_assert(false, "f: never valid"); }
#endif
int main() {}
