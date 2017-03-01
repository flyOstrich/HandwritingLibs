#include "addAnalyzer.h"
#include "config.h"

AddAnalyzer::AddAnalyzer(list <StrokeSet> strokeSets) {
    this->inputStrokeSets = strokeSets;
    this->analyze();
}

void AddAnalyzer::analyze() {
    this->findHorizontalAndVerticalStrokeSets();
    this->findAddByHorizontalAndVerticalStrokeSets();
}

/**
 * 找出所有减号
 */
void AddAnalyzer::findHorizontalAndVerticalStrokeSets() {
    for (auto it = this->inputStrokeSets.cbegin(); it != this->inputStrokeSets.cend(); ++it) {
        StrokeSet strokeSet = *it;
        if (strokeSet.recognizeResult == MINUS_OR_FRACTION_BAR_LABEL) {
            this->horizontalLineStrokeSets.push_back(strokeSet);
        } else if (strokeSet.recognizeResult == VERTICAL_OR_NUMBER_ONE_LABEL) {
            this->verticalLineStrokeSets.push_back(strokeSet);
        } else {
            this->outputStrokeSets.push_back(strokeSet);
        }
    }
}

/**
  * 根据横竖相交的关系找到加号
  */
void AddAnalyzer::findAddByHorizontalAndVerticalStrokeSets() {
    while (this->findPlusByHorizontalAndVerticalStrokeSetsIteration()) {}
    for (auto it = this->horizontalLineStrokeSets.begin(); it != this->horizontalLineStrokeSets.end(); ++it) {
        this->outputStrokeSets.push_back(*it);
    }
    for (auto it = this->verticalLineStrokeSets.begin(); it != this->verticalLineStrokeSets.end(); ++it) {
        this->outputStrokeSets.push_back(*it);
    }
}

bool AddAnalyzer::findPlusByHorizontalAndVerticalStrokeSetsIteration() {
    StrokeSet strokeSet1;
    StrokeSet strokeSet2;
    list<StrokeSet>::iterator iteration;
    list<StrokeSet>::iterator iteration2;
    StrokeSet addStrokeSet;
    bool found = false;
    for (list<StrokeSet>::iterator it = this->horizontalLineStrokeSets.begin();
         it != this->horizontalLineStrokeSets.end(); ++it) {
        strokeSet1 = *it;
        for (list<StrokeSet>::iterator it2 = this->verticalLineStrokeSets.begin();
             it2 != this->verticalLineStrokeSets.end(); ++it2) {
            strokeSet2 = *it2;
            if (this->detectXYIntersect(strokeSet1.main_part_border, strokeSet2.main_part_border)) {
                iteration = it;
                iteration2 = it2;
                found = true;
                break;
            }
        }
        if (found) break;
    }
    if (found) {
        addStrokeSet.recognizeResult = ADD_LABEL;
        addStrokeSet.strokeSetType = ADD_EXP_STROKE_SET;
        addStrokeSet.strokes.push_back(strokeSet1.strokes.front());
        addStrokeSet.strokes.push_back(strokeSet2.strokes.front());
        this->calculateOuterBoxAndCenterPt(&addStrokeSet);
        this->horizontalLineStrokeSets.erase(iteration);
        this->verticalLineStrokeSets.erase(iteration2);
        this->outputStrokeSets.push_back(addStrokeSet);
    }
    return found;
}

void AddAnalyzer::calculateOuterBoxAndCenterPt(StrokeSet *addStrokeSet) {
    list <Stroke> strokes = addStrokeSet->strokes;
    int xMin = -1, xMax = -1, yMin = -1, yMax = -1;
    for (auto it = strokes.cbegin(); it != strokes.cend(); ++it) {
        Stroke stroke = *it;
        xMin == -1 ? xMin = stroke.main_part_border.x : xMin = min(xMin, stroke.main_part_border.x);
        xMax == -1 ? xMax = stroke.main_part_border.x + stroke.main_part_border.width
                   : xMax = max(xMax, stroke.main_part_border.x + stroke.main_part_border.width);
        yMin == -1 ? yMin = stroke.main_part_border.y : yMin = min(yMin, stroke.main_part_border.y);
        yMax == -1 ? yMax = stroke.main_part_border.y + stroke.main_part_border.height
                   : yMax = max(yMax, stroke.main_part_border.y + stroke.main_part_border.height);
    }
    Rect outBox(xMin, yMin, xMax - xMin, yMax - yMin);
    Point centerPt(outBox.x + outBox.width / 2, outBox.y + outBox.height / 2);
    addStrokeSet->main_part_border = outBox;
    addStrokeSet->centerPt = centerPt;
}

/**
 * 检测两个矩形是否相交
 */
bool AddAnalyzer::detectXYIntersect(Rect rect1, Rect rect2) {
    if (rect1.x > rect2.x + rect2.width) { return false; }
    if (rect1.y > rect2.y + rect2.height) { return false; }
    if (rect1.x + rect1.width < rect2.x) { return false; }
    if (rect1.y + rect1.height < rect2.y) { return false; }
    return true;
}