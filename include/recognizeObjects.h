#pragma once

#include "common.h"

enum StrokeSetType {
    FRACTION_STROKE_SET,
    NORMAL_STROKE_SET
};

struct Direction {
    bool top = false;
    bool bottom = false;
    bool leftTop = false;
    bool leftBottom = false;
    bool rightTop = false;
    bool rightBottom = false;
    bool validHeight = false;
    bool validWidth = false;
};

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
    //笔画字符的方位信息
    Direction direction;
};

struct StrokeSet {
    list <Stroke> strokes;
    Rect main_part_border;
    Point centerPt;
    StrokeSetType strokeSetType;
    int recognizeResult = -1; //识别结果，-1表示还未识别
    /*
     * 判断识别结果是否为数字
     */
    bool isNumber() {
        return recognizeResult >= 0 && recognizeResult <= 9;
    };
    list <StrokeSet> top;
    list <StrokeSet> bottom;
    list <StrokeSet> left;
    list <StrokeSet> right;
    list <StrokeSet> topLeft;
    list <StrokeSet> topRight;
    list <StrokeSet> bottomLeft;
    list <StrokeSet> bottomRight;

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
