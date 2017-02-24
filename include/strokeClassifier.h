#pragma once

#include "recognizeObjects.h"
#include "recognizer.h"


namespace Recognizer {


    class StrokeClassifier {
    private:
        //按照分数线找出所有属于分数的笔画
        list <StrokeSet> getStrokeSetsByFractionBar();
        //找出分数先上方及下方的内容
        list <StrokeSet> gatherFractionStrokeItem(list <StrokeSet> strokeSets, StrokeSet fractionStrokeSet);
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

        /*************************************************
        Function:       getPosition
        Description:    计算secondStroke相对于firstStroke的位置关系
        Input:          firstStroke
        Input:          secondStroke
        Return:         位置关系
       *************************************************/
        Position getPosition(Stroke firstStroke, Stroke secondStroke);
        /*************************************************
          Function:       detectRectIntersect
          Description:    计算两个矩形是否相交
        *************************************************/
        bool detectRectIntersect(Rect rect1,Rect rect2);

    };
}