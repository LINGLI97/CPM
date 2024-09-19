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


struct Point {
    vector<double> coords;

    Point(const vector<double>& c) : coords(c) {}

    // 维度大小
    int dimension() const {
        return coords.size();
    }

    // 访问坐标值
    double operator[](int idx) const {
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


    vector<Point> rangeSearch(const vector<pair<double, double>>& ranges);

    Node* root;

    int dim; // dimension

    // build kd-tree
    Node* buildIterative(vector<Point>& points);


    void rangeSearchIterative(Node* node, const vector<pair<double, double>>& ranges, vector<Point>& result);
    ~KDTree();
};



#endif
