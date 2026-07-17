#pragma once

#include <cstddef>
#include <span>

namespace qfcpp::quant {

// Fast normal CDF approximation (Abramowitz & Stegun 26.2.17): a fixed
// 5-term polynomial plus one exp() call, no branches on the hot path other
// than the final sign fold, and no libm erfc. Absolute error < 7.5e-8,
// which is far below the precision that matters for option pricing, and
// its cost is fixed regardless of input -- unlike std::erfc, whose latency
// can vary by input range. Use standardNormalCdf() (EuropeanOption.hpp)
// when you want the library's default full-precision implementation, and
// this one when you're pricing in a tight, latency-sensitive loop.
[[nodiscard]] double fastStandardNormalCdf(double x) noexcept;

// Prices N independent European calls in a single pass over contiguous
// input arrays, writing into a caller-provided output span (no allocation,
// no exceptions). All spans must be the same length -- callers validate
// that once, up front, rather than paying for it per element. The loop
// body has no data-dependent branches, so it auto-vectorizes under -O3
// on both GCC/Clang (SSE2/AVX) and MSVC.
void priceEuropeanCallsBatch(std::span<const double> spot,
                              std::span<const double> strike,
                              std::span<const double> volatility,
                              std::span<const double> riskFreeRate,
                              std::span<const double> timeToExpiry,
                              std::span<double> outPrices) noexcept;

}  // namespace qfcpp::quant
