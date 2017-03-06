#pragma once

#include "recognizeObjects.h"
#include "recognizer.h"


namespace Recognizer {


    class StrokeClassifier {
    private:
        //按照分数线找出所有属于分数的笔画
        list <StrokeSet> getStrokeSetsByFractionBar();

        //按照横竖相交的关系识别加号
        void getAddStrokeSets();

        //识别等号
        void getEquStrokeSets();

        //识别乘号
        void getPlusStrokeSets();

        //识别5
        void getFiveStrokeSets();

        //识别乘幂
        void getPowStrokeSets();

        //识别分数
        void getFractions();

        //迭代识别
        void recognizeIteration();

        std::pair<std::list<StrokeSet>, std::list<StrokeSet> > getRowIteration(list <StrokeSet> strokeSets);

        string combineRes(list <StrokeSet> strokeSets);

        static bool sortStrokeByX(StrokeSet first, StrokeSet second);

        static bool sortRow(list <StrokeSet> first, list <StrokeSet> second);

    public:
        //笔画与笔画之间的方位关系
        //共分为8种
        //上、下、左、右、上右、下右、上左、下左
        enum Position {
            POSITION_TOP,
            POSITION_BOTTOM,
            POSITION_TOP_LEFT,
            POSITION_TOP_RIGHT,
            POSITION_BOTTOM_LEFT,
            POSITION_BOTTOM_RIGHT,
            POSITION_RIGHT,
            POSITION_LEFT
        };

        static bool sortFractionStrokeSetByWidth(StrokeSet first, StrokeSet second);

        StrokeClassifier();

        StrokeClassifier(Size canvas_size);

        string label_character_map;

        Size canvas_size;

        list <Stroke> strokes;

        list <StrokeSet> stroke_set;

        list <StrokeSet> restStrokeSets;

        list <Stroke> stroke_cache;

        SymbolRecognizer *symbol_recognizer;

        //笔画平均宽度
        float avgStrokeWidth = -1;

        //笔画平均高度
        float avgStrokeHeight = -1;

        /*************************************************
        Function:       calculateAvgStrokeWidthHeight
        Description:    计算所有笔画的平均高度和宽度
       *************************************************/
        void calculateAvgStrokeWidthHeight();

        /*************************************************
        Function:       addStroke
        Description:    添加新的笔画
        Input:          original_points  原始点列表
        Return:         Stroke 由点阵生成的笔画
        *************************************************/
        Stroke addStroke(list <Point> original_points);

        /*************************************************
        Function:       getStrokeSet
        Description:    根据新增的笔画对所有笔画进行重新分组
        Input:          stroke  新增的笔画
        Return:         笔画分组
        *************************************************/
        void getStrokeSet();


        list <list<StrokeSet>> getRows();

        string getResult();

    };
}