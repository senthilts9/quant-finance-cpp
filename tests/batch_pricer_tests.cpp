#include "quant/BatchPricer.hpp"
#include "quant/EuropeanOption.hpp"
#include "quant/OptionTypes.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <vector>

using namespace qfcpp::quant;

TEST(FastStandardNormalCdfTest, MatchesReferenceCdfWithinPublishedErrorBound) {
    // Abramowitz & Stegun 26.2.17 is documented accurate to < 7.5e-8.
    for (double x = -5.0; x <= 5.0; x += 0.1) {
        EXPECT_NEAR(fastStandardNormalCdf(x), standardNormalCdf(x), 7.5e-8) << "x=" << x;
    }
}

TEST(FastStandardNormalCdfTest, SymmetryAroundZeroPointFive) {
    EXPECT_NEAR(fastStandardNormalCdf(0.0), 0.5, 1e-9);
    EXPECT_NEAR(fastStandardNormalCdf(1.0) + fastStandardNormalCdf(-1.0), 1.0, 1e-8);
}

TEST(PriceEuropeanCallsBatchTest, MatchesScalarEuropeanOptionPricer) {
    const std::array<double, 4> spot = {80.0, 100.0, 120.0, 42.0};
    const std::array<double, 4> strike = {100.0, 100.0, 100.0, 40.0};
    const std::array<double, 4> vol = {0.2, 0.2, 0.2, 0.2};
    const std::array<double, 4> rate = {0.05, 0.05, 0.05, 0.1};
    const std::array<double, 4> expiry = {0.5, 0.5, 0.5, 0.5};
    std::array<double, 4> batchPrices{};

    priceEuropeanCallsBatch(spot, strike, vol, rate, expiry, batchPrices);

    for (std::size_t i = 0; i < spot.size(); ++i) {
        OptionParameters params{
            .spot = spot[i], .strike = strike[i], .volatility = vol[i],
            .riskFreeRate = rate[i], .timeToExpiry = expiry[i],
        };
        EuropeanOption reference(params, OptionType::Call);
        // Tolerance reflects the CDF approximation's own ~7.5e-8 error
        // bound scaled by spot/strike magnitude, not implementation drift.
        EXPECT_NEAR(batchPrices[i], reference.price(), 5e-5) << "index=" << i;
    }
}

TEST(PriceEuropeanCallsBatchTest, MatchesHullTextbookExample) {
    const std::array<double, 1> spot = {42.0};
    const std::array<double, 1> strike = {40.0};
    const std::array<double, 1> vol = {0.2};
    const std::array<double, 1> rate = {0.1};
    const std::array<double, 1> expiry = {0.5};
    std::array<double, 1> price{};

    priceEuropeanCallsBatch(spot, strike, vol, rate, expiry, price);

    EXPECT_NEAR(price[0], 4.76, 1e-2);
}
