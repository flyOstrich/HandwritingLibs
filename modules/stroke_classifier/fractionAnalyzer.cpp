#include "fractionAnalyzer/fractionAnalyzer.h"

FractionAnalyzer::FractionAnalyzer(list <StrokeSet> strokeSets, StrokeSet fractionStrokeSet, float avgStrokeHeight) {
    this->inputStrokeSets = strokeSets;
    this->restStrokeSets = strokeSets;
    this->fractionStrokeSet = fractionStrokeSet;
    this->avgStrokeHeight = avgStrokeHeight;
    this->findTopAndBottomStrokeSet();
}

bool sortDesc(StrokeSet strokeSet1, StrokeSet strokeSet2) {
    return strokeSet1.centerPt.y < strokeSet2.centerPt.y;
}

bool sortAsc(StrokeSet strokeSet1, StrokeSet strokeSet2) {
    return strokeSet1.centerPt.y > strokeSet2.centerPt.y;
}

bool FractionAnalyzer::findTopAndBottomStrokeSet() {
    StrokeSet strokeSet;
    Point centerPt;
    list <StrokeSet> rStrokeSets;
    bool topFound = false;
    bool bottomFound = false;

    //first.找出X方向位于分数线以内的所有笔画集合
    list <StrokeSet> xAxisSatisfiedStrokeSets;
    for (auto it = this->restStrokeSets.cbegin(); it != this->restStrokeSets.cend(); ++it) {
        strokeSet = *it;
        if (strokeSet.centerPt.x >= fractionStrokeSet.main_part_border.x
            &&
            strokeSet.centerPt.x <= fractionStrokeSet.main_part_border.x + fractionStrokeSet.main_part_border.width) {
            xAxisSatisfiedStrokeSets.push_back(strokeSet);
        } else {
            rStrokeSets.push_back(strokeSet);
        }
    }
    cout << "xAxisSatisfiedStrokeSets size->" << xAxisSatisfiedStrokeSets.size() << endl;
    //second.将中心点y值大于和小于分数线y值的笔画集合分为两组，两组笔画集合均按与分数线y轴距离从近到远排列
    list <StrokeSet> yAxisTopSatisfiedStrokeSets;
    list <StrokeSet> yAxisBottomSatisfiedStrokeSets;
    for (auto it = xAxisSatisfiedStrokeSets.cbegin(); it != xAxisSatisfiedStrokeSets.cend(); ++it) {
        strokeSet = *it;
        if (strokeSet.centerPt.y <= this->fractionStrokeSet.centerPt.y) {
            yAxisTopSatisfiedStrokeSets.push_back(strokeSet);
        } else {
            yAxisBottomSatisfiedStrokeSets.push_back(strokeSet);
        }
    }
    yAxisTopSatisfiedStrokeSets.sort(sortAsc);
    yAxisBottomSatisfiedStrokeSets.sort(sortDesc);
    cout << "top size->" << yAxisTopSatisfiedStrokeSets.size() << endl;
    cout << "bottom size->" << yAxisBottomSatisfiedStrokeSets.size() << endl;
    //third.找出分数线上方包围盒y轴相交的所有笔画，加入分子笔画集合
    Rect tYRect = yAxisTopSatisfiedStrokeSets.front().main_part_border;
    int yMin = tYRect.y;
    int yMax = tYRect.y + tYRect.height;
    while (!yAxisTopSatisfiedStrokeSets.empty()) {
        cout << "while1" << endl;
        StrokeSet frontStrokeSet = yAxisTopSatisfiedStrokeSets.front();
        Rect r = frontStrokeSet.main_part_border;
        if ((r.y >= tYRect.y && r.y <= tYRect.y + tYRect.height)
            || (r.y + r.height >= tYRect.y && r.y + r.height <= tYRect.y + tYRect.height)) {
            this->topStrokeSets.push_back(frontStrokeSet);
            yMin = std::min(yMin, r.y);
            yMax = std::max(yMax, r.y + r.height);
            tYRect.y = yMin;
            tYRect.height = yMax - yMin;
            topFound = true;
        } else {
            rStrokeSets.push_back(frontStrokeSet);
        }
        yAxisTopSatisfiedStrokeSets.pop_front();
    }
    //fourth.找出分数线下方包围盒y轴相交的所有笔画，加入分母笔画集合
    tYRect = yAxisBottomSatisfiedStrokeSets.front().main_part_border;
    yMin = tYRect.y;
    yMax = tYRect.y + tYRect.height;
    while (!yAxisBottomSatisfiedStrokeSets.empty()) {
        cout << "while2" << endl;

        StrokeSet frontStrokeSet = yAxisBottomSatisfiedStrokeSets.front();
        Rect r = frontStrokeSet.main_part_border;
        if ((r.y >= tYRect.y && r.y <= tYRect.y + tYRect.height)
            || (r.y + r.height >= tYRect.y && r.y + r.height <= tYRect.y + tYRect.height)) {
            this->bottomStrokeSets.push_back(frontStrokeSet);
            yMin = std::min(yMin, r.y);
            yMax = std::max(yMax, r.y + r.height);
            tYRect.y = yMin;
            tYRect.height = yMax - yMin;
            bottomFound = true;
        } else {
            rStrokeSets.push_back(frontStrokeSet);
        }
        yAxisBottomSatisfiedStrokeSets.pop_front();
    }

    if (topFound && bottomFound) {
        this->restStrokeSets = rStrokeSets;
        this->fractionStrokeSet.top = this->topStrokeSets;
        this->fractionStrokeSet.bottom = this->bottomStrokeSets;
        this->fractionStrokeSet.strokeSetType = FRACTION_STROKE_SET;
        this->mergeStrokesAndReCalculateMainPartBorder();
        this->restStrokeSets.push_back(this->fractionStrokeSet);
        return true;
    }
    this->restStrokeSets.push_back(this->fractionStrokeSet);
    return false;
}

