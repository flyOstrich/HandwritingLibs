#pragma once

#include <iostream>
#include "cluster_analysis.h"
#include <rapidjson/document.h>

using namespace rapidjson;

class Recognizer {
private:
    ClusterAnalysis clusterAnalyzer;
    Ptr<ml::RTrees> svm;
public:
    Recognizer();

    Recognizer(string svm_model_file, string label_character_map_file, Size canvas_size);

    list<Stroke> strokes;

    string label_character_map;

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

    /*************************************************
         Function:       combineStrokeMat
         Description:    合并笔画数据,并生成合并后的图像矩阵
                         合并后的图像矩阵大小为能包围所有笔画的最小外界矩形的大小
         Input:          strokes  笔画路径数据
         Input:          target_size 合并后目标图像的大小
         Return:         合并后的图像
    *************************************************/
    Mat combineStrokeMat(list<Stroke> strokes, Size target_size);

    /*************************************************
     Function:       combineStrokeMat
     Description:    合并笔画数据,并生成合并后的图像矩阵
     Input:          strokes  笔画路径数据
     Return:         合并后的图像
    *************************************************/
    Mat combineStrokeMat(list<Stroke> strokes);

    //计算平均每行的高度
    int calculateAvgRowHeight(list<Stroke> strokes);

    list<list<string> > recognize();

};
