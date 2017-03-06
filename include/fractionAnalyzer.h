#pragma once

#include "recognizeObjects.h"


class FractionAnalyzer {
public:
    list <StrokeSet> inputStrokeSets; //待分析的笔画列表
    list <StrokeSet> parsedStrokeSets;//分析后的笔画列表
    list <StrokeSet> topStrokeSets;//分子笔画列表
    list <StrokeSet> bottomStrokeSets;//分母笔画列表
    list <StrokeSet> restStrokeSets;
    list <StrokeSet> fractionStrokeSets;
    StrokeSet fractionStrokeSet;
    float avgStrokeHeight;


    /**
     * 将分数分子分母的所有笔画merge,并算出该分数的包围盒
     */
    void mergeStrokesAndReCalculateMainPartBorder();

    /**
     * 找出分数笔画的
     * @return
     */
    bool findTopAndBottomStrokeSet();

    /**
     * 获取分数的行高
     * @return
     */
    int getFractionLineHeight();

    /**
     * 找出所有的分数线
     */
    void findFractionStrokeSets();
    static  bool detectRectYAxisIntersect(Rect rect1, Rect rect2);
    FractionAnalyzer(list <StrokeSet> strokeSets,
                     list <StrokeSet> fractionStrokeSets,
                     StrokeSet fractionStrokeSet,
                     float avgStrokeHeight);
};