#include "quant/BatchPricer.hpp"

#include <cassert>
#include <cmath>

namespace qfcpp::quant {

double fastStandardNormalCdf(double x) noexcept {
    static constexpr double kB1 = 0.319381530;
    static constexpr double kB2 = -0.356563782;
    static constexpr double kB3 = 1.781477937;
    static constexpr double kB4 = -1.821255978;
    static constexpr double kB5 = 1.330274429;
    static constexpr double kP = 0.2316419;
    static constexpr double kInvSqrt2Pi = 0.3989422804014327;

    const double ax = std::abs(x);
    const double t = 1.0 / (1.0 + kP * ax);
    const double poly = t * (kB1 + t * (kB2 + t * (kB3 + t * (kB4 + t * kB5))));
    const double upperTail = kInvSqrt2Pi * std::exp(-0.5 * x * x) * poly;

    return x >= 0.0 ? 1.0 - upperTail : upperTail;
}

void priceEuropeanCallsBatch(std::span<const double> spot,
                              std::span<const double> strike,
                              std::span<const double> volatility,
                              std::span<const double> riskFreeRate,
                              std::span<const double> timeToExpiry,
                              std::span<double> outPrices) noexcept {
    const std::size_t n = spot.size();
    assert(strike.size() == n && volatility.size() == n && riskFreeRate.size() == n &&
           timeToExpiry.size() == n && outPrices.size() == n);

    for (std::size_t i = 0; i < n; ++i) {
        const double s = spot[i];
        const double k = strike[i];
        const double sigma = volatility[i];
        const double r = riskFreeRate[i];
        const double t = timeToExpiry[i];

        const double sqrtT = std::sqrt(t);
        const double d1 = (std::log(s / k) + (r + sigma * sigma * 0.5) * t) / (sigma * sqrtT);
        const double d2 = d1 - sigma * sqrtT;

        outPrices[i] = s * fastStandardNormalCdf(d1) - k * std::exp(-r * t) * fastStandardNormalCdf(d2);
    }
}

}  // namespace qfcpp::quant
