#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <algorithm>
#include <cmath>

#include "point.h"

struct RealPoint {
    double x{}, y{};

    RealPoint() = default;

    RealPoint(Point p) : x(p.x), y(p.y) {}

    RealPoint(double x, double y) : x(x), y(y) {}
};

double distance(RealPoint A, RealPoint B);

double manhattanDistance(RealPoint A,RealPoint B);

struct Line {
    double a{}, b{}, c{};

    Line() = default;

    Line(RealPoint A, RealPoint B);
};

struct LineSegment : public Line {
    RealPoint A, B;

    LineSegment(RealPoint A, RealPoint B);
};

bool intersect(LineSegment l1, LineSegment l2);

double triangleArea(RealPoint A, RealPoint B, RealPoint C);

bool inTriangle(RealPoint A, RealPoint B, RealPoint C, RealPoint X);

#endif // GEOMETRY_H
