// Prints, for rigid(3) motors acting on points, lines, and planes, the
// rounding counts of the coefficients transform drops, and replays
// transforms and compositions read from standard input, so that a script can
// compare its own floating-point emulation with the library bit for bit.
//
// Input lines: "<kind> <scalar> <coefficients...>" with kind one of point,
// line, plane (a motor's 8 coefficients, then the object's) or compose (two
// motors, the first applied second), scalar f or d, coefficients as hex
// floats. Each output line holds the result's coefficients as hex floats.
#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;

namespace {

template <auto Layout>
void print_roundings(const char* name) {
  constexpr const auto& k =
      core::detail::transform_dropped_roundings<rga3::algebra,
                                                rga3::motor_layout, Layout>;
  std::printf("dropped-roundings %s", name);
  for (const int r : k) {
    std::printf(" %d", r);
  }
  std::printf("\n");
}

template <class T, std::size_t N>
std::array<T, N> read(std::istringstream& in) {
  std::array<T, N> out{};
  for (T& c : out) {
    std::string word;
    in >> word;
    c = static_cast<T>(std::strtod(word.c_str(), nullptr));
  }
  return out;
}

template <class T, std::size_t N>
void print(const std::array<T, N>& a) {
  for (std::size_t i = 0; i < N; ++i) {
    std::printf(i == 0 ? "%a" : " %a", static_cast<double>(a[i]));
  }
  std::printf("\n");
}

template <class T>
void replay(const std::string& kind, std::istringstream& in) {
  rga3::Motor<T> q;
  q.coefficients = read<T, 8>(in);
  if (kind == "point") {
    rga3::Point<T> x;
    x.coefficients = read<T, 4>(in);
    print(core::transform(x, q).coefficients);
  } else if (kind == "line") {
    rga3::Line<T> x;
    x.coefficients = read<T, 6>(in);
    print(core::transform(x, q).coefficients);
  } else if (kind == "plane") {
    rga3::Plane<T> x;
    x.coefficients = read<T, 4>(in);
    print(core::transform(x, q).coefficients);
  } else {
    rga3::Motor<T> first;
    first.coefficients = read<T, 8>(in);
    print(rga3::Motor<T>{core::compose(q, first)}.coefficients);
  }
}

} // namespace

int main() {
  print_roundings<rga3::point_layout>("point");
  print_roundings<rga3::line_layout>("line");
  print_roundings<rga3::plane_layout>("plane");
  std::string line;
  while (std::getline(std::cin, line)) {
    std::istringstream in(line);
    std::string kind;
    std::string scalar;
    in >> kind >> scalar;
    if (scalar == "f") {
      replay<float>(kind, in);
    } else {
      replay<double>(kind, in);
    }
  }
  return 0;
}
