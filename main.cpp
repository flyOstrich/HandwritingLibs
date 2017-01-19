#include <opencv2/opencv.hpp>
#include "json_util.h"
#include "mat-util.h"
#include "recognizer.h"
#include "type-util.h"
#include <fstream>
#include <image-util.h>
#include "cluster_analysis.h"
#include "painter.h"
#include "trainer.h"
#include "file-util.h"

using namespace cv;
using namespace std;
using namespace rapidjson;

//用手写板手写并识别
void draw_and_recognize() {
    string stroke_file="D:\\workspace\\HandwritingLibs\\assets\\painter_stroke.txt";
    string svm_model_file="D:\\workspace\\HandwritingLibs\\assets\\train.yml";
    MouseHelper4OpenCV helper(stroke_file);
    Mat res = helper.MouseDraw("开始画画吧", Mat(400, 400, CV_8UC3, Scalar(0,0,0)), Scalar(255,255,255), 1);

    Recognizer recognizer(svm_model_file, Size(400, 400));
    ifstream in(stroke_file);
    if (!in.is_open()) {
        cout << "open file error" << endl;
    }
    char buf[6000];
    int idx = 0;
    while (!in.eof()) {
        idx++;
        in.getline(buf, 6000);
        string s = buf;
        if (s.length() == 0)continue;
        list<Point> stroke_points = JsonUtil::getPointListFromJsonString(s);
        recognizer.pushStroke(stroke_points, "aaaaa" + idx);
    }
    recognizer.recognize();
    Mat final = recognizer.combineStrokeMat(recognizer.strokes);
    imshow("result", final);
    waitKey(0);
}

int main() {

    draw_and_recognize();

    return 0;
}