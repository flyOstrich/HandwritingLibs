#pragma once

#include "rapidjson/document.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <list>
using namespace cv;
using namespace std;
using namespace rapidjson;

class JsonUtil {
private:
	Document document;
public:
	list<Point> getPointListFromJsonString(string json_string);
};