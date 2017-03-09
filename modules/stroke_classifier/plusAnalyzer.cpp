#include "plusAnalyzer.h"
#include "config.hpp"
#include "util.hpp"

PlusAnalyzer::PlusAnalyzer(list <StrokeSet> inputStrokeSets) {
    this->inputStrokeSets = inputStrokeSets;
    this->analyze();
}

void PlusAnalyzer::analyze() {
    this->findSlashStrokeSets();
    this->findPlusStrokeSets();
}

void PlusAnalyzer::findSlashStrokeSets() {
    for (auto it = this->inputStrokeSets.cbegin(); it != this->inputStrokeSets.cend(); ++it) {
        StrokeSet strokeSet = *it;
        if (strokeSet.recognizeResult == LEFT_SLASH) {
            this->leftSlashStrokeSets.push_back(strokeSet);
        } else if (strokeSet.recognizeResult == RIGHT_SLASH) {
            this->rightSlashStrokeSets.push_back(strokeSet);
        } else {
            this->outputStrokeSets.push_back(strokeSet);
        }
    }
}

void PlusAnalyzer::findPlusStrokeSets() {
    while (this->findPlusStrokeSetsIteration()) {};
    for (auto it = this->leftSlashStrokeSets.cbegin(); it != this->leftSlashStrokeSets.cend(); ++it) {
        this->outputStrokeSets.push_back(*it);
    }
    for (auto it = this->rightSlashStrokeSets.cbegin(); it != this->rightSlashStrokeSets.cend(); ++it) {
        this->outputStrokeSets.push_back(*it);
    }
}

bool PlusAnalyzer::findPlusStrokeSetsIteration() {
    StrokeSet strokeSet1;
    StrokeSet strokeSet2;
    list<StrokeSet>::iterator iteration;
    list<StrokeSet>::iterator iteration2;
    StrokeSet plusStrokeSet;
    bool found = false;
    for (list<StrokeSet>::iterator it = this->leftSlashStrokeSets.begin();
         it != this->leftSlashStrokeSets.end(); ++it) {
        strokeSet1 = *it;
        for (list<StrokeSet>::iterator it2 = this->rightSlashStrokeSets.begin();
             it2 != this->rightSlashStrokeSets.end() && it2 != it; ++it2) {
            strokeSet2 = *it2;
            if (detectRectXYAxisIntersect(strokeSet1.main_part_border, strokeSet2.main_part_border)) {
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
        plusStrokeSet.recognizeResult = PLUS_LABEL;
        plusStrokeSet.recognizeCharacter = "×";
        plusStrokeSet.strokeSetType = PLUS_STROKE_SET;
        plusStrokeSet.strokes.push_back(strokeSet1.strokes.front());
        plusStrokeSet.strokes.push_back(strokeSet2.strokes.front());
        plusStrokeSet.centerPt = this->centerPt;
        plusStrokeSet.main_part_border = this->outerBox;
        this->leftSlashStrokeSets.erase(iteration);
        this->rightSlashStrokeSets.erase(iteration2);
        this->outputStrokeSets.push_back(plusStrokeSet);
    }
    return found;
}

void PlusAnalyzer::calculateOuterBoxAndCenterPt(StrokeSet strokeSet1, StrokeSet strokeSet2) {
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