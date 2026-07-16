#pragma once

namespace qfcpp::quant {

enum class OptionType { Call, Put };

// Determines how cost-of-carry (b) is derived from the risk-free rate.
// Mirrors the classic Black-Scholes family of generalized carry models.
enum class CarryModel {
    StockOption,             // b = r
    ContinuousDividendYield,  // b = r - q  (Merton, 1973)
    FuturesOption,            // b = 0      (Black, 1976)
    CurrencyOption            // b = r      (Garman-Kohlhagen, 1983; rf handled via dividendYield)
};

struct OptionParameters {
    double spot = 100.0;
    double strike = 100.0;
    double volatility = 0.2;
    double riskFreeRate = 0.05;
    double timeToExpiry = 0.5;
    double dividendYield = 0.0;
};

}  // namespace qfcpp::quant
