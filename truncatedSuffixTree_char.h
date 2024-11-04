
#ifndef CPM_TRUNCATEDSUFFIXTREE_H
#define CPM_TRUNCATEDSUFFIXTREE_H
//#define USE_KEPT_LENGTH

#include "suffixTree.h"
#include <stack>
#include <tuple>
#include <iostream>
class truncatedSuffixTree {
public:
    unsigned char* T;

    stNode* root;

    truncatedSuffixTree(suffixTree *& originalTree, INT &B, vector<INT> &hash_positions);


    stNode* copyAndTruncate(stNode* originalRoot, INT &B);

    //binary search
    INT find_lower_bound_value(INT &value);
    vector<INT> *hash_positions;
    void deleteTreeIteratively();

    void clearLeaves();

    void generateDot(stNode* node, std::ofstream& dotFile, bool suf);
    void exportSuffixTreeToDot(const std::string& filename,bool suf);
    void initHLD();
    unordered_set<stNode*> lightNodes;

    stNode * forward_search( unsigned char* P, INT& pattern_size);

    ~truncatedSuffixTree();



};

#endif