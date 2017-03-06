#include "strokeClassifier.h"
#include "mat-util.h"
#include "image-util.h"
#include "config.h"
#include <fstream>
#include "type-util.h"
#include "debugUtil.h"
#include "util.hpp"
#include "fractionAnalyzer.h"
#include "addAnalyzer.h"
#include "equAnalyzer.h"
#include "plusAnalyzer.h"
#include "fiveAnalyzer.h"
#include "powAnalyzer.h"
#include "fracAnalyzer.h"

using namespace Util;
using namespace DebugUtil;
using namespace std;

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
    writingStroke.recognizeCharacter = cfg["val"].GetString();

    StrokeSet strokeSet;
    strokeSet.id = std::rand();
    strokeSet.main_part_border = writingStroke.main_part_border;
    strokeSet.strokes.push_front(writingStroke);
    strokeSet.centerPt = writingStroke.centerPt;
    strokeSet.strokeSetType = writingStroke.single_stroke_recognize_result
                              == MINUS_OR_FRACTION_BAR_LABEL ? FRACTION_BAR_STROKE_SET
                                                             : NORMAL_STROKE_SET;
    strokeSet.recognizeResult = writingStroke.single_stroke_recognize_result;
    strokeSet.direction = writingStroke.direction;
    strokeSet.recognizeCharacter = writingStroke.recognizeCharacter;
    this->strokes.push_back(writingStroke);
    this->stroke_set.push_back(strokeSet);

    /**********************计算所有笔画的平均高度和宽度*****************************/
    cout << "**********************calculate stroke average width and height*****************************" << endl;
    this->calculateAvgStrokeWidthHeight();
    cout << "avgStrokeWidth->" << this->avgStrokeWidth << ";  avgStrokeHeight->" << this->avgStrokeHeight << endl;

    return writingStroke;
}

bool sortFN(StrokeSet strokeSet1, StrokeSet strokeSet2) {
    return strokeSet1.centerPt.x < strokeSet2.centerPt.x;
}

void Recognizer::StrokeClassifier::getStrokeSet() {
    this->getFiveStrokeSets();
    cout << "after find five" << this->stroke_set.size() << endl;
    this->getPlusStrokeSets();
    cout << "after find plus" << this->stroke_set.size() << endl;
    this->getEquStrokeSets();
    cout << "after find equ" << this->stroke_set.size() << endl;
    this->getAddStrokeSets();
    cout << "after find add" << this->stroke_set.size() << endl;
    this->getFractions();
    cout << "after find frac" << this->stroke_set.size() << endl;
//    this->getStrokeSetsByFractionBar();
    this->getPowStrokeSets();
    this->recognizeIteration();
//    this->stroke_set.sort(sortFN);
//    showStrokeSets(this->stroke_set);


//    showStrokeSets(this->stroke_set);
}

void Recognizer::StrokeClassifier::recognizeIteration() {
    list <StrokeSet> nextStrokeSet;
    while (!this->stroke_set.empty()) {
        StrokeSet strokeSet = this->stroke_set.front();
        if (strokeSet.strokeSetType == FRACTION_EXP_STROKE_SET) {
            StrokeClassifier classifier(this->canvas_size);
            classifier.stroke_set = strokeSet.top;
            classifier.getStrokeSet();
            string topRes = this->combineRes(classifier.stroke_set);
            strokeSet.top = classifier.stroke_set;
            classifier.stroke_set = strokeSet.bottom;
            classifier.getStrokeSet();
            string bottomRes = this->combineRes(classifier.stroke_set);
            strokeSet.recognizeCharacter = "\\frac{" + topRes + "}{" + bottomRes + "}";
            strokeSet.bottom = classifier.stroke_set;
        }
        nextStrokeSet.push_back(strokeSet);
        this->stroke_set.pop_front();
    }
    this->stroke_set = nextStrokeSet;
}

