#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <algorithm>
#include <cmath>

#include "point.h"

struct PointLf {
    double x{}, y{};

    PointLf() = default;

    // This constructor is not marked explicit for convenience, BEWARE!
    explicit PointLf(Point p) : x(p.x), y(p.y) {}

    PointLf(double x, double y) : x(x), y(y) {}
};

double distance(PointLf A, PointLf B);

double mDistance(PointLf A, PointLf B);

struct Line {
    double a{}, b{}, c{};

    Line() = default;

    Line(PointLf A, PointLf B);
};

struct LineSegment : public Line {
    PointLf A, B;

    LineSegment(PointLf A, PointLf B);
};

bool intersect(LineSegment l1, LineSegment l2);

double triangleArea(PointLf A, PointLf B, PointLf C);

bool inTriangle(PointLf A, PointLf B, PointLf C, PointLf X);

#endif // GEOMETRY_H
