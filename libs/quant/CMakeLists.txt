add_library(quant
    src/EuropeanOption.cpp
    src/MatrixPricer.cpp
)
add_library(qfcpp::quant ALIAS quant)

target_include_directories(quant PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(quant PUBLIC cxx_std_20)
