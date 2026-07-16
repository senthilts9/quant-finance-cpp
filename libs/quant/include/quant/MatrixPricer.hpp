#pragma once

#include "quant/EuropeanOption.hpp"

#include <vector>

namespace qfcpp::quant {

// A parallel sweep of option parameters: element i across every vector
// describes one scenario. All vectors must be the same length.
struct ParameterSweep {
    std::vector<double> timeToExpiry;
    std::vector<double> volatility;
    std::vector<double> riskFreeRate;
    std::vector<double> strike;
    std::vector<double> spot;
};

// Prices a batch of scenarios against a single base option configuration.
class MatrixPricer {
public:
    explicit MatrixPricer(EuropeanOption baseOption) : baseOption_(baseOption) {}

    [[nodiscard]] std::vector<double> price(const ParameterSweep& sweep, OptionType type) const;

private:
    EuropeanOption baseOption_;
};

}  // namespace qfcpp::quant
