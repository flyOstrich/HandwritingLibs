#include <recognizeObjects.h>

/**
 * 定义 主从关系 形如 m^n(m的n次方)这种形式，拥有上标或者下标的数学表达式，
 * 被称为主从关系，其中m为主（Master）,n为从（Follower）
 * 注：本程序只考虑上标关系
 * 如果 firstStrokeSet为主，则返回true,反之 false
 * @param firstStrokeSet
 * @param secondStrokeSet
 * @return
 */
bool hasTopRightRelationship(StrokeSet firstStrokeSet, StrokeSet secondStrokeSet) {
    float firstArea = firstStrokeSet.main_part_border.width * firstStrokeSet.main_part_border.height;
    float secondArea = secondStrokeSet.main_part_border.width * secondStrokeSet.main_part_border.height;
    StrokeSet masterStrokeSet;
    StrokeSet followerStrokeSet;
    if (firstArea > secondArea) { //面积大的笔画集为主
        masterStrokeSet = firstStrokeSet;
        followerStrokeSet = secondStrokeSet;
    } else {
        masterStrokeSet = secondStrokeSet;
        followerStrokeSet = firstStrokeSet;
    }
}