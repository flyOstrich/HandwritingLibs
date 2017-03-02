#pragma once

#include "recognizeObjects.h"

/**
 * 数字“5”识别器
 */
class FiveAnalyzer {
private:
    list <StrokeSet> firstOfFiveStrokeSets;//数字5的第一笔的符号集合
    list <StrokeSet> horizontalLineStrokeSets;//横线符号集合
    Rect outerBox;
    Point centerPt;

    void analyze();

    /**
     * 找出所有 数字“5”的第一笔和所有横线的符号集合
     */
    void findFistOfFiveAndHorizontalLineStrokeSets();

    /**
      * 找出"5"的笔画集
      */
    void findFiveStrokeSets();

    /**
      * 找出乘号的笔画集迭代函数
      */
    bool findFiveStrokeSetsIteration();
    /**
     * 判断5的第二笔是否在第一笔的右上方，并且包围盒相交
     * @param strokeSet1 "5"的第一笔
     * @param strokeSet2“5”的第二笔“-”
     * @return
     */
    bool isSatisfyingTopRightCriteria(StrokeSet strokeSet1,StrokeSet strokeSet2);

    void calculateOuterBoxAndCenterPt(StrokeSet strokeSet1, StrokeSet strokeSet2);

public:
    list <StrokeSet> inputStrokeSets;
    list <StrokeSet> outputStrokeSets;

    FiveAnalyzer(list <StrokeSet> inputStrokeSets);
};