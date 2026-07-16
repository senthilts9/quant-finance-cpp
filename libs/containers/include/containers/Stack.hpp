#pragma once

#include "containers/Array.hpp"
#include "containers/StackException.hpp"

#include <cstddef>

namespace qfcpp::containers {

// Fixed-capacity LIFO stack. Capacity is a constructor argument rather than
// a non-type template parameter, so stacks of different capacities are the
// same type and can be assigned/compared.
template <typename T>
class Stack {
public:
    explicit Stack(std::size_t capacity) : data_(capacity), top_(0) {}

    void push(const T& value) {
        if (top_ >= data_.size()) {
            throw StackFullException();
        }
        data_[top_++] = value;
    }

    [[nodiscard]] T pop() {
        if (top_ == 0) {
            throw StackEmptyException();
        }
        return data_[--top_];
    }

    [[nodiscard]] std::size_t size() const noexcept { return top_; }
    [[nodiscard]] std::size_t capacity() const noexcept { return data_.size(); }
    [[nodiscard]] bool empty() const noexcept { return top_ == 0; }

private:
    Array<T> data_;
    std::size_t top_;
};

}  // namespace qfcpp::containers
