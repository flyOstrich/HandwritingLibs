#pragma once

#include "recognizeObjects.h"

class FracAnalyzer {
private:
    list <StrokeSet> inputStrokesSets;
    list <StrokeSet> fractionBarStrokesSets;
    list <StrokeSet> restStrokeSets;
    float avgStrokeHeight;

    void analyze();

    void findFractionBarStrokeSets();

    void findFractionStrokeSets();

    bool findFractionStrokeSetsIteration(StrokeSet fractionBarStrokeSet);

    bool
    isFractionStrokeSetAndFractionBarStrokeSetIntersect(StrokeSet fractionStrokeSet, StrokeSet fractionBarStrokeSet);

    int getFractionLineHeight(StrokeSet fractionStrokeSet);

public:
    list <StrokeSet> outputStrokesSets;

    FracAnalyzer(list <StrokeSet> inputStrokesSets, float avgStrokeHeight);
};