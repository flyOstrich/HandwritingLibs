#include "json_util.h"
#include "recognizer.h"
#include "type-util.h"
#include <fstream>
#include <image-util.h>
#include "painter.h"
#include "trainer.h"
#include "file-util.h"

using namespace cv;
using namespace std;
using namespace rapidjson;

//用手写板手写并识别
void draw_and_recognize() {
//    string stroke_file="D:\\workspace\\HandwritingLibs\\assets\\painter_stroke.txt";
//    string svm_model_file="D:\\workspace\\HandwritingLibs\\assets\\train.yml";
//    string label_character_map_file="D:\\workspace\\HandwritingLibs\\modules\\trainer\\label_character_map.txt";


    string stroke_file = "/Users/pjl/HandwritingLibs/assets/painter_stroke.txt";
    string svm_model_file = "/Users/pjl/HandwritingLibs/assets/train.yml";
    string label_character_map_file = "/Users/pjl/HandwritingLibs/modules/trainer/label_character_map.txt";
    MouseHelper4OpenCV helper(stroke_file);
    Mat res = helper.MouseDraw("开始画画吧", Mat(400, 400, CV_8UC3, Scalar(0, 0, 0)), Scalar(255, 255, 255), 1);

    Recognizer recognizer(svm_model_file, label_character_map_file, Size(400, 400));
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
//    cv::ml::RTrees::Flags::PREDICT_SUM
}

void train() {
    Trainer::ImageLoader imageLoader;
    Trainer::HogComputer hogComputer;
    Util::FileUtil fileUtil;
    vector<string> files;
#ifdef OP_WINDOWS
    fileUtil.getFiles("D:/workspace/HandwritingLibs/assets/train-images", files);
#endif
#ifdef OP_DARWIN
    fileUtil.getFiles("/Users/pjl/HandwritingLibs/assets/train-images", files);
#endif
//    char *dir = "D:/workspace/HandwritingLibs/assets/train-images";
    char *dir = "/Users/pjl/HandwritingLibs/assets/train-images";
    std::list<std::pair<int, cv::Mat> > img_list = imageLoader.loadImages(files, dir);
    std::list<std::pair<int, cv::Mat> > gradient_list = Trainer::HogComputer::getGradientList(
            img_list);
    std::pair<cv::Mat, cv::Mat> train_data = Trainer::HogComputer::convertGradientToMlFormat(
            gradient_list);

    hogComputer.trainSvm(train_data, "D:\\workspace\\HandwritingLibs\\assets\\train.yml");
}

int main() {
    cout << "aaaaaaa" << endl;

    train();
    draw_and_recognize();

    return 0;
}