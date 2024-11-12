
#ifndef CPM_PRETREE_H
#define CPM_PRETREE_H
#include "prefixNode.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstring>  // For strlen and strcmp
#include "SA_LCP_LCE.h"
#ifdef COUNT
#include "kd_tree_counting.h"

#else
#include "kd_tree_reporting.h"

#endif

#include "stNode.h"
class prefixTree
{
private:
    unsigned char* T;

public:
    explicit prefixTree(std::vector<pair<INT,INT>> &prefixesStarting, SA_LCP_LCE & DS);

    pfNode * root;
    void updatePhi();
    void addPoints(std::vector<Point> &pointsD1,std::vector<Point> &pointsD2,std::vector<Point> &pointsDl,stNode* lightNode);


    void deleteTreeIteratively();
    ~prefixTree();
};











#endif