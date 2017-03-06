#include "fracAnalyzer.h"
#include "util.hpp"
#include "debugUtil.h"

FracAnalyzer::FracAnalyzer(list <StrokeSet> inputStrokesSets) {
    this->inputStrokesSets = inputStrokesSets;
    this->analyze();
}

void FracAnalyzer::analyze() {
    StrokeSet longestFractionBarStrokeSet;
    while (this->findLongestFractionBarStrokeSet(&longestFractionBarStrokeSet)) {
        cout << "fracAnalyzer:" << this->inputStrokesSets.size() << endl;
        StrokeSet fractionStrokeSet = this->findFractionStrokeSets(longestFractionBarStrokeSet);
        this->inputStrokesSets.push_back(fractionStrokeSet);
        cout << "fracAnalyzer:" << this->inputStrokesSets.size() << endl;
    }
    this->outputStrokesSets = this->inputStrokesSets;
}

bool FracAnalyzer::sortFractionStrokeSetByWidth(StrokeSet first, StrokeSet second) {
    return first.main_part_border.width > second.main_part_border.width;
}

bool FracAnalyzer::findLongestFractionBarStrokeSet(StrokeSet *strokeSet) {
    list <StrokeSet> restStrokeSet;
    list <StrokeSet> fractionBarStrokeSets;
    for (auto it = this->inputStrokesSets.begin(); it != this->inputStrokesSets.end(); ++it) {
        if (it->strokeSetType == FRACTION_BAR_STROKE_SET)fractionBarStrokeSets.push_back(*it);
        else restStrokeSet.push_back(*it);
    }
    if (fractionBarStrokeSets.size() == 0)return false;

    fractionBarStrokeSets.sort(FracAnalyzer::sortFractionStrokeSetByWidth);
    StrokeSet longestFractionBarStokeSet = fractionBarStrokeSets.front();
    fractionBarStrokeSets.pop_front();
    *strokeSet = longestFractionBarStokeSet;
    while (!fractionBarStrokeSets.empty()) {
        StrokeSet st = fractionBarStrokeSets.front();
        restStrokeSet.push_back(st);
        fractionBarStrokeSets.pop_front();
    }
    this->inputStrokesSets = restStrokeSet;
    return true;
}

StrokeSet FracAnalyzer::findFractionStrokeSets(StrokeSet fractionBarStrokeSet) {
    list <StrokeSet> strokeSets;
    list <StrokeSet> restStrokeSets;
    //step1.排除与分数线位于同一基准线上的所有笔画
    for (auto it = this->inputStrokesSets.cbegin(); it != this->inputStrokesSets.cend(); ++it) {
        if (detectRectXAxisIntersect(fractionBarStrokeSet.main_part_border, it->main_part_border)) {
            strokeSets.push_back(*it);
        } else {
            restStrokeSets.push_back(*it);
        }
    }
    //step2.找出位于x轴分数线以内，分别位于分数线上下方的笔画
    list <StrokeSet> topStrokeSets;
    list <StrokeSet> bottomStrokeSets;
//    list <list<StrokeSet>::iterator> topStrokeSets;
//    list <list<StrokeSet>::iterator> bottomStrokeSets;

    bool topFound = false;
    bool bottomFound = false;
    for (auto it = strokeSets.begin(); it != strokeSets.end(); ++it) {
        if (detectRectXAxisIntersect(fractionBarStrokeSet.main_part_border, it->main_part_border)) {
            if (it->main_part_border.y + it->main_part_border.height < fractionBarStrokeSet.main_part_border.y) {
                topStrokeSets.push_back(*it);
//                topStrokeSets.push_back(it);
                topFound = true;
//                imshow("top", DebugUtil::combineStrokeMat(it->strokes, Size(400, 400)));
//                waitKey(0);
            }
            if (it->main_part_border.y >=
                fractionBarStrokeSet.main_part_border.y + fractionBarStrokeSet.main_part_border.height) {
                bottomStrokeSets.push_back(*it);
//                bottomStrokeSets.push_back(it);
                bottomFound = true;
//                imshow("bottom", DebugUtil::combineStrokeMat(it->strokes, Size(400, 400)));
//                waitKey(0);
            }
        }
    }
    if (topFound && bottomFound) {
        //step3.找到上下笔画中从离分数线最近的笔画开始，y轴相交的所有笔画集合
        topStrokeSets.sort(FracAnalyzer::sortTopFractionStrokes);
        bottomStrokeSets.sort(FracAnalyzer::sortBottomFractionStrokes);

        StrokeSet topFront = topStrokeSets.front();
        list <StrokeSet> topFractionStrokeSets;
        int topYMin = topFront.main_part_border.y;
        int topYMax = topFront.main_part_border.y + topFront.main_part_border.height;
        while (!topStrokeSets.empty()) {
            StrokeSet front = topStrokeSets.front();
            if (front.main_part_border.y + front.main_part_border.height >= topYMin &&
                front.main_part_border.y + front.main_part_border.height <= topYMax) {
                topFractionStrokeSets.push_back(front);
                topYMin = min(topYMin, front.main_part_border.y);
            } else {
                restStrokeSets.push_back(front);
            }
            topStrokeSets.pop_front();
        }

        StrokeSet bottomFront = bottomStrokeSets.front();
        list <StrokeSet> bottomFractionStrokeSets;
        int bottomYMin = bottomFront.main_part_border.y;
        int bottomYMax = bottomFront.main_part_border.y + bottomFront.main_part_border.height;
        while (!bottomStrokeSets.empty()) {
            StrokeSet front = bottomStrokeSets.front();
            if (front.main_part_border.y >= bottomYMin &&
                front.main_part_border.y <= bottomYMax) {
                bottomFractionStrokeSets.push_back(front);
                bottomYMax = max(bottomYMax, front.main_part_border.y + front.main_part_border.height);
            } else {
                restStrokeSets.push_back(front);
            }
            bottomStrokeSets.pop_front();
        }


//        imshow("top", DebugUtil::combineStrokeMat(topStrokeSets.front()->strokes, Size(400, 400)));
//        imshow("bottom", DebugUtil::combineStrokeMat(bottomStrokeSets.front()->strokes, Size(400, 400)));
//        waitKey(0);

        cout << " FracAnalyzer::findFractionStrokeSets size" << this->inputStrokesSets.size() << endl;
        fractionBarStrokeSet.top = topFractionStrokeSets;
        fractionBarStrokeSet.bottom = bottomFractionStrokeSets;
        fractionBarStrokeSet.strokeSetType = FRACTION_EXP_STROKE_SET;
        this->mergeStrokesAndReCalculateMainPartBorder(&fractionBarStrokeSet);
        this->inputStrokesSets = restStrokeSets;
//        imshow("fractionBarStrokeSet", DebugUtil::combineStrokeMat(fractionBarStrokeSet.strokes, Size(400, 400)));
//        waitKey(0);
        return fractionBarStrokeSet;
    }
    fractionBarStrokeSet.strokeSetType = SUB_EXP_STROKE_SET;
    return fractionBarStrokeSet;
}

