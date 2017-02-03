//#include "random-forest.h"
//
//std::pair<int, float> RandomForest::predict_multi_class(const Mat *sample) const {
//    int result = 0;
//    int k;
//
//    if( this->getVarCount() > 0 ) //classification
//    {
//        int max_nvotes = 0;
//        int* votes = out_votes;
//        memset( votes, 0, sizeof(*votes)*nclasses );
//        for( k = 0; k < ntrees; k++ )
//        {
//            CvDTreeNode* predicted_node = trees[k]->predict( sample, missing );
//            int nvotes;
//            int class_idx = predicted_node->class_idx;
//            CV_Assert( 0 <= class_idx && class_idx < nclasses );
//
//            nvotes = ++votes[class_idx];
//
//        }
//
//        result = ntrees;
//    }
//    else // regression
//    {
//        throw std::runtime_error(__FUNCTION__ "can only be used classification");
//    }
//
//    return result;
//}