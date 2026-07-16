#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

namespace qfcpp::containers {

class ArrayException : public std::runtime_error {
public:
    explicit ArrayException(const std::string& message) : std::runtime_error(message) {}
};

class OutOfBoundsException : public ArrayException {
public:
    explicit OutOfBoundsException(std::size_t index)
        : ArrayException("Array index out of bounds at index: " + std::to_string(index)),
          index_(index) {}

    [[nodiscard]] std::size_t index() const noexcept { return index_; }

private:
    std::size_t index_;
};

}  // namespace qfcpp::containers
