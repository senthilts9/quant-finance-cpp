# quant-finance-cpp

[![CI](https://github.com/senthilts9/quant-finance-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/senthilts9/quant-finance-cpp/actions/workflows/ci.yml)
![C++](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

A modern C++20 library for closed-form European option pricing, built and
tested with CMake + GoogleTest, with CI on Linux (GCC, Clang) and Windows
(MSVC).

A portfolio project exploring core C++ Programming for Financial
Engineering topics — closed-form option pricing, object-oriented design,
templates, and RAII resource management — organized into three reusable
libraries with full unit test coverage and CI.

## What's here

```
libs/
  geometry/    Shape / Point / Line / Circle — small polymorphic CAD kernel
  containers/  Array<T> / NumericArray<T> / Stack<T> — RAII, bounds-checked
  quant/       EuropeanOption / MatrixPricer — Black-Scholes-family pricer
apps/
  geometry_demo/          polymorphic dispatch over Shape*
  containers_demo/        Stack<Point> push/pop + exception handling
  option_pricing_demo/    single option, mesh, and batch pricing
tests/
  geometry_tests.cpp      GoogleTest suite for libs/geometry
  containers_tests.cpp    GoogleTest suite for libs/containers
  quant_tests.cpp         GoogleTest suite for libs/quant, incl. regression
                          tests against published closed-form reference prices
```

### `libs/quant` — the main deliverable

`EuropeanOption` prices European calls/puts in closed form under a
configurable cost-of-carry model (`CarryModel`): plain stock options,
continuous dividend yield (Merton), futures options (Black), and currency
options (Garman-Kohlhagen). It supports:

- `price(spot)` / `price()` — closed-form price at a given or current spot.
- `priceOverMesh(spots)` — vectorized pricing across a spot mesh.
- `parityResidual()` / `satisfiesPutCallParity()` — numerically robust
  put-call parity check with a tolerance, rather than exact floating-point
  equality (which is essentially never true for derived quantities).
- `toggleType()` — flip between call and put in place.

`MatrixPricer` prices a batch of scenarios (a "parameter sweep" across
time-to-expiry, volatility, rate, strike, and spot) against one option
configuration in a single call.

The normal PDF/CDF used internally are implemented with `std::erfc` from
`<cmath>`, so the library has **no third-party dependency**.

## Building

Requires a C++20 compiler (GCC 13+, Clang 16+, or MSVC 19.36+) and CMake
3.20+.

```sh
cmake --preset default
cmake --build --preset default
ctest --preset default
```

On Windows with Visual Studio:

```sh
cmake --preset windows-msvc
cmake --build --preset windows-msvc
ctest --test-dir build/windows-msvc -C Release --output-on-failure
```

Demo executables are built as `geometry_demo`, `containers_demo`, and
`option_pricing_demo` in the build directory; run them directly to see
sample output.

## Testing

Unit tests use [GoogleTest](https://github.com/google/googletest) (fetched
automatically via `FetchContent`) and run via CTest. The `quant` test suite
checks the pricer against published closed-form Black-Scholes reference
values (e.g. Hull's canonical textbook example) so correctness is verified
against independently-known results, not just self-consistency.

CI (`.github/workflows/ci.yml`) builds and runs the full test suite on
Ubuntu (GCC, Clang) and Windows (MSVC) on every push and pull request.

## License

[MIT](LICENSE).
