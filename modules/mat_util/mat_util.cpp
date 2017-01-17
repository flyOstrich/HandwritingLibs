#include "mat_util.h"
#include "json_util.h"
#include "linear_interpolation.h"
Mat MatUtil::getGrayImageFromJsonString(string json_string, Size size) {
    JsonUtil jsonUtil;
    LinearInterpolation linearInterpolation;

    Mat res = Mat::zeros(size.width, size.height, CV_8UC1);
    list<Point> points = jsonUtil.getPointListFromJsonString(json_string);
    while (!points.empty()) {
        Point firstPt = points.front();
        points.pop_front();
        if (points.empty())break;
        Point secondPt = points.front();
        list<Point> interpolatedStrokePoints=linearInterpolation.linear_interpolation(firstPt,secondPt);
        while(!interpolatedStrokePoints.empty()){
            Point pt=interpolatedStrokePoints.front();
            uchar* row=res.ptr<uchar>(pt.y);
            row[pt.x]=255;
            interpolatedStrokePoints.pop_front();
        }
    }
    return res;
}