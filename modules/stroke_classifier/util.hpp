#pragma once

#include <recognizeObjects.h>


inline bool detectRectXYAxisIntersect(Rect rect1, Rect rect2) {
    if (rect1.x > rect2.x + rect2.width) { return false; }
    if (rect1.y > rect2.y + rect2.height) { return false; }
    if (rect1.x + rect1.width < rect2.x) { return false; }
    if (rect1.y + rect1.height < rect2.y) { return false; }
    return true;
}

inline bool detectRectYAxisIntersect(Rect rect1, Rect rect2) {
    if (rect1.y > rect2.y + rect2.height) { return false; }
    if (rect1.y + rect1.height < rect2.y) { return false; }
    return true;
}


inline bool detectRectXAxisIntersect(Rect rect1, Rect rect2) {
    if (rect1.x > rect2.x + rect2.width) { return false; }
    if (rect1.x + rect1.width < rect2.x) { return false; }
    return true;
}





