#include "recognizer.h"
#include "image-util.h"

using namespace Util;
using namespace cv;

void Recognizer::pushStroke(Mat &stroke, string stroke_id) {
    Mat preProccessRes = stroke.clone();
    int bgColor = ImageConverter::getImageBgColor(preProccessRes);
    Rect strokeBorder = ImageConverter::getImageBorderBox(preProccessRes, bgColor);
    Point centerPt = ImageConverter::getStrokeCenterPoint(preProccessRes, strokeBorder, bgColor);
//    imshow("res",preProccessRes);
//    waitKey(0);
    Stroke writingStroke;
    writingStroke.stroke_mat = preProccessRes;
    writingStroke.stroke_border = strokeBorder;
    writingStroke.stroke_id = stroke_id;
    writingStroke.strokeBgColor = bgColor;
    writingStroke.centerPt = centerPt;

//    this->drawBorderForStroke(writingStroke);
//    this->drawCenterPtForStroke(writingStroke);

    this->strokes.push_front(writingStroke);
}

void Recognizer::drawBorderForStroke(Stroke stroke) {
    Rect border = stroke.stroke_border;
    const Scalar color(150);//画笔颜色
    //画上边界
    line(stroke.stroke_mat, Point(border.x, border.y), Point(border.x + border.width, border.y), color);
    line(stroke.stroke_mat, Point(border.x, border.y), Point(border.x, border.y + border.height), color);
    line(stroke.stroke_mat, Point(border.x + border.width, border.y),
         Point(border.x + border.width, border.y + border.height), color);
    line(stroke.stroke_mat, Point(border.x, border.y + border.height),
         Point(border.x + border.width, border.y + border.height), color);
}

void Recognizer::drawCenterPtForStroke(Stroke stroke) {
    const Scalar color(150);//画笔颜色
    circle(stroke.stroke_mat, stroke.centerPt, 5, color);
}

Mat Recognizer::combineStrokeMat(list<Stroke> strokes) {
    Mat fstMat = strokes.front().stroke_mat;
    Mat res = fstMat.clone();
    for (int i = 0; i < res.rows; i++) {
        for (int j = 0; j < res.cols; j++) {
            uchar *rowPtr = res.ptr<uchar>(i);
            int maxVal = rowPtr[j];
            for (auto it = strokes.cbegin(); it != strokes.cend(); ++it) {
                Stroke s = *it;
                uchar *rowPtr2 = s.stroke_mat.ptr<uchar>(i);
                maxVal = std::max(maxVal, (int) rowPtr2[j]);
            }
            rowPtr[j] = (int) maxVal;
        }
    }
    return res;
}

int Recognizer::calculateAvgRowHeight(list<Stroke> strokes) {
    Rect border;
    int cnt = 0;
    int total = 0;
    for (auto it = strokes.cbegin(); it != strokes.cend(); ++it) {
        border = ((Stroke) *it).stroke_border;
        if (border.height > 10) { //高小于10的可能时写 横，忽略不计
            total += border.height;
            cnt++;
        }
    }
    if (cnt == 0)return 10;
    else return total / cnt*0.8;
}

list<Row> Recognizer::getRows(list<Stroke> strokes) {
    struct Group {
       list<Stroke> strokes;

    };
    int averageHeight=this->calculateAvgRowHeight(strokes);

}


list<Stroke> Recognizer::getStrokes() {
    return this->strokes;
}