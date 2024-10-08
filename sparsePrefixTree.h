#ifndef CPM_SPARSEPREFIXTREE_H
#define CPM_SPARSEPREFIXTREE_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstring>
#include "SA_LCP_LCE.h"
#include "kd_tree.h"
#include "sstNode.h"
#include "sparsePrefixNode.h"

class sparsePrefixTree
{
private:
    unsigned char* T;

public:
    sparsePrefixTree(std::vector<pair<INT, INT>> &prefixesStarting,unsigned char * T, INT &text_size, std::unordered_map <INT, INT> &inv_final_ssa_rev, vector<uint64_t> * &final_lcp_rev, sdsl::rmq_succinct_sct<> &rmq);

    spfNode * root;
    void updatePhi();
    void addPoints(std::vector<Point> &pointsD1, std::vector<Point> &pointsD2, std::vector<Point> &pointsDl, sstNode* lightNode);

    void deleteTreeIteratively();
    ~sparsePrefixTree();
};

#endif
