#include "geometry/Line.hpp"

#include <sstream>

namespace qfcpp::geometry {

Line::Line(Point p1, Point p2) noexcept : p1_(p1), p2_(p2) {}

Line::Line(double x1, double y1, double x2, double y2) noexcept
    : p1_(x1, y1), p2_(x2, y2) {}

double Line::length() const noexcept { return p1_.distanceTo(p2_); }

std::string Line::toString() const {
    std::ostringstream oss;
    oss << Shape::toString() << ": Line[Point(" << p1_.x() << ", " << p1_.y()
        << "), Point(" << p2_.x() << ", " << p2_.y() << ")]";
    return oss.str();
}

const Point& Line::p1() const noexcept { return p1_; }
const Point& Line::p2() const noexcept { return p2_; }
void Line::setP1(Point p) noexcept { p1_ = p; }
void Line::setP2(Point p) noexcept { p2_ = p; }

std::ostream& operator<<(std::ostream& os, const Line& l) {
    os << l.toString();
    return os;
}

}  // namespace qfcpp::geometry
