// A validated wrapper: the tag derives from signature<M>, whose definition
// checks M when it is instantiated, once for each distinct M in a unit. The
// concept then needs only to ask whether the tag derives from some
// signature<M>. TRY_INVALID enables a tag whose metric is asymmetric, which
// must not compile; TRY_SPECIALIZED enables a specialization of signature for
// that metric, which skips the check and does compile.
#include <array>
#include <cstddef>
#include <type_traits>

#include <galoot/core/metric.hpp>

namespace core = galoot::core;

template <core::metric M>
struct signature {
  static_assert(M.generator_count >= 1 && M.is_symmetric() &&
                    M.entries_in_range(),
                "signature: the form must be symmetric with entries in range");
  static constexpr auto value = M;
};

template <core::metric M>
void derives_from_signature(const signature<M>&);

template <class Alg>
concept signed_metric =
    requires(const Alg& a) { derives_from_signature(a); };

struct rga3 : signature<core::rigid<3>()> {};
static_assert(signed_metric<rga3>);
static_assert(!signed_metric<int>);

// A tag written the old way, without the wrapper, is not accepted.
struct plain {
  static constexpr auto value = core::rigid<3>();
};
static_assert(!signed_metric<plain>);

[[maybe_unused]] constexpr core::metric<2> skew{{{{0, 1}, {2, 0}}}};

#ifdef TRY_INVALID
struct skewed : signature<skew> {};
static_assert(signed_metric<skewed>);
#endif

#ifdef TRY_SPECIALIZED
template <>
struct signature<skew> {
  static constexpr auto value = skew;
};
struct skewed : signature<skew> {};
static_assert(signed_metric<skewed>);
#endif

int main() { return 0; }
