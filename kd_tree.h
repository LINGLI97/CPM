//
// Created by ling on 16/09/24.
//

#ifndef CPM_KD_TREE_H
#define CPM_KD_TREE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
struct Point {
    std::vector<double> coords;
};

struct Node {
    Point point;
    Node* left;
    Node* right;
    ~Node() {
        delete left;
        delete right;
    }
};

class KD{

public:

    KD(std::vector<Point> &points);
    Node* newNode(Point point);

    Node* insertNode(Node* root, Point point, unsigned depth);

    Point high;
    Node* KDroot;

    // Perform range search
    void range_Search(Node* root, Point &low, Point &high, unsigned depth);
    ~KD();
};
#endif
