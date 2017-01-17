#pragma once
#include <iostream>
#include <list>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
class LinearInterpolation {
private:
	float getA(Point x1,Point x2);
	float getB(Point x1,Point x2,float a);
public:
	list<Point> linear_interpolation(Point x1, Point x2);

};