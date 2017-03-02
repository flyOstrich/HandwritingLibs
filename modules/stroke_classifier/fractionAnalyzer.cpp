#include "fractionAnalyzer.h"
#include "debugUtil.h"

using namespace DebugUtil;

FractionAnalyzer::FractionAnalyzer(list <StrokeSet> strokeSets, list <StrokeSet> fractionStrokeSets,
                                   StrokeSet fractionStrokeSet, float avgStrokeHeight) {
    this->inputStrokeSets = strokeSets;
    this->fractionStrokeSets = fractionStrokeSets;
    this->restStrokeSets = strokeSets;
    this->fractionStrokeSet = fractionStrokeSet;
    this->avgStrokeHeight = avgStrokeHeight;
    this->findTopAndBottomStrokeSet();

}

bool sortDesc(StrokeSet strokeSet1, StrokeSet strokeSet2) {
    return strokeSet1.main_part_border.y < strokeSet2.main_part_border.y;
}

bool sortAsc(StrokeSet strokeSet1, StrokeSet strokeSet2) {
    return strokeSet1.main_part_border.y + strokeSet1.main_part_border.height
           > strokeSet2.main_part_border.y + strokeSet2.main_part_border.height;
}

bool isFractionStrokeSetAndFractionBarStrokeSetIntersect(StrokeSet fractionStrokeSet, StrokeSet fractionBarStrokeSet) {
    return (fractionBarStrokeSet.main_part_border.x <= fractionStrokeSet.main_part_border.x &&
            fractionBarStrokeSet.main_part_border.x + fractionBarStrokeSet.main_part_border.width
            >= fractionStrokeSet.main_part_border.x)
           || (fractionStrokeSet.main_part_border.x <= fractionBarStrokeSet.main_part_border.x
               && fractionStrokeSet.main_part_border.x + fractionStrokeSet.main_part_border.width
                  >= fractionBarStrokeSet.main_part_border.x);
}

bool detectRectYAxisIntersect(Rect rect1, Rect rect2) {
    if (rect1.y > rect2.y + rect2.height)return false;
    if (rect1.y + rect1.height < rect2.y)return false;
    return true;
}

