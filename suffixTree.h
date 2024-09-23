#include <string>
#include <map>

#include "stNode.h"
#ifndef CPM_SUFFIXTREE_H
#define CPM_SUFFIXTREE_H


class suffixTree
{




public:

    explicit suffixTree(unsigned char* T,INT &text_size);
    INT n;
    stNode * root;

    unsigned char* T;

    stNode * createNode(  stNode * u, INT d );
    void createLeaf(INT i, stNode * u, INT d );
    void ComputeSuffixLink( stNode * u );

//    unordered_map<int,stNode*> pos2leaf;
    unordered_set<stNode*> lightNodes;
    stNode * forward_search( unsigned char* P, INT& pattern_size);

    void initHLD();
    void clearLeaves();
//    void generateDot(stNode* node, std::ofstream& dotFile, bool suf);
//    void exportSuffixTreeToDot(const std::string& filename,bool suf);

//    void getAll_ul(unordered_map<stNode *, stNode *> &result);


    void deleteTreeIteratively();
    ~suffixTree();
};


#endif