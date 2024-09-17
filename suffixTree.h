#include <string>
#include <map>

#include "stNode.h"
#ifndef CPM_SUFFIXTREE_H
#define CPM_SUFFIXTREE_H


class suffixTree
{
private:

    unsigned char* T;


public:
    explicit suffixTree(unsigned char* T,INT &text_size);
    int n;
    stNode * root;



    stNode * createNode(  stNode * u, int d );
    void createLeaf(int i, stNode * u, int d );
    void ComputeSuffixLink( stNode * u );

//    unordered_map<int,stNode*> pos2leaf;
    unordered_set<stNode*> lightNodes;
    stNode * forward_search( unsigned char* P, INT& pattern_size);

    void initHLD();
//    void getAll_ul(unordered_map<stNode *, stNode *> &result);


    void deleteTreeIteratively();
    ~suffixTree();
};


#endif