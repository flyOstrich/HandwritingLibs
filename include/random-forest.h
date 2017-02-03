#include "common.h"

class RandomForest : public cv::ml::RTrees {
public:
    std::pair<int, float> predict_multi_class(const Mat *sample) const;
};