#include "mat_util.h"
#include "json_util.h"

Mat MatUtil::getGrayImageFromJsonString(string json_string, Size size) {
    JsonUtil jsonUtil;
    Scalar color(255);
    Mat res = Mat::zeros(size.width, size.height, CV_8UC1);
    list<Point> points = jsonUtil.getPointListFromJsonString(json_string);
    while (!points.empty()) {
        Point firstPt = points.front();
        points.pop_front();
        if (points.empty())break;
        Point secondPt = points.front();
        line(res, firstPt, secondPt, color);
    }
    return res;
}