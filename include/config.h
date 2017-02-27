#ifdef OP_WINDOWS
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

/*
class LabelCharacterMap {
private:

public:
    struct ConfigItem {
        string character_label;
        bool top;
        bool bottom;
        bool rtop;
        bool rbottom;
    };

    ConfigItem operator[](index) {

    }
};

#define  LABEL_CHARACTER_MAP new LabelCharacterMap({\
 LabelCharacterMap::ConfigItem()\
})
*/
