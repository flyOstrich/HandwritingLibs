#pragma once

#include "recognizeObjects.h"

class FracAnalyzer {
private:
    list <StrokeSet> inputStrokesSets;

    void analyze();

    bool findLongestFractionBarStrokeSet(StrokeSet *strokeSet);

    StrokeSet findFractionStrokeSets(StrokeSet fractionBarStrokeSet);


    bool
    isFractionStrokeSetAndFractionBarStrokeSetIntersect(StrokeSet fractionStrokeSet, StrokeSet fractionBarStrokeSet);

    int getFractionLineHeight(StrokeSet fractionStrokeSet);

    void mergeStrokesAndReCalculateMainPartBorder(StrokeSet *fractionStrokeSet);

public:
    list <StrokeSet> outputStrokesSets;

    FracAnalyzer(list <StrokeSet> inputStrokesSets);

    static bool sortFractionStrokeSetByWidth(StrokeSet first, StrokeSet second);

    static bool sortTopFractionStrokes(StrokeSet first, StrokeSet second);

    static bool sortBottomFractionStrokes(StrokeSet first, StrokeSet second);

};