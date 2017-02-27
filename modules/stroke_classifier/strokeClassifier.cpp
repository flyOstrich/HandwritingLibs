#include "strokeClassifier.h"
#include "mat-util.h"
#include "image-util.h"
#include "config.h"
#include <fstream>
#include "type-util.h"
#include "fractionAnalyzer/fractionAnalyzer.h"
#include "debugUtil.h"

using namespace Util;
using namespace DebugUtil;

Recognizer::StrokeClassifier::StrokeClassifier() {

}

Recognizer::StrokeClassifier::StrokeClassifier(Size canvas_size) {
    this->canvas_size = canvas_size;
    this->symbol_recognizer = new Recognizer::SymbolRecognizer(Size(400, 400), "NONE", "0");
    ifstream ifs(LABEL_CHARACTER_MAP_FILE);
    string s = "";
    if (ifs.is_open()) {
        int bufLen = 200;
        char buf[bufLen];
        while (!ifs.eof()) {
            ifs.getline(buf, bufLen);
            this->label_character_map += buf;
        }
    }
}

/*************************************************
   Function:       addStroke
   Description:    ����µıʻ�
   Input:          original_points  ԭʼ���б�
   Return:         Stroke �ɵ������ɵıʻ�
*************************************************/
Stroke Recognizer::StrokeClassifier::addStroke(list <Point> original_points) {
    Mat preProccessRes = MatUtil::getGrayImageFromPointList(original_points, this->canvas_size);
    Rect stroke_border = ImageConverter::getImageBorderBox(preProccessRes, 0);
    Point centerPt = ImageConverter::getStrokeCenterPoint(preProccessRes, stroke_border, 0);
    Rect mainPartBorder = ImageConverter::getStrokeMainPartBorder(centerPt, stroke_border);
    Stroke writingStroke;

    writingStroke.stroke_mat = preProccessRes;
    writingStroke.stroke_border = stroke_border;
    writingStroke.centerPt = centerPt;
    writingStroke.main_part_border = mainPartBorder;
    writingStroke.original_points = original_points;
    writingStroke.single_stroke_recognize_result = this->symbol_recognizer->recognizeSingleStroke(
            writingStroke);

    Document document;
    document.Parse(this->label_character_map.c_str());
    Value label_character_m = document.GetObject();
    Value cfg = label_character_m[Util::TypeConverter::int2String(
            writingStroke.single_stroke_recognize_result).c_str()].GetObject();
    writingStroke.direction.bottom = cfg["bottom"].GetBool();
    writingStroke.direction.top = cfg["top"].GetBool();
    writingStroke.direction.leftBottom = cfg["lbottom"].GetBool();
    writingStroke.direction.leftTop = cfg["ltop"].GetBool();
    writingStroke.direction.rightBottom = cfg["rbottom"].GetBool();
    writingStroke.direction.rightTop = cfg["rtop"].GetBool();
    writingStroke.direction.validHeight = cfg["validHeight"].GetBool();
    writingStroke.direction.validWidth = cfg["validWidth"].GetBool();

    StrokeSet strokeSet;
    strokeSet.id = std::rand();
    strokeSet.main_part_border = writingStroke.main_part_border;
    strokeSet.strokes.push_front(writingStroke);
    strokeSet.centerPt = writingStroke.centerPt;
    strokeSet.strokeSetType = NORMAL_STROKE_SET;
    strokeSet.recognizeResult = writingStroke.single_stroke_recognize_result;
    this->strokes.push_back(writingStroke);
    this->stroke_set.push_back(strokeSet);

    /**********************计算所有笔画的平均高度和宽度*****************************/
    cout << "**********************calculate stroke average width and height*****************************" << endl;
    this->calculateAvgStrokeWidthHeight();
    cout << "avgStrokeWidth->" << this->avgStrokeWidth << ";  avgStrokeHeight->" << this->avgStrokeHeight << endl;

//    this->getStrokeSet();
    return writingStroke;
}

void Recognizer::StrokeClassifier::getStrokeSet() {
    list <StrokeSet> strokeSets = this->getStrokeSetsByFractionBar();
}

bool sortFractionStrokeSetByWidth(StrokeSet first, StrokeSet second) {
    return first.main_part_border.width < second.main_part_border.width;
}

