#pragma once

#include <iostream>
#include "cluster_analysis.h"


class Recognizer {
private:
    ClusterAnalysis clusterAnalyzer;
    Ptr<ml::SVM> svm;

public:
    Recognizer();

    Recognizer(string svmModelFile, Size canvasSize);

    list<Stroke> strokes;

    Size canvasSize;

    enum RECOGNIZE_STATUS {
        RECOGNIZE_START,
        RECOGNIZE_CONTINUE,
        RECOGNIZE_RESET
    };

    enum RECOGNIZE_MAT_TYPE {
        RECOGNIZE_ORIGINAL_STROKE_MAT = 0,
        RECOGNIZE_RESIZED_STROKE_MAT = 1,
    };

    void pushStroke(list<Point> original_points, string stroke_id);

    void drawBorderForStroke(Stroke stroke, Rect border);

    void drawCenterPtForStroke(Stroke stroke);

    /*************************************************
      Function:       getCategories
      Description:    对canvas上的原始点进行缩放
      Input:          original_points  原始点列表
      Input:          original_size  原始点阵的大小
      Input:          target_size    目标点阵大小
      Return:         缩放后的点列表
      *************************************************/
    list<Point> resizeOriginalPoints(list<Point> original_points, Size original_size, Size target_size);

    Mat combineStrokeMat(list<Stroke> strokes);

    //计算平均每行的高度
    int calculateAvgRowHeight(list<Stroke> strokes);

    list<list<int> > recognize();

};
