#ifndef SPIKE_E_BENCH_HARNESS_HPP
#define SPIKE_E_BENCH_HARNESS_HPP

// Times one form of a rigid(3) transform by a motor over double, once per
// process:
//
//   <program> <form> throughput|latency <passes> <seed>
//   <program> <form> batch <passes> <seed> <objects>
//   <program> check <seed>
//
// A form is a pair of always-inlined functions: setup(q), the work that reads
// only the motor, and apply(m, q, x). A run draws `pool` motors and `pool`
// objects from the seed.
//
// throughput  passes * pool calls apply(setup(q), q, x), the objects shifted
//             by one place against the motors at each pass, so consecutive
//             passes compute different results and no call waits for
//             another.
// latency     the same calls, each result fed to the next call as x.
// batch       for each motor, setup once, then apply to each of <objects>
//             objects drawn for the run: passes * pool * <objects>
//             transforms.
//
// Every result is added, as bits, into a checksum modulo 2^64. A run prints
// the program, form, loop, number of transforms, seed, nanoseconds per
// transform by the steady clock, the checksum, and the number of objects of
// a batch (1 otherwise). Instructions and cycles come from a counter over
// the whole process; the difference of two runs with different passes,
// divided by the difference of their transforms, leaves out the start, the
// draws, and the printing.
//
// check applies every form to every drawn object and motor and prints the
// largest difference of any coefficient from the library's result, relative
// to the largest coefficient of that result, and exits 1 if one is larger
// than 1e-12. The forms round differently, so their bits differ.

#include <array>
#include <bit>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <print>
#include <random>
#include <span>
#include <string_view>
#include <tuple>
#include <vector>

