#include <opencv2/opencv.hpp>
#include "json_util.h"
#include "mat_util.h"
#include "recognizer.h"
#include "type-util.h"
#include <fstream>
#include "cluster_analysis.h"
#include "painter.h"
#include "trainer.h"
#include "file-util.h"

using namespace cv;
using namespace std;
using namespace rapidjson;


int main() {

    Trainer::ImageLoader imageLoader;
    Util::FileUtil fileUtil;
    vector<string> files;
    fileUtil.getFiles("D:\\workspace\\HandwritingLibs\\assets\\t10k-images", files);
    std::list<std::pair<int, cv::Mat> > res = imageLoader.loadImages(files);
//     imageLoader.loadImages()



//    MouseHelper4OpenCV helper;

//    Mat res = helper.MouseDraw ("开始画画吧",Mat(400,400,CV_8UC3,Scalar(0,0,0)),Scalar(255,255,255),1);

//    imwrite ("d:\\test.jpg",res); //保存画画的图像到test.jpg
//    Mat res = Mat::zeros(400, 400, CV_8UC1);
//    string winName = "testWin";
//    imshow(winName, res);
//    setMouseCallback(winName, onMouse);


//    waitKey(0);

//    Recognizer recognizer;
//    ifstream in("D:\\workspace\\HandwritingLibs\\assets\\strokes4.txt");
//    if (!in.is_open()) {
//        cout << "open file error" << endl;
//    }
//    char buf[6000];
//    int idx = 0;
//    while (!in.eof()) {
//        idx++;
//        in.getline(buf, 6000);
//        string s = buf;
//        Mat result = MatUtil::getGrayImageFromJsonString(s, Size(400, 400));
//        recognizer.pushStroke(result, "aaaaa" + idx);
//    }
//    int height = recognizer.calculateAvgRowHeight(recognizer.getStrokes());
//    ClusterAnalysis clusterAnalysis;
//    clusterAnalysis.cluster_max_height = height;
//    list<Category> categories = clusterAnalysis.getRecognizeUnitsForCategories(recognizer.getStrokes());
//    Mat final = recognizer.combineStrokeMat(recognizer.getStrokes());
//    int i = 0;
//    for (auto it = categories.cbegin(); it != categories.cend(); ++it) {
//        int j = 0;
//        i++;
//        Category category = *it;
//        list<RecognizeUnit> units = category.recognize_units;
//        for (auto it2 = units.cbegin(); it2 != units.cend(); ++it2) {
//            j++;
//            string winName="result"+Util::TypeConverter::int2String(i)+Util::TypeConverter::int2String(j);
//            RecognizeUnit unit = *it2;
//            imshow(winName, recognizer.combineStrokeMat(unit.strokes));
//            moveWindow(winName,j*200,i*200);
//        }
//    }
//    imshow("result", final);
//    waitKey(0);



//    list<Stroke> strokes = recognizer.getStrokes();
//    Mat res = Mat::zeros(strokes.front().stroke_mat.rows, strokes.front().stroke_mat.cols, CV_8UC1);
//    while (!strokes.empty()) {
//        Stroke st = strokes.front();
//        imshow("result", st.stroke_mat);
//        strokes.pop_front();
//        waitKey(1000);
//    }

    return 0;
}