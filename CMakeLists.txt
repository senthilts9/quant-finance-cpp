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
