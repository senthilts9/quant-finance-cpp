#include "geometry/Circle.hpp"

#include <numbers>
#include <sstream>

namespace qfcpp::geometry {

Circle::Circle(Point center, double radius) noexcept : center_(center), radius_(radius) {}

const Point& Circle::center() const noexcept { return center_; }
void Circle::setCenter(Point p) noexcept { center_ = p; }
double Circle::radius() const noexcept { return radius_; }
void Circle::setRadius(double r) noexcept { radius_ = r; }

double Circle::diameter() const noexcept { return radius_ * 2.0; }
double Circle::area() const noexcept { return std::numbers::pi * radius_ * radius_; }
double Circle::circumference() const noexcept { return 2.0 * std::numbers::pi * radius_; }

std::string Circle::toString() const {
    std::ostringstream oss;
    oss << Shape::toString() << ": Circle with radius " << radius_ << " and center "
        << center_.toString();
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Circle& c) {
    os << c.toString();
    return os;
}

}  // namespace qfcpp::geometry
