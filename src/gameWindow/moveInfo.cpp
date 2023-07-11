#include "gameWindow.h"

MoveInfo::MoveInfo() = default;

MoveInfo::MoveInfo(Point start, int direction, bool flag50p) :
    startX(start.x), startY(start.y), direction(direction), flag50p(flag50p) {}

MoveInfo::MoveInfo(int startX, int startY, int direction, bool flag50p) :
    startX(startX), startY(startY), direction(direction), flag50p(flag50p) {}
