#include "geometry/Circle.hpp"
#include "geometry/Line.hpp"
#include "geometry/Point.hpp"

#include <gtest/gtest.h>

#include <numbers>

using namespace qfcpp::geometry;

TEST(PointTest, DefaultConstructedIsOrigin) {
    Point p;
    EXPECT_DOUBLE_EQ(p.x(), 0.0);
    EXPECT_DOUBLE_EQ(p.y(), 0.0);
}

TEST(PointTest, DistanceToOrigin) {
    Point p(3.0, 4.0);
    EXPECT_DOUBLE_EQ(p.distanceToOrigin(), 5.0);
}

TEST(PointTest, DistanceBetweenPoints) {
    Point a(0.0, 0.0);
    Point b(3.0, 4.0);
    EXPECT_DOUBLE_EQ(a.distanceTo(b), 5.0);
    EXPECT_DOUBLE_EQ(b.distanceTo(a), 5.0);
}

TEST(PointTest, OperatorEquality) {
    EXPECT_TRUE(Point(1.0, 2.0) == Point(1.0, 2.0));
    EXPECT_FALSE(Point(1.0, 2.0) == Point(2.0, 1.0));
}

TEST(PointTest, OperatorNegation) {
    Point p(1.0, -2.0);
    Point neg = -p;
    EXPECT_DOUBLE_EQ(neg.x(), -1.0);
    EXPECT_DOUBLE_EQ(neg.y(), 2.0);
}

TEST(PointTest, OperatorAdditionAndScaling) {
    Point a(1.0, 2.0);
    Point b(3.0, 4.0);
    Point sum = a + b;
    EXPECT_DOUBLE_EQ(sum.x(), 4.0);
    EXPECT_DOUBLE_EQ(sum.y(), 6.0);

    Point scaled = a * 2.0;
    EXPECT_DOUBLE_EQ(scaled.x(), 2.0);
    EXPECT_DOUBLE_EQ(scaled.y(), 4.0);
}

TEST(PointTest, CompoundScaling) {
    Point p(2.0, 3.0);
    p *= 3.0;
    EXPECT_DOUBLE_EQ(p.x(), 6.0);
    EXPECT_DOUBLE_EQ(p.y(), 9.0);
}

TEST(ShapeTest, EachInstanceGetsAUniqueId) {
    Point a;
    Point b;
    EXPECT_NE(a.id(), b.id());
}

TEST(LineTest, Length) {
    Line line(Point{0.0, 0.0}, Point{3.0, 4.0});
    EXPECT_DOUBLE_EQ(line.length(), 5.0);
}

TEST(LineTest, ConstructFromCoordinates) {
    Line line(0.0, 0.0, 6.0, 8.0);
    EXPECT_DOUBLE_EQ(line.length(), 10.0);
}

TEST(CircleTest, GeometryFormulas) {
    Circle circle(Point{0.0, 0.0}, 2.0);
    EXPECT_DOUBLE_EQ(circle.diameter(), 4.0);
    EXPECT_NEAR(circle.area(), std::numbers::pi * 4.0, 1e-9);
    EXPECT_NEAR(circle.circumference(), std::numbers::pi * 4.0, 1e-9);
}

TEST(CircleTest, ToStringIncludesRadiusAndCenter) {
    Circle circle(Point{1.0, 1.0}, 2.5);
    const std::string s = circle.toString();
    EXPECT_NE(s.find("2.5"), std::string::npos);
}
