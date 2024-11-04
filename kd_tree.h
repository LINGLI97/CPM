//
// Created by ling on 16/09/24.
//

#ifndef CPM_KD_TREE_H
#define CPM_KD_TREE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stack>
using namespace std;
#ifdef _USE_64
typedef int64_t INT;
#endif

#ifdef _USE_32
typedef int32_t INT;
#endif

struct Point {
    vector<INT> coords;

    Point(const vector<INT>& c) : coords(c) {}


    int dimension() const {
        return coords.size();
    }

    // 访问坐标值
    INT operator[](int idx) const {
        return coords[idx];
    }
};

// kd-tree 节点
struct Node {
    Point point;
    Node* left;
    Node* right;
    Node(const Point& pt) : point(pt), left(nullptr), right(nullptr) {}
    ~Node() {
        delete left;  // Recursively delete left child
        delete right; // Recursively delete right child
    }
};






class KDTree {
public:
    KDTree(vector<Point>& points);


    INT rangeSearch(const vector<pair<INT, INT>>& ranges);

    Node* root;

    INT dim; // dimension

    // build kd-tree
    Node* buildIterative(vector<Point>& points);


    void rangeSearchIterative(Node* node, const vector<pair<INT, INT>>& ranges, INT &counter);
    ~KDTree();
};



#endif
