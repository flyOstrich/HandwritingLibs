#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <list>
using namespace cv;
using namespace std;
class MatUtil {
public:
	static Mat getGrayImageFromJsonString(string json_string,Size size);
};
