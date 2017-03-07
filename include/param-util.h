#pragma once

#include <iostream>
#include <vector>
#include <jni.h>
#include <cv.h>
#include <list>
#include <opencv2/opencv.hpp>

namespace Util {
    class ParamConverter{
    public:
        static std::vector<std::string> convertJobjectArrayToVector(JNIEnv *env,jobjectArray joarr);
        static std::list< std::pair<int,cv::Mat> > convertJobjectArrayToMatVector(JNIEnv *env,jobjectArray joarr,jintArray labels);
        static std::string convertJstringToString(JNIEnv *env,jstring jstr);

    };
}