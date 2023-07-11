#ifndef POINT_H
#define POINT_H

struct Point {
    int x, y;

    explicit Point(int _x = 0, int _y = 0) : x(_x), y(_y) {}
};

bool operator==(Point,Point);

bool operator!=(Point,Point);

#endif // POINT_H
