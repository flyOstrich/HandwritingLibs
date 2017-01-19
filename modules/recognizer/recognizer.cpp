#include "recognizer.h"
#include "image-util.h"
#include "type-util.h"
#include "mat-util.h"
#include "trainer.h"
#include <fstream>


using namespace Util;
using namespace cv;

Recognizer::Recognizer() {}


Recognizer::Recognizer(string svm_model_file, string label_character_map_file, Size canvas_size) {
    Document document;
    this->svm = ml::StatModel::load<ml::SVM>(svm_model_file.c_str());
    this->canvasSize = canvas_size;
    this->label_character_map="";
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

void Recognizer::pushStroke(list<Point> original_points, string stroke_id) {
    Mat preProccessRes = MatUtil::getGrayImageFromPointList(original_points, this->canvasSize);
    int bgColor = ImageConverter::getImageBgColor(preProccessRes);
    Rect stroke_border = ImageConverter::getImageBorderBox(preProccessRes, bgColor);
    Point centerPt = ImageConverter::getStrokeCenterPoint(preProccessRes, stroke_border, bgColor);
    Rect mainPartBorder = ImageConverter::getStrokeMainPartBorder(centerPt, stroke_border);
    list<Point> resized_points = this->resizeOriginalPoints(original_points, this->canvasSize, TRAIN_IMAGE_SIZE);
    Mat resized_stroke_mat = MatUtil::getGrayImageFromPointList(resized_points, TRAIN_IMAGE_SIZE);

    Stroke writingStroke;
    writingStroke.stroke_mat = preProccessRes;
    writingStroke.stroke_border = stroke_border;
    writingStroke.stroke_id = stroke_id;
    writingStroke.strokeBgColor = bgColor;
    writingStroke.centerPt = centerPt;
    writingStroke.main_part_border = mainPartBorder;
    writingStroke.original_points = original_points;
    writingStroke.resized_stroke_mat = resized_stroke_mat;

//    this->drawBorderForStroke(writingStroke,writingStroke.main_part_border);
//    this->drawCenterPtForStroke(writingStroke);

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

Mat Recognizer::combineStrokeMat(list<Stroke> strokes) {
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
    else return total / cnt * 0.8;
}


list<list<string> > Recognizer::recognize() {
    printf("\n");
    Document document;
    document.Parse(this->label_character_map.c_str());
    Value label_character_m=document.GetObject();
    list<Stroke> strokes = this->strokes;
    this->clusterAnalyzer.cluster_max_height = this->calculateAvgRowHeight(strokes);
    list<Category> categories = this->clusterAnalyzer.getRecognizeUnitsForCategories(strokes);
    int i = 0;
    list<list<string> > rt;
    for (auto it = categories.cbegin(); it != categories.cend(); ++it) {
        int j = 0;
        i++;
        Category category = *it;
        list<RecognizeUnit> units = category.recognize_units;
        list<string> rowResult;
        for (auto it2 = units.cbegin(); it2 != units.cend(); ++it2) {
            j++;
            string winName = "result" + TypeConverter::int2String(i) + TypeConverter::int2String(j);
            RecognizeUnit unit = *it2;
            Mat combinedMat = this->combineStrokeMat(unit.strokes);
            combinedMat = Util::ImageConverter::removeEmptySpace(combinedMat);
            combinedMat = Util::ImageConverter::resize(combinedMat, TRAIN_IMAGE_SIZE);
//            combinedMat= Util::ImageConverter::thinImage(combinedMat);
            Mat descriptorMat = Trainer::HogComputer::getHogDescriptorForImage(combinedMat);
//            imshow("res" + TypeConverter::int2String(i) + TypeConverter::int2String(j), combinedMat);
            int rec_label = this->svm->predict(descriptorMat);
            string key=TypeConverter::int2String(rec_label);
            string res=label_character_m[key.c_str()].GetString();
            rowResult.push_front(res);
            printf("%s\t", res.c_str());
            imshow(winName,combinedMat);
            moveWindow(winName, (j-1) * 100, (i-1) * 100);
//            cvWaitKey(0);
        }
        printf("\n");
        rt.push_front(rowResult);
    }
    return rt;
}

list<Point> Recognizer::resizeOriginalPoints(list<Point> original_points, Size original_size, Size target_size) {
    float scaleX = (float) target_size.width / original_size.width;
    float scaleY = (float) target_size.height / original_size.height;
    list<Point> rt;
    for (auto it = original_points.cbegin(); it != original_points.cend(); ++it) {
        Point pt = *it;
        float targetX = std::round(pt.x * scaleX);
        float targetY = std::round(pt.y * scaleY);
        rt.push_front(Point(targetX, targetY));
    }
    return rt;
}

