#include "mat-util.h"
#include "json_util.h"

Mat MatUtil::getGrayImageFromJsonString(string json_string, Size size) {
    Scalar color(255);
    Mat res = Mat::zeros(size.width, size.height, CV_8UC1);
    list<Point> points = JsonUtil::getPointListFromJsonString(json_string);
    while (!points.empty()) {
        Point firstPt = points.front();
        points.pop_front();
        if (points.empty())break;
        Point secondPt = points.front();
        line(res, firstPt, secondPt, color);
    }
    return res;
}

Mat MatUtil::getGrayImageFromPointList(list<Point> point_list, Size size) {
    Scalar color(255);
    Mat res = Mat::zeros(size.width, size.height, CV_8UC1);
    list<Point> points = point_list;
    while (!points.empty()) {
        Point firstPt = points.front();
        points.pop_front();
        if (points.empty())break;
        Point secondPt = points.front();
        line(res, firstPt, secondPt, color);
    }
    return res;
}