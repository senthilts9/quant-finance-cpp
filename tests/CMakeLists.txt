include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(qfcpp_tests
    geometry_tests.cpp
    containers_tests.cpp
    quant_tests.cpp
    batch_pricer_tests.cpp
)

target_link_libraries(qfcpp_tests PRIVATE
    qfcpp::geometry
    qfcpp::containers
    qfcpp::quant
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(qfcpp_tests)
