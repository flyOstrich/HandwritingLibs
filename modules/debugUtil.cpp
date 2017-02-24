#include "debugUtil.h"
#include "image-util.h"

using namespace Util;

void showFactionsIteration(list <StrokeSet> strokeSets,string direction="none") {
    for (auto it = strokeSets.cbegin(); it != strokeSets.cend(); ++it) {
        StrokeSet strokeSet = *it;
        cout<<"debug::StrokeSet Type->"<<direction<<" "<<(strokeSet.strokeSetType==FRACTION_STROKE_SET)<<endl;

        if (strokeSet.strokeSetType == FRACTION_STROKE_SET) {
            Mat combinedMat = DebugUtil::combineStrokeMat(strokeSet.strokes, Size(400, 400));
            imshow("frac", combinedMat);
            cvWaitKey(0);
            cout<<"debug::top size"<<strokeSet.top.size()<<endl;
            cout<<"debug::bottom size"<<strokeSet.bottom.size()<<endl;
            if (strokeSet.top.size() > 0) {
                showFactionsIteration(strokeSet.top,"top");
            }
            if (strokeSet.bottom.size() > 0) {
                showFactionsIteration(strokeSet.bottom,"bottom");
            }
        }else{
//            Mat combinedMat = DebugUtil::combineStrokeMat(strokeSet.strokes, Size(400, 400));
//            imshow("not frac", combinedMat);
//            cvWaitKey(0);
        }
    }
}

void DebugUtil::showFractions(list <StrokeSet> strokeSets) {
    showFactionsIteration(strokeSets);
}


Mat DebugUtil::combineStrokeMat(list <Stroke> strokes, Size target_size) {

//1. 找出所有笔画的最小外接矩形
    Rect combinedMatSize(-1, -1, 0, 0);
    Stroke s;
    int x, y, width, height;
    for (auto it = strokes.cbegin(); it != strokes.cend(); ++it) {
        s = *it;
        combinedMatSize.x == -1 ? x = s.stroke_border.x
                                : x = std::min(combinedMatSize.x, s.stroke_border.x);
        combinedMatSize.y == -1 ? y = s.stroke_border.y
                                : y = std::min(combinedMatSize.y, s.stroke_border.y);
        combinedMatSize.width == 0 ? width = s.stroke_border.width
                                   : width = std::max(combinedMatSize.x + combinedMatSize.width,
                                                      s.stroke_border.x + s.stroke_border.width) - x;
        combinedMatSize.height == 0 ? height = s.stroke_border.height
                                    : height = std::max(combinedMatSize.y + combinedMatSize.height,
                                                        s.stroke_border.y + s.stroke_border.height) - y;
        combinedMatSize.x = x;
        combinedMatSize.y = y;
        combinedMatSize.width = width;
        combinedMatSize.height = height;
    }
//2.以最小外接矩形的左上角点为原点，重新调整所有笔画的路径顺序
    struct StokePrecess {
        list <Point> stroke_points;
        string stroke_id;
    };
    list <StokePrecess> commonOuterBoxStrokes; //拥有共同原点的笔画路径
    Point ori_pt, pt;
    for (auto it = strokes.cbegin(); it != strokes.cend(); ++it) {
        s = *it;
        StokePrecess sp;
        sp.stroke_id = s.stroke_id;
        for (auto it2 = s.original_points.cbegin(); it2 != s.original_points.cend(); ++it2) {
            ori_pt = *it2;
            pt.y = ori_pt.y - combinedMatSize.y;
            pt.x = ori_pt.x - combinedMatSize.x;
            sp.stroke_points.push_back(pt);
        }
        commonOuterBoxStrokes.push_back(sp);
    }
//3.将调整后笔画路径数据缩放到target_size大小的矩形内
    list <StokePrecess> targetSizeStrokes;
    list <Point> all_stroke_points;
    bool isScaleWidth = combinedMatSize.width > combinedMatSize.height;
    float scale;
    if (isScaleWidth)
        scale = (float) target_size.width / combinedMatSize.width;
    else
        scale = (float) target_size.height / combinedMatSize.height;
    for (auto it = commonOuterBoxStrokes.cbegin(); it != commonOuterBoxStrokes.cend(); ++it) {
        StokePrecess sp = *it;
        list <Point> stroke_points;
        for (auto it2 = sp.stroke_points.cbegin(); it2 != sp.stroke_points.cend(); ++it2) {
            ori_pt = *it2;
            pt.y = (int) std::floor(ori_pt.y * scale);
            pt.x = (int) std::floor(ori_pt.x * scale);
            stroke_points.push_back(pt);
            all_stroke_points.push_back(pt);
        }
        StokePrecess nextSp;
        nextSp.stroke_points = stroke_points;
        nextSp.stroke_id = sp.stroke_id;
        targetSizeStrokes.push_back(nextSp);
    }
//4.调整点的位置，使笔画组成的字在target_size大小的窗口中居中
    list <StokePrecess> centeredStrokes;
    Rect targetSizeCombinedMatBorder = ImageConverter::getImageBorderBox(all_stroke_points);
    int xStep = target_size.width / 2 - (targetSizeCombinedMatBorder.x + targetSizeCombinedMatBorder.width / 2);
    int yStep = target_size.height / 2 - (targetSizeCombinedMatBorder.y + targetSizeCombinedMatBorder.height / 2);
    for (auto it = targetSizeStrokes.cbegin(); it != targetSizeStrokes.cend(); ++it) {
        StokePrecess sp = *it;
        StokePrecess nextSp;
        list <Point> stroke_points;
        for (auto it2 = sp.stroke_points.cbegin(); it2 != sp.stroke_points.cend(); ++it2) {
            Point pt = *it2;
            Point nextPt;
            nextPt.x = pt.x + xStep;
            nextPt.y = pt.y + yStep;
            stroke_points.push_back(nextPt);
        }
        nextSp.stroke_id = sp.stroke_id;
        nextSp.stroke_points = stroke_points;
        centeredStrokes.push_back(nextSp);
    }
//5.将调整后的笔画中的点连接起来，构成最后字的图片
    Mat combinedMat = Mat::zeros(target_size.height, target_size.width, CV_8UC1);
    for (auto it = centeredStrokes.cbegin(); it != centeredStrokes.cend(); ++it) {
        StokePrecess sp = *it;
        Point previousPt = sp.stroke_points.front();
        for (auto it2 = sp.stroke_points.cbegin(); it2 != sp.stroke_points.cend(); ++it2) {
            Point nextPt = *it2;
            line(combinedMat, previousPt, nextPt, Scalar(255));
            previousPt = nextPt;
        }
    }


//    Mat fstMat = strokes.front().stroke_mat;
//    Mat res = fstMat.clone();
//    uchar *rowPtr2;
//    for (int i = 0; i < res.rows; i++) {
//        for (int j = 0; j < res.cols; j++) {
//            uchar *rowPtr = res.ptr<uchar>(i);
//            int maxVal = rowPtr[j];
//            for (auto it = strokes.cbegin(); it != strokes.cend(); ++it) {
//                s = *it;
//                rowPtr2 = s.stroke_mat.ptr<uchar>(i);
//                maxVal = std::max(maxVal, (int) rowPtr2[j]);
//            }
//            rowPtr[j] = maxVal;
//        }
//    }
    return combinedMat;
}