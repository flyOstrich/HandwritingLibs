#pragma once
#include "recognizeObjects.h"

class EquAnalyzer {
private:
    list <StrokeSet> horizontalLineStrokeSets;//横线符号集
    Rect outerBox;
    Point centerPt;
    float avgStrokeHeight;

    void analyze();

    /**
    * 找出所有横线
    */
    void findHorizontalStrokeSets();

    /**
     * 找出等号的笔画集
     */
    void findEquationStrokeSets();

    /**
     *  找出等号笔画集
     */
    bool findEquationStrokeSetsIteration();

    /**
     * 是否满足等号形成的条件
     *    条件：
     *    1.由两个笔画“横”组成
     *    2.两横在x轴相交的区域长度至少为两横较长一横长度的 x(x>=0.3)倍
     *    3.两横之间不存在其余的笔画
     *    4.如果存在平均高度，则两横中点的Y值得距离应当小于平均高度
     * @param strokeSet1
     * @param strokeSet2
     * @return
     */
    bool isSatisfyingAllCriteria(StrokeSet strokeSet1, StrokeSet strokeSet2);

    /**
     * 是否满足  两横在x轴相交的区域长度至少为两横较长一横长度的 x(x>=0.3)倍
     * @param strokeSet1
     * @param strokeSet2
     * @return
     */
    bool isSatisfyingWidthCriteria(StrokeSet strokeSet1, StrokeSet strokeSet2, float widthScale = 0.3);

    /**
     * 是否满足  两横之间不存在其余的笔画
     * @param strokeSet1
     * @param strokeSet2
     * @return
     */
    bool isSatisfyingWithoutOtherStrokes(StrokeSet strokeSet1, StrokeSet strokeSet2);

    /**
     * 是否满足 如果存在平均高度，则两横中点的Y值得距离应当小于平均高度
     * @param strokeSet1
     * @param strokeSet2
     * @return
     */
    bool isSatisfyingHeightCriteria(StrokeSet strokeSet1, StrokeSet strokeSet2);

    /**
     * 计算笔画包围盒及中心点
     * @param strokeSet1
     * @param strokeSet2
     */
    void calculateOuterBoxAndCenterPt(StrokeSet strokeSet1, StrokeSet strokeSet2);

public:
    list <StrokeSet> inputStrokeSets;
    list <StrokeSet> outputStrokeSets;

    EquAnalyzer(list <StrokeSet> inputStrokeSets, float avgStrokeHeight);

};