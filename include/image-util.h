#pragma once

#include <iostream>
#include <vector>
#include <cv.h>
#include <list>

namespace Util {
    class ImageConverter {
    public:
        static const int COLOR_BLACK = 0;

        static const int COLOR_WHITE = 255;

        //去除图像四周空白的部分
        static cv::Mat removeEmptySpace(cv::Mat &src);

        //重新调整图片大小
        static cv::Mat resize(cv::Mat &src, cv::Size size);

        //设置图片的背景和前景色
        static cv::Mat swapBgAndFgColor(cv::Mat &src, int bgColor);

        //打印矩阵到控制台
        static void printMatrix(cv::Mat src);

        //将图片上的字进行行列剪切
        static std::list<std::list<cv::Mat> > cutImage(cv::Mat src);

        //获取图片背景色
        static int getImageBgColor(cv::Mat src);

        //图像边缘检测
        static cv::Mat cannyImage(cv::Mat src);

        //图像细化
        static cv::Mat thinImage(cv::Mat src);

        //图像二值化
        static cv::Mat twoValue(cv::Mat src, int value, bool swapColor);

        //图像膨胀
        static cv::Mat dilate(cv::Mat src);

        //获取灰度图给出颜色点的个数
        static int getMatColorCount(cv::Mat src, int bgColor);

        //获取图片的边界
        static cv::Rect getImageBorderBox(cv::Mat src, int bgColor);

        //将图片灰度化
        static cv::Mat getGrayImage(cv::Mat src);

        //获取比划中点
        static cv::Point getStrokeCenterPoint(cv::Mat &stroke, cv::Rect strokeBorder,int bgColor);

    };

}