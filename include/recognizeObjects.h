#pragma once

#include "common.h"

enum StrokeSetType {
    FRACTION_BAR_STROKE_SET,
    FRACTION_EXP_STROKE_SET,
    ADD_EXP_STROKE_SET,
    SUB_EXP_STROKE_SET,
    EQUATION_STROKE_SET,
    POW_STROKE_SET,
    PLUS_STROKE_SET,
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
    //
    string recognizeCharacter;
};

struct StrokeSet {
    int id;
    //该笔画集所占行高，默认为1，如果形如 frac{1}{2}（二分之一）这种形式，则行高为2
    //形如 frac{1}{frac{1}{2}}这种形式行高为3，以此类推
    int lineHeight = 1;
    list <Stroke> strokes;
    Rect main_part_border;
    Point centerPt;
    StrokeSetType strokeSetType;
    bool isFractionBarFlag = true;
    int recognizeResult = -1; //识别结果，-1表示还未识别
    string recognizeCharacter;
    /*
     * 判断识别结果是否为数字
     */
    bool isNumber() {
        return recognizeResult >= 0 && recognizeResult <= 9;
    };

    bool isFractionBar() {
        return recognizeResult == 11 && isFractionBarFlag;
    }

    Direction direction;
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