string Recognizer::StrokeClassifier::combineRes(list <StrokeSet> strokeSets) {
    strokeSets.sort(Recognizer::StrokeClassifier::sortStrokeByX);
    string res = "";
    while (!strokeSets.empty()) {
        StrokeSet front = strokeSets.front();
        res += front.recognizeCharacter;
        strokeSets.pop_front();
    }
    return res;
}

void Recognizer::StrokeClassifier::getFractions() {
    FracAnalyzer fracAnalyzer(this->stroke_set);
    this->stroke_set = fracAnalyzer.outputStrokesSets;
}

bool Recognizer::StrokeClassifier::sortFractionStrokeSetByWidth(StrokeSet first, StrokeSet second) {
    return first.main_part_border.width < second.main_part_border.width;
}

void Recognizer::StrokeClassifier::getAddStrokeSets() {
    AddAnalyzer addAnalyzer(this->stroke_set);
    this->stroke_set = addAnalyzer.outputStrokeSets;
//    showStrokeSets(this->stroke_set);
    cout << "restStrokeSets after find plus" << this->stroke_set.size() << endl;
}

void Recognizer::StrokeClassifier::getEquStrokeSets() {
    EquAnalyzer equAnalyzer(this->stroke_set, this->avgStrokeHeight);
    this->stroke_set = equAnalyzer.outputStrokeSets;
//    showStrokeSets(this->stroke_set);
    cout << "restStrokeSets after find equ" << this->stroke_set.size() << endl;
}

void Recognizer::StrokeClassifier::getPlusStrokeSets() {
    PlusAnalyzer plusAnalyzer(this->stroke_set);
    this->stroke_set = plusAnalyzer.outputStrokeSets;
//    showStrokeSets(this->stroke_set);
}

void Recognizer::StrokeClassifier::getFiveStrokeSets() {
    FiveAnalyzer fiveAnalyzer(this->stroke_set);
    this->stroke_set = fiveAnalyzer.outputStrokeSets;
}

void Recognizer::StrokeClassifier::getPowStrokeSets() {
    PowAnalyzer powAnalyzer(this->stroke_set);
    this->stroke_set = powAnalyzer.outputStrokeSets;
}

