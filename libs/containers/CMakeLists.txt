# Header-only template library.
add_library(containers INTERFACE)
add_library(qfcpp::containers ALIAS containers)

target_include_directories(containers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(containers INTERFACE cxx_std_20)
