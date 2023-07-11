#include "point.h"

bool operator==(Point p, Point q) {
    return p.x == q.x && p.y == q.y;
}

bool operator!=(Point p,Point q) {
    return !(p==q);
}
