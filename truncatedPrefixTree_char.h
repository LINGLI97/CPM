#ifndef CPMLING_PREFIXTREE_CHAR_H
#define CPMLING_PREFIXTREE_CHAR_H

#include "prefixTree.h"

class truncatedPrefixTree {
public:
    pfNode* root;

    truncatedPrefixTree(prefixTree& originalTree, INT &B, vector<INT> &hash_positions);

//    stNode* copyAndTruncateB(stNode* originalRoot, INT &B, vector<INT> &hash_positions);

    pfNode* copyAndTruncate(pfNode* originalRoot, INT &B);

    //binary search
    INT find_lower_bound_value(INT &value);
    vector<INT> *hash_positions;
    void updatePhi();
    void addPoints(std::vector<Point> &pointsD1,std::vector<Point> &pointsD2,std::vector<Point> &pointsDl,stNode* lightNode);

    void deleteTreeIteratively();

    unordered_set<pfNode*> lightNodes;


    ~truncatedPrefixTree();



};

#endif