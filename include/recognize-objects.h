#pragma once

#include "common.h"


struct Stroke {
    string stroke_id;
    //笔画边界
    Rect stroke_border;
    //主成分边界
    Rect main_part_border;
    //原canvas大小的笔画矩阵
    Mat stroke_mat;
    //缩放后的的笔画矩阵（用于SVM model识别）
    Point centerPt;
    int strokeBgColor = -1;
    //画布上该笔画走过的点的列表
    list <Point> original_points;
    //单个笔画的识别结果 （对笔画进行聚类时，会首先对识别结果为分数中的分数线以及根号等特殊形式进行处理）
    int single_stroke_recognize_result;
};
/**
 * 由一些笔画组成的识别单元，单元内包含的笔画之间可能满足以下几个关系
 *   1.笔画之间相交
 *   2.笔画之间满足分数的形式
 *   3.笔画之间满足指数的形式
 */
struct RecognizeUnit {
    list <Stroke> strokes;
    Rect main_part_border;
    Point centerPt;
};
struct Category {
    Point2f centerPt;
    list <Stroke> strokes;
    list <RecognizeUnit> recognize_units;
};