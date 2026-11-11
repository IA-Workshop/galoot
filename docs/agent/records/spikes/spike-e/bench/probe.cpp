// One function per form and object, each one call of the form, and for
// forms with work that reads only the motor, its two halves apart, so that
// each can be read on its own in the compiler's assembly.
#include <array>

#include <galoot/presets/rga3.hpp>

#include "forms.hpp"
#include "wrappers.hpp"

namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Line = rga3::Line<double>;
using Plane = rga3::Plane<double>;
using Q = std::array<double, 8>;

std::array<double, 4> point_galoot(const Q& q, const std::array<double, 4>& x) {
  using F = spike_bench::library<Point>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 4> point_a(const Q& q, const std::array<double, 4>& x) {
  using F = forms::transform_point_a<double>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 4> point_b(const Q& q, const std::array<double, 4>& x) {
  using F = forms::transform_point_b<double>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 4> point_bp(const Q& q, const std::array<double, 4>& x) {
  using F = spike_bench::collected<Point>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 4> point_c(const Q& q, const std::array<double, 4>& x) {
  using F = forms::transform_point_c<double>;
  return F::apply(F::setup(q), q, x);
}

auto point_b_setup(const Q& q) { return forms::transform_point_b<double>::setup(q); }

std::array<double, 4> point_b_apply(const decltype(point_b_setup(Q{}))& m, const Q& q,
    const std::array<double, 4>& x) {
  return forms::transform_point_b<double>::apply(m, q, x);
}

auto point_bp_setup(const Q& q) { return spike_bench::collected<Point>::setup(q); }

std::array<double, 4> point_bp_apply(const decltype(point_bp_setup(Q{}))& m, const Q& q,
    const std::array<double, 4>& x) {
  return spike_bench::collected<Point>::apply(m, q, x);
}

auto point_c_setup(const Q& q) { return forms::transform_point_c<double>::setup(q); }

std::array<double, 4> point_c_apply(const decltype(point_c_setup(Q{}))& m, const Q& q,
    const std::array<double, 4>& x) {
  return forms::transform_point_c<double>::apply(m, q, x);
}

std::array<double, 6> line_galoot(const Q& q, const std::array<double, 6>& x) {
  using F = spike_bench::library<Line>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 6> line_a(const Q& q, const std::array<double, 6>& x) {
  using F = forms::transform_line_a<double>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 6> line_b(const Q& q, const std::array<double, 6>& x) {
  using F = forms::transform_line_b<double>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 6> line_bp(const Q& q, const std::array<double, 6>& x) {
  using F = spike_bench::collected<Line>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 6> line_c(const Q& q, const std::array<double, 6>& x) {
  using F = forms::transform_line_c<double>;
  return F::apply(F::setup(q), q, x);
}

auto line_b_setup(const Q& q) { return forms::transform_line_b<double>::setup(q); }

std::array<double, 6> line_b_apply(const decltype(line_b_setup(Q{}))& m, const Q& q,
    const std::array<double, 6>& x) {
  return forms::transform_line_b<double>::apply(m, q, x);
}

auto line_bp_setup(const Q& q) { return spike_bench::collected<Line>::setup(q); }

std::array<double, 6> line_bp_apply(const decltype(line_bp_setup(Q{}))& m, const Q& q,
    const std::array<double, 6>& x) {
  return spike_bench::collected<Line>::apply(m, q, x);
}

auto line_c_setup(const Q& q) { return forms::transform_line_c<double>::setup(q); }

std::array<double, 6> line_c_apply(const decltype(line_c_setup(Q{}))& m, const Q& q,
    const std::array<double, 6>& x) {
  return forms::transform_line_c<double>::apply(m, q, x);
}

std::array<double, 4> plane_galoot(const Q& q, const std::array<double, 4>& x) {
  using F = spike_bench::library<Plane>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 4> plane_a(const Q& q, const std::array<double, 4>& x) {
  using F = forms::transform_plane_a<double>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 4> plane_b(const Q& q, const std::array<double, 4>& x) {
  using F = forms::transform_plane_b<double>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 4> plane_bp(const Q& q, const std::array<double, 4>& x) {
  using F = spike_bench::collected<Plane>;
  return F::apply(F::setup(q), q, x);
}

std::array<double, 4> plane_c(const Q& q, const std::array<double, 4>& x) {
  using F = forms::transform_plane_c<double>;
  return F::apply(F::setup(q), q, x);
}

auto plane_b_setup(const Q& q) { return forms::transform_plane_b<double>::setup(q); }

std::array<double, 4> plane_b_apply(const decltype(plane_b_setup(Q{}))& m, const Q& q,
    const std::array<double, 4>& x) {
  return forms::transform_plane_b<double>::apply(m, q, x);
}

auto plane_bp_setup(const Q& q) { return spike_bench::collected<Plane>::setup(q); }

std::array<double, 4> plane_bp_apply(const decltype(plane_bp_setup(Q{}))& m, const Q& q,
    const std::array<double, 4>& x) {
  return spike_bench::collected<Plane>::apply(m, q, x);
}

auto plane_c_setup(const Q& q) { return forms::transform_plane_c<double>::setup(q); }

std::array<double, 4> plane_c_apply(const decltype(plane_c_setup(Q{}))& m, const Q& q,
    const std::array<double, 4>& x) {
  return forms::transform_plane_c<double>::apply(m, q, x);
}
