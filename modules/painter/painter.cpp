#include <painter.h>
#include <rapidjson/rapidjson.h>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace cv;

MouseHelper4OpenCV::MouseHelper4OpenCV(string stroke_file) {
    ofstream ofs(stroke_file);
    ofs << "";
    ofs.close();
    this->params.stroke_file = stroke_file;
}

void MouseHelper4OpenCV::handlerT::save_stroke() {
    printf("save stroke");
    ofstream ofs(this->stroke_file, ios::app);
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    writer.StartArray();
    for (auto it = this->points.cbegin(); it != this->points.cend(); ++it) {
        Point pt = *it;
        writer.StartObject();
        writer.Key("x");
        writer.Int(pt.x);
        writer.Key("y");
        writer.Int(pt.y);
        writer.EndObject();
    }
    writer.EndArray();
//    cout << s.GetString() << endl;
    ofs << s.GetString() << endl;
    ofs.close();
}

void MouseHelper4OpenCV::mouseHandler(int event, int x, int y, int flags, void *param) {
    MouseHelper4OpenCV *self = static_cast<MouseHelper4OpenCV *>(param);
    self->opencv_mouse_callback(event, x, y, flags, param);
}

void MouseHelper4OpenCV::opencv_mouse_callback(int event, int x, int y, int, void *param) {
    handlerT *data = (handlerT *) param;
    switch (event) {
        // update the selected bounding box
        case cv::EVENT_MOUSEMOVE:
            if (data->isDrawing) {
                data->points.push_back(cv::Point(x, y));
            }
            break;
        case cv::EVENT_LBUTTONDOWN:
            data->isDrawing = true;
            data->points.clear();
            data->points.push_back(cv::Point(x, y));
            break;
        case cv::EVENT_LBUTTONUP:
            data->isDrawing = false;
            data->save_stroke();
            break;
    }
}

cv::Mat MouseHelper4OpenCV::MouseDraw(cv::Mat img) {
    return MouseDraw("DRAW AN DIGIT", img, cv::Scalar(0, 0, 0), 4);
}

cv::Mat MouseHelper4OpenCV::MouseDraw(const std::string &windowName, cv::Mat img, cv::Scalar color, int border) {
    int key = 0;
    cv::imshow(windowName, img);
    printf("DRAW AN DIGIT and then press SPACE/BACKSPACE/ENTER button!\n");
    params.image = img.clone();
    cv::setMouseCallback(windowName, mouseHandler, (void *) &params);
    while (!(key == 32 || key == 27 || key == 13)) {
        int length = params.points.size();
        for (int i = 0; i < length && length > 2; i++) {
            if (i == length - 1) {
                cv::line(params.image, params.points.at(i), params.points.at(i), color, border);
            } else {
                cv::line(params.image, params.points.at(i), params.points.at(i + 1), color, border);
            }
        }
        cv::imshow(windowName, params.image);
        key = cv::waitKey(1);
    }
    return params.image;
}