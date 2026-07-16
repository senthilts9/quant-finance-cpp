#include "quant/EuropeanOption.hpp"

#include <cmath>
#include <numbers>
#include <sstream>

namespace qfcpp::quant {

double standardNormalPdf(double x) noexcept {
    return std::exp(-0.5 * x * x) / std::sqrt(2.0 * std::numbers::pi);
}

double standardNormalCdf(double x) noexcept {
    return 0.5 * std::erfc(-x / std::numbers::sqrt2);
}

EuropeanOption::EuropeanOption(OptionParameters params, OptionType type, CarryModel model)
    : params_(params), type_(type), model_(model) {}

double EuropeanOption::costOfCarry() const noexcept {
    switch (model_) {
        case CarryModel::StockOption:
            return params_.riskFreeRate;
        case CarryModel::ContinuousDividendYield:
            return params_.riskFreeRate - params_.dividendYield;
        case CarryModel::FuturesOption:
            return 0.0;
        case CarryModel::CurrencyOption:
            return params_.riskFreeRate;
    }
    return params_.riskFreeRate;
}

double EuropeanOption::callPrice(double spot) const {
    const double b = costOfCarry();
    const double sigma = params_.volatility;
    const double t = params_.timeToExpiry;
    const double k = params_.strike;
    const double r = params_.riskFreeRate;

    const double d1 = (std::log(spot / k) + (b + sigma * sigma / 2.0) * t) / (sigma * std::sqrt(t));
    const double d2 = d1 - sigma * std::sqrt(t);

    return spot * std::exp((b - r) * t) * standardNormalCdf(d1) -
           k * std::exp(-r * t) * standardNormalCdf(d2);
}

double EuropeanOption::putPrice(double spot) const {
    const double b = costOfCarry();
    const double sigma = params_.volatility;
    const double t = params_.timeToExpiry;
    const double k = params_.strike;
    const double r = params_.riskFreeRate;

    const double d1 = (std::log(spot / k) + (b + sigma * sigma / 2.0) * t) / (sigma * std::sqrt(t));
    const double d2 = d1 - sigma * std::sqrt(t);

    return k * std::exp(-r * t) * standardNormalCdf(-d2) -
           spot * std::exp((b - r) * t) * standardNormalCdf(-d1);
}

double EuropeanOption::price(double spot) const {
    return type_ == OptionType::Call ? callPrice(spot) : putPrice(spot);
}

std::vector<double> EuropeanOption::priceOverMesh(const std::vector<double>& spotMesh) const {
    std::vector<double> result;
    result.reserve(spotMesh.size());
    for (double s : spotMesh) {
        result.push_back(price(s));
    }
    return result;
}

double EuropeanOption::parityResidual() const {
    const double s = params_.spot;
    const double k = params_.strike;
    const double discountedStrike = k * std::exp(-params_.riskFreeRate * params_.timeToExpiry);

    if (type_ == OptionType::Call) {
        return (callPrice(s) + discountedStrike) - (putPrice(s) + s);
    }
    return (putPrice(s) + s) - (callPrice(s) + discountedStrike);
}

bool EuropeanOption::satisfiesPutCallParity(double tolerance) const {
    return std::abs(parityResidual()) < tolerance;
}

void EuropeanOption::toggleType() noexcept {
    type_ = (type_ == OptionType::Call) ? OptionType::Put : OptionType::Call;
}

std::string EuropeanOption::toString() const {
    std::ostringstream oss;
    oss << "EuropeanOption[type=" << (type_ == OptionType::Call ? "Call" : "Put")
        << ", spot=" << params_.spot << ", strike=" << params_.strike
        << ", volatility=" << params_.volatility << ", riskFreeRate=" << params_.riskFreeRate
        << ", timeToExpiry=" << params_.timeToExpiry << "]";
    return oss.str();
}

}  // namespace qfcpp::quant
