#include <opencv2/opencv.hpp>
#include "json_util.h"
#include "mat_util.h"
#include "recognizer.h"
#include <fstream>
#include "cluster_analysis.h"

using namespace cv;
using namespace std;
using namespace rapidjson;

int main() {
    Recognizer recognizer;
    ifstream in("D:\\workspace\\HandwritingLibs\\assets\\strokes4.txt");
    if (!in.is_open()) {
        cout << "open file error" << endl;
    }
    char buf[6000];
    int idx = 0;
    while (!in.eof()) {
        idx++;
        in.getline(buf, 6000);
        string s = buf;
        Mat result = MatUtil::getGrayImageFromJsonString(s, Size(400, 400));
        recognizer.pushStroke(result, "aaaaa" + idx);
    }
    int height = recognizer.calculateAvgRowHeight(recognizer.getStrokes());
    ClusterAnalysis clusterAnalysis;
    clusterAnalysis.cluster_max_height=height;
    list<ClusterAnalysis::Category> categories=clusterAnalysis.getCategories(recognizer.getStrokes());
    Mat final=recognizer.combineStrokeMat(recognizer.getStrokes());
    int i=0;
    for(auto it=categories.cbegin();it!=categories.cend();++it){
        i++;
        ClusterAnalysis::Category category=*it;
        imshow("aaa"+i,recognizer.combineStrokeMat(category.strokes));
    }
    imshow("result", final);
    waitKey(0);



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