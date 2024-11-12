
#ifndef CPM_KD_TREE_COUNTING_H
#define CPM_KD_TREE_COUNTING_H

#include <climits>



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


    INT operator[](int idx) const {
        return coords[idx];
    }
};





struct Node {
    Point point;
    Node* left = nullptr;
    Node* right = nullptr;
    INT size;  // size of subtree
    vector<pair<INT, INT>> cell; // cell

    Node(const Point& p, const vector<pair<INT, INT>>& bounds) : point(p), cell(bounds) {}

    ~Node() {
        delete left;
        delete right;
    }

};

struct StackSearchNode {
    Node* node;
    INT depth;
};


struct StackNode {
    Node** nodeRef;                         // 当前节点的指针
    vector<Point>::iterator start;          // 点的起始迭代器
    vector<Point>::iterator end;            // 点的结束迭代器
    INT depth;                              // 当前节点的深度
    vector<pair<INT, INT>> bounds;          // 当前节点的边界（需要添加）
};


//struct Node {
//    Point point;
//    Node* left;
//    Node* right;
//    Node(const Point& pt) : point(pt), left(nullptr), right(nullptr) {}
//    ~Node() {
//        delete left;  // Recursively delete left child
//        delete right; // Recursively delete right child
//    }
//};



class KDTreeCounting {
public:
    KDTreeCounting(vector<Point>& points);


    INT rangeSearch(const vector<pair<INT, INT>>& ranges);

    Node* root;

    INT dim; // dimension

    // build kd-tree
    Node* buildIterative(vector<Point>& points);
    bool isTreeBalancedBySize(Node* node);


//    void rangeSearchIterative(Node* node, const vector<pair<INT, INT>>& ranges, INT &counter);
    ~KDTreeCounting();



};













#endif


