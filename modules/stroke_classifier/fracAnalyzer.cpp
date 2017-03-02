//#include "fracAnalyzer.h"
//#include "config.h"
//
//FracAnalyzer::FracAnalyzer(list <StrokeSet> inputStrokesSets, float avgStrokeHeight) {
//    this->inputStrokesSets = inputStrokesSets;
//    this->avgStrokeHeight = avgStrokeHeight;
//    this->analyze();
//}
//
//void FracAnalyzer::analyze() {
//    this->findFractionBarStrokeSets();
//    this->findFractionStrokeSets();
//}
//
//bool sortFractionStrokeSetByWidth(StrokeSet first, StrokeSet second) {
//    return first.main_part_border.width < second.main_part_border.width;
//}
//
//void FracAnalyzer::findFractionBarStrokeSets() {
//    for (auto it = this->inputStrokesSets.cbegin(); it != this->inputStrokesSets.cend(); ++it) {
//        StrokeSet strokeSet = *it;
//        if (strokeSet.recognizeResult == MINUS_OR_FRACTION_BAR_LABEL) {//减号或分数线
////            strokeSet.strokeSetType = FRACTION_BAR_STROKE_SET;
//            this->fractionBarStrokesSets.push_back(strokeSet);
//        } else {
//            this->restStrokeSets.push_back(strokeSet);
//        }
//    }
//    this->fractionBarStrokesSets.sort(sortFractionStrokeSetByWidth);
//}
//
//void FracAnalyzer::findFractionStrokeSets() {
//    //根据分数比划获取分数的分子和分母的笔画
//    while (!this->fractionBarStrokesSets.empty()) {
//        StrokeSet fractionBarStrokeSet = this->fractionBarStrokesSets.front();
//        this->findFractionStrokeSetsIteration(fractionBarStrokeSet);
//        this->fractionBarStrokesSets.pop_front();
//    }
//}
//
//bool sortDesc(StrokeSet strokeSet1, StrokeSet strokeSet2) {
//    return strokeSet1.main_part_border.y < strokeSet2.main_part_border.y;
//}
//
//bool sortAsc(StrokeSet strokeSet1, StrokeSet strokeSet2) {
//    return strokeSet1.main_part_border.y + strokeSet1.main_part_border.height
//           > strokeSet2.main_part_border.y + strokeSet2.main_part_border.height;
//}
//
//bool detectRectYAxisIntersect(Rect rect1, Rect rect2) {
//    if (rect1.y > rect2.y + rect2.height)return false;
//    if (rect1.y + rect1.height < rect2.y)return false;
//    return true;
//}
//
//
//bool FracAnalyzer::findFractionStrokeSetsIteration(StrokeSet fractionBarStrokeSet) {
//    //first.找出X方向位于分数线以内的所有笔画集合
//    list <StrokeSet> xAxisSatisfiedStrokeSets;
//    StrokeSet strokeSet;
//    list <StrokeSet> rStrokeSets;
//    list <StrokeSet> topStrokeSets;
//    list <StrokeSet> bottomStrokeSets;
//    bool topFound = false;
//    bool bottomFound = false;
//    for (auto it = this->restStrokeSets.cbegin(); it != this->restStrokeSets.cend(); ++it) {
//        strokeSet = *it;
//        if ((strokeSet.strokeSetType == NORMAL_STROKE_SET || strokeSet.strokeSetType == ADD_EXP_STROKE_SET) &&
//            strokeSet.centerPt.x >= fractionBarStrokeSet.main_part_border.x &&
//            strokeSet.centerPt.x <=
//            fractionBarStrokeSet.main_part_border.x + fractionBarStrokeSet.main_part_border.width) {
//            strokeSet.isFractionBarFlag = false;
//            xAxisSatisfiedStrokeSets.push_back(strokeSet);
//        } else if (strokeSet.strokeSetType == FRACTION_EXP_STROKE_SET &&
//                   this->isFractionStrokeSetAndFractionBarStrokeSetIntersect(strokeSet, fractionBarStrokeSet)) {
//            strokeSet.isFractionBarFlag = false;
//            xAxisSatisfiedStrokeSets.push_back(strokeSet);
//        } else {
//            rStrokeSets.push_back(strokeSet);
//        }
//    }
//    //second
//    list <StrokeSet> yAxisTopSatisfiedStrokeSets;
//    list <StrokeSet> yAxisBottomSatisfiedStrokeSets;
//
//    for (auto it = xAxisSatisfiedStrokeSets.cbegin(); it != xAxisSatisfiedStrokeSets.cend(); ++it) {
//        strokeSet = *it;
//        if (strokeSet.centerPt.y <= fractionBarStrokeSet.centerPt.y) {
//            yAxisTopSatisfiedStrokeSets.push_back(strokeSet);
//        } else {
//            yAxisBottomSatisfiedStrokeSets.push_back(strokeSet);
//        }
//    }
//    yAxisTopSatisfiedStrokeSets.sort(sortAsc);
//    yAxisBottomSatisfiedStrokeSets.sort(sortDesc);
//    cout << "xAxisSatisfiedStrokeSets size->" << xAxisSatisfiedStrokeSets.size() << endl;
//    cout << "yAxisTopSatisfiedStrokeSets size->" << yAxisTopSatisfiedStrokeSets.size() << endl;
//    cout << "yAxisBottomSatisfiedStrokeSets size->" << yAxisBottomSatisfiedStrokeSets.size() << endl;
//    //third.找出分数线上方包围盒y轴相交的所有笔画，加入分子笔画集合
//    StrokeSet yAxisTopFirstStrokeSet;
//    Rect tYRect;
//    int topMaxLineHeight;
//    int idx = 0;
//    if (!yAxisTopSatisfiedStrokeSets.empty()) {
//        yAxisTopFirstStrokeSet = yAxisTopSatisfiedStrokeSets.front();
//        tYRect = yAxisTopFirstStrokeSet.main_part_border;
//        topMaxLineHeight = yAxisTopFirstStrokeSet.lineHeight;
//    }
//
//    while (!yAxisTopSatisfiedStrokeSets.empty()) {
//        StrokeSet frontStrokeSet = yAxisTopSatisfiedStrokeSets.front();
//        if (idx == 0 && frontStrokeSet.isFractionBar()) {
//            cout << "top nearest is fraction bar!" << endl;
//            break;
//        };
//        if (!detectRectYAxisIntersect(tYRect, frontStrokeSet.main_part_border)) {
//            rStrokeSets.push_back(frontStrokeSet);
//            cout << "top item is not interset!" << endl;
//            break;
//        }
//        topMaxLineHeight = max(topMaxLineHeight, frontStrokeSet.lineHeight);
//
//        int yMax = max(tYRect.y + tYRect.height, frontStrokeSet.main_part_border.y + frontStrokeSet.main_part_border.y);
//        int yMin = min(tYRect.y, frontStrokeSet.main_part_border.y);
//        tYRect.y = yMin;
//        tYRect.height = yMax - yMin;
//        idx++;
//        if (std::abs(frontStrokeSet.centerPt.y - fractionBarStrokeSet.centerPt.y) <=
//            topMaxLineHeight * this->avgStrokeHeight * 2) {
//            topStrokeSets.push_back(frontStrokeSet);
//            topFound = true;
//        } else {
//            if (frontStrokeSet.strokeSetType != FRACTION_BAR_STROKE_SET) {
//                rStrokeSets.push_back(frontStrokeSet);
//            }
//        }
//        yAxisTopSatisfiedStrokeSets.pop_front();
//    }
//    //fourth.找出分数线下方包围盒y轴相交的所有笔画，加入分母笔画集合
//    StrokeSet yAxisBottomFirstStrokeSet;
//    int bottomMaxLineHeight;
//    idx = 0;
//    if (!yAxisBottomSatisfiedStrokeSets.empty()) {
//        yAxisBottomFirstStrokeSet = yAxisBottomSatisfiedStrokeSets.front();
//        tYRect = yAxisBottomFirstStrokeSet.main_part_border;
//        bottomMaxLineHeight = yAxisBottomFirstStrokeSet.lineHeight;
//    }
//    while (!yAxisBottomSatisfiedStrokeSets.empty()) {
//        StrokeSet frontStrokeSet = yAxisBottomSatisfiedStrokeSets.front();
//        if (idx == 0 && frontStrokeSet.isFractionBar()) {
//            cout << "bottom nearest is fraction bar!" << endl;
//            break;
//        };
//        if (!detectRectYAxisIntersect(tYRect, frontStrokeSet.main_part_border)) {
//            rStrokeSets.push_back(frontStrokeSet);
//            cout << "top item is not interset!" << endl;
//            break;
//        }
//        bottomMaxLineHeight = max(bottomMaxLineHeight, frontStrokeSet.lineHeight);
//        int yMax = max(tYRect.y + tYRect.height, frontStrokeSet.main_part_border.y + frontStrokeSet.main_part_border.y);
//        int yMin = min(tYRect.y, frontStrokeSet.main_part_border.y);
//        tYRect.y = yMin;
//        tYRect.height = yMax - yMin;
//        idx++;
//        if (std::abs(frontStrokeSet.centerPt.y - fractionBarStrokeSet.centerPt.y) <=
//            bottomMaxLineHeight * this->avgStrokeHeight * 2) {
//            bottomStrokeSets.push_back(frontStrokeSet);
//            bottomFound = true;
//        } else {
//            if (frontStrokeSet.strokeSetType != FRACTION_BAR_STROKE_SET) {
//                rStrokeSets.push_back(frontStrokeSet);
//            }
//        }
//        yAxisBottomSatisfiedStrokeSets.pop_front();
//    }
//
//    if (topFound && bottomFound) {
//        this->restStrokeSets = rStrokeSets;
//        fractionBarStrokeSet.top = topStrokeSets;
//        fractionBarStrokeSet.bottom = bottomStrokeSets;
//        fractionBarStrokeSet.strokeSetType = FRACTION_EXP_STROKE_SET;
//        fractionBarStrokeSet.lineHeight = this->getFractionLineHeight(fractionBarStrokeSet);
////        this->mergeStrokesAndReCalculateMainPartBorder();
//        this->restStrokeSets.push_back(fractionBarStrokeSet);
//
//        return true;
//    }
//
//    fractionBarStrokeSet.isFractionBarFlag = false;
//    fractionBarStrokeSet.strokeSetType = NORMAL_STROKE_SET;
//    this->restStrokeSets.push_back(fractionBarStrokeSet);
//    return false;
//}
//
//bool isFractionStrokeSetAndFractionBarStrokeSetIntersect(StrokeSet fractionStrokeSet, StrokeSet fractionBarStrokeSet) {
//    return (fractionBarStrokeSet.main_part_border.x <= fractionStrokeSet.main_part_border.x &&
//            fractionBarStrokeSet.main_part_border.x + fractionBarStrokeSet.main_part_border.width
//            >= fractionStrokeSet.main_part_border.x)
//           || (fractionStrokeSet.main_part_border.x <= fractionBarStrokeSet.main_part_border.x
//               && fractionStrokeSet.main_part_border.x + fractionStrokeSet.main_part_border.width
//                  >= fractionBarStrokeSet.main_part_border.x);
//}
//
//int FracAnalyzer::getFractionLineHeight(StrokeSet fractionStrokeSet) {
//    StrokeSet strokeSet;
//    int maxTop = 1;
//    int maxBottom = 1;
//    for (auto it = fractionStrokeSet.top.cbegin(); it != fractionStrokeSet.top.cend(); ++it) {
//        strokeSet = *it;
//        maxTop = std::max(maxTop, strokeSet.lineHeight);
//    }
//    for (auto it = fractionStrokeSet.bottom.cbegin(); it != fractionStrokeSet.bottom.cend(); ++it) {
//        strokeSet = *it;
//        maxBottom = std::max(maxBottom, strokeSet.lineHeight);
//    }
//    return maxTop + maxBottom;
//}
