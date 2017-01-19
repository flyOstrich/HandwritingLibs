#pragma once

#include "recognize-objects.h"

class ClusterAnalysis {
public:

    int cluster_max_height = -1;

    /*************************************************
    Function:       getCategories
    Description:    根据比划数据由聚类分析算法将不同的比划归于不同的行
    Input:          strokes  比划数据列表
    Return:         划分后每行的数据
    *************************************************/
    list<Category> getCategories(list<Stroke> strokes);

    /*************************************************
    Function:       getRecognizeUnitsForCategories
    Description:    将每行的比划数据进行分类，分类规则为
                    1.比划有相交的点则分为一类
    Input:          ori_strokes  比划数据列表
    Return:         划分后每行的数据
    *************************************************/
    list<Category> getRecognizeUnitsForCategories(list<Stroke> ori_strokes);


private:
    struct RecognizeCategoryIterationObject {
        Category category;
        list<Stroke> rest_strokes;
        int decrease_count = 0;
    };
    struct RecognizeUnitIterationObject {
        RecognizeUnit recognize_unit;
        list<Stroke> rest_strokes;
        int decrease_count = 0;
    };
    const int MAX_ITERATION = 100;
    list<Category> categories;

    /*************************************************
      Function:       calculateCategoryCenterPoint
      Description:    计算每行的中心点
                      中心点y值取该行y值最大与最小两点的中点
      Input:          category
    *************************************************/
    Point calculateCategoryCenterPoint(Category category);

    /*************************************************
      Function:       calculateUnitMainPartBorder
      Description:    计算识别单元（RecognizeUnit）的主成分区域
                      主成分区域为改识别单元所有笔画的主成分区域的外接矩形
      Input:          unit
    *************************************************/
    Rect calculateUnitMainPartBorder(RecognizeUnit unit);

    /*************************************************
    Function:       detectRectIntersect
    Description:    判断两个矩形是否相交
    Input:          rect1
    Input:          rect2
    Return:         相交图形面积占总面积的比值，如果不相交，则为0
    *************************************************/
    float detectRectIntersect(Rect rect1, Rect rect2);

    RecognizeCategoryIterationObject genCategoryIterationObject(list<Stroke> strokes);

    RecognizeUnitIterationObject genRecognizeUnitIterationObject(list<Stroke> strokes);

    //识别行的迭代函数
    RecognizeCategoryIterationObject recognizeCategoryIteration(RecognizeCategoryIterationObject input_result);

    //识别行
    RecognizeCategoryIterationObject getCategory(RecognizeCategoryIterationObject input_iteration_result);

    //识别字的迭代函数
    RecognizeUnitIterationObject recognizeUnitsIteration(RecognizeUnitIterationObject recognizeUnitIterationObject);

    //识别字
    RecognizeUnitIterationObject recognizeUnits(RecognizeUnitIterationObject recognizeUnitIterationObject);
};