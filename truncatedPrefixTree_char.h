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
    //kd
//    void addPoints(std::vector<Point> &pointsD1,std::vector<Point> &pointsD2,std::vector<Point> &pointsDl,stNode* lightNode);

//Range tree
//    void addPoints(std::vector<RangeTree::Point<INT, INT>> &pointsD1,std::vector<RangeTree::Point<INT, INT>>  &pointsD2,std::vector<RangeTree::Point<INT, INT>> &pointsDl,stNode* lightNode);
//Rtree
    void addPoints(vector<point5> &pointsD1,vector<point5> &pointsD2,vector<point3> &pointsDl,stNode* lightNode);


    void deleteTreeIteratively();

    unordered_set<pfNode*> lightNodes;


    ~truncatedPrefixTree();



};

#endif