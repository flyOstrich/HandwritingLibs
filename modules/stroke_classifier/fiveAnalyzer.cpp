#include "fiveAnalyzer.h"
#include "config.h"
#include "util.hpp"

FiveAnalyzer::FiveAnalyzer(list <StrokeSet> inputStrokeSets) {
    this->inputStrokeSets = inputStrokeSets;
    this->analyze();
}

void FiveAnalyzer::analyze() {
    this->findFistOfFiveAndHorizontalLineStrokeSets();
    cout<<"first of five->"<<this->firstOfFiveStrokeSets.size()<<endl;
    cout<<"last of five->"<<this->horizontalLineStrokeSets.size()<<endl;
    this->findFiveStrokeSets();
}

void FiveAnalyzer::findFistOfFiveAndHorizontalLineStrokeSets() {
    for (auto it = this->inputStrokeSets.cbegin(); it != this->inputStrokeSets.cend(); ++it) {
        StrokeSet strokeSet = *it;
        if (strokeSet.recognizeResult == FIRST_OF_FIVE) {
            this->firstOfFiveStrokeSets.push_back(strokeSet);
        } else if (strokeSet.recognizeResult == MINUS_OR_FRACTION_BAR_LABEL) {
            this->horizontalLineStrokeSets.push_back(strokeSet);
        } else {
            this->outputStrokeSets.push_back(strokeSet);
        }
    }
}

void FiveAnalyzer::findFiveStrokeSets() {
    while (this->findFiveStrokeSetsIteration()) {}
    for (auto it = this->horizontalLineStrokeSets.cbegin(); it != this->horizontalLineStrokeSets.cend(); ++it) {
        this->outputStrokeSets.push_back(*it);
    }
    for (auto it = this->firstOfFiveStrokeSets.cbegin(); it != this->firstOfFiveStrokeSets.cend(); ++it) {
        this->outputStrokeSets.push_back(*it);
    }
}

bool FiveAnalyzer::findFiveStrokeSetsIteration() {
    StrokeSet strokeSet1;
    StrokeSet strokeSet2;
    list<StrokeSet>::iterator iteration;
    list<StrokeSet>::iterator iteration2;
    StrokeSet fiveStrokeSet;
    bool found = false;
    for (list<StrokeSet>::iterator it = this->horizontalLineStrokeSets.begin();
         it != this->horizontalLineStrokeSets.end(); ++it) {
        strokeSet1 = *it;
        for (list<StrokeSet>::iterator it2 = this->firstOfFiveStrokeSets.begin();
             it2 != this->firstOfFiveStrokeSets.end() && it2 != it; ++it2) {
            strokeSet2 = *it2;
            if (this->isSatisfyingTopRightCriteria(strokeSet2,strokeSet1 )) {
                found = true;
                iteration = it;
                iteration2 = it2;
                break;
            }
        }
        if (found) break;
    }
    if (found) {
        this->calculateOuterBoxAndCenterPt(strokeSet1, strokeSet2);
        fiveStrokeSet.recognizeResult = FIVE_LABEL;
        fiveStrokeSet.recognizeCharacter="5";
        fiveStrokeSet.strokeSetType = NORMAL_STROKE_SET;
        fiveStrokeSet.strokes.push_back(strokeSet1.strokes.front());
        fiveStrokeSet.strokes.push_back(strokeSet2.strokes.front());
        fiveStrokeSet.main_part_border = this->outerBox;
        fiveStrokeSet.centerPt = this->centerPt;
        this->horizontalLineStrokeSets.erase(iteration);
        this->horizontalLineStrokeSets.erase(iteration2);
        this->outputStrokeSets.push_back(fiveStrokeSet);
    }
    return found;
}

bool FiveAnalyzer::isSatisfyingTopRightCriteria(StrokeSet strokeSet1, StrokeSet strokeSet2) {
    Point stroke2CenterPt = strokeSet2.centerPt;
    return detectRectXYAxisIntersect(strokeSet1.main_part_border, strokeSet2.main_part_border)
           && stroke2CenterPt.y >= strokeSet1.main_part_border.y &&//y轴位于上方三分之一处
           stroke2CenterPt.y <= strokeSet1.main_part_border.y + strokeSet1.main_part_border.height * 0.33;
}

void FiveAnalyzer::calculateOuterBoxAndCenterPt(StrokeSet strokeSet1, StrokeSet strokeSet2) {
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


