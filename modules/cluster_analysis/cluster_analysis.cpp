#include "cluster_analysis.h"

using namespace std;

// comparison, not case sensitive.
bool sort_category_fn(const Category &first, const Category &second) {
    return first.centerPt.y < second.centerPt.y;
}

list<Category> ClusterAnalysis::getCategories(list<Stroke> strokes) {
    RecognizeCategoryIterationObject iterationResult = this->genCategoryIterationObject(strokes);
    list<Stroke> rest_strokes = strokes;
    list<Category> categories;
    do {
        iterationResult = this->getCategory(iterationResult);
        rest_strokes = iterationResult.rest_strokes;
        categories.push_front(iterationResult.category);
        iterationResult = this->genCategoryIterationObject(rest_strokes);
    } while (rest_strokes.size() > 0);
    categories.sort(sort_category_fn);
    return categories;
}

// comparison, not case sensitive.
bool sort_recognize_unit_fn(const RecognizeUnit &first, const RecognizeUnit &second) {
    return first.main_part_border.x < second.main_part_border.x;
}


list<Category> ClusterAnalysis::getRecognizeUnitsForCategories(list<Stroke> ori_strokes) {
    list<Category> res;
    list<Category> categories = this->getCategories(ori_strokes);
    for (auto it = categories.cbegin(); it != categories.cend(); ++it) {
        Category category = *it;
        list<Stroke> strokes = category.strokes;
        RecognizeUnitIterationObject iterationObject = this->genRecognizeUnitIterationObject(strokes);
        list<Stroke> rest_strokes = strokes;
        list<RecognizeUnit> recognizeUnits;
        do {
            iterationObject = this->recognizeUnits(iterationObject);
            rest_strokes = iterationObject.rest_strokes;
            recognizeUnits.push_front(iterationObject.recognize_unit);
            iterationObject = this->genRecognizeUnitIterationObject(rest_strokes);
        } while (rest_strokes.size() > 0);
        recognizeUnits.sort(sort_recognize_unit_fn);
        category.recognize_units = recognizeUnits;
        res.push_back(category);
    }
    return res;
}

ClusterAnalysis::RecognizeCategoryIterationObject ClusterAnalysis::genCategoryIterationObject(list<Stroke> strokes) {
    RecognizeCategoryIterationObject iterationObject;
    if (strokes.size() > 0) {
        Stroke beginStroke = *strokes.cbegin();
        Point2f centerPt(0, beginStroke.centerPt.y);
        iterationObject.category.centerPt = centerPt;
        iterationObject.decrease_count = 1;
        iterationObject.rest_strokes = strokes;
    }
    return iterationObject;
}

ClusterAnalysis::RecognizeUnitIterationObject ClusterAnalysis::genRecognizeUnitIterationObject(list<Stroke> strokes) {
    RecognizeUnitIterationObject iterationObject;
    if (strokes.size() > 0) {
        Stroke beginStroke = *strokes.cbegin();
        Rect mainPartBorder = beginStroke.main_part_border;
        iterationObject.recognize_unit.main_part_border = mainPartBorder;
        iterationObject.decrease_count = 1;
        iterationObject.rest_strokes = strokes;
    }
    return iterationObject;
}

ClusterAnalysis::RecognizeCategoryIterationObject
ClusterAnalysis::getCategory(RecognizeCategoryIterationObject input_iteration_result) {
    int iteration_count = 0;
    while (input_iteration_result.decrease_count != 0) {
        if (iteration_count == this->MAX_ITERATION)break;
        input_iteration_result = this->recognizeCategoryIteration(input_iteration_result);
        iteration_count++;
    }
    return input_iteration_result;
}

ClusterAnalysis::RecognizeCategoryIterationObject
ClusterAnalysis::recognizeCategoryIteration(RecognizeCategoryIterationObject input_result) {
    list<Stroke> input_strokes = input_result.rest_strokes;
    list<Stroke> rest_strokes;
    Point2f centerPt = input_result.category.centerPt;
    int decrease_count = 0;
    for (auto it = input_strokes.cbegin(); it != input_strokes.cend(); ++it) {
        Stroke s = *it;
        Point centerPoint = s.centerPt;
        if (std::abs(centerPoint.y - centerPt.y) <= this->cluster_max_height) {
            input_result.category.strokes.push_front(s);
            centerPt = this->calculateCategoryCenterPoint(input_result.category);
            decrease_count++;
        } else {
            rest_strokes.push_front(s);
        }
    }
    input_result.decrease_count = decrease_count;
    input_result.rest_strokes = rest_strokes;
    input_result.category.centerPt = centerPt;
    return input_result;
}

