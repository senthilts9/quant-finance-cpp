# Performance

## Scope: what "fast" means here

A single closed-form Black-Scholes evaluation is already sub-microsecond in
native C++ — the pricing math itself was never the bottleneck. In a real
HFT system, tick-to-trade latency is dominated by network I/O, kernel
scheduling jitter, and memory allocation, not floating-point arithmetic.
So "performance work" on a pricing library means two concrete things:

1. **Throughput at scale**: repricing an entire chain (hundreds to
   thousands of strikes) on every market update, where allocation and
   function-call overhead per element start to matter.
2. **Latency predictability**: avoiding operations whose cost can spike
   unpredictably (exceptions, heap allocation, `libm` calls with
   input-dependent cost paths) in code that runs on every tick.

This document covers what was done for both, how it was measured, and what
a genuinely latency-critical deployment would still need on top of this
(kernel bypass networking, CPU pinning/affinity, huge pages, etc. — all
outside a pricing library's scope).

## What changed

- **`noexcept` on the pricing hot path.** `EuropeanOption::price()`,
  `callPrice()`, `putPrice()`, `parityResidual()`, and
  `satisfiesPutCallParity()` are all `noexcept`. None of them can actually
  throw (no allocation, no exception-throwing calls), so this was a
  correctness gap, not just an optimization — but it also lets the compiler
  drop unwind-table bookkeeping from that call path and inline more freely.

- **A branch-free, allocation-free batch kernel** (`BatchPricer.hpp`):
  `priceEuropeanCallsBatch()` takes `std::span` inputs and writes into a
  caller-provided output span. No heap allocation, no exceptions, and no
  data-dependent branches in the loop body, so it auto-vectorizes under
  `-O3` — the compiler can pack 2-4 doubles per SIMD lane without any
  hand-written intrinsics. This is the entry point to use when repricing a
  chain, instead of looping scalar `EuropeanOption::price()` calls.

- **A fixed-cost normal CDF approximation.** `fastStandardNormalCdf()`
  (Abramowitz & Stegun 26.2.17) replaces `std::erfc` in the batch path: a
  5-term polynomial plus one `exp()` call, same cost for every input.
  `std::erfc` is correctly rounded but its latency isn't guaranteed
  constant across implementations/input ranges, which matters more than
  raw average speed in a latency-sensitive loop. The approximation's error
  is bounded at < 7.5e-8 in the CDF, which is far inside the precision that
  matters for pricing (`batch_pricer_tests.cpp` checks both the bound
  itself and that batch-kernel prices match the reference scalar pricer to
  within that error, propagated through the formula).

  The library keeps **both**: `standardNormalCdf()` (full-precision,
  `std::erfc`-based) stays the default for `EuropeanOption`, since that's
  the correctness-first API most callers should use; `fastStandardNormalCdf()`
  is opt-in for the batch path where the tradeoff is explicit.

- **Struct-of-arrays scenario layout.** `ParameterSweep` (used by
  `MatrixPricer`) already stored each parameter as its own contiguous
  `std::vector<double>` rather than an array of structs — that layout is
  what makes the batch kernel's vectorization possible in the first place.

- **Build flags** (`CMakeLists.txt` options, all opt-in except LTO):
  - `QFCPP_ENABLE_LTO` (default **ON** in Release) — interprocedural
    optimization via `CMAKE_INTERPROCEDURAL_OPTIMIZATION`, gated on
    `check_ipo_supported()` so it's a no-op where unsupported.
  - `QFCPP_ENABLE_NATIVE_ARCH` (default OFF) — `-march=native` /
    `/arch:AVX2`, opt-in because the resulting binary isn't portable to
    other CPUs; enable it for local benchmarking or a deployment build
    targeting known hardware, not for a distributed binary.
  - `QFCPP_ENABLE_COVERAGE` (default OFF) — see Testing below.

## Benchmarking

Benchmarks use [Google Benchmark](https://github.com/google/benchmark)
(fetched via `FetchContent`, same pattern as GoogleTest). They're opt-in
(`QFCPP_BUILD_BENCHMARKS=ON`) so a normal build/CI run doesn't pay for
fetching and building them.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DQFCPP_BUILD_BENCHMARKS=ON
cmake --build build --target qfcpp_benchmarks
./build/benchmarks/qfcpp_benchmarks
```

Add `-DQFCPP_ENABLE_NATIVE_ARCH=ON` to benchmark with the host CPU's full
instruction set when you're measuring on the machine you'll actually run
on.

**These numbers are relative, not absolute** — re-run on your own target
hardware before trusting them for a deployment decision; this session ran
on a virtualized sandbox (12x2.6GHz), not a production trading box with
CPU pinning and frequency-scaling disabled.

### Measured results — and an honest finding

Two runs, same machine: default `-O3` (no target-CPU flags), and again
with `QFCPP_ENABLE_NATIVE_ARCH=ON` (`-march=native`) to give the
auto-vectorizer the widest instruction set available.

| Benchmark | `-O3` (baseline) | `-O3 -march=native` |
|---|---|---|
| `BM_SingleOptionPrice` | 17.7 ns | 18.4 ns |
| `BM_StandardNormalCdf` (`std::erfc`) | 7.85 ns | 8.48 ns |
| `BM_FastStandardNormalCdf` (polynomial) | 9.52 ns | 8.91 ns |
| `BM_PriceOverMesh_Scalar` @10k | 21.97M items/s | 18.38M items/s |
| `BM_PriceEuropeanCallsBatch` @10k | 21.50M items/s | 20.00M items/s |
| `BM_MatrixPricer_Sweep` @10k | 23.46M items/s | 24.98M items/s |

**The hypothesis going in was that the polynomial CDF approximation and
branch-free batch kernel would clearly beat `std::erfc` and the scalar
loop. The measurements don't support that** — on this toolchain
(llvm-mingw/Clang 22, UCRT), `std::erfc` is already fast, the polynomial
approximation is not consistently faster (7.85-8.48 ns vs 8.91-9.52 ns,
i.e. comparable to slightly slower), and the batch kernel's throughput at
10k elements is statistically indistinguishable from the plain scalar
mesh loop — sometimes slightly ahead, sometimes slightly behind, with
run-to-run variance (e.g. the scalar path swung from 21.97M to 18.38M
items/s between the two runs) larger than the differences between kernels.
`-march=native` didn't produce a clear win either, which suggests the
auto-vectorizer isn't packing meaningfully wider SIMD lanes for this
workload regardless of target flags, at least not in a way this
benchmark's granularity can resolve.

What *is* real, independent of these numbers: the `noexcept` annotations
are a correctness fix (none of these functions could throw, so the
exception specification was simply wrong before), and the batch kernel's
zero-allocation, array-in/array-out shape is the correct API to expose for
a caller building a truly zero-allocation hot loop elsewhere in a larger
system — even though, measured here, it doesn't itself outrun the simpler
scalar version. Chasing a micro-optimization that a benchmark doesn't
actually support would be exactly the mistake this document is trying to
avoid; reporting that plainly is more useful than a nicer-sounding result.

**What this means for genuine HFT-grade work**: don't take
`fastStandardNormalCdf`/`priceEuropeanCallsBatch` as a proven speedup on
faith — re-benchmark on your actual target hardware with CPU affinity
pinned and frequency scaling disabled (Google Benchmark supports
`--benchmark_repetitions` and reports variance for exactly this reason),
profile with `perf stat`/VTune to see actual vectorization and cache
behavior rather than inferring it from wall-clock alone, and consider
whether the real win is architectural (avoiding allocation/exceptions
entirely in the call path) rather than in the arithmetic itself. For
genuinely latency-critical repricing, the next real steps beyond this
library would be hand-written SIMD intrinsics (AVX-512 `_mm512_*` on 8
doubles at once) profiled against a realistic production data pattern,
not a synthetic uniform mesh.

## Testing: coverage strategy

Unit tests (`tests/`, GoogleTest) are organized by library and checked
against externally-known values, not just internal self-consistency:

| Area | Test file | What's covered |
|---|---|---|
| Option pricing correctness | `quant_tests.cpp` | Published textbook reference prices (Hull's example, standard ATM case), zero-rate call/put equality, deep-OTM/long-dated no-arbitrage bounds, put-call parity, carry-model differentiation (stock vs. futures), mesh-vs-scalar consistency |
| Batch/fast-path correctness | `batch_pricer_tests.cpp` | Fast CDF approximation vs. reference CDF across `[-5, 5]`, batch kernel vs. scalar `EuropeanOption` pricer agreement, Hull example via the batch path |
| Sweep pricing | `quant_tests.cpp` (`MatrixPricerTest`) | Multi-scenario sweep correctness, mismatched-length input rejection |
| Container correctness/safety | `containers_tests.cpp` | Bounds checking and exception types, copy semantics (deep copy, not aliasing), arithmetic operator correctness, stack overflow/underflow |
| Geometry correctness | `geometry_tests.cpp` | Distance/area/circumference formulas, operator overloads, polymorphic `toString()`, unique-id invariant |

### Generating a coverage report

```sh
cmake -S . -B build-coverage -DCMAKE_BUILD_TYPE=Debug -DQFCPP_ENABLE_COVERAGE=ON
cmake --build build-coverage
ctest --test-dir build-coverage

# GCC:
lcov --directory build-coverage --capture --output-file coverage.info
genhtml coverage.info --output-directory coverage-html

# Clang (llvm-mingw/clang toolchains use llvm-cov + llvm-profdata instead
# of gcov-format lcov; use llvm-cov gcov as the gcov tool):
lcov --gcov-tool "llvm-cov gcov" --directory build-coverage --capture --output-file coverage.info
genhtml coverage.info --output-directory coverage-html
```

Open `coverage-html/index.html` to see per-line/per-branch coverage. When
adding a new function to `libs/`, the checklist is: does it have a test
that (a) hits its normal-case behavior, (b) hits every branch, and (c) for
anything that can throw, a test that verifies the specific exception type,
not just "it throws something"? All three container exception paths
(`OutOfBoundsException`, `StackFullException`, `StackEmptyException`) and
the `MatrixPricer` mismatched-length rejection are covered this way.

### A concrete example: what running this actually found

Running the coverage build above against `EuropeanOption.cpp` as it stood
before this document was written showed **79.69% line coverage** — and
named the exact gaps: `standardNormalPdf()` was never called by any test,
the `ContinuousDividendYieldCarryModel` and `CurrencyOption` branches of
`costOfCarry()` were untested (only `StockOption` and `FuturesOption`
had coverage), and `toString()` had zero test coverage at all.

Four tests were added in response —
`ContinuousDividendYieldCarryModelLowersCallPrice`,
`CurrencyOptionCarryModelMatchesStockOptionToday`,
`ToStringIncludesTypeAndParameters`, and
`PdfPeaksAtZeroAndIsSymmetric` (all in `quant_tests.cpp`) — bringing the
same file to **98.44%**. The `CurrencyOption` test doubles as documentation
of a real limitation the coverage exercise surfaced: this library doesn't
yet model a separate foreign risk-free rate, so Garman-Kohlhagen currency
options currently price identically to plain stock options (b = r rather
than the correct b = domestic_r − foreign_r) — a known simplification now
pinned down by a test instead of sitting as a silent gap, so a future fix
has something to update rather than discovering the behavior change by
accident.

The one line still uncovered is the fallback `return` after an exhaustive
`switch` over `CarryModel`'s four enumerators — genuinely unreachable for
any valid enum value. That's the right kind of gap to leave: coverage
tooling tells you *where* you haven't tested, not whether every uncovered
line is a bug. Chasing 100% by testing unreachable defensive code adds
noise, not confidence — the goal is covering every reachable behavior, not
every source line.
