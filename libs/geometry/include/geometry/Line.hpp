#pragma once

#include "geometry/Point.hpp"
#include "geometry/Shape.hpp"

#include <ostream>
#include <string>

namespace qfcpp::geometry {

class Line : public Shape {
public:
    Line() = default;
    Line(Point p1, Point p2) noexcept;
    Line(double x1, double y1, double x2, double y2) noexcept;

    [[nodiscard]] double length() const noexcept;
    [[nodiscard]] std::string toString() const override;

    [[nodiscard]] const Point& p1() const noexcept;
    [[nodiscard]] const Point& p2() const noexcept;
    void setP1(Point p) noexcept;
    void setP2(Point p) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const Line& l);

private:
    Point p1_;
    Point p2_;
};

}  // namespace qfcpp::geometry
