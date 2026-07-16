#pragma once

#include <atomic>
#include <string>

namespace qfcpp::geometry {

// Base type for a small CAD-style shape hierarchy. Each instance gets a
// unique, monotonically increasing id, so output stays deterministic and
// testable.
class Shape {
public:
    Shape();
    explicit Shape(int id) noexcept;

    Shape(const Shape&) = default;
    Shape(Shape&&) = default;
    Shape& operator=(const Shape&) = default;
    Shape& operator=(Shape&&) = default;
    virtual ~Shape() = default;

    [[nodiscard]] int id() const noexcept;
    [[nodiscard]] virtual std::string toString() const;

private:
    static std::atomic<int> next_id_;
    int id_;
};

}  // namespace qfcpp::geometry
