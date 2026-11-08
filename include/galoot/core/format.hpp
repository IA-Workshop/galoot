#ifndef GALOOT_CORE_FORMAT_HPP
#define GALOOT_CORE_FORMAT_HPP

// Text output of multivectors: a std::formatter, so that std::format,
// std::format_to, and std::print write a multivector by the names of its
// blades:
//
//   struct rga3 {
//     static constexpr auto value = galoot::core::rigid<3>();
//   };
//   using Line = galoot::core::mv<
//       rga3, galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;
//
//   std::format("{}", Line{0, 0, 1, 0, 0, 0})
//   // {-e14: 0, -e24: 0, -e34: 1, e23: 0, -e13: 0, e12: 0}
//
// e41 = -e14, so a coefficient stored on e41 multiplies -e14, and is printed
// under that name unless the algebra tag names the blade e41 itself:
//
//   struct rga3 {
//     static constexpr auto value = galoot::core::rigid<3>();
//     static constexpr auto names = "e41 e42 e43 e23 e31 e12";
//   };
//   // {e41: 0, e42: 0, e43: 1, e23: 0, e31: 0, e12: 0}
//
// This header is not included by galoot/core.hpp, because <format> costs
// every includer several times what the rest of the core does to compile.

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <string_view>
#include <type_traits>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace galoot::core::detail {

// Whether an algebra tag declares names for its blades: a static member
// names that converts to std::string_view. A member of that name that does
// not convert is not read.
template <class Alg>
concept declares_names = requires {
  { Alg::names } -> std::convertible_to<std::string_view>;
};

// Why an algebra tag's names were rejected, for the first one that is wrong.
enum class TagNamesFault : std::uint8_t { none, not_a_blade, named_twice };

// Checked once per algebra, and only for n up to 9. An empty list declares
// no name and is no fault.
template <metric_like Alg>
consteval TagNamesFault make_tag_names_fault() {
  if constexpr (declares_names<Alg>) {
    constexpr std::size_t n =
        std::remove_cvref_t<decltype(Alg::value)>::generator_count;
    switch (first_error(Alg::names, n)) {
    case NameError::none:
    case NameError::no_names:
      return TagNamesFault::none;
    case NameError::repeated_blade:
      return TagNamesFault::named_twice;
    default:
      return TagNamesFault::not_a_blade;
    }
  } else {
    return TagNamesFault::none;
  }
}

template <metric_like Alg>
inline constexpr TagNamesFault tag_names_fault = make_tag_names_fault<Alg>();

// The name a component is printed under: "-", "e", and at most nine digits.
struct printed_name {
  std::array<char, 2 + max_compact_generators> chars;
  std::size_t length;

  [[nodiscard]] constexpr std::string_view text() const noexcept {
    return {chars.data(), length};
  }
};

constexpr void append(printed_name& name, std::string_view text) noexcept {
  for (const char c : text) {
    name.chars[name.length] = c;
    ++name.length;
  }
}

// The name of each blade of Blades, in its order: the name the tag declares
// with the blade's orientation, if there is one, and otherwise the canonical
// name, preceded by "-" for orientation -1. The tag's names are read as
// blades<Alg, "..."> reads them, and are well formed by the time this runs.
template <metric_like Alg, blade_set Blades>
consteval auto make_printed_names() {
  constexpr std::size_t n =
      std::remove_cvref_t<decltype(Alg::value)>::generator_count;
  std::array<printed_name, Blades.size()> names{};
  for (std::size_t i = 0; i < Blades.size(); ++i) {
    const oriented_blade blade = Blades.blades[i];
    printed_name& name = names[i];
    if constexpr (declares_names<Alg>) {
      const std::string_view declared = Alg::names;
      std::size_t at = 0;
      for (std::string_view text = next_name(declared, at); !text.empty();
           text = next_name(declared, at)) {
        if (parse_name(text, n).blade == blade) {
          append(name, text);
          break;
        }
      }
    }
    if (name.length == 0) {
      if (blade.sign == -1) {
        append(name, "-");
      }
      append(name, canonical_name(blade.mask).text());
    }
  }
  return names;
}

template <metric_like Alg, blade_set Blades>
inline constexpr auto printed_names = make_printed_names<Alg, Blades>();

// Stands in for the formatter of a scalar type that has none, so that the
// formatter of a multivector over it is still a complete, default
// constructible type and rejects the scalar only when it formats.
struct no_scalar_formatter {};

} // namespace galoot::core::detail

