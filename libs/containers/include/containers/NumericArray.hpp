#pragma once

#include "containers/Array.hpp"
#include "containers/ArrayException.hpp"

namespace qfcpp::containers {

// Arithmetic extension of Array<T>. Uses compound-assignment operators
// (*=, +=) rather than operator*/operator+ that would mutate `this` while
// pretending to return a new value, which would contradict normal C++
// operator semantics.
template <typename T>
class NumericArray : public Array<T> {
public:
    using Array<T>::Array;

    [[nodiscard]] T dotProduct(const NumericArray<T>& other) const {
        if (this->size() != other.size()) {
            throw ArrayException("NumericArray: sizes must match for dot product");
        }
        T sum{};
        for (std::size_t i = 0; i < this->size(); ++i) {
            sum += this->at(i) * other.at(i);
        }
        return sum;
    }

    NumericArray<T>& operator*=(const T& factor) {
        for (std::size_t i = 0; i < this->size(); ++i) {
            this->at(i) *= factor;
        }
        return *this;
    }

    NumericArray<T>& operator+=(const NumericArray<T>& other) {
        if (this->size() != other.size()) {
            throw ArrayException("NumericArray: sizes must match for addition");
        }
        for (std::size_t i = 0; i < this->size(); ++i) {
            this->at(i) += other.at(i);
        }
        return *this;
    }
};

}  // namespace qfcpp::containers
