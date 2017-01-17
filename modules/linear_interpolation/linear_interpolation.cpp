/**************************************************************************

Author: pjl

Date:2017-1-14

Description:˫���Բ�ֵ

**************************************************************************/
#include "linear_interpolation.h"

float LinearInterpolation::getA(Point x1, Point x2) {
    return (float) (x1.y - x2.y) / (x1.x - x2.x);
}

float LinearInterpolation::getB(Point x1, Point x2, float a) {
    return (float) (x1.y - a * x1.x);
}

list<Point> LinearInterpolation::linear_interpolation(Point x1, Point x2) {
    float a = this->getA(x1, x2);
    float b = this->getB(x1, x2, a);
    int x_step = x1.x > x2.x ? -1 : 1;
    int y_step = x1.y > x2.y ? -1 : 1;
    list<Point> res;
    res.push_front(x1);
    res.push_front(x2);
    for (int i = x1.x; i != x2.x; i += x_step) {
        Point pt;
        pt.x = i;
        pt.y = a * i + b;
        res.push_front(pt);
    }
    for (int i = x1.y; i != x2.y; i += y_step) {
        Point pt;
        pt.y = i;
        if (x1.x == x2.x)pt.x = x1.x;
        else pt.x = (i - b) / a;
        res.push_front(pt);
    }
    return res;
}