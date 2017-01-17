#pragma once

#include "recognizer.h"

class ClusterAnalysis {
public:
    struct Category {
        Point2f centerPt;
        list<Stroke> strokes;
    };
    int cluster_max_height = -1;

    list<Category> getCategories(list<Stroke> strokes);

private:
    struct IterationResult {
        Category category;
        list<Stroke> rest_strokes;
        int decrease_count = 0;
    };
    const int MAX_ITERATION = 100;
    list<Category> categories;

    list<Category> initCategories(list<Stroke> strokes);

    Point calculateCategoryCenterPoint(Category category);

    IterationResult genIterationResult(list<Stroke> strokes);

    //迭代函数
    IterationResult iteration(IterationResult input_result);

    IterationResult getCategory(IterationResult input_iteration_result);
};