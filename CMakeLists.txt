cmake_minimum_required(VERSION 3.20)

project(quant-finance-cpp
    VERSION 1.0.0
    DESCRIPTION "Modern C++20 quantitative finance library: European option pricing, a small geometry kernel, and generic RAII containers"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    endif()
endif()

option(QFCPP_BUILD_APPS "Build the demo applications" ON)
option(QFCPP_BUILD_TESTS "Build the test suite" ON)
option(QFCPP_BUILD_BENCHMARKS "Build the Google Benchmark performance suite" OFF)
option(QFCPP_ENABLE_LTO "Enable link-time/interprocedural optimization in Release builds" ON)
option(QFCPP_ENABLE_NATIVE_ARCH "Compile for the host CPU's instruction set (-march=native); not portable to other machines" OFF)
option(QFCPP_ENABLE_COVERAGE "Instrument with --coverage for gcov/llvm-cov (GCC/Clang only)" OFF)

if(QFCPP_ENABLE_LTO AND CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT QFCPP_IPO_SUPPORTED OUTPUT QFCPP_IPO_ERROR)
    if(QFCPP_IPO_SUPPORTED)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(STATUS "LTO requested but not supported: ${QFCPP_IPO_ERROR}")
    endif()
endif()

if(QFCPP_ENABLE_NATIVE_ARCH)
    if(MSVC)
        add_compile_options(/arch:AVX2)
    else()
        add_compile_options(-march=native)
    endif()
endif()

if(QFCPP_ENABLE_COVERAGE)
    if(MSVC)
        message(FATAL_ERROR "QFCPP_ENABLE_COVERAGE requires GCC or Clang, not MSVC")
    endif()
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

add_subdirectory(libs/geometry)
add_subdirectory(libs/containers)
add_subdirectory(libs/quant)

if(QFCPP_BUILD_APPS)
    add_subdirectory(apps/geometry_demo)
    add_subdirectory(apps/containers_demo)
    add_subdirectory(apps/option_pricing_demo)
endif()

if(QFCPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(QFCPP_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
