#include "android-interface.h"
#include "common.h"
#include "image-util.h"
#include "trainer.h"

using namespace Util;

void saveTrainImage(Mat train_img, string img_save_location) {
    Mat noEmptyRes = ImageConverter::removeEmptySpace(train_img);
    Mat resizedRes = Util::ImageConverter::resize(noEmptyRes, TRAIN_IMAGE_SIZE);
    imwrite(img_save_location.c_str(), resizedRes);
}
