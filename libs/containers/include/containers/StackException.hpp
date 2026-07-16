#pragma once

#include <stdexcept>
#include <string>

namespace qfcpp::containers {

class StackException : public std::runtime_error {
public:
    explicit StackException(const std::string& message) : std::runtime_error(message) {}
};

class StackFullException : public StackException {
public:
    StackFullException() : StackException("Stack is full") {}
};

class StackEmptyException : public StackException {
public:
    StackEmptyException() : StackException("Stack is empty") {}
};

}  // namespace qfcpp::containers
