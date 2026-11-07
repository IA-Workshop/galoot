#ifndef GALOOT_TEST_CORE_PRODUCT_CHECKS_HPP
#define GALOOT_TEST_CORE_PRODUCT_CHECKS_HPP

// The algebras and the checks that the sources of the product test share:
// product_test.cpp, and product_conformal3_test.cpp, which holds the test
// cases that build an antiproduct of two full multivectors of conformal(3).
//
// The antiproducts are composed from the exact blade-product tables and the
// complement tables of the reference, as left(right(a) ⟑ right(b)) and
// left(right(a) ∧ right(b)).

#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include <string>
#include <vector>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>

namespace galoot_test::products {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga2 {
  static constexpr auto value = core::conformal<2>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};

// The coefficient of the result on the blade with this mask, or zero when
// the result's layout lacks it.
template <class Type>
std::int64_t value_on(const Type& r, core::mask_t mask) {
  for (std::size_t k = 0; k < Type::component_count; ++k) {
    const core::oriented_blade blade = Type::layout.blades.at(k);
    if (blade.mask == mask) {
      return blade.sign * r.coefficients.at(k);
    }
  }
  return 0;
}

// The coefficient of the table's product on the blade with this mask.
inline std::int64_t table_value(const golden::product& entry,
                                core::mask_t mask) {
  std::int64_t sum = 0;
  for (std::size_t k = 0; k < entry.size; ++k) {
    if (entry.terms.at(k).blade == mask) {
      sum += entry.terms.at(k).coefficient;
    }
  }
  return sum;
}

template <class Type>
Type random_integers(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> digit(-3, 3);
  Type x{};
  for (auto& c : x.coefficients) {
    c = digit(source);
  }
  return x;
}

// The name of a blade, e followed by the indices of its generators, or 1,
// with a minus sign for an orientation of -1, so that a failure names the
// blades it is about.
inline std::string blade_name(core::mask_t mask, int sign = 1) {
  std::string name = sign < 0 ? "-" : "";
  if (mask == 0) {
    return name + "1";
  }
  name += 'e';
  for (unsigned i = 0; i < 32; ++i) {
    if (((mask >> i) & 1U) != 0) {
      name += std::to_string(i + 1);
    }
  }
  return name;
}

// The coefficients, indexed by mask, of the geometric antiproduct and the
// antiwedge of the canonical blades of the masks a and b, composed from
// the tables alone: a ⟇ b = left(right(a) ⟑ right(b)) and
// a ∨ b = left(right(a) ∧ right(b)), with right and left read from the
// complement table and the product from the product table. The wedge of two
// blades with no generator in common is the term of their product on the
// union of their generators; any other wedge is zero. `found` is false when
// a table does not hold an entry where its index says it does.
struct composition {
  std::vector<std::int64_t> antiproduct_on;
  std::vector<std::int64_t> antiwedge_on;
  bool found;
};

inline composition compose(std::span<const golden::product> table,
                           std::span<const golden::complement_pair> complements,
                           core::mask_t a, core::mask_t b) {
  const std::size_t size = complements.size();
  composition out{
      .antiproduct_on = std::vector<std::int64_t>(size),
      .antiwedge_on = std::vector<std::int64_t>(size),
      .found = true,
  };
  const golden::signed_blade ra = complements[a].right;
  const golden::signed_blade rb = complements[b].right;
  const golden::product& entry = table[(ra.blade * size) + rb.blade];
  out.found = complements[a].blade == a && complements[b].blade == b &&
              entry.a == ra.blade && entry.b == rb.blade;
  if (!out.found) {
    return out;
  }

  const std::int64_t sign = std::int64_t{ra.sign} * rb.sign;
  for (std::size_t k = 0; k < entry.size; ++k) {
    const golden::term& t = entry.terms.at(k);
    const golden::complement_pair& d = complements[t.blade];
    out.found = out.found && d.blade == t.blade;
    out.antiproduct_on.at(d.left.blade) += sign * t.coefficient * d.left.sign;
  }
  if ((ra.blade & rb.blade) == 0) {
    const core::mask_t joined = ra.blade | rb.blade;
    const golden::complement_pair& d = complements[joined];
    out.antiwedge_on.at(d.left.blade) +=
        sign * table_value(entry, joined) * d.left.sign;
  }
  return out;
}

// 1, with a failure that shows `what`, when `wrong`; otherwise 0.
inline int failure_if(bool wrong, const std::string& what) {
  if (wrong) {
    FAIL_CHECK(what);
    return 1;
  }
  return 0;
}

// The number of coefficients, over every pair of basis blades of the layout
// of X and every blade of the algebra, on which geometric_antiproduct or
// antiwedge differs from the composition of the tables. Each basis blade is
// the blade of its place in the layout with its orientation there, so a
// value of 1 at a place whose orientation is -1 is minus the canonical
// blade. Each difference is reported with the blades it is about.
template <class X>
int antiproduct_mismatches(
    std::span<const golden::product> table,
    std::span<const golden::complement_pair> complements) {
  int mismatches = 0;
  for (std::size_t i = 0; i < X::component_count; ++i) {
    for (std::size_t j = 0; j < X::component_count; ++j) {
      const core::oriented_blade a = X::layout.blades.at(i);
      const core::oriented_blade b = X::layout.blades.at(j);
      X x{};
      X y{};
      x.coefficients.at(i) = 1;
      y.coefficients.at(j) = 1;
      const composition expected = compose(table, complements, a.mask, b.mask);
      const std::string pair = "(" + blade_name(a.mask, a.sign) + ", " +
                               blade_name(b.mask, b.sign) + ")";
      if (!expected.found) {
        mismatches += failure_if(true, "no table entry for " + pair);
        continue;
      }

      const auto ap = core::geometric_antiproduct(x, y);
      const auto aw = core::antiwedge(x, y);
      const std::int64_t sign = std::int64_t{a.sign} * b.sign;
      for (core::mask_t m = 0; m < complements.size(); ++m) {
        const auto on = [&](const char* op, std::int64_t got,
                            std::int64_t want) {
          return op + pair + " on " + blade_name(m) + " is " +
                 std::to_string(got) + ", not " + std::to_string(want);
        };
        const std::int64_t want_ap = sign * expected.antiproduct_on.at(m);
        const std::int64_t want_aw = sign * expected.antiwedge_on.at(m);
        mismatches +=
            failure_if(value_on(ap, m) != want_ap,
                       on("geometric_antiproduct", value_on(ap, m), want_ap));
        mismatches += failure_if(value_on(aw, m) != want_aw,
                                 on("antiwedge", value_on(aw, m), want_aw));
      }
    }
  }
  return mismatches;
}

} // namespace galoot_test::products

#endif // GALOOT_TEST_CORE_PRODUCT_CHECKS_HPP