list <StrokeSet> Recognizer::StrokeClassifier::getStrokeSetsByFractionBar() {
    imshow("original image", combineStrokeMat(this->strokes, Size(400, 400)));
    waitKey(0);
    /**************************找出笔画中所有的分数线*********************/
    cout << "**************************find all fraction bars *********************" << endl;
    list <StrokeSet> fractionBarStrokeSets;
    list <StrokeSet> restStrokeSets;
    cout << "restStrokeSets start find frac" << this->stroke_set.size() << endl;
    for (auto it = this->stroke_set.cbegin(); it != this->stroke_set.cend(); ++it) {
        StrokeSet strokeSet = *it;
        if (strokeSet.recognizeResult == MINUS_OR_FRACTION_BAR_LABEL) {//减号或分数线
            strokeSet.strokeSetType = FRACTION_BAR_STROKE_SET;
            fractionBarStrokeSets.push_back(strokeSet);
        } else {
            restStrokeSets.push_back(strokeSet);
        }
    }
    fractionBarStrokeSets.sort(Recognizer::StrokeClassifier::sortFractionStrokeSetByWidth);
    this->restStrokeSets = restStrokeSets;
    cout << "faction bar sets size->" << fractionBarStrokeSets.size() << endl;
    cout << "rest stroke sets size->" << restStrokeSets.size() << endl;
    //根据分数比划获取分数的分子和分母的笔画
    while (!fractionBarStrokeSets.empty()) {
        StrokeSet fractionBarStrokeSet = fractionBarStrokeSets.front();
        Mat res = combineStrokeMat(this->strokes, Size(400, 400));
        const Scalar color(255);//画笔颜色
        FractionAnalyzer fractionAnalyzer(this->restStrokeSets,
                                          fractionBarStrokeSets,
                                          fractionBarStrokeSet,
                                          this->avgStrokeHeight);

        this->restStrokeSets = fractionAnalyzer.restStrokeSets;

        Point centerPt = fractionBarStrokeSet.strokes.front().centerPt;
        circle(res, centerPt, 5, color);

        imshow("add stroke", res);
        waitKey(0);

        fractionBarStrokeSets.pop_front();


        cout << "strokeClassifier::getStrokeSetsByFractionBar fraction top:->" << fractionAnalyzer.topStrokeSets.size()
             << endl;
        cout << "strokeClassifier::getStrokeSetsByFractionBar fraction bottom:->"
             << fractionAnalyzer.bottomStrokeSets.size() << endl;
        cout << "strokeClassifier::getStrokeSetsByFractionBar rest fraction stroke size:->"
             << fractionBarStrokeSets.size() << endl;
        cout << "strokeClassifier::getStrokeSetsByFractionBar rest stroke size:->"
             << fractionAnalyzer.restStrokeSets.size() << endl;
    }
    this->stroke_set = this->restStrokeSets;
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

string Recognizer::StrokeClassifier::getResult() {
    list <list<StrokeSet>> rows = this->getRows();
    cout << "row size->" << rows.size() << endl;
    string res = "";
    while (!rows.empty()) {
        list <StrokeSet> row = rows.front();
        string rowRes = "";
        while (!row.empty()) {
            StrokeSet rowItem = row.front();
            rowRes += rowItem.recognizeCharacter;
            row.pop_front();
        }
        res += rowRes + "\n";
        rows.pop_front();
    }
    return res;
}

list <list<StrokeSet>> Recognizer::StrokeClassifier::getRows() {
    this->stroke_set.sort(Recognizer::StrokeClassifier::sortStrokeByX);
    list <StrokeSet> strokeSets = this->stroke_set;
    list <list<StrokeSet>> rt;
    while (strokeSets.size() > 0) {
        std::pair<std::list<StrokeSet>, std::list<StrokeSet> > rowStrokeSets = this->getRowIteration(strokeSets);
        rt.push_back(rowStrokeSets.first);
        strokeSets = rowStrokeSets.second;
    }
    rt.sort(Recognizer::StrokeClassifier::sortRow);
    return rt;
}

std::pair<std::list<StrokeSet>, std::list<StrokeSet> > Recognizer::StrokeClassifier::getRowIteration(
        list <StrokeSet> strokeSets) {
    list <StrokeSet> recognizedStrokeSets;
    list <StrokeSet> restStrokeSets;
    std::pair<std::list<StrokeSet>, std::list<StrokeSet> > rt;
    StrokeSet first = strokeSets.front();
    strokeSets.pop_front();
    recognizedStrokeSets.push_back(first);
    Rect yBorder = first.main_part_border;
    while (!strokeSets.empty()) {
        StrokeSet front = strokeSets.front();
        if (detectRectYAxisIntersect(front.main_part_border, yBorder)) {
            recognizedStrokeSets.push_back(front);
            int y = min(yBorder.y, front.main_part_border.y);
            int height = max(yBorder.y + yBorder.height, front.main_part_border.y + front.main_part_border.height) -
                         yBorder.y;
            yBorder.y = y;
            yBorder.height = height;
        } else {
            restStrokeSets.push_back(front);
        }
        strokeSets.pop_front();
    }
    recognizedStrokeSets.sort(Recognizer::StrokeClassifier::sortStrokeByX);
    rt.first = recognizedStrokeSets;
    rt.second = restStrokeSets;
    return rt;
}

bool Recognizer::StrokeClassifier::sortStrokeByX(StrokeSet first, StrokeSet second) {
    return first.main_part_border.x < second.main_part_border.x;
}

bool Recognizer::StrokeClassifier::sortRow(list <StrokeSet> first, list <StrokeSet> second) {
    return first.front().main_part_border.y < second.front().main_part_border.y;
}