void FractionAnalyzer::mergeStrokesAndReCalculateMainPartBorder() {
    int xMin = -1;
    int xMax = -1;
    int yMin = -1;
    int yMax = -1;
    for (auto it = this->fractionStrokeSet.top.cbegin(); it != this->fractionStrokeSet.top.cend(); ++it) {
        StrokeSet strokeSet = *it;
        for (auto it2 = strokeSet.strokes.cbegin(); it2 != strokeSet.strokes.cend(); ++it2) {
            Stroke stroke = *it2;
            Rect outerBox = stroke.main_part_border;
            xMin == -1 ? xMin = outerBox.x : xMin = std::min(xMin, outerBox.x);
            xMax == -1 ? xMax = outerBox.x + outerBox.width : xMax = std::max(xMax, outerBox.x + outerBox.width);
            yMin == -1 ? yMin = outerBox.y : yMin = std::min(yMin, outerBox.y);
            yMax == -1 ? yMax = outerBox.y + outerBox.height : yMax = std::max(yMax, outerBox.y + outerBox.height);
            this->fractionStrokeSet.strokes.push_back(stroke);
        }
    }
    for (auto it = this->fractionStrokeSet.bottom.cbegin(); it != this->fractionStrokeSet.bottom.cend(); ++it) {
        StrokeSet strokeSet = *it;
        for (auto it2 = strokeSet.strokes.cbegin(); it2 != strokeSet.strokes.cend(); ++it2) {
            Stroke stroke = *it2;
            Rect outerBox = stroke.main_part_border;
            xMin == -1 ? xMin = outerBox.x : xMin = std::min(xMin, outerBox.x);
            xMax == -1 ? xMax = outerBox.x + outerBox.width : xMax = std::max(xMax, outerBox.x + outerBox.width);
            yMin == -1 ? yMin = outerBox.y : yMin = std::min(yMin, outerBox.y);
            yMax == -1 ? yMax = outerBox.y + outerBox.height : yMax = std::max(yMax, outerBox.y + outerBox.height);
            this->fractionStrokeSet.strokes.push_back(stroke);
        }
    }
    Rect outerBox(xMin, yMin, xMax - xMin, yMax - yMin);
    this->fractionStrokeSet.main_part_border = outerBox;
    cout << outerBox << endl;

}

/**
 * 检查笔画集strokeSet 能否被添加到分数的分子笔画集合
 * @param strokeSet 将被判断的笔画集
 * @param fractionStrokeSet 分数线笔画集
 * @return
 */
bool FractionAnalyzer::checkIfCanAddToTop(StrokeSet strokeSet, StrokeSet fractionStrokeSet) {
    Point fractionCenterPt = fractionStrokeSet.centerPt;
    Point centerPt = strokeSet.centerPt;
    if (centerPt.y < fractionCenterPt.y) return false;//该笔画集中心点必须位于分数线上方

    //下面判断该笔画集与已经加入分子笔画集列表的笔画集和可能存在的各种位置关系
    StrokeSet topStrokeSet;
    for (auto it = this->topStrokeSets.cbegin(); it != this->topStrokeSets.cend(); ++it) {
        topStrokeSet = *it;
        if (topStrokeSet.isNumber()) {

        }

    }
}

/**
 * 检查笔画集strokeSet 能否被添加到分数的分母笔画集合
 * @param strokeSet 将被判断的笔画集
 * @param fractionStrokeSet 分数线笔画集
 * @return
 */
bool FractionAnalyzer::checkIfCanAddToBottom(StrokeSet strokeSet, StrokeSet fractionStrokeSet) {

}



