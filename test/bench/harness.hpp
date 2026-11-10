#ifndef GALOOT_TEST_BENCH_HARNESS_HPP
#define GALOOT_TEST_BENCH_HARNESS_HPP

// A harness that times one rga3 operation over double in one form, the
// library's or its straight-line reference kernel's, once per process:
//
//   <program> galoot|reference throughput|latency <passes> <seed>
//   <program> check <passes> <seed>
//
// A run draws `pool` sets of operands from the seed and applies the
// operation passes * pool times. The throughput loop shifts the first
// operands by one place against the others at each pass, so consecutive
// passes compute different results, and no operation waits for another. The
// latency loop, for an operation whose result has its first operand's type,
// feeds each result to the next operation as that operand. Every result is
// added, as bits, into a checksum modulo 2^64, so no part of any result can be
// dropped. A run prints one line: the operation, form, loop, number of
// operations, seed, nanoseconds per operation by the steady clock, and the
// checksum. `check` runs each loop in both forms and exits 1 unless their
// checksums agree. Instructions and cycles come from outside, from a
// counter over the whole process; repeats are separate processes.

#include <array>
#include <bit>
#include <charconv>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <print>
#include <random>
#include <span>
#include <string_view>
#include <tuple>
#include <vector>

#include <galoot/presets/rga3.hpp>

namespace bench {

// Operand sets per run: 128 sets take at most 16 KiB, two motors each for
// compose, half of a first-level data cache of 32 KiB, so that the loops
// time arithmetic rather than memory.
inline constexpr std::size_t pool = 128;

// The checksum is stored here before the clock is read again, so the timed
// loop cannot be moved past that read.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
inline volatile std::uint64_t sink = 0;

// A double in [-1, 1) from the generator's top 53 bits, the same with every
// standard library, since std::mt19937_64 is specified in full.
inline double draw(std::mt19937_64& rng) {
  return (static_cast<double>(rng() >> 11U) * 0x1p-52) - 1.0;
}

// Drawn coefficients. A motor is unitized, so that Q v antireverse(Q) is
// the antiscalar e1234: its first four coefficients are scaled to a unit
// vector u and the last four, w, are made orthogonal to it, w - (u . w) u.
// A latency loop then moves its object by a rigid motion at each step, and
// the values stay bounded.
template <class Mv>
Mv drawn(std::mt19937_64& rng) {
  Mv x;
  for (double& c : x.coefficients) {
    c = draw(rng);
  }
  if constexpr (std::same_as<Mv, galoot::presets::rga3::Motor<double>>) {
    auto& q = x.coefficients;
    const double length =
        std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    const double along =
        (q[0] * q[4] + q[1] * q[5] + q[2] * q[6] + q[3] * q[7]) / length;
    for (std::size_t i = 0; i < 4; ++i) {
      q[i] /= length;
      q[i + 4] -= along * q[i];
    }
  }
  return x;
}

// The coefficients of a reference kernel's result as the rga3 type Mv.
template <class Mv, std::size_t N>
Mv as(const std::array<double, N>& coefficients) {
  Mv x;
  x.coefficients = coefficients;
  return x;
}

template <class R>
std::uint64_t bits_of(const R& result) {
  std::uint64_t sum = 0;
  for (const double c : result.coefficients) {
    sum += std::bit_cast<std::uint64_t>(c);
  }
  return sum;
}

struct timing {
  double nanoseconds;
  std::uint64_t checksum;
};

template <class Mv>
std::vector<Mv> drawn_pool(std::mt19937_64& rng) {
  std::vector<Mv> operands(pool);
  for (Mv& x : operands) {
    x = drawn<Mv>(rng);
  }
  return operands;
}

template <bool Latency, class First, class... Rest>
timing run(const auto& operation, std::uint64_t passes,
           const std::vector<First>& first, const std::vector<Rest>&... rest) {
  std::uint64_t checksum = 0;
  const auto start = std::chrono::steady_clock::now();
  if constexpr (Latency) {
    First x = first[0];
    for (std::uint64_t pass = 0; pass < passes; ++pass) {
      for (std::size_t j = 0; j < pool; ++j) {
        x = operation(x, rest[j]...);
        checksum += bits_of(x);
      }
    }
  } else {
    for (std::uint64_t pass = 0; pass < passes; ++pass) {
      for (std::size_t j = 0; j < pool; ++j) {
        checksum += bits_of(operation(first[(j + pass) % pool], rest[j]...));
      }
    }
  }
  sink = checksum;
  const std::chrono::duration<double, std::nano> elapsed =
      std::chrono::steady_clock::now() - start;
  return {elapsed.count() / static_cast<double>(passes * pool), checksum};
}

// Runs the program's command line for the operation `name` on operands of
// the types Operands; returns the exit status.
template <class... Operands>
int main(std::string_view name, std::span<char*> args, const auto& galoot,
         const auto& reference) {
  using First = std::tuple_element_t<0, std::tuple<Operands...>>;
  using Result = decltype(galoot(Operands{}...));
  constexpr bool chains = std::same_as<Result, First>;
  const std::vector<std::string_view> words(args.begin() + 1, args.end());
  const bool check = words.size() == 3 && words[0] == "check";
  const bool timed =
      words.size() == 4 && (words[0] == "galoot" || words[0] == "reference") &&
      (words[1] == "throughput" || (chains && words[1] == "latency"));
  std::uint64_t passes = 0;
  std::uint64_t seed = 0;
  const auto number = [](std::string_view word, std::uint64_t& value) {
    const auto [end, error] =
        std::from_chars(word.data(), word.data() + word.size(), value);
    return error == std::errc{} && end == word.data() + word.size();
  };
  if ((!check && !timed) || !number(words[words.size() - 2], passes) ||
      !number(words.back(), seed) || passes == 0) {
    std::println(stderr,
                 "usage: {} galoot|reference throughput{} <passes> "
                 "<seed>\n       {} check <passes> <seed>",
                 name, chains ? "|latency" : "", name);
    return 2;
  }

  std::mt19937_64 rng{seed};
  const std::tuple<std::vector<Operands>...> operands{
      drawn_pool<Operands>(rng)...};
  const auto measure = [&](std::string_view form, std::string_view loop) {
    const auto time = [&](const auto& operation) {
      return std::apply(
          [&](const auto&... pools) {
            return loop == "latency" ? run<chains>(operation, passes, pools...)
                                     : run<false>(operation, passes, pools...);
          },
          operands);
    };
    const timing t = form == "galoot" ? time(galoot) : time(reference);
    std::println("{} {} {} {} {} {:.3f} {:#018x}", name, form, loop,
                 passes * pool, seed, t.nanoseconds, t.checksum);
    return t.checksum;
  };
  if (timed) {
    measure(words[0], words[1]);
    return 0;
  }
  const std::uint64_t throughput = measure("galoot", "throughput");
  bool agree = throughput == measure("reference", "throughput");
  if constexpr (chains) {
    const std::uint64_t latency = measure("galoot", "latency");
    agree = agree && latency == measure("reference", "latency");
  }
  return agree ? 0 : 1;
}

} // namespace bench

#endif // GALOOT_TEST_BENCH_HARNESS_HPP
