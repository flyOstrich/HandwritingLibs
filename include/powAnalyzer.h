#include "recognizeObjects.h"

/**
 * 乘幂识别器
 */
class PowAnalyzer {
private:
    list <StrokeSet> inputStrokeSets;
    list <StrokeSet> hasSuperscriptsStrokeSets;//可以带上标的符号集合
    void analyze();

    /**
     * 找到带上标的符号集合
     */
    void findHasSuperscriptsStrokeSets();

    /**
     * 找到乘幂笔画集合
     */
    void findPowExpStrokeSets();

    bool findPowExpStrokeSetsIteration();

    /**
     * 判断两笔是否构成乘幂的位置关系
     *    如果构成，且第一笔strokeSet1为底数，return 1,反之return2;
     *    如果不构成，return -1;
     * @param strokeSet1
     * @param strokeSet2
     * @return
     */
    int isSatisfyingPowCriteria(StrokeSet strokeSet1, StrokeSet strokeSet2);

public:
    list <StrokeSet> outputStrokeSets;

    PowAnalyzer(list <StrokeSet> inputStrokeSets);
};