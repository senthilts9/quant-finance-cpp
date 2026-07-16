#include "geometry/Shape.hpp"

#include <sstream>

namespace qfcpp::geometry {

std::atomic<int> Shape::next_id_{1};

Shape::Shape() : id_(next_id_.fetch_add(1, std::memory_order_relaxed)) {}

Shape::Shape(int id) noexcept : id_(id) {}

int Shape::id() const noexcept { return id_; }

std::string Shape::toString() const {
    std::ostringstream oss;
    oss << "Shape[id=" << id_ << "]";
    return oss.str();
}

}  // namespace qfcpp::geometry
