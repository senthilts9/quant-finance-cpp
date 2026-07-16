#include "containers/Array.hpp"
#include "containers/ArrayException.hpp"
#include "containers/NumericArray.hpp"
#include "containers/Stack.hpp"
#include "containers/StackException.hpp"

#include <gtest/gtest.h>

using namespace qfcpp::containers;

TEST(ArrayTest, DefaultSizeIsTen) {
    Array<int> arr;
    EXPECT_EQ(arr.size(), 10u);
}

TEST(ArrayTest, ExplicitSize) {
    Array<int> arr(3);
    EXPECT_EQ(arr.size(), 3u);
}

TEST(ArrayTest, SetAndGetElement) {
    Array<int> arr(3);
    arr[1] = 42;
    EXPECT_EQ(arr[1], 42);
    EXPECT_EQ(arr.at(1), 42);
}

TEST(ArrayTest, OutOfBoundsThrows) {
    Array<int> arr(3);
    EXPECT_THROW((void)arr.at(3), OutOfBoundsException);
    EXPECT_THROW((void)arr.at(100), OutOfBoundsException);
}

TEST(ArrayTest, CopySemantics) {
    Array<int> a(2);
    a[0] = 1;
    a[1] = 2;
    Array<int> b = a;
    b[0] = 99;
    EXPECT_EQ(a[0], 1);  // deep copy: mutating b must not affect a
    EXPECT_EQ(b[0], 99);
}

TEST(NumericArrayTest, DotProduct) {
    NumericArray<int> a(3);
    NumericArray<int> b(3);
    for (int i = 0; i < 3; ++i) {
        a[i] = i + 1;   // 1, 2, 3
        b[i] = i + 4;   // 4, 5, 6
    }
    // 1*4 + 2*5 + 3*6 = 32
    EXPECT_EQ(a.dotProduct(b), 32);
}

TEST(NumericArrayTest, DotProductThrowsOnSizeMismatch) {
    NumericArray<int> a(2);
    NumericArray<int> b(3);
    EXPECT_THROW((void)a.dotProduct(b), ArrayException);
}

TEST(NumericArrayTest, CompoundMultiply) {
    NumericArray<int> a(2);
    a[0] = 2;
    a[1] = 3;
    a *= 10;
    EXPECT_EQ(a[0], 20);
    EXPECT_EQ(a[1], 30);
}

TEST(NumericArrayTest, CompoundAdd) {
    NumericArray<int> a(2);
    NumericArray<int> b(2);
    a[0] = 1; a[1] = 2;
    b[0] = 10; b[1] = 20;
    a += b;
    EXPECT_EQ(a[0], 11);
    EXPECT_EQ(a[1], 22);
}

TEST(StackTest, PushAndPopOrdering) {
    Stack<int> stack(3);
    stack.push(1);
    stack.push(2);
    stack.push(3);
    EXPECT_EQ(stack.pop(), 3);
    EXPECT_EQ(stack.pop(), 2);
    EXPECT_EQ(stack.pop(), 1);
}

TEST(StackTest, PushBeyondCapacityThrows) {
    Stack<int> stack(2);
    stack.push(1);
    stack.push(2);
    EXPECT_THROW(stack.push(3), StackFullException);
}

TEST(StackTest, PopEmptyThrows) {
    Stack<int> stack(2);
    EXPECT_THROW((void)stack.pop(), StackEmptyException);
}

TEST(StackTest, SizeAndEmptyTrackCorrectly) {
    Stack<int> stack(5);
    EXPECT_TRUE(stack.empty());
    stack.push(1);
    stack.push(2);
    EXPECT_EQ(stack.size(), 2u);
    EXPECT_FALSE(stack.empty());
    (void)stack.pop();
    EXPECT_EQ(stack.size(), 1u);
}