/// Writes a multivector x as {name: value, name: value, ...}: every component
/// of x, zero or not, in the order of x's layout, each as the name of its
/// blade, a colon and a space, and its coefficient as std::formatter<T, char>
/// writes it, with a comma and a space between components. An empty layout
/// writes {}. A format specification, as in {:.3f} or {:>5}, is read by
/// std::formatter<T, char> and applies to each coefficient; nothing else is
/// rounded or dropped.
///
/// A blade is printed by the name the algebra tag declares for it, when the
/// tag has a static member names, a constant convertible to std::string_view
/// that lists oriented names as blades<Alg, "..."> reads them, and one of
/// them stands for the blade with the orientation the layout stores. Any
/// other blade is printed by its canonical name, 1 for the scalar and
/// otherwise e and its indices in increasing order, preceded by - when the
/// layout stores it with orientation -1: a coefficient c on e41 = -e14 with
/// no declared name prints as -e14: c. A product's result holds its blades
/// with orientation +1, in canonical order, so it prints as e12, e13, e14, and
/// so on.
///
/// Requires: char output; a std::formatter<T, char> for the scalar type; an
/// algebra of at most nine generators, since names have one digit per index;
/// and, if the tag declares names, that each is a blade of the algebra (1, or
/// e and distinct indices from 1 to n) and that no blade is named twice, in
/// either orientation. Each is checked at compile time where x is formatted,
/// with one error whose message names it. Because the scalar is checked
/// there, std::formattable<mv<Alg, Blades, T>, char> is true also for a
/// scalar T without a formatter.
///
/// The specialization matches mv<Alg, Blades, T> exactly, so a class derived
/// from a multivector is not formattable until it is given its base's
/// formatter, in one line:
///
///   template <>
///   struct std::formatter<NamedLine, char> : std::formatter<Line, char> {};
template <galoot::core::metric_like Alg, galoot::core::blade_set Blades,
          std::destructible T>
struct std::formatter<galoot::core::mv<Alg, Blades, T>, char> {
  /// Reads the format specification as std::formatter<T, char> reads it.
  template <class ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& context) {
    if constexpr (std::formattable<T, char>) {
      return scalar.parse(context);
    } else {
      // Skip to the closing brace, so that the format string is accepted and
      // format below gives the one error.
      auto at = context.begin();
      while (at != context.end() && *at != '}') {
        ++at;
      }
      return at;
    }
  }

  /// Writes x as described above.
  template <class FormatContext>
  FormatContext::iterator format(const galoot::core::mv<Alg, Blades, T>& x,
                                 FormatContext& context) const {
    namespace detail = galoot::core::detail;
    constexpr std::size_t n =
        std::remove_cvref_t<decltype(Alg::value)>::generator_count;

    // One rejection at most, so that a misuse gives one error.
    if constexpr (!std::formattable<T, char>) {
      static_assert(false, "format: the scalar type has no std::formatter; "
                           "give it a std::formatter, as double has");
      return context.out();
    } else if constexpr (n > galoot::core::max_compact_generators) {
      static_assert(false, "format: names such as e123 have one digit per "
                           "index, so a multivector prints by name only for "
                           "n up to 9; format its coefficients instead");
      return context.out();
    } else if constexpr (detail::tag_names_fault<Alg> ==
                         detail::TagNamesFault::not_a_blade) {
      static_assert(false, "format: an entry of the algebra tag's names is "
                           "not a blade of the algebra; write 1, or e and "
                           "distinct indices from 1 to n, as in e41");
      return context.out();
    } else if constexpr (detail::tag_names_fault<Alg> ==
                         detail::TagNamesFault::named_twice) {
      static_assert(false, "format: the algebra tag's names name one blade "
                           "twice, perhaps in both orientations; name each "
                           "blade once");
      return context.out();
    } else {
      return write(x, context);
    }
  }

private:
  template <class FormatContext>
  FormatContext::iterator write(const galoot::core::mv<Alg, Blades, T>& x,
                                FormatContext& context) const {
    constexpr auto& names = galoot::core::detail::printed_names<Alg, Blades>;
    auto out = context.out();
    *out++ = '{';
    for (std::size_t i = 0; i < Blades.size(); ++i) {
      for (const char c : std::string_view{i == 0 ? "" : ", "}) {
        *out++ = c;
      }
      for (const char c : names[i].text()) {
        *out++ = c;
      }
      *out++ = ':';
      *out++ = ' ';
      context.advance_to(out);
      out = scalar.format(x.coefficients[i], context);
    }
    *out++ = '}';
    return out;
  }

  std::conditional_t<std::formattable<T, char>, std::formatter<T, char>,
                     galoot::core::detail::no_scalar_formatter>
      scalar;
};

#endif // GALOOT_CORE_FORMAT_HPP
