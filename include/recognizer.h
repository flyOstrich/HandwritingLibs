#pragma once

#include <iostream>
#include <list>
#include <opencv2/opencv.hpp>

#define RECOGNIZE_SIZE Size(1024,768)

using namespace std;
using namespace cv;
struct Stroke {
    string stroke_id;
    Rect stroke_border;
    Mat stroke_mat;
    Point centerPt;
    int strokeBgColor = -1;
};

struct Row {
    int row_index;
    list<Stroke> strokes;
    Rect row_border;
};

class Recognizer {
private:
    list<Stroke> strokes;
public:
    enum RECOGNIZE_STATUS {
        RECOGNIZE_START,
        RECOGNIZE_CONTINUE,
        RECOGNIZE_RESET
    };

    void pushStroke(Mat &stroke, string stroke_id);

    void drawBorderForStroke(Stroke stroke);

    void drawCenterPtForStroke(Stroke stroke);

    Mat combineStrokeMat(list<Stroke> strokes);

    //计算平均每行的高度
    int calculateAvgRowHeight(list<Stroke> strokes);
    //将所有的比划信息进行分行(聚类)
    list<Row> getRows(list<Stroke> strokes);

    list<Stroke> getStrokes();

    list<pair<int, int> > recognizeRows(Stroke recognizingImg);
};
