add_library(geometry
    src/Shape.cpp
    src/Point.cpp
    src/Line.cpp
    src/Circle.cpp
)
add_library(qfcpp::geometry ALIAS geometry)

target_include_directories(geometry PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(geometry PUBLIC cxx_std_20)
