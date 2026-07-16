// Demonstrates polymorphic dispatch across the Shape hierarchy: each shape
// overrides toString() and is invoked through a common base-class pointer.
#include "geometry/Circle.hpp"
#include "geometry/Line.hpp"
#include "geometry/Point.hpp"
#include "geometry/Shape.hpp"

#include <iostream>
#include <memory>
#include <vector>

int main() {
    using namespace qfcpp::geometry;

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Point>(3.0, 4.0));
    shapes.push_back(std::make_unique<Line>(Point{0.0, 0.0}, Point{3.0, 4.0}));
    shapes.push_back(std::make_unique<Circle>(Point{1.0, 1.0}, 2.5));

    for (const auto& shape : shapes) {
        std::cout << shape->toString() << '\n';
    }

    const Point origin{0.0, 0.0};
    const Point p{3.0, 4.0};
    std::cout << "\nDistance from origin to (3, 4): " << p.distanceTo(origin) << '\n';

    const Circle circle{Point{0.0, 0.0}, 2.0};
    std::cout << "Circle area: " << circle.area() << ", circumference: "
              << circle.circumference() << '\n';

    return 0;
}
