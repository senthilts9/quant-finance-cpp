// Demonstrates the bounds-checked Stack<T> container: pushing to capacity,
// popping in LIFO order, and the exception types raised on overflow/underflow.
#include "containers/Stack.hpp"
#include "containers/StackException.hpp"
#include "geometry/Point.hpp"

#include <iostream>

int main() {
    using qfcpp::containers::Stack;
    using qfcpp::containers::StackEmptyException;
    using qfcpp::containers::StackFullException;
    using qfcpp::geometry::Point;

    Stack<Point> stack(5);

    for (int i = 0; i < 5; ++i) {
        stack.push(Point(static_cast<double>(i), static_cast<double>(i)));
    }

    try {
        stack.push(Point(99.0, 99.0));
    } catch (const StackFullException& e) {
        std::cout << "Expected failure pushing a 6th element: " << e.what() << '\n';
    }

    while (!stack.empty()) {
        std::cout << "Popped: " << stack.pop() << '\n';
    }

    try {
        stack.pop();
    } catch (const StackEmptyException& e) {
        std::cout << "Expected failure popping an empty stack: " << e.what() << '\n';
    }

    return 0;
}
