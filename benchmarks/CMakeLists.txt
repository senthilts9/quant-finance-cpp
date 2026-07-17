include(FetchContent)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.1
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

# Google Benchmark builds itself with -pedantic-errors; on very new Clang
# releases that turns a __COUNTER__ idiom in its own header into a hard
# error (-Werror=c2y-extensions) even though the library itself doesn't
# use that feature. Not our code -- silence it only for this dependency.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    foreach(_qfcpp_bm_target benchmark benchmark_main)
        if(TARGET ${_qfcpp_bm_target})
            target_compile_options(${_qfcpp_bm_target} PRIVATE -Wno-error=c2y-extensions)
        endif()
    endforeach()
endif()

add_executable(qfcpp_benchmarks
    quant_benchmarks.cpp
)

target_link_libraries(qfcpp_benchmarks PRIVATE
    qfcpp::quant
    benchmark::benchmark_main
)
