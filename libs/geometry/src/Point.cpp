#include "geometry/Point.hpp"

#include <cmath>
#include <sstream>

namespace qfcpp::geometry {

Point::Point(double x, double y) noexcept : x_(x), y_(y) {}

double Point::x() const noexcept { return x_; }
double Point::y() const noexcept { return y_; }
void Point::setX(double x) noexcept { x_ = x; }
void Point::setY(double y) noexcept { y_ = y; }

double Point::distanceToOrigin() const noexcept { return std::hypot(x_, y_); }

double Point::distanceTo(const Point& other) const noexcept {
    return std::hypot(x_ - other.x_, y_ - other.y_);
}

std::string Point::toString() const {
    std::ostringstream oss;
    oss << Shape::toString() << ": Point(" << x_ << ", " << y_ << ")";
    return oss.str();
}

Point Point::operator-() const noexcept { return {-x_, -y_}; }

Point Point::operator*(double factor) const noexcept { return {x_ * factor, y_ * factor}; }

Point Point::operator+(const Point& other) const noexcept {
    return {x_ + other.x_, y_ + other.y_};
}

Point& Point::operator*=(double factor) noexcept {
    x_ *= factor;
    y_ *= factor;
    return *this;
}

bool Point::operator==(const Point& other) const noexcept {
    return x_ == other.x_ && y_ == other.y_;
}

std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << p.toString();
    return os;
}

}  // namespace qfcpp::geometry
