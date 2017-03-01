#include "equAnalyzer.h"
#include "config.h"
#include "util.hpp"

EquAnalyzer::EquAnalyzer(list <StrokeSet> inputStrokeSets, float avgStrokeHeight) {
    this->inputStrokeSets = inputStrokeSets;
    this->avgStrokeHeight = avgStrokeHeight;
    this->analyze();
}

void EquAnalyzer::analyze() {
    this->findHorizontalStrokeSets();
    this->findEquationStrokeSets();
}

void EquAnalyzer::findHorizontalStrokeSets() {
    for (auto it = this->inputStrokeSets.cbegin(); it != this->inputStrokeSets.cend(); ++it) {
        StrokeSet strokeSet = *it;
        if (strokeSet.recognizeResult == MINUS_OR_FRACTION_BAR_LABEL) {
            this->horizontalLineStrokeSets.push_back(strokeSet);
        } else {
            this->outputStrokeSets.push_back(strokeSet);
        }
    }
}

void EquAnalyzer::findEquationStrokeSets() {
    while (this->findEquationStrokeSetsIteration()) {};
    for (auto it = this->horizontalLineStrokeSets.cbegin(); it != this->horizontalLineStrokeSets.cend(); ++it) {
        this->outputStrokeSets.push_back(*it);
    }
}

/**
 *  找出等号笔画集迭代函数
 *  条件：
 *    1.由两个笔画“横”组成
 *    2.两横在x轴相交的区域长度至少为两横较长一横长度的 x(x>=0.3)倍
 *    3.两横之间不存在其余的笔画
 *    4.如果存在平均高度，则两横中点的Y值得距离应当小于平均高度
 */
bool EquAnalyzer::findEquationStrokeSetsIteration() {
    StrokeSet strokeSet1;
    StrokeSet strokeSet2;
    list<StrokeSet>::iterator iteration;
    list<StrokeSet>::iterator iteration2;
    StrokeSet equStrokeSet;
    bool found = false;
    for (list<StrokeSet>::iterator it = this->horizontalLineStrokeSets.begin();
         it != this->horizontalLineStrokeSets.end(); ++it) {
        strokeSet1 = *it;
        for (list<StrokeSet>::iterator it2 = this->horizontalLineStrokeSets.begin();
             it2 != this->horizontalLineStrokeSets.end() && it2 != it; ++it2) {
            strokeSet2 = *it2;
            if (this->isSatisfyingAllCriteria(strokeSet1, strokeSet2)) {
                found = true;
                iteration=it;
                iteration2=it2;
                break;
            }
        }
        if (found) break;
    }
    if (found) {
        equStrokeSet.recognizeResult = EQUATION_LABEL;
        equStrokeSet.strokeSetType = EQUATION_STROKE_SET;
        equStrokeSet.strokes.push_back(strokeSet1.strokes.front());
        equStrokeSet.strokes.push_back(strokeSet2.strokes.front());
//        this->calculateOuterBoxAndCenterPt(&plusStrokeSet);
        equStrokeSet.main_part_border = this->outerBox;
        equStrokeSet.centerPt = this->centerPt;
        this->horizontalLineStrokeSets.erase(iteration);
        this->horizontalLineStrokeSets.erase(iteration2);
        this->outputStrokeSets.push_back(equStrokeSet);
    }
    return found;
}

bool EquAnalyzer::isSatisfyingAllCriteria(StrokeSet strokeSet1, StrokeSet strokeSet2) {
    return this->isSatisfyingWidthCriteria(strokeSet1, strokeSet2) &&
           this->isSatisfyingWithoutOtherStrokes(strokeSet1, strokeSet2) &&
           this->isSatisfyingHeightCriteria(strokeSet1, strokeSet2);

}

bool EquAnalyzer::isSatisfyingWidthCriteria(StrokeSet strokeSet1, StrokeSet strokeSet2, float widthScale) {

    int maxWidth = max(strokeSet1.main_part_border.width, strokeSet2.main_part_border.width);
    int xLeft = strokeSet1.main_part_border.x;
    int xRight = strokeSet1.main_part_border.x + strokeSet1.main_part_border.width;
    if (xLeft >= strokeSet2.main_part_border.x + strokeSet2.main_part_border.width)return false;
    if (xRight <= strokeSet2.main_part_border.x)return false;
    int xIntersectLeft = max(xLeft, strokeSet2.main_part_border.x);
    int xIntersectRight = min(xRight, strokeSet2.main_part_border.x + strokeSet2.main_part_border.width);
    int xIntersectWidth = xIntersectRight - xIntersectLeft;
    float scale = xIntersectWidth / maxWidth;
    return scale < widthScale;
}

bool EquAnalyzer::isSatisfyingWithoutOtherStrokes(StrokeSet strokeSet1, StrokeSet strokeSet2) {
    this->calculateOuterBoxAndCenterPt(strokeSet1, strokeSet2);
    StrokeSet strokeSet;
    bool rt = true;
    for (auto it = this->outputStrokeSets.cbegin(); it != this->outputStrokeSets.cend(); ++it) {
        strokeSet = *it;
        if (detectRectXYAxisIntersect(this->outerBox, strokeSet.main_part_border)) {
            rt = false;
            break;
        }
    }
    return rt;
}

bool EquAnalyzer::isSatisfyingHeightCriteria(StrokeSet strokeSet1, StrokeSet strokeSet2) {
    if (this->avgStrokeHeight == -1)return true;
    return abs(strokeSet1.centerPt.y - strokeSet2.centerPt.y) <= this->avgStrokeHeight;
}

void EquAnalyzer::calculateOuterBoxAndCenterPt(StrokeSet strokeSet1, StrokeSet strokeSet2) {
    int xMin = min(strokeSet1.main_part_border.x, strokeSet2.main_part_border.x);
    int yMin = min(strokeSet1.main_part_border.y, strokeSet2.main_part_border.y);
    int xMax = max(strokeSet1.main_part_border.x + strokeSet1.main_part_border.width,
                   strokeSet2.main_part_border.x + strokeSet2.main_part_border.width);
    int yMax = max(strokeSet1.main_part_border.y + strokeSet1.main_part_border.height,
                   strokeSet2.main_part_border.y + strokeSet2.main_part_border.height);
    Rect outerBox(xMin, yMin, xMax - xMin, yMax - yMin);
    Point centerPt(outerBox.x + outerBox.width / 2, outerBox.y + outerBox.height / 2);
    this->centerPt = centerPt;
    this->outerBox = outerBox;
}