list <StrokeSet> Recognizer::StrokeClassifier::getStrokeSetsByFractionBar() {

    /**************************找出笔画中所有的分数线*********************/
    cout << "**************************find all fraction bars *********************" << endl;
    list <StrokeSet> fractionBarStrokeSets;
    list <StrokeSet> restStrokeSets;
    for (auto it = this->stroke_set.cbegin(); it != this->stroke_set.cend(); ++it) {
        StrokeSet strokeSet = *it;
        if (strokeSet.strokes.front().single_stroke_recognize_result == 11) {//减号或分数线
            strokeSet.strokeSetType = FRACTION_BAR_STROKE_SET;
            fractionBarStrokeSets.push_back(strokeSet);
        } else {
            restStrokeSets.push_back(strokeSet);
        }
    }
    fractionBarStrokeSets.sort(sortFractionStrokeSetByWidth);
    this->restStrokeSets = restStrokeSets;
    cout << "faction bar sets size->" << fractionBarStrokeSets.size() << endl;
    cout << "rest stroke sets size->" << restStrokeSets.size() << endl;
    //根据分数比划获取分数的分子和分母的笔画
    while (!fractionBarStrokeSets.empty()) {
        StrokeSet fractionBarStrokeSet = fractionBarStrokeSets.front();
        Mat res = combineStrokeMat(this->strokes, Size(400, 400));
        const Scalar color(255);//画笔颜色
        for (auto it = this->restStrokeSets.cbegin(); it != this->restStrokeSets.cend(); ++it) {
            StrokeSet strokeSet1 = *it;
            Mat res = combineStrokeMat(strokeSet1.strokes, Size(400, 400));
            cout << "stroke set type->" << strokeSet1.strokeSetType << endl;
            cout << "stroke set recognize->" << strokeSet1.recognizeResult << endl;
//            imshow("Rest Before", res);
//            waitKey(0);
        }
        FractionAnalyzer fractionAnalyzer(this->restStrokeSets,
                                          fractionBarStrokeSets,
                                          fractionBarStrokeSet,
                                          this->avgStrokeHeight);

        this->restStrokeSets = fractionAnalyzer.restStrokeSets;

        Point centerPt = fractionBarStrokeSet.strokes.front().centerPt;
        circle(res, centerPt, 5, color);

//        imshow("add stroke", res);


        fractionBarStrokeSets.pop_front();


        cout << "strokeClassifier::getStrokeSetsByFractionBar fraction top:->" << fractionAnalyzer.topStrokeSets.size()
             << endl;
        cout << "strokeClassifier::getStrokeSetsByFractionBar fraction bottom:->"
             << fractionAnalyzer.bottomStrokeSets.size() << endl;
        cout << "strokeClassifier::getStrokeSetsByFractionBar rest fraction stroke size:->"
             << fractionBarStrokeSets.size() << endl;
        cout << "strokeClassifier::getStrokeSetsByFractionBar rest stroke size:->"
             << fractionAnalyzer.restStrokeSets.size() << endl;
        for (auto it = this->restStrokeSets.cbegin(); it != this->restStrokeSets.cend(); ++it) {
            StrokeSet strokeSet1 = *it;
            Mat res = combineStrokeMat(strokeSet1.strokes, Size(400, 400));
            cout << "stroke set type->" << strokeSet1.strokeSetType << endl;
            cout << "stroke set recognize->" << strokeSet1.recognizeResult << endl;
//            imshow("Rest After", res);
//            waitKey(0);
        }
//        waitKey(0);
    }
//    for (auto it = fractionBarStrokeSets.cbegin(); it != fractionBarStrokeSets.cend(); ++it) {
//        StrokeSet fractionBarStrokeSet = *it;
//        Mat res = combineStrokeMat(this->strokes, Size(400, 400));
//        const Scalar color(255);//画笔颜色
//        FractionAnalyzer fractionAnalyzer(this->restStrokeSets,
//                                          fractionBarStrokeSets,
//                                          fractionBarStrokeSet,
//                                          this->avgStrokeHeight);
//
//        this->restStrokeSets = fractionAnalyzer.restStrokeSets;
//        cout << "top:->" << fractionAnalyzer.topStrokeSets.size() << endl;
//        cout << "bottom:->" << fractionAnalyzer.bottomStrokeSets.size() << endl;
//        cout << "rest:->" << fractionAnalyzer.restStrokeSets.size() << endl;
//        Point centerPt = fractionBarStrokeSet.strokes.front().centerPt;
//        circle(res, centerPt, 5, color);
//        imshow("add stroke", res);
//        waitKey(0);
//    }
    return fractionBarStrokeSets;
}

/*************************************************
 Function:       calculateAvgStrokeWidthHeight
*************************************************/
void Recognizer::StrokeClassifier::calculateAvgStrokeWidthHeight() {
    float totalWidth = -1;
    float totalHeight = -1;
    int validWidthCount = 0;
    int validHeightCount = 0;
    for (auto it = this->strokes.cbegin(); it != this->strokes.cend(); ++it) {
        Stroke stroke = *it;
        if (stroke.direction.validWidth) {
            validWidthCount++;
            totalWidth += stroke.main_part_border.width;
        }
        if (stroke.direction.validHeight) {
            validHeightCount++;
            totalHeight += stroke.main_part_border.height;
        }
    }
    if (validWidthCount) {
        this->avgStrokeWidth = totalWidth / validWidthCount;
    }
    if (validHeightCount) {
        this->avgStrokeHeight = totalHeight / validHeightCount;
    }
}

/*************************************************
  Function:       getPosition
  Description:    计算secondStroke相对于firstStroke的位置关系
  Input:          firstStroke
  Input:          secondStroke
  Return:         位置关系
*************************************************/
Recognizer::StrokeClassifier::Position Recognizer::StrokeClassifier::getPosition(Stroke firstStroke,
                                                                                 Stroke secondStroke) {
    Point firstCenterPt = firstStroke.centerPt;
    Point secondCenterPt = secondStroke.centerPt;

}

/*************************************************
  Function:       detectRectIntersect
  Description:    计算两个矩形是否相交
*************************************************/
bool Recognizer::StrokeClassifier::detectRectIntersect(Rect rect1, Rect rect2) {
    if (rect1.x > rect2.x + rect2.width) { return false; }
    if (rect1.x + rect1.width < rect2.x) { return false; }
    return true;
}

