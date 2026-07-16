#include "quant/MatrixPricer.hpp"

#include <stdexcept>

namespace qfcpp::quant {

std::vector<double> MatrixPricer::price(const ParameterSweep& sweep, OptionType type) const {
    const std::size_t n = sweep.timeToExpiry.size();
    if (sweep.volatility.size() != n || sweep.riskFreeRate.size() != n ||
        sweep.strike.size() != n || sweep.spot.size() != n) {
        throw std::invalid_argument("MatrixPricer: all parameter sweep vectors must have equal length");
    }

    EuropeanOption option = baseOption_;
    if (option.type() != type) {
        option.toggleType();
    }

    std::vector<double> prices;
    prices.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        option.setTimeToExpiry(sweep.timeToExpiry[i]);
        option.setVolatility(sweep.volatility[i]);
        option.setRiskFreeRate(sweep.riskFreeRate[i]);
        option.setStrike(sweep.strike[i]);
        prices.push_back(option.price(sweep.spot[i]));
    }
    return prices;
}

}  // namespace qfcpp::quant
