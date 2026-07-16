// Prices a European option, checks put-call parity, and prices a
// mesh/matrix of scenarios.
#include "quant/EuropeanOption.hpp"
#include "quant/MatrixPricer.hpp"
#include "quant/OptionTypes.hpp"

#include <iostream>
#include <vector>

using namespace qfcpp::quant;

namespace {

std::vector<double> createMesh(double start, double end, double step) {
    std::vector<double> mesh;
    for (double v = start; v <= end; v += step) {
        mesh.push_back(v);
    }
    return mesh;
}

}  // namespace

int main() {
    std::cout << "--- Single option pricing ---\n";

    // Hull's canonical textbook example (Options, Futures, and Other
    // Derivatives): S=42, K=40, r=10%, vol=20%, T=0.5y -> call ~= 4.76,
    // put ~= 0.81.
    OptionParameters params{
        .spot = 42.0,
        .strike = 40.0,
        .volatility = 0.2,
        .riskFreeRate = 0.1,
        .timeToExpiry = 0.5,
    };
    EuropeanOption option(params, OptionType::Call);

    std::cout << option.toString() << '\n';
    std::cout << "Call price: " << option.price() << " (reference: ~4.76)\n";

    option.toggleType();
    std::cout << "Put price: " << option.price() << " (reference: ~0.81)\n";

    std::cout << "\n--- Put-call parity ---\n";
    std::cout << "Parity residual: " << option.parityResidual() << '\n';
    std::cout << "Satisfies parity: " << std::boolalpha << option.satisfiesPutCallParity() << '\n';

    std::cout << "\n--- Price over a spot mesh ---\n";
    option.toggleType();  // back to Call
    const std::vector<double> mesh = createMesh(10.0, 50.0, 5.0);
    const std::vector<double> prices = option.priceOverMesh(mesh);
    for (std::size_t i = 0; i < mesh.size(); ++i) {
        std::cout << "spot=" << mesh[i] << " -> price=" << prices[i] << '\n';
    }

    std::cout << "\n--- Scenario sweep (matrix) pricing ---\n";
    ParameterSweep sweep{
        .timeToExpiry = {0.25, 0.5, 0.75, 1.0},
        .volatility = {0.2, 0.25, 0.3, 0.35},
        .riskFreeRate = {0.03, 0.04, 0.05, 0.06},
        .strike = {95.0, 100.0, 105.0, 110.0},
        .spot = {100.0, 100.0, 100.0, 100.0},
    };
    MatrixPricer pricer{EuropeanOption{}};
    const std::vector<double> sweepPrices = pricer.price(sweep, OptionType::Call);
    for (double price : sweepPrices) {
        std::cout << "price=" << price << '\n';
    }

    return 0;
}
