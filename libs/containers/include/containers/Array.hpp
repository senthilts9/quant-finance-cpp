#pragma once

#include "containers/ArrayException.hpp"

#include <cstddef>
#include <vector>

namespace qfcpp::containers {

// Bounds-checked, RAII dynamic array. Backed by std::vector so copy/move,
// exception safety, and memory management come from the standard library
// instead of hand-rolled new[]/delete[].
template <typename T>
class Array {
public:
    explicit Array(std::size_t size = kDefaultSize) : data_(size) {}

    Array(const Array&) = default;
    Array(Array&&) noexcept = default;
    Array& operator=(const Array&) = default;
    Array& operator=(Array&&) noexcept = default;
    ~Array() = default;

    [[nodiscard]] std::size_t size() const noexcept { return data_.size(); }
    void resize(std::size_t newSize) { data_.resize(newSize); }

    [[nodiscard]] T& at(std::size_t index) {
        checkBounds(index);
        return data_[index];
    }

    [[nodiscard]] const T& at(std::size_t index) const {
        checkBounds(index);
        return data_[index];
    }

    [[nodiscard]] T& operator[](std::size_t index) { return at(index); }
    [[nodiscard]] const T& operator[](std::size_t index) const { return at(index); }

    [[nodiscard]] static constexpr std::size_t defaultSize() noexcept { return kDefaultSize; }

private:
    void checkBounds(std::size_t index) const {
        if (index >= data_.size()) {
            throw OutOfBoundsException(index);
        }
    }

    static constexpr std::size_t kDefaultSize = 10;
    std::vector<T> data_;
};

}  // namespace qfcpp::containers
