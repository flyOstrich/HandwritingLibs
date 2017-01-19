#include "json_util.h"

list<Point> JsonUtil::getPointListFromJsonString(string json_string) {
    list<Point> res;
    Document document;
    if (document.Parse(json_string.c_str()).HasParseError()) {
//        LOGD("%s", " JsonUtil::getPointListFromJsonString: parse json error ! ");
        return res;
    }
    if (!document.IsArray()) {
//        LOGD("%s",
//             " JsonUtil::getPointListFromJsonString: param is not json array string ! , value -> \n " + json_string);
        return res;
    }
    Value ary = document.GetArray();
    Point pt;
    for (int i = 0; i < ary.Size(); i++) {
        Value &aryItem = ary[i];
        pt.x = round(aryItem["x"].GetFloat());
        pt.y = round(aryItem["y"].GetFloat());
        res.push_front(pt);
    }
    return res;
}