bool FractionAnalyzer::findTopAndBottomStrokeSet() {
    StrokeSet strokeSet;
    list <StrokeSet> rStrokeSets;
    bool topFound = false;
    bool bottomFound = false;
    cout << "fraction stroke sets size" << fractionStrokeSets.size() << endl;
    //first.找出X方向位于分数线以内的所有笔画集合
    list <StrokeSet> xAxisSatisfiedStrokeSets;
    for (auto it = this->restStrokeSets.cbegin(); it != this->restStrokeSets.cend(); ++it) {
        strokeSet = *it;
        if ((strokeSet.strokeSetType == NORMAL_STROKE_SET || strokeSet.strokeSetType == ADD_EXP_STROKE_SET) &&
            strokeSet.centerPt.x >= fractionStrokeSet.main_part_border.x &&
            strokeSet.centerPt.x <= fractionStrokeSet.main_part_border.x + fractionStrokeSet.main_part_border.width) {
            strokeSet.isFractionBarFlag = false;
            xAxisSatisfiedStrokeSets.push_back(strokeSet);
        } else if (strokeSet.strokeSetType == FRACTION_EXP_STROKE_SET &&
                   isFractionStrokeSetAndFractionBarStrokeSetIntersect(strokeSet, this->fractionStrokeSet)) {
            strokeSet.isFractionBarFlag = false;
            xAxisSatisfiedStrokeSets.push_back(strokeSet);
        } else {
            rStrokeSets.push_back(strokeSet);
        }
    }
//    for (auto it = this->fractionStrokeSets.cbegin(); it != this->fractionStrokeSets.cend(); ++it) {
//        strokeSet = *it;
//        if (strokeSet.id != fractionStrokeSet.id &&
//            strokeSet.centerPt.x >= fractionStrokeSet.main_part_border.x &&
//            strokeSet.centerPt.x <= fractionStrokeSet.main_part_border.x + fractionStrokeSet.main_part_border.width) {
//            xAxisSatisfiedStrokeSets.push_back(strokeSet);
//        }
//    }
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


    cout << "xAxisSatisfiedStrokeSets size->" << xAxisSatisfiedStrokeSets.size() << endl;
    cout << "yAxisTopSatisfiedStrokeSets size->" << yAxisTopSatisfiedStrokeSets.size() << endl;
    cout << "yAxisBottomSatisfiedStrokeSets size->" << yAxisBottomSatisfiedStrokeSets.size() << endl;
    //third.找出分数线上方包围盒y轴相交的所有笔画，加入分子笔画集合
    string topRecognizeCharacter = "";
    StrokeSet yAxisTopFirstStrokeSet;
    Rect tYRect;
    int topMaxLineHeight;
    int idx = 0;
    if (!yAxisTopSatisfiedStrokeSets.empty()) {
        yAxisTopFirstStrokeSet = yAxisTopSatisfiedStrokeSets.front();
        tYRect = yAxisTopFirstStrokeSet.main_part_border;
        topMaxLineHeight = yAxisTopFirstStrokeSet.lineHeight;
    }

    while (!yAxisTopSatisfiedStrokeSets.empty()) {
        StrokeSet frontStrokeSet = yAxisTopSatisfiedStrokeSets.front();
        if (idx == 0 && frontStrokeSet.isFractionBar()) {
            cout << "top nearest is fraction bar!" << endl;
            break;
        };
        if (!detectRectYAxisIntersect(tYRect, frontStrokeSet.main_part_border)) {
            rStrokeSets.push_back(frontStrokeSet);
            cout << "top item is not interset!" << endl;
            break;
        }
        topMaxLineHeight = max(topMaxLineHeight, frontStrokeSet.lineHeight);

        int yMax = max(tYRect.y + tYRect.height, frontStrokeSet.main_part_border.y + frontStrokeSet.main_part_border.y);
        int yMin = min(tYRect.y, frontStrokeSet.main_part_border.y);
        tYRect.y = yMin;
        tYRect.height = yMax - yMin;
        idx++;
        if (std::abs(frontStrokeSet.centerPt.y - this->fractionStrokeSet.centerPt.y) <=
            topMaxLineHeight * this->avgStrokeHeight * 2) {
            this->topStrokeSets.push_back(frontStrokeSet);
            topRecognizeCharacter += frontStrokeSet.recognizeCharacter;
            topFound = true;
        } else {
            if (frontStrokeSet.strokeSetType != FRACTION_BAR_STROKE_SET) {
                rStrokeSets.push_back(frontStrokeSet);
            }
        }
        yAxisTopSatisfiedStrokeSets.pop_front();
    }
    //fourth.找出分数线下方包围盒y轴相交的所有笔画，加入分母笔画集合
    string bottomRecognizeCharacter = "";
    StrokeSet yAxisBottomFirstStrokeSet;
    int bottomMaxLineHeight;
    idx = 0;
    if (!yAxisBottomSatisfiedStrokeSets.empty()) {
        yAxisBottomFirstStrokeSet = yAxisBottomSatisfiedStrokeSets.front();
        tYRect = yAxisBottomFirstStrokeSet.main_part_border;
        bottomMaxLineHeight = yAxisBottomFirstStrokeSet.lineHeight;
    }
    while (!yAxisBottomSatisfiedStrokeSets.empty()) {
        StrokeSet frontStrokeSet = yAxisBottomSatisfiedStrokeSets.front();
        if (idx == 0 && frontStrokeSet.isFractionBar()) {
            cout << "bottom nearest is fraction bar!" << endl;
            break;
        };
        if (!detectRectYAxisIntersect(tYRect, frontStrokeSet.main_part_border)) {
            rStrokeSets.push_back(frontStrokeSet);
            cout << "top item is not interset!" << endl;
            break;
        }
        bottomMaxLineHeight = max(bottomMaxLineHeight, frontStrokeSet.lineHeight);
        int yMax = max(tYRect.y + tYRect.height, frontStrokeSet.main_part_border.y + frontStrokeSet.main_part_border.y);
        int yMin = min(tYRect.y, frontStrokeSet.main_part_border.y);
        tYRect.y = yMin;
        tYRect.height = yMax - yMin;
        idx++;
        if (std::abs(frontStrokeSet.centerPt.y - this->fractionStrokeSet.centerPt.y) <=
            bottomMaxLineHeight * this->avgStrokeHeight * 2) {
            this->bottomStrokeSets.push_back(frontStrokeSet);
            bottomRecognizeCharacter += frontStrokeSet.recognizeCharacter;
            bottomFound = true;
        } else {
            if (frontStrokeSet.strokeSetType != FRACTION_BAR_STROKE_SET) {
                rStrokeSets.push_back(frontStrokeSet);
            }
        }
        yAxisBottomSatisfiedStrokeSets.pop_front();
    }

    if (topFound && bottomFound) {
        this->restStrokeSets = rStrokeSets;
        this->fractionStrokeSet.top = this->topStrokeSets;
        this->fractionStrokeSet.bottom = this->bottomStrokeSets;
        this->fractionStrokeSet.recognizeCharacter =
                "frac{" + topRecognizeCharacter + "}{" + bottomRecognizeCharacter + "}";
        this->fractionStrokeSet.strokeSetType = FRACTION_EXP_STROKE_SET;
        this->fractionStrokeSet.lineHeight = this->getFractionLineHeight();
        this->mergeStrokesAndReCalculateMainPartBorder();
        this->restStrokeSets.push_back(this->fractionStrokeSet);
        cout << "fraction top size->" << this->topStrokeSets.size() << endl;
        cout << "bottom top size->" << this->bottomStrokeSets.size() << endl;
        return true;
    }

    this->fractionStrokeSet.isFractionBarFlag = false;
    this->fractionStrokeSet.strokeSetType = NORMAL_STROKE_SET;
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
    Point centerPt(outerBox.x + outerBox.width / 2, outerBox.y + outerBox.height / 2);
    this->fractionStrokeSet.main_part_border = outerBox;
    this->fractionStrokeSet.centerPt = centerPt;


}

int FractionAnalyzer::getFractionLineHeight() {
    StrokeSet strokeSet;
    int maxTop = 1;
    int maxBottom = 1;
    for (auto it = this->fractionStrokeSet.top.cbegin(); it != this->fractionStrokeSet.top.cend(); ++it) {
        strokeSet = *it;
        maxTop = std::max(maxTop, strokeSet.lineHeight);
    }
    for (auto it = this->fractionStrokeSet.bottom.cbegin(); it != this->fractionStrokeSet.bottom.cend(); ++it) {
        strokeSet = *it;
        maxBottom = std::max(maxBottom, strokeSet.lineHeight);
    }
    return maxTop + maxBottom;
}






