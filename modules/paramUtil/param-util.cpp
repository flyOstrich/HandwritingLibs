#include "include/param-util.h"

std::vector<std::string> Util::ParamConverter::convertJobjectArrayToVector(JNIEnv *env,
                                                                           jobjectArray joarr) {
    std::vector<std::string> res_vector;
    int len = env->GetArrayLength(joarr);
    for (int i = 0; i < len; i++) {
        jstring j_str = (jstring) env->GetObjectArrayElement(joarr, i);
        const char *c_str = env->GetStringUTFChars(j_str, 0);
        std::string str = c_str;
        env->ReleaseStringUTFChars(j_str,c_str);
        env->DeleteLocalRef(j_str);
        res_vector.push_back(str);
    }
    return res_vector;
}

std::string Util::ParamConverter::convertJstringToString(JNIEnv *env, jstring jstr) {
    const char* c_path_str=env->GetStringUTFChars(jstr,0);
    std::string str=c_path_str;
    env->ReleaseStringUTFChars(jstr,c_path_str);
    return str;
}

std::list< std::pair<int,cv::Mat> > Util::ParamConverter::convertJobjectArrayToMatVector(JNIEnv *env,
                                                                          jobjectArray joarr,jintArray labels) {
    jobject obj;
    jclass class_mat;
    jmethodID id_getNativeObj;
    jlong matAddr;
    std::list<  std::pair<int,cv::Mat> > res_list;
    int len = env->GetArrayLength(joarr);
    jint* j_labels=env->GetIntArrayElements(labels,0);
    for (int i = 0; i < len; i++) {
        obj=env->GetObjectArrayElement(joarr, i);
        jint j_label=j_labels[i];
        class_mat = env->GetObjectClass(obj);
        id_getNativeObj=env->GetMethodID(class_mat,"getNativeObjAddr","()J");
        matAddr=env->CallLongMethod(obj,id_getNativeObj);
        cv::Mat imgMat=*(cv::Mat*)matAddr;
        int i_label=j_label;
        res_list.push_back(std::pair<int,cv::Mat>(i_label,imgMat));
        env->DeleteLocalRef(obj);
        env->DeleteLocalRef(class_mat);
    }
    env->ReleaseIntArrayElements(labels,j_labels,0);
    return res_list;
}