bool FracAnalyzer::sortTopFractionStrokes(StrokeSet first, StrokeSet second) {
    return first.main_part_border.y + first.main_part_border.height >
           second.main_part_border.y + second.main_part_border.height;
}

bool FracAnalyzer::sortBottomFractionStrokes(StrokeSet first, StrokeSet second) {
    return first.main_part_border.y < second.main_part_border.y;
}

void FracAnalyzer::mergeStrokesAndReCalculateMainPartBorder(StrokeSet *fractionStrokeSet) {
    int xMin = -1;
    int xMax = -1;
    int yMin = -1;
    int yMax = -1;
    for (auto it = fractionStrokeSet->top.cbegin(); it != fractionStrokeSet->top.cend(); ++it) {
        StrokeSet strokeSet = *it;
        for (auto it2 = strokeSet.strokes.cbegin(); it2 != strokeSet.strokes.cend(); ++it2) {
            Stroke stroke = *it2;
            Rect outerBox = stroke.main_part_border;
            xMin == -1 ? xMin = outerBox.x : xMin = std::min(xMin, outerBox.x);
            xMax == -1 ? xMax = outerBox.x + outerBox.width : xMax = std::max(xMax, outerBox.x + outerBox.width);
            yMin == -1 ? yMin = outerBox.y : yMin = std::min(yMin, outerBox.y);
            yMax == -1 ? yMax = outerBox.y + outerBox.height : yMax = std::max(yMax, outerBox.y + outerBox.height);
            fractionStrokeSet->strokes.push_back(stroke);
        }
    }
    for (auto it = fractionStrokeSet->bottom.cbegin(); it != fractionStrokeSet->bottom.cend(); ++it) {
        StrokeSet strokeSet = *it;
        for (auto it2 = strokeSet.strokes.cbegin(); it2 != strokeSet.strokes.cend(); ++it2) {
            Stroke stroke = *it2;
            Rect outerBox = stroke.main_part_border;
            xMin == -1 ? xMin = outerBox.x : xMin = std::min(xMin, outerBox.x);
            xMax == -1 ? xMax = outerBox.x + outerBox.width : xMax = std::max(xMax, outerBox.x + outerBox.width);
            yMin == -1 ? yMin = outerBox.y : yMin = std::min(yMin, outerBox.y);
            yMax == -1 ? yMax = outerBox.y + outerBox.height : yMax = std::max(yMax, outerBox.y + outerBox.height);
            fractionStrokeSet->strokes.push_back(stroke);
        }
    }
    Rect outerBox(xMin, yMin, xMax - xMin, yMax - yMin);
    Point centerPt(outerBox.x + outerBox.width / 2, outerBox.y + outerBox.height / 2);
    fractionStrokeSet->main_part_border = outerBox;
    fractionStrokeSet->centerPt = centerPt;
}
