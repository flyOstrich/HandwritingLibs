#pragma once
#include "recognizeObjects.h"

/**
 * 加号笔画分析器
 */
class PlusAnalyzer {
private:
    list <StrokeSet> leftSlashStrokeSets;//左斜杠符号集
    list <StrokeSet> rightSlashStrokeSets;//右斜杠符号集
    Rect outerBox;
    Point centerPt;

    void analyze();

    /**
     * 找出所有斜杠
     */
    void findSlashStrokeSets();

    /**
      * 找出乘号的笔画集
      */
    void findPlusStrokeSets();

    /**
      * 找出乘号的笔画集迭代函数
      */
    bool findPlusStrokeSetsIteration();

    void calculateOuterBoxAndCenterPt(StrokeSet strokeSet1, StrokeSet strokeSet2);

public:
    list <StrokeSet> inputStrokeSets;
    list <StrokeSet> outputStrokeSets;

    PlusAnalyzer(list <StrokeSet> inputStrokeSets);
};