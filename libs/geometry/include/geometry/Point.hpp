#pragma once

#include "geometry/Shape.hpp"

#include <ostream>
#include <string>

namespace qfcpp::geometry {

class Point : public Shape {
public:
    Point() = default;
    Point(double x, double y) noexcept;

    [[nodiscard]] double x() const noexcept;
    [[nodiscard]] double y() const noexcept;
    void setX(double x) noexcept;
    void setY(double y) noexcept;

    // Euclidean distance to the origin.
    [[nodiscard]] double distanceToOrigin() const noexcept;
    // Euclidean distance to another point.
    [[nodiscard]] double distanceTo(const Point& other) const noexcept;

    [[nodiscard]] std::string toString() const override;

    [[nodiscard]] Point operator-() const noexcept;
    [[nodiscard]] Point operator*(double factor) const noexcept;
    [[nodiscard]] Point operator+(const Point& other) const noexcept;
    Point& operator*=(double factor) noexcept;
    [[nodiscard]] bool operator==(const Point& other) const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const Point& p);

private:
    double x_ = 0.0;
    double y_ = 0.0;
};

}  // namespace qfcpp::geometry
