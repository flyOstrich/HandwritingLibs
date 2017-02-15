#include "opencv2/ml.hpp"
#include "recognizer.h"
#include "type-util.h"
#include "trainer.h"
#include "file-util.h"
#include <fstream>
#include "mat-util.h"
#include "image-util.h"


using namespace Util;
using namespace cv;

Recognizer::Recognizer() {}


Recognizer::Recognizer(string svm_model_file, string label_character_map_file, Size canvas_size, string splitImageDir,
                       string label) {
    Document document;
    this->svm = ml::StatModel::load<ml::RTrees>(svm_model_file.c_str());
    this->canvasSize = canvas_size;
    this->label_character_map = "";
    this->splitImageDir = splitImageDir;
    this->label = label;
    ifstream ifs(label_character_map_file);
    string s = "";
    if (ifs.is_open()) {
        int bufLen = 200;
        char buf[bufLen];
        while (!ifs.eof()) {
            ifs.getline(buf, bufLen);
            this->label_character_map += buf;
        }
//        printf("%s", this->label_character_map.c_str());
        document.Parse(this->label_character_map.c_str());
//        printf("\n----------------------------%s",document.GetObject()["5"].GetString());
    }
}

void Recognizer::pushStroke(list <Point> original_points, string stroke_id) {
    Mat preProccessRes = MatUtil::getGrayImageFromPointList(original_points, this->canvasSize);
    int bgColor = ImageConverter::getImageBgColor(preProccessRes);
    Rect stroke_border = ImageConverter::getImageBorderBox(preProccessRes, bgColor);
    Point centerPt = ImageConverter::getStrokeCenterPoint(preProccessRes, stroke_border, bgColor);
    Rect mainPartBorder = ImageConverter::getStrokeMainPartBorder(centerPt, stroke_border);
//    list<Point> resized_points = this->resizeOriginalPoints(original_points, this->canvasSize, TRAIN_IMAGE_SIZE);
//    Mat resized_stroke_mat = MatUtil::getGrayImageFromPointList(resized_points, TRAIN_IMAGE_SIZE);

    Stroke writingStroke;
    writingStroke.stroke_mat = preProccessRes;
    writingStroke.stroke_border = stroke_border;
    writingStroke.stroke_id = stroke_id;
    writingStroke.strokeBgColor = bgColor;
    writingStroke.centerPt = centerPt;
    writingStroke.main_part_border = mainPartBorder;
    writingStroke.original_points = original_points;
    writingStroke.single_stroke_recognize_result = this->recognizeSingleStroke(writingStroke);

    this->drawBorderForStroke(writingStroke, writingStroke.main_part_border);
    this->drawCenterPtForStroke(writingStroke);
    this->strokes.push_front(writingStroke);
}

void Recognizer::drawBorderForStroke(Stroke stroke, Rect border) {
    const Scalar color(150);//画笔颜色
    rectangle(stroke.stroke_mat, border, color);
}

void Recognizer::drawCenterPtForStroke(Stroke stroke) {
    const Scalar color(150);//画笔颜色
    circle(stroke.stroke_mat, stroke.centerPt, 5, color);
}

Mat Recognizer::combineStrokeMat(list <Stroke> strokes, Size target_size) {

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

Mat Recognizer::combineStrokeMat(list <Stroke> strokes) {
    Mat fstMat = strokes.front().stroke_mat;
    Mat res = fstMat.clone();
    uchar *rowPtr2;
    Stroke s;
    for (int i = 0; i < res.rows; i++) {
        for (int j = 0; j < res.cols; j++) {
            uchar *rowPtr = res.ptr<uchar>(i);
            int maxVal = rowPtr[j];
            for (auto it = strokes.cbegin(); it != strokes.cend(); ++it) {
                s = *it;
                rowPtr2 = s.stroke_mat.ptr<uchar>(i);
                maxVal = std::max(maxVal, (int) rowPtr2[j]);
            }
            rowPtr[j] = maxVal;
        }
    }
    return res;
}

int Recognizer::calculateAvgRowHeight(list <Stroke> strokes) {
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
    else return total / cnt;
}


list <list<string>> Recognizer::recognize() {
    printf("\n");
    Document document;
    document.Parse(this->label_character_map.c_str());
    Value label_character_m = document.GetObject();
    list <Stroke> strokes = this->strokes;
    this->clusterAnalyzer.cluster_max_height = this->calculateAvgRowHeight(strokes);
    list <Category> categories = this->clusterAnalyzer.getRecognizeUnitsForCategories(strokes);
    int i = 0;
    list <list<string>> rt;
    for (auto it = categories.cbegin(); it != categories.cend(); ++it) {
        int j = 0;
        i++;
        Category category = *it;
        list <RecognizeUnit> units = category.recognize_units;
        list <string> rowResult;
        for (auto it2 = units.cbegin(); it2 != units.cend(); ++it2) {
            j++;
            string winName = "result" + TypeConverter::int2String(i) + TypeConverter::int2String(j);
            RecognizeUnit unit = *it2;
            Mat combinedMat = this->combineStrokeMat(unit.strokes, TRAIN_IMAGE_SIZE);
            imshow(winName, combinedMat);
            moveWindow(winName, (j - 1) * 100, (i - 1) * 100);
            if (this->splitImageDir != "NONE") {
                this->saveSplitImage(combinedMat, this->label);
            }
            Mat descriptorMat = Trainer::HogComputer::getHogDescriptorForImage(combinedMat);
            int rec_label = this->svm->predict(descriptorMat);
            std::list<std::pair<int, float> > resa = this->svm->predict_prob(descriptorMat);
            cout << "********predict result********" << endl;
            while (!resa.empty()) {
                pair<int, float> it = resa.front();
                cout << it.first << "----->" << it.second << endl;
                resa.pop_front();
            }
            string key = TypeConverter::int2String(rec_label);
            string res = label_character_m[key.c_str()].GetString();
            rowResult.push_front(res);
            printf("use: %s\t\n", res.c_str());
//            cvWaitKey(0);
        }
        rt.push_front(rowResult);
    }
    return rt;
}

list <Point> Recognizer::resizeOriginalPoints(list <Point> original_points, Size original_size, Size target_size) {
    float scaleX = (float) target_size.width / original_size.width;
    float scaleY = (float) target_size.height / original_size.height;
    list <Point> rt;
    for (auto it = original_points.cbegin(); it != original_points.cend(); ++it) {
        Point pt = *it;
        float targetX = std::round(pt.x * scaleX);
        float targetY = std::round(pt.y * scaleY);
        rt.push_front(Point(targetX, targetY));
    }
    return rt;
}

void Recognizer::saveSplitImage(Mat image, string label) {
    vector<string> files;
    Util::FileUtil::getFiles(this->splitImageDir, files);
    string imageLoc = this->splitImageDir + "/" + label + "_" + TypeConverter::int2String(files.size() + 1) + ".bmp";
    imwrite(imageLoc.c_str(), image);
}

int Recognizer::recognizeSingleStroke(Stroke stroke) {
    list <Stroke> strokes;
    strokes.push_back(stroke);
    Mat combinedMat = this->combineStrokeMat(strokes, TRAIN_IMAGE_SIZE);
    Mat descriptorMat = Trainer::HogComputer::getHogDescriptorForImage(combinedMat);
    int res = this->svm->predict(descriptorMat);
    cout << "Recognizer::recognizeSingleStroke->" << res << endl;
    return res;
}

