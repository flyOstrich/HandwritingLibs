#include "recognizeObjects.h"

namespace DebugUtil {
    void showFractions(list <StrokeSet> strokeSets);

    Mat combineStrokeMat(list <Stroke> strokes, Size target_size);

    void drawCenterPtForStroke(Stroke stroke);

    void drawBorderForStroke(Stroke stroke, Rect border);
}