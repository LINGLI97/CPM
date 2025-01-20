
#ifndef CPM_PRETREE_H
#define CPM_PRETREE_H

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

#include "prefixNode.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstring>  // For strlen and strcmp
#include "SA_LCP_LCE.h"



//#include "RangeTree.h"





#include "stNode.h"

using point5 = bg::model::point<INT, 5, bg::cs::cartesian>;
using point3 = bg::model::point<INT, 3, bg::cs::cartesian>;

class prefixTree
{
private:
    unsigned char* T;

public:
    explicit prefixTree(std::vector<pair<INT,INT>> &prefixesStarting, SA_LCP_LCE & DS);

    pfNode * root;
    void updatePhi();

    // KD tree
//    void addPoints(std::vector<Point> &pointsD1,std::vector<Point> &pointsD2,std::vector<Point> &pointsDl,stNode* lightNode);
//Rtree
    void addPoints(vector<point5> &pointsD1,vector<point5> &pointsD2,vector<point3> &pointsDl,stNode* lightNode);


    void deleteTreeIteratively();
    ~prefixTree();
};











#endif
