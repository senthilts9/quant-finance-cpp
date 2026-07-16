#include "quant/EuropeanOption.hpp"
#include "quant/MatrixPricer.hpp"
#include "quant/OptionTypes.hpp"

#include <gtest/gtest.h>

using namespace qfcpp::quant;

// Reference values below come from well-known, independently published
// closed-form Black-Scholes examples, so the pricer is checked against
// externally-known results rather than only self-consistency.

TEST(EuropeanOptionTest, HullTextbookExample) {
    // Hull, "Options, Futures, and Other Derivatives": S=42, K=40, r=10%,
    // vol=20%, T=0.5y -> call ~= 4.76, put ~= 0.81. One of the most widely
    // cited worked examples in quantitative finance texts.
    OptionParameters params{
        .spot = 42.0, .strike = 40.0, .volatility = 0.2,
        .riskFreeRate = 0.1, .timeToExpiry = 0.5,
    };
    EuropeanOption option(params, OptionType::Call);
    EXPECT_NEAR(option.price(), 4.76, 1e-2);

    option.toggleType();
    EXPECT_NEAR(option.price(), 0.81, 1e-2);
}

TEST(EuropeanOptionTest, AtTheMoneyOneYearExample) {
    // Standard at-the-money example (S=K=100, r=5%, vol=20%, T=1y),
    // commonly published as call ~= 10.45, put ~= 5.57.
    OptionParameters params{
        .spot = 100.0, .strike = 100.0, .volatility = 0.2,
        .riskFreeRate = 0.05, .timeToExpiry = 1.0,
    };
    EuropeanOption option(params, OptionType::Call);
    EXPECT_NEAR(option.price(), 10.45, 1e-2);

    option.toggleType();
    EXPECT_NEAR(option.price(), 5.57, 1e-2);
}

TEST(EuropeanOptionTest, ZeroRateAtTheMoneyCallEqualsPut) {
    // r = 0, spot == strike: call and put prices must coincide exactly.
    OptionParameters params{
        .spot = 100.0, .strike = 100.0, .volatility = 0.2,
        .riskFreeRate = 0.0, .timeToExpiry = 1.0,
    };
    EuropeanOption option(params, OptionType::Call);
    const double callPrice = option.price();

    option.toggleType();
    EXPECT_NEAR(option.price(), callPrice, 1e-9);
}

TEST(EuropeanOptionTest, DeepOutOfTheMoneyLongDatedCallStaysBounded) {
    // Sanity/robustness check far from the money over a long horizon: the
    // price must stay within the model's no-arbitrage bounds and satisfy
    // put-call parity.
    OptionParameters params{
        .spot = 5.0, .strike = 100.0, .volatility = 0.3,
        .riskFreeRate = 0.08, .timeToExpiry = 30.0,
    };
    EuropeanOption option(params, OptionType::Call);
    EXPECT_GE(option.price(), 0.0);
    EXPECT_LE(option.price(), params.spot);
    EXPECT_TRUE(option.satisfiesPutCallParity());
}

TEST(EuropeanOptionTest, PutCallParityHoldsForCallAndPut) {
    OptionParameters params{
        .spot = 100.0, .strike = 95.0, .volatility = 0.25,
        .riskFreeRate = 0.04, .timeToExpiry = 0.75,
    };
    EuropeanOption call(params, OptionType::Call);
    EXPECT_TRUE(call.satisfiesPutCallParity());

    EuropeanOption put(params, OptionType::Put);
    EXPECT_TRUE(put.satisfiesPutCallParity());
}

TEST(EuropeanOptionTest, ToggleTypeFlipsBetweenCallAndPut) {
    EuropeanOption option({}, OptionType::Call);
    EXPECT_EQ(option.type(), OptionType::Call);
    option.toggleType();
    EXPECT_EQ(option.type(), OptionType::Put);
    option.toggleType();
    EXPECT_EQ(option.type(), OptionType::Call);
}

TEST(EuropeanOptionTest, PriceOverMeshMatchesPerElementPrice) {
    OptionParameters params{
        .spot = 100.0, .strike = 100.0, .volatility = 0.2,
        .riskFreeRate = 0.05, .timeToExpiry = 0.5,
    };
    EuropeanOption option(params, OptionType::Call);
    const std::vector<double> mesh = {80.0, 90.0, 100.0, 110.0, 120.0};
    const std::vector<double> prices = option.priceOverMesh(mesh);

    ASSERT_EQ(prices.size(), mesh.size());
    for (std::size_t i = 0; i < mesh.size(); ++i) {
        EXPECT_DOUBLE_EQ(prices[i], option.price(mesh[i]));
    }
}

TEST(EuropeanOptionTest, FuturesOptionCarryModelZeroesCostOfCarry) {
    // With b = 0 (Black, 1976), the call price should differ from the
    // standard stock-option model (b = r) whenever r != 0.
    OptionParameters params{
        .spot = 100.0, .strike = 100.0, .volatility = 0.2,
        .riskFreeRate = 0.05, .timeToExpiry = 1.0,
    };
    EuropeanOption stockOption(params, OptionType::Call, CarryModel::StockOption);
    EuropeanOption futuresOption(params, OptionType::Call, CarryModel::FuturesOption);
    EXPECT_NE(stockOption.price(), futuresOption.price());
}

TEST(MatrixPricerTest, PricesEachScenarioInTheSweep) {
    ParameterSweep sweep{
        .timeToExpiry = {0.5, 1.0},
        .volatility = {0.2, 0.25},
        .riskFreeRate = {0.05, 0.05},
        .strike = {100.0, 100.0},
        .spot = {100.0, 100.0},
    };
    MatrixPricer pricer{EuropeanOption{}};
    const std::vector<double> prices = pricer.price(sweep, OptionType::Call);

    ASSERT_EQ(prices.size(), 2u);

    EuropeanOption expected0({.spot = 100.0, .strike = 100.0, .volatility = 0.2,
                              .riskFreeRate = 0.05, .timeToExpiry = 0.5},
                             OptionType::Call);
    EuropeanOption expected1({.spot = 100.0, .strike = 100.0, .volatility = 0.25,
                              .riskFreeRate = 0.05, .timeToExpiry = 1.0},
                             OptionType::Call);

    EXPECT_NEAR(prices[0], expected0.price(), 1e-9);
    EXPECT_NEAR(prices[1], expected1.price(), 1e-9);
}

TEST(MatrixPricerTest, ThrowsOnMismatchedSweepLengths) {
    ParameterSweep sweep{
        .timeToExpiry = {0.5, 1.0},
        .volatility = {0.2},  // mismatched length
        .riskFreeRate = {0.05, 0.05},
        .strike = {100.0, 100.0},
        .spot = {100.0, 100.0},
    };
    MatrixPricer pricer{EuropeanOption{}};
    EXPECT_THROW((void)pricer.price(sweep, OptionType::Call), std::invalid_argument);
}
