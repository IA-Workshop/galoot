// Input to the functions of galoot/core/bits.hpp that take a number of
// generators n, at run time in a checking build. The first argument names the
// function and the second the input:
//
//   n_0      the scalar with n = 0, below the range
//   n_above  the scalar with n = mask_bits + 1, above the range
//   mask     e3 with n = 2, a generator beyond the algebra
//   widest   the antiscalar of mask_bits generators, the widest input in range
//
// The first three must abort with the precondition's message, which the tests
// core.bits_rejects_* check, and `widest` must exit 0. The input is chosen at
// run time, so no call is evaluated at compile time. A usage error exits 2
// with a message, which fails every test that runs this program.
#include <cstddef>
#include <cstdio>
#include <span>
#include <string_view>

#include <galoot/core/bits.hpp>

namespace {

struct input {
  galoot::core::mask_t a;
  std::size_t n;
};

// Calls `function` with `in`. Returns false if no function has that name.
bool call(std::string_view function, input in) {
  using namespace galoot::core;
  if (function == "antigrade") {
    static_cast<void>(antigrade(in.a, in.n));
  } else if (function == "complement_mask") {
    static_cast<void>(complement_mask(in.a, in.n));
  } else if (function == "right_complement_sign") {
    static_cast<void>(right_complement_sign(in.a, in.n));
  } else if (function == "left_complement_sign") {
    static_cast<void>(left_complement_sign(in.a, in.n));
  } else if (function == "antireverse_sign") {
    static_cast<void>(antireverse_sign(in.a, in.n));
  } else {
    return false;
  }
  return true;
}

int usage_error() {
  std::fputs("usage: bits_assert_fail <function> n_0|n_above|mask|widest\n",
             stderr);
  return 2;
}

} // namespace

int main(int argc, char** argv) {
  using galoot::core::mask_bits;
  using galoot::core::mask_t;

  if (argc != 3) {
    return usage_error();
  }
  const auto given =
      std::span<char*>(argv, static_cast<std::size_t>(argc)).subspan(1);
  const std::string_view name = given.back();
  input in{};
  if (name == "n_0") {
    in = {.a = 0, .n = 0};
  } else if (name == "n_above") {
    in = {.a = 0, .n = mask_bits + 1};
  } else if (name == "mask") {
    in = {.a = 0b100, .n = 2};
  } else if (name == "widest") {
    in = {.a = ~mask_t{0}, .n = mask_bits};
  } else {
    return usage_error();
  }

  if (!call(given.front(), in)) {
    return usage_error();
  }
  // Reached only when the call returned, so the input was accepted.
  return 0;
}
