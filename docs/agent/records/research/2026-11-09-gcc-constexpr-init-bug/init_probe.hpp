// A run-time check that a value-initialized local holds what value
// initialization gives, for an instrumented build of the library and the
// tests (instrument.py). instrument.py appends
//   ::galoot_init_probe::check(name, __FILE__, __LINE__);
// to each line that declares `T name{};` or `T name = {};`, and the build
// force-includes this header. At run time the check compares the value with
// a fresh `const T fresh{}` built here, which no constant evaluation writes
// into, and appends one line to the file named by GALOOT_INIT_PROBE_LOG (or
// to stderr) for each mismatch: "damaged <file>:<line> <type>". A type it
// cannot compare is logged once per site as "unchecked". In a constant
// evaluation the check does nothing. Each line also names the program.
//
// check_kernel replaces check in the library's run-time kernels (run_map,
// run_product, run_sum; patched by instrument.py --kernels). It logs, for a
// damaged result array, the damaged places and those of them that no term
// of the kernel writes, which keep the damaged value in the result:
// "kernel <file>:<line> damaged=<places> unwritten=<places> <signature>".
#ifndef GALOOT_INIT_PROBE_HPP
#define GALOOT_INIT_PROBE_HPP

#include <array>
#include <concepts>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <cstring>
#include <type_traits>
#include <utility>

namespace galoot_init_probe {

inline void note(const char* what, const char* file, int line,
                 const char* type) {
  const char* path = std::getenv("GALOOT_INIT_PROBE_LOG");
  std::FILE* out = path != nullptr ? std::fopen(path, "a") : stderr;
  if (out == nullptr) {
    return;
  }
  std::fprintf(out, "%s %s %s:%d %s\n", what, program_invocation_short_name,
               file, line, type);
  if (out != stderr) {
    std::fclose(out);
  }
}

template <class T>
const char* type_name() {
  return __PRETTY_FUNCTION__;
}

template <class T>
bool equal_values(const T& a, const T& b) {
  if constexpr (std::is_array_v<T>) {
    for (std::size_t i = 0; i < std::extent_v<T>; ++i) {
      if (!::galoot_init_probe::equal_values(a[i], b[i])) {
        return false;
      }
    }
    return true;
  } else if constexpr (requires { std::tuple_size<T>::value; a.data(); }) {
    for (std::size_t i = 0; i < a.size(); ++i) {
      if (!::galoot_init_probe::equal_values(a[i], b[i])) {
        return false;
      }
    }
    return true;
  } else if constexpr (requires { a.coefficients; }) {
    return ::galoot_init_probe::equal_values(a.coefficients, b.coefficients);
  } else if constexpr (std::equality_comparable<T>) {
    return a == b;
  } else if constexpr (requires {
                         a.value == b.value;
                         a.magnitude == b.magnitude;
                         a.roundings == b.roundings;
                       }) {
    return a.value == b.value && a.magnitude == b.magnitude &&
           a.roundings == b.roundings;
  } else {
    return std::memcmp(&a, &b, sizeof(T)) == 0;
  }
}

template <class T>
void check_at_run_time(const T& value, const char* file, int line) {
  if constexpr (std::is_default_constructible_v<T> &&
                std::is_trivially_copyable_v<T>) {
    const T fresh{};
    if (!::galoot_init_probe::equal_values(value, fresh)) {
      note("damaged", file, line, type_name<T>());
    }
  } else {
    static bool logged = false;
    if (!logged) {
      logged = true;
      note("unchecked", file, line, type_name<T>());
    }
  }
}

template <class T>
constexpr void check(const T& value, const char* file, int line) {
  if !consteval {
    check_at_run_time(value, file, line);
  }
}

// The places of a result array that a constant evaluation damaged, and
// those of them that no term of the kernels writes.
template <class T, std::size_t R, std::size_t N>
void check_places(const T* out, const bool (&written)[N], const char* file,
                  int line, const char* signature) {
  const T fresh{};
  std::string damaged;
  std::string unwritten;
  for (std::size_t i = 0; i < R; ++i) {
    if (!::galoot_init_probe::equal_values(out[i], fresh)) {
      damaged += std::to_string(i) + ",";
      if (!written[i]) {
        unwritten += std::to_string(i) + ",";
      }
    }
  }
  if (!damaged.empty()) {
    const std::string what = "kernel damaged=" + damaged +
                             " unwritten=" + (unwritten.empty() ? "-" : unwritten);
    note(what.c_str(), file, line, signature);
  }
}

template <const auto& Kernel, std::size_t R, class T, std::size_t... K>
constexpr void check_kernel(const std::array<T, R>& out, const char* file,
                            int line, std::index_sequence<K...> /*terms*/) {
  if !consteval {
    bool written[R + 1]{};
    ((written[Kernel[K].out] = true), ...);
    check_places<T, R>(out.data(), written, file, line, __PRETTY_FUNCTION__);
  }
}

template <const auto& KX, const auto& KY, std::size_t R, class T,
          std::size_t... I, std::size_t... J>
constexpr void check_kernels(const std::array<T, R>& out, const char* file,
                             int line, std::index_sequence<I...> /*x_terms*/,
                             std::index_sequence<J...> /*y_terms*/) {
  if !consteval {
    bool written[R + 1]{};
    ((written[KX[I].out] = true), ...);
    ((written[KY[J].out] = true), ...);
    check_places<T, R>(out.data(), written, file, line, __PRETTY_FUNCTION__);
  }
}

} // namespace galoot_init_probe

#endif
