#include "geometry.h"

double distance(PointLf A, PointLf B) {
    return sqrt((B.x - A.x) * (B.x - A.x) + (B.y - A.y) * (B.y - A.y));
}

double mDistance(PointLf A, PointLf B) {
    return fabs(A.x - B.x) + fabs(A.y - B.y);
}

bool intersect(LineSegment l1, LineSegment l2) {
    using std::min;
    using std::max;

    if (l1.a * l2.b == l2.a * l1.b)
        return false;
    PointLf p = {(l1.b * l2.c - l2.b * l1.c) / (l1.a * l2.b - l2.a * l1.b),
                   (l2.a * l1.c - l1.a * l2.c) / (l1.a * l2.b - l2.a * l1.b)};
    return min(l1.A.x, l1.B.x) <= p.x && p.x <= max(l1.A.x, l1.B.x) &&
           min(l1.A.y, l1.B.y) <= p.y && p.y <= max(l1.B.y, l1.B.y) &&
           min(l2.A.x, l2.B.x) <= p.x && p.x <= max(l2.A.x, l2.B.x) &&
           min(l2.A.y, l2.B.y) <= p.y && p.y <= max(l2.A.y, l2.B.y);
}

double triangleArea(PointLf A, PointLf B, PointLf C) {
    double a = distance(B, C), b = distance(A, C), c = distance(A, B);
    double p = (a + b + c) / 2, s = p * (p - a) * (p - b) * (p - c);
    return s <= 1e-3 ? 0 : sqrt(s);
}

bool inTriangle(PointLf A, PointLf B, PointLf C, PointLf X) {
    double S1 = triangleArea(A, B, C);
    double S2 = triangleArea(A, B, X);
    double S3 = triangleArea(A, C, X);
    double S4 = triangleArea(B, C, X);
    return fabs(S2 + S3 + S4 - S1) <= 1e-3;
}

Line::Line(PointLf A, PointLf B) {
    a = A.y - B.y;
    b = B.x - A.x;
    c = A.x * B.y - B.x * A.y;
}

LineSegment::LineSegment(PointLf A, PointLf B) : A(A), B(B) {
    a = A.y - B.y;
    b = B.x - A.x;
    c = A.x * B.y - B.x * A.y;
}
