#include "json_util.h"
#include "recognizer.h"
#include "type-util.h"
#include <fstream>
#include <image-util.h>
#include "painter.h"
#include "trainer.h"
#include "file-util.h"
#include "config.h"
#include "strokeClassifier.h"
#include "debugUtil.h"

using namespace cv;
using namespace std;
using namespace rapidjson;

//用手写板手写并识别
void draw_and_recognize() {
    MouseHelper4OpenCV helper(STROKE_FILE);
    Mat res = helper.MouseDraw("开始画画吧", Mat(400, 400, CV_8UC3, Scalar(0, 0, 0)), Scalar(255, 255, 255), 1);
    string label = "21";
    Recognizer::SymbolRecognizer recognizer(Size(400, 400), TRAIN_IMAGE_DIR, label);
    ifstream in(STROKE_FILE);
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
        list <Point> stroke_points = JsonUtil::getPointListFromJsonString(s);
        recognizer.pushStroke(stroke_points, "aaaaa" + idx);
    }
//    recognizer.recognize();
//    Mat final = recognizer.combineStrokeMat(recognizer.strokes);
//    imshow("result", final);
//    waitKey(0);
}


void train() {
    Trainer::ImageLoader imageLoader;
    Trainer::HogComputer hogComputer;
    Util::FileUtil fileUtil;
    vector<string> files;

    fileUtil.getFiles(SVM_TRAIN_IMAGE_DIR, files);
    std::list<std::pair<int, cv::Mat> > img_list = imageLoader.loadImages(files, SVM_TRAIN_IMAGE_DIR);
    std::list<std::pair<int, cv::Mat> > gradient_list = Trainer::HogComputer::getGradientList(
            img_list);
    std::pair<cv::Mat, cv::Mat> train_data = Trainer::HogComputer::convertGradientToMlFormat(
            gradient_list);
    hogComputer.trainSvm(train_data, SVM_MODEL_FILE);
}

void testStrokeClassifier() {
    Recognizer::StrokeClassifier classifier(Size(400, 400));
    ifstream in(STROKE_FILE);
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
        list <Point> stroke_points = JsonUtil::getPointListFromJsonString(s);
        classifier.addStroke(stroke_points);
    }
    classifier.getStrokeSet();
    cout<<"size---->"<<classifier.getResult()<<endl;
//    DebugUtil::showStrokeSets(classifier.stroke_set);
//    DebugUtil::showFractions(classifier.stroke_set);
}

int main() {
//    train();
    draw_and_recognize();
    testStrokeClassifier();

    return 0;
}