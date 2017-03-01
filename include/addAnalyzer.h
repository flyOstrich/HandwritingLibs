#pragma once
#include "recognizeObjects.h"

/**
 * 加号笔画分析器
 */
class AddAnalyzer {
private:
    list <StrokeSet> horizontalLineStrokeSets;//横线符号集
    list <StrokeSet> verticalLineStrokeSets;//竖线符号集

    void analyze();

    /**
     * 找出所有横线和竖线
     */
    void findHorizontalAndVerticalStrokeSets();

    /**
     * 根据横竖相交的关系找到加号
     */
    void findAddByHorizontalAndVerticalStrokeSets();

    bool findPlusByHorizontalAndVerticalStrokeSetsIteration();

    /**
     * 检测两个矩形是否相交
     */
    bool detectXYIntersect(Rect rect1, Rect rect2);

    /**
     * 计算加号外接矩形和中点
     * @return
     */
    void calculateOuterBoxAndCenterPt(StrokeSet* addStrokeSet);

public:
    list <StrokeSet> inputStrokeSets;
    list <StrokeSet> outputStrokeSets;

    AddAnalyzer(list <StrokeSet> strokeSets);
};