namespace spike_bench {

// 128 motors and 128 objects take 8 KiB and at most 6 KiB, well inside the
// 48 KiB first-level data cache of a performance core. A batch of 1,024
// objects takes 32 KiB for points and planes and 48 KiB for lines.
inline constexpr std::size_t pool = 128;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
inline volatile std::uint64_t sink = 0;

inline double draw(std::mt19937_64& rng) {
  return (static_cast<double>(rng() >> 11U) * 0x1p-52) - 1.0;
}

// A motor with |v|^2 + s^2 = 1 and v . m + s t = 0 up to rounding: the first
// four coefficients scaled to a unit vector u, the last four, w, made
// orthogonal to it, w - (u . w) u.
inline std::array<double, 8> drawn_motor(std::mt19937_64& rng) {
  std::array<double, 8> q{};
  for (double& c : q) {
    c = draw(rng);
  }
  const double length =
      std::sqrt((q[0] * q[0]) + (q[1] * q[1]) + (q[2] * q[2]) + (q[3] * q[3]));
  const double along =
      ((q[0] * q[4]) + (q[1] * q[5]) + (q[2] * q[6]) + (q[3] * q[7])) / length;
  for (std::size_t i = 0; i < 4; ++i) {
    q[i] /= length;
    q[i + 4] -= along * q[i];
  }
  return q;
}

template <std::size_t N>
std::array<double, N> drawn_object(std::mt19937_64& rng) {
  std::array<double, N> x{};
  for (double& c : x) {
    c = draw(rng);
  }
  return x;
}

template <std::size_t N>
std::uint64_t bits_of(const std::array<double, N>& x) {
  std::uint64_t sum = 0;
  for (const double c : x) {
    sum += std::bit_cast<std::uint64_t>(c);
  }
  return sum;
}

struct timing {
  double nanoseconds;
  std::uint64_t checksum;
  std::uint64_t transforms;
};

using motors = std::vector<std::array<double, 8>>;

template <std::size_t N>
using objects = std::vector<std::array<double, N>>;

template <class Form, std::size_t N>
timing run(std::string_view loop, std::uint64_t passes, const motors& q,
           const objects<N>& x, const objects<N>& batch) {
  std::uint64_t checksum = 0;
  std::uint64_t transforms = passes * pool;
  const auto start = std::chrono::steady_clock::now();
  if (loop == "latency") {
    std::array<double, N> y = x[0];
    for (std::uint64_t pass = 0; pass < passes; ++pass) {
      for (std::size_t j = 0; j < pool; ++j) {
        y = Form::apply(Form::setup(q[j]), q[j], y);
        checksum += bits_of(y);
      }
    }
  } else if (loop == "throughput") {
    for (std::uint64_t pass = 0; pass < passes; ++pass) {
      for (std::size_t j = 0; j < pool; ++j) {
        checksum += bits_of(
            Form::apply(Form::setup(q[j]), q[j], x[(j + pass) % pool]));
      }
    }
  } else {
    transforms *= batch.size();
    for (std::uint64_t pass = 0; pass < passes; ++pass) {
      for (std::size_t j = 0; j < pool; ++j) {
        const auto m = Form::setup(q[j]);
        for (const auto& b : batch) {
          checksum += bits_of(Form::apply(m, q[j], b));
        }
      }
    }
  }
  sink = checksum;
  const std::chrono::duration<double, std::nano> elapsed =
      std::chrono::steady_clock::now() - start;
  return {elapsed.count() / static_cast<double>(transforms), checksum,
          transforms};
}

template <class Form, std::size_t N>
double deviation(const motors& q, const objects<N>& x, const auto& library) {
  double worst = 0;
  for (std::size_t j = 0; j < pool; ++j) {
    for (std::size_t k = 0; k < pool; ++k) {
      const auto y = Form::apply(Form::setup(q[j]), q[j], x[k]);
      const auto r = library(q[j], x[k]);
      double size = 0;
      double gap = 0;
      for (std::size_t i = 0; i < N; ++i) {
        size = std::fmax(size, std::fabs(r[i]));
        gap = std::fmax(gap, std::fabs(y[i] - r[i]));
      }
      worst = std::fmax(worst, gap / size);
    }
  }
  return worst;
}

// Runs the command line for the transform `name` of objects of N
// coefficients; Forms are the forms' types, each with a static `name`, and
// Forms...[0] is the library's.
template <std::size_t N, class... Forms>
int main(std::string_view name, std::span<char*> args) {
  const std::vector<std::string_view> words(args.begin() + 1, args.end());
  const auto number = [](std::string_view word, std::uint64_t& value) {
    const auto [end, error] =
        std::from_chars(word.data(), word.data() + word.size(), value);
    return error == std::errc{} && end == word.data() + word.size();
  };
  std::uint64_t passes = 0;
  std::uint64_t seed = 0;
  std::uint64_t count = 1;
  const bool check = words.size() == 2 && words[0] == "check" &&
                     number(words[1], seed);
  const bool known = ((words.size() >= 1 && words[0] == Forms::name) || ...);
  const bool timed =
      known &&
      ((words.size() == 4 &&
        (words[1] == "throughput" || words[1] == "latency")) ||
       (words.size() == 5 && words[1] == "batch" && number(words[4], count) &&
        count > 0)) &&
      number(words[2], passes) && number(words[3], seed) && passes > 0;
  if (!check && !timed) {
    std::println(stderr,
                 "usage: {} <form> throughput|latency <passes> <seed>\n"
                 "       {} <form> batch <passes> <seed> <objects>\n"
                 "       {} check <seed>",
                 name, name, name);
    return 2;
  }

  std::mt19937_64 rng{seed};
  motors q(pool);
  objects<N> x(pool);
  for (std::size_t j = 0; j < pool; ++j) {
    q[j] = drawn_motor(rng);
  }
  for (auto& y : x) {
    y = drawn_object<N>(rng);
  }
  objects<N> batch(timed && words[1] == "batch" ? count : 0);
  for (auto& y : batch) {
    y = drawn_object<N>(rng);
  }

  if (check) {
    using Library = std::tuple_element_t<0, std::tuple<Forms...>>;
    const auto library = [](const std::array<double, 8>& v,
                            const std::array<double, N>& y) {
      return Library::apply(Library::setup(v), v, y);
    };
    bool good = true;
    (
        [&] {
          const double d = deviation<Forms>(q, x, library);
          std::println("{} {} largest relative difference from {}: {:.3e}",
                       name, Forms::name, Library::name, d);
          good = good && d <= 1e-12;
        }(),
        ...);
    return good ? 0 : 1;
  }

  (
      [&] {
        if (words[0] == Forms::name) {
          const timing t = run<Forms>(words[1], passes, q, x, batch);
          std::println("{} {} {} {} {} {:.3f} {:#018x} {}", name, Forms::name,
                       words[1], t.transforms, seed, t.nanoseconds,
                       t.checksum, words[1] == "batch" ? count : 1);
        }
      }(),
      ...);
  return 0;
}

} // namespace spike_bench

#endif // SPIKE_E_BENCH_HARNESS_HPP
