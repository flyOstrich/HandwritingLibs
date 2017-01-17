#include "cluster_analysis.h"

using namespace std;


list<ClusterAnalysis::Category> ClusterAnalysis::getCategories(list<Stroke> strokes) {
//    list<Category> initCategories = this->initCategories(strokes);
    IterationResult iterationResult = this->genIterationResult(strokes);
    list<Stroke> rest_strokes = strokes;
    list<ClusterAnalysis::Category> categories;
    do {
        iterationResult = this->getCategory(iterationResult);
        rest_strokes = iterationResult.rest_strokes;
        categories.push_front(iterationResult.category);
        iterationResult = this->genIterationResult(rest_strokes);
    } while (rest_strokes.size() > 0);
    return categories;
}

ClusterAnalysis::IterationResult ClusterAnalysis::genIterationResult(list<Stroke> strokes) {
    IterationResult iterationResult;
    if(strokes.size()>0){
        Stroke beginStroke = *strokes.cbegin();
        Point2f centerPt(0, beginStroke.centerPt.y);
        iterationResult.category.centerPt = centerPt;
        iterationResult.decrease_count=1;
        iterationResult.rest_strokes=strokes;
    }
    return iterationResult;
}

ClusterAnalysis::IterationResult ClusterAnalysis::getCategory(IterationResult input_iteration_result) {
    int iteration_count = 0;
    while (input_iteration_result.decrease_count != 0 ) {
        if(iteration_count == this->MAX_ITERATION)break;
        input_iteration_result = this->iteration(input_iteration_result);
        iteration_count++;
    }
    return input_iteration_result;
}

ClusterAnalysis::IterationResult ClusterAnalysis::iteration(IterationResult input_result) {
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

Point ClusterAnalysis::calculateCategoryCenterPoint(ClusterAnalysis::Category category) {
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