Point ClusterAnalysis::calculateCategoryCenterPoint(Category category) {
    list<Stroke> strokes = category.strokes;
    int yMin = -1;
    int yMax = -1;
    for (auto it = strokes.cbegin(); it != strokes.cend(); ++it) {
        Stroke s = *it;
        Point centerPoint = s.centerPt;
        if (yMax == -1) {
            yMax = centerPoint.y;
        } else {
            yMax = std::max(yMax, centerPoint.y);
        }
        if (yMin == -1) {
            yMin = centerPoint.y;
        } else {
            yMin = std::min(yMin, centerPoint.y);
        }
    }
    Point2f res(0, (float) (yMin + (yMax - yMin) / 2));
    return res;
}

ClusterAnalysis::RecognizeUnitIterationObject ClusterAnalysis::recognizeUnits(
        RecognizeUnitIterationObject recognizeUnitIterationObject) {
    int iteration_count = 0;
    while (recognizeUnitIterationObject.decrease_count != 0) {
        if (iteration_count == this->MAX_ITERATION)break;
        recognizeUnitIterationObject = this->recognizeUnitsIteration(recognizeUnitIterationObject);
        iteration_count++;
    }
    return recognizeUnitIterationObject;
}

ClusterAnalysis::RecognizeUnitIterationObject ClusterAnalysis::recognizeUnitsIteration(
        RecognizeUnitIterationObject recognizeUnitIterationObject) {
    list<Stroke> input_strokes = recognizeUnitIterationObject.rest_strokes;
    list<Stroke> rest_strokes;
    Rect mainPartBorder = recognizeUnitIterationObject.recognize_unit.main_part_border;
    int decrease_count = 0;
    for (auto it = input_strokes.cbegin(); it != input_strokes.cend(); ++it) {
        Stroke s = *it;
        if (this->detectRectIntersect(recognizeUnitIterationObject.recognize_unit.main_part_border,
                                      s.main_part_border) != 0) {
            recognizeUnitIterationObject.recognize_unit.strokes.push_front(s);
            mainPartBorder = this->calculateUnitMainPartBorder(recognizeUnitIterationObject.recognize_unit);
            decrease_count++;
        } else {
            rest_strokes.push_front(s);
        }
    }
    recognizeUnitIterationObject.decrease_count = decrease_count;
    recognizeUnitIterationObject.rest_strokes = rest_strokes;
    recognizeUnitIterationObject.recognize_unit.main_part_border = mainPartBorder;
    return recognizeUnitIterationObject;
}

float ClusterAnalysis::detectRectIntersect(Rect rect1, Rect rect2) {
    if (rect1.x > rect2.x + rect2.width) { return 0.0; }
//    if (rect1.y > rect2.y + rect2.height) { return 0.0; }
    if (rect1.x + rect1.width < rect2.x) { return 0.0; }
//    if (rect1.y + rect1.height < rect2.y) { return 0.0; }
    return 1;
//    float colInt = min(rect1.x + rect1.width, rect2.x + rect2.width) - max(rect1.x, rect2.x);
//    float rowInt = min(rect1.y + rect1.height, rect2.y + rect2.height) - max(rect1.y, rect2.y);
//    float intersection = colInt * rowInt;
//    float area1 = rect1.width * rect1.height;
//    float area2 = rect2.width * rect2.height;
//    return intersection / (area1 + area2 - intersection);
}

Rect ClusterAnalysis::calculateUnitMainPartBorder(RecognizeUnit unit) {
    list<Stroke> strokes = unit.strokes;
    Rect fst = strokes.front().main_part_border;
    Rect res(fst.x, fst.y, fst.width, fst.height);
    for (auto it = strokes.cbegin(); it != strokes.cend(); ++it) {
        Stroke s = *it;
        int rightBound1 = res.x + res.width;
        int rightBound2 = s.main_part_border.x + s.main_part_border.width;
        int rightBound = std::max(rightBound1, rightBound2);

        res.x = std::min(res.x, s.main_part_border.x);
        res.width = rightBound - res.x;


        int bottomBound1 = res.y + res.height;
        int bottomBound2 = s.main_part_border.y + s.main_part_border.height;
        int bottomBound = std::max(bottomBound1, bottomBound2);
        res.y = std::min(res.y, s.main_part_border.y);
        res.height = bottomBound - res.y;
    }
    return res;
}