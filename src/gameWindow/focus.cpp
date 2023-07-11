#include "gameWindow.h"

Focus::Focus() {
    x = y = -1;
}

bool Focus::valid(int _x, int _y) {
    return 0 < _x && _x <= height && 0 < _y && _y <= width;
}

void Focus::init(const int _width, const int _height) {
    width = _width;
    height = _height;
}

bool Focus::move(const int dx, const int dy) {
    int _x = x + dx, _y = y + dy;
    return set(_x, _y);
}

bool Focus::set(const int _x, const int _y) {
    if (valid(_x, _y)) {
        x = _x;
        y = _y;
        return true;
    }
    return false;
}
