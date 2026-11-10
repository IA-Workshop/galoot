#include <cstddef>

#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct alg {
  static constexpr auto value = core::rigid<15>();
};

constexpr auto point_layout = core::blade_set<4>{{{{1U, 1}, {2U, 1}, {4U, 1}, {32768U, 1}}}};
constexpr auto line_layout = core::blade_set<6>{{{{32769U, -1}, {32770U, -1}, {32772U, -1}, {6U, 1}, {5U, -1}, {3U, 1}}}};
constexpr auto plane_layout = core::blade_set<4>{{{{32774U, 1}, {32773U, -1}, {32771U, 1}, {7U, -1}}}};
constexpr auto motor_layout = core::blade_set<8>{{{{32769U, -1}, {32770U, -1}, {32772U, -1}, {32775U, 1}, {6U, 1}, {5U, -1}, {3U, 1}, {0U, 1}}}};

using Point = core::mv<alg, point_layout, double>;
using Line = core::mv<alg, line_layout, double>;
using Plane = core::mv<alg, plane_layout, double>;
using Motor = core::mv<alg, motor_layout, double>;

template <class Mv>
Mv read(const double*& in) {
  Mv x;
  for (double& c : x.coefficients) {
    c = *in++;
  }
  return x;
}

template <class Mv>
double sum(const Mv& x) {
  double s = 0;
  for (const double c : x.coefficients) {
    s += c;
  }
  return s;
}

} // namespace

double use(const double* in);

double use(const double* in) {
  const auto p = read<Point>(in);
  const auto q = read<Point>(in);
  const auto r = read<Point>(in);
  const auto l = read<Line>(in);
  const auto g = read<Plane>(in);
  const auto h = read<Plane>(in);
  const auto Q1 = read<Motor>(in);
  const auto Q2 = read<Motor>(in);
  return sum(p) +
         sum(q) +
         sum(r) +
         sum(l) +
         sum(g) +
         sum(h) +
         sum(Q1) +
         sum(Q2) +
         sum(core::bulk_dual(p)) +
         sum(core::bulk_dual(l)) +
         sum(core::bulk_dual(g));
}
