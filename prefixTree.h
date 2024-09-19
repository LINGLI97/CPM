
#ifndef CPM_PRETREE_H
#define CPM_PRETREE_H
#include "prefixNode.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstring>  // For strlen and strcmp
#include "SA_LCP_LCE.h"

#include "kd_tree.h"
#include "stNode.h"
class prefixTree
{
private:
    unsigned char* T;

    std::vector<pair<INT,INT>> prefixesStarting;
public:
    explicit prefixTree(std::vector<pair<INT,INT>> prefixesStarting, SA_LCP_LCE & DS);

    pfNode * root;
    void updatePhi();
    void addPoints(std::vector<Point> &pointsD1,std::vector<Point> &pointsD2,std::vector<Point> &pointsDl,stNode* lightNode, bool flag_Dl);


    void deleteTreeIteratively();
    ~prefixTree();
};











#endif