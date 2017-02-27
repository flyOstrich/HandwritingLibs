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
     * 检查笔画集strokeSet 能否被添加到分数的分子笔画集合
     * @param strokeSet 将被判断的笔画集
     * @param fractionStrokeSet 分数线笔画集
     * @return
     */
    bool checkIfCanAddToTop(StrokeSet strokeSet, StrokeSet fractionStrokeSet);

    /**
     * 检查笔画集strokeSet 能否被添加到分数的分母笔画集合
     * @param strokeSet 将被判断的笔画集
     * @param fractionStrokeSet 分数线笔画集
     * @return
     */
    bool checkIfCanAddToBottom(StrokeSet strokeSet, StrokeSet fractionStrokeSet);

    FractionAnalyzer(list <StrokeSet> strokeSets,
                     list <StrokeSet> fractionStrokeSets,
                     StrokeSet fractionStrokeSet,
                     float avgStrokeHeight);
};