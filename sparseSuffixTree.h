#ifndef CPM_SPARSESUFFIXTREE_H
#define CPM_SPARSESUFFIXTREE_H



#include <string>
#include <map>

#include "sstNode.h"

#include <sdsl/bit_vectors.hpp>					  // include header for bit vectors
#include <sdsl/rmq_support.hpp>					  //include header for range minimum queries
using namespace sdsl;
class sparseSuffixTree
{

public:

    sparseSuffixTree(vector<uint64_t> * &final_ssa, vector<uint64_t> * &final_lcp,unsigned char* T, INT &text_size);
    sstNode * root;

    unsigned char* T;
    INT n;


//    unordered_map<int,stNode*> pos2leaf;
    unordered_set<sstNode*> lightNodes;
    sstNode * forward_search( unsigned char* P, INT& pattern_size);

    void initHLD();
    void clearLeaves();
//    void generateDot(stNode* node, std::ofstream& dotFile, bool suf);
//    void exportSuffixTreeToDot(const std::string& filename,bool suf);

//    void getAll_ul(unordered_map<stNode *, stNode *> &result);


    void deleteTreeIteratively();
    ~sparseSuffixTree();
};

#endif


