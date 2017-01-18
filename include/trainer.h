#pragma once

#include "common.h"
#include <vector>
#include <list>

#define TRAIN_IMAGE_SIZE  cv::Size(56,56)
namespace Trainer {
    class HogComputer {
    public:
        static cv::Mat getHogDescriptorMat(const char *recognizing_img_path);

        static cv::Mat getHogDescriptorMat(cv::Mat mat);

        static cv::Mat getHogDescriptorForImage(cv::Mat image);

        static std::list<std::pair<int, cv::Mat> > getGradientList(std::list<std::pair<int, cv::Mat> > &image_list);

        static std::pair<cv::Mat, cv::Mat>
        convertGradientToMlFormat(std::list<std::pair<int, cv::Mat> > &gradient_list);

        static void trainSvm(std::pair<cv::Mat, cv::Mat> train_data, std::string trained_result_location);
    };

    class ImageLoader {
    private:
        std::vector<int> imageLabels;
    public:
        std::list<std::pair<int, cv::Mat> > loadImages(std::vector<std::string> image_path_list, std::string dir);
        std::list<std::pair<int, cv::Mat> > loadImages(std::vector<std::string> image_path_list);
    };
}