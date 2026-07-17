// Benchmarks for the quant pricing library. Run with:
//   cmake --build build --target qfcpp_benchmarks
//   ./build/benchmarks/qfcpp_benchmarks
//
// These exist so performance claims/changes are backed by numbers measured
// on this machine, not assumptions -- see PERFORMANCE.md for methodology
// and results.
#include "quant/BatchPricer.hpp"
#include "quant/EuropeanOption.hpp"
#include "quant/MatrixPricer.hpp"
#include "quant/OptionTypes.hpp"

#include <benchmark/benchmark.h>

#include <vector>

using namespace qfcpp::quant;

namespace {

OptionParameters SampleParams() {
    return OptionParameters{
        .spot = 100.0, .strike = 100.0, .volatility = 0.2,
        .riskFreeRate = 0.05, .timeToExpiry = 0.5,
    };
}

}  // namespace

static void BM_SingleOptionPrice(benchmark::State& state) {
    EuropeanOption option(SampleParams(), OptionType::Call);
    for (auto _ : state) {
        benchmark::DoNotOptimize(option.price());
    }
}
BENCHMARK(BM_SingleOptionPrice);

static void BM_StandardNormalCdf(benchmark::State& state) {
    double x = 0.35;
    for (auto _ : state) {
        benchmark::DoNotOptimize(standardNormalCdf(x));
        x += 1e-9;  // avoid the optimizer hoisting a constant call out of the loop
    }
}
BENCHMARK(BM_StandardNormalCdf);

static void BM_FastStandardNormalCdf(benchmark::State& state) {
    double x = 0.35;
    for (auto _ : state) {
        benchmark::DoNotOptimize(fastStandardNormalCdf(x));
        x += 1e-9;
    }
}
BENCHMARK(BM_FastStandardNormalCdf);

// Scalar loop over EuropeanOption::price(), one call per element -- the
// "naive" batch pricing approach.
static void BM_PriceOverMesh_Scalar(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    EuropeanOption option(SampleParams(), OptionType::Call);
    std::vector<double> mesh(n);
    for (std::size_t i = 0; i < n; ++i) {
        mesh[i] = 50.0 + static_cast<double>(i) * 0.1;
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(option.priceOverMesh(mesh));
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_PriceOverMesh_Scalar)->Arg(100)->Arg(1000)->Arg(10000);

// Branchless batch kernel operating directly on contiguous arrays with the
// fast CDF approximation -- the SIMD-friendly path.
static void BM_PriceEuropeanCallsBatch(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    std::vector<double> spot(n), strike(n, 100.0), vol(n, 0.2), rate(n, 0.05), expiry(n, 0.5);
    std::vector<double> out(n);
    for (std::size_t i = 0; i < n; ++i) {
        spot[i] = 50.0 + static_cast<double>(i) * 0.1;
    }

    for (auto _ : state) {
        priceEuropeanCallsBatch(spot, strike, vol, rate, expiry, out);
        benchmark::DoNotOptimize(out.data());
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_PriceEuropeanCallsBatch)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_MatrixPricer_Sweep(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    ParameterSweep sweep;
    sweep.timeToExpiry.assign(n, 0.5);
    sweep.volatility.assign(n, 0.2);
    sweep.riskFreeRate.assign(n, 0.05);
    sweep.strike.assign(n, 100.0);
    sweep.spot.resize(n);
    for (std::size_t i = 0; i < n; ++i) {
        sweep.spot[i] = 50.0 + static_cast<double>(i) * 0.1;
    }
    MatrixPricer pricer{EuropeanOption{}};

    for (auto _ : state) {
        benchmark::DoNotOptimize(pricer.price(sweep, OptionType::Call));
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_MatrixPricer_Sweep)->Arg(100)->Arg(1000)->Arg(10000);
