#pragma once

#include "geometry/Point.hpp"
#include "geometry/Shape.hpp"

#include <ostream>
#include <string>

namespace qfcpp::geometry {

class Circle : public Shape {
public:
    Circle() = default;
    Circle(Point center, double radius) noexcept;

    [[nodiscard]] const Point& center() const noexcept;
    void setCenter(Point p) noexcept;
    [[nodiscard]] double radius() const noexcept;
    void setRadius(double r) noexcept;

    [[nodiscard]] double diameter() const noexcept;
    [[nodiscard]] double area() const noexcept;
    [[nodiscard]] double circumference() const noexcept;

    [[nodiscard]] std::string toString() const override;

    friend std::ostream& operator<<(std::ostream& os, const Circle& c);

private:
    Point center_;
    double radius_ = 1.0;
};

}  // namespace qfcpp::geometry
