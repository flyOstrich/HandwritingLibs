#pragma once

#include <iostream>
#include <list>
#include <opencv2/opencv.hpp>

#define RECOGNIZE_SIZE Size(1024,768)

using namespace std;
using namespace cv;
struct Stroke {
    string stroke_id;
    //笔画边界
    Rect stroke_border;
    //主成分边界
    Rect main_part_border;
    Mat stroke_mat;
    Point centerPt;
    int strokeBgColor = -1;
};
/**
 * 由一些笔画组成的识别单元，单元内包含的笔画之间可能满足以下几个关系
 *   1.笔画之间相交
 *   2.笔画之间满足分数的形式
 *   3.笔画之间满足指数的形式
 */
struct RecognizeUnit {
    list<Stroke> strokes;
    Rect main_part_border;
    Point centerPt;
};
struct Category {
    Point2f centerPt;
    list<Stroke> strokes;
    list<RecognizeUnit> recognize_units;
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

    void drawBorderForStroke(Stroke stroke,Rect border);

    void drawCenterPtForStroke(Stroke stroke);

    Mat combineStrokeMat(list<Stroke> strokes);

    //计算平均每行的高度
    int calculateAvgRowHeight(list<Stroke> strokes);

    //将所有的比划信息进行分行(聚类)
    list<Row> getRows(list<Stroke> strokes);

    list<Stroke> getStrokes();

    list<pair<int, int> > recognizeRows(Stroke recognizingImg);
};
