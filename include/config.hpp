#pragma once

#ifdef OP_WINDOWS
//Config::stringFile = "D:\\workspace\\HandwritingLibs\\assets\\painter_stroke.txt";
//Config::svmModelFile = "D:\\workspace\\HandwritingLibs\\assets\\train.yml";
//Config::labelCharacterMapFile = "D:\\workspace\\HandwritingLibs\\modules\\trainer\\label_character_map.json";
//Config::trainImageDir = "D:\\workspace\\HandwritingLibs\\modules\\trainer\\train-images";
//Config::svmTrainImageDir = "D:\\workspace\\HandwritingLibs\\assets\\train-images";
#define STROKE_FILE "D:\\workspace\\HandwritingLibs\\assets\\painter_stroke.txt"
#define SVM_MODEL_FILE "D:\\workspace\\HandwritingLibs\\assets\\train.yml"
#define LABEL_CHARACTER_MAP_FILE "D:\\workspace\\HandwritingLibs\\modules\\trainer\\label_character_map.json"
#define TRAIN_IMAGE_DIR  "D:\\workspace\\HandwritingLibs\\modules\\trainer\\train-images"
#define SVM_TRAIN_IMAGE_DIR "D:\\workspace\\HandwritingLibs\\assets\\train-images"
#endif
#ifdef OP_DARWIN
#define STROKE_FILE  "/Users/pjl/HandwritingLibs/assets/painter_stroke3.txt"
#define SVM_MODEL_FILE "/Users/pjl/HandwritingLibs/assets/train.yml"
#define LABEL_CHARACTER_MAP_FILE "/Users/pjl/HandwritingLibs/modules/trainer/label_character_map.json"
#define TRAIN_IMAGE_DIR "/Users/pjl/HandwritingLibs/modules/trainer/train-images"
#define SVM_TRAIN_IMAGE_DIR "/Users/pjl/HandwritingLibs/assets/train-images"
#endif
#ifdef OP_ANDROID
#define STROKE_FILE  "/Users/pjl/HandwritingLibs/assets/painter_stroke3.txt"
#define SVM_MODEL_FILE "/Users/pjl/HandwritingLibs/assets/train.yml"
#define LABEL_CHARACTER_MAP_FILE "/Users/pjl/HandwritingLibs/modules/trainer/label_character_map.json"
#define TRAIN_IMAGE_DIR "/Users/pjl/HandwritingLibs/modules/trainer/train-images"
#define SVM_TRAIN_IMAGE_DIR "/Users/pjl/HandwritingLibs/assets/train-images"
#endif


#define MINUS_OR_FRACTION_BAR_LABEL 11 //减号或者分数线的label
#define VERTICAL_OR_NUMBER_ONE_LABEL 1//竖线或者数字1的label
#define EQUATION_LABEL 18 //等号label
#define ADD_LABEL 10 //加号label
#define LEFT_SLASH 19 //左斜杠
#define RIGHT_SLASH 20 //右斜杠
#define PLUS_LABEL 12//乘号
#define FIRST_OF_FIVE 21//数字“5”的第一笔
#define FIVE_LABEL 5 //5


