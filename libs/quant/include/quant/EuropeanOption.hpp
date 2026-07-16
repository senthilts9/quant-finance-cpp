#pragma once

#include "quant/OptionTypes.hpp"

#include <string>
#include <vector>

namespace qfcpp::quant {

// Standard normal probability density and cumulative distribution functions,
// implemented with std::erfc so the library has no external math dependency.
[[nodiscard]] double standardNormalPdf(double x) noexcept;
[[nodiscard]] double standardNormalCdf(double x) noexcept;

// Closed-form European option pricer under the generalized Black-Scholes
// framework (Black-Scholes-Merton with a configurable cost of carry).
class EuropeanOption {
public:
    EuropeanOption() = default;
    explicit EuropeanOption(OptionParameters params,
                             OptionType type = OptionType::Call,
                             CarryModel model = CarryModel::StockOption);

    [[nodiscard]] double price(double spot) const;
    [[nodiscard]] double price() const { return price(params_.spot); }
    [[nodiscard]] std::vector<double> priceOverMesh(const std::vector<double>& spotMesh) const;

    // Residual of the put-call parity identity, evaluated at the option's
    // own spot/strike. Zero (within tolerance) means parity holds.
    [[nodiscard]] double parityResidual() const;
    [[nodiscard]] bool satisfiesPutCallParity(double tolerance = 1e-6) const;

    void toggleType() noexcept;
    [[nodiscard]] OptionType type() const noexcept { return type_; }
    [[nodiscard]] CarryModel carryModel() const noexcept { return model_; }

    void setSpot(double s) noexcept { params_.spot = s; }
    void setStrike(double k) noexcept { params_.strike = k; }
    void setVolatility(double sigma) noexcept { params_.volatility = sigma; }
    void setRiskFreeRate(double r) noexcept { params_.riskFreeRate = r; }
    void setTimeToExpiry(double t) noexcept { params_.timeToExpiry = t; }
    void setDividendYield(double q) noexcept { params_.dividendYield = q; }

    [[nodiscard]] const OptionParameters& parameters() const noexcept { return params_; }

    [[nodiscard]] std::string toString() const;

private:
    [[nodiscard]] double costOfCarry() const noexcept;
    [[nodiscard]] double callPrice(double spot) const;
    [[nodiscard]] double putPrice(double spot) const;

    OptionParameters params_{};
    OptionType type_ = OptionType::Call;
    CarryModel model_ = CarryModel::StockOption;
};

}  // namespace qfcpp::quant
