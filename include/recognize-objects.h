#pragma once

#include "common.h"

#define RECOGNIZE_SIZE Size(1024,768)
struct Stroke {
    string stroke_id;
    //笔画边界
    Rect stroke_border;
    //主成分边界
    Rect main_part_border;
    //原canvas大小的笔画矩阵
    Mat stroke_mat;
    //缩放后的的笔画矩阵（用于SVM model识别）
    Mat resized_stroke_mat;
    Point centerPt;
    int strokeBgColor = -1;
    //画布上该笔画走过的点的列表
    list<Point> original_points;
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