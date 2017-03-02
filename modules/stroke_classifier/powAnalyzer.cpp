#include "powAnalyzer.h"

PowAnalyzer::PowAnalyzer(list <StrokeSet> inputStrokeSets) {
    this->inputStrokeSets = inputStrokeSets;
    this->analyze();
}

void PowAnalyzer::analyze() {
    this->findHasSuperscriptsStrokeSets();
    cout<<"hasSuperscriptsStrokeSets->"<<this->hasSuperscriptsStrokeSets.size()<<endl;
    this->findPowExpStrokeSets();
}

void PowAnalyzer::findHasSuperscriptsStrokeSets() {
    for (auto it = this->inputStrokeSets.cbegin(); it != this->inputStrokeSets.cend(); ++it) {
        StrokeSet strokeSet = *it;
        if (strokeSet.direction.rightTop) {
            this->hasSuperscriptsStrokeSets.push_back(strokeSet);
        } else {
            this->outputStrokeSets.push_back(strokeSet);
        }
    }
}

void PowAnalyzer::findPowExpStrokeSets() {
    while (this->findPowExpStrokeSetsIteration()) {}
    for (auto it = this->hasSuperscriptsStrokeSets.begin(); it != this->hasSuperscriptsStrokeSets.end(); ++it) {
        this->outputStrokeSets.push_back(*it);
    }
}

bool PowAnalyzer::findPowExpStrokeSetsIteration() {
    StrokeSet strokeSet1;
    StrokeSet strokeSet2;
    list<StrokeSet>::iterator iteration;
    list<StrokeSet>::iterator iteration2;
    StrokeSet powStrokeSet;
    bool found = false;
    for (list<StrokeSet>::iterator it = this->hasSuperscriptsStrokeSets.begin();
         it != this->hasSuperscriptsStrokeSets.end(); ++it) {
        strokeSet1 = *it;
        for (list<StrokeSet>::iterator it2 = this->hasSuperscriptsStrokeSets.begin();
             it2 != this->hasSuperscriptsStrokeSets.end() && it2 != it; ++it2) {
            strokeSet2 = *it2;
            int criteria = this->isSatisfyingPowCriteria(strokeSet1, strokeSet2);
            if (criteria != -1) {
                found = true;
                iteration = it;
                iteration2 = it2;
                if (criteria == 1) {
                    powStrokeSet.topRight.push_back(strokeSet2);
                    powStrokeSet.centerPt=strokeSet1.centerPt;
                    powStrokeSet.recognizeCharacter=strokeSet1.recognizeCharacter+"^"+strokeSet2.recognizeCharacter;
                }
                if (criteria == 2) {
                    powStrokeSet.topRight.push_back(strokeSet1);
                    powStrokeSet.centerPt=strokeSet2.centerPt;
                    powStrokeSet.recognizeCharacter=strokeSet2.recognizeCharacter+"^"+strokeSet1.recognizeCharacter;
                }
                break;
            }
        }
        if (found) break;
    }
    if (found) {
//        powStrokeSet.recognizeResult = EQUATION_LABEL;
        powStrokeSet.strokeSetType = EQUATION_STROKE_SET;
        powStrokeSet.strokes.push_back(strokeSet1.strokes.front());
        powStrokeSet.strokes.push_back(strokeSet2.strokes.front());
//        this->calculateOuterBoxAndCenterPt(&plusStrokeSet);
//        powStrokeSet.main_part_border = this->outerBox;
//        powStrokeSet.centerPt = this->centerPt;
        this->hasSuperscriptsStrokeSets.erase(iteration);
        this->hasSuperscriptsStrokeSets.erase(iteration2);
        this->outputStrokeSets.push_back(powStrokeSet);
    }
    return found;
}

int PowAnalyzer::isSatisfyingPowCriteria(StrokeSet strokeSet1, StrokeSet strokeSet2) {
    Rect r1 = strokeSet1.main_part_border;
    Rect r2 = strokeSet2.main_part_border;
    Point c1 = strokeSet1.centerPt;
    Point c2 = strokeSet2.centerPt;
    Point leftBottomR1(r1.x, r1.y + r1.height);
    Point leftBottomR2(r2.x, r2.y + r2.height);
    if (leftBottomR1.x > c2.x && leftBottomR1.x < r2.x + r2.width*1.5 && leftBottomR1.y < c2.y &&
        leftBottomR1.y > r2.y*0.5)
        return 2;
    if (leftBottomR2.x > c1.x && leftBottomR2.x < r1.x + r1.width*1.5 && leftBottomR2.y < c1.y &&
        leftBottomR2.y > r1.y*0.5)
        return 1;
    return -1;
}