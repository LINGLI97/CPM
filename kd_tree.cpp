#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "kd_tree.h"
// Function to create a new node
Node* KD::newNode(Point point) {
    Node* node = new Node;
    node->point = point;
    node->left = node->right = nullptr;
    return node;
}

// Function to insert a new node into the K-D tree
Node*  KD::insertNode(Node* root, Point point, unsigned depth)
{
    if (root == nullptr)
    {
        return newNode(point);
    }
    unsigned k = point.coords.size(); // number of demensions
    unsigned cd = depth % k;
    if (point.coords[cd] < (root->point.coords[cd]))
    {
        root->left = insertNode(root->left, point, depth + 1);
    }
    else
    {
        root->right = insertNode(root->right, point, depth + 1);
    }
    return root;
}

// Function to perform a range search and return points within a specified range
void KD::range_Search(Node* root, Point &low,  Point &high, unsigned depth){
    if (root == nullptr) return;
    unsigned k = low.coords.size();
    unsigned cd = depth % k;
    if (root->point.coords[cd] >= low.coords[cd])
    {
        range_Search(root->left, low, high, depth + 1);
    }
    if (root->point.coords[cd] <= high.coords[cd] && root->point.coords[cd] >= low.coords[cd])
    {
        // This point is within the range
        std::cout << "Point: [";
        for (size_t i = 0; i < k - 1; ++i)
        {
            std::cout << root->point.coords[i] << ", ";
        }
        std::cout << root->point.coords[k - 1] << "]\n";
    }
    if (root->point.coords[cd] <= high.coords[cd])
    {
        range_Search(root->right, low, high, depth + 1);
    }
}

KD::KD(std::vector<Point> &points){
    this->KDroot = nullptr;
    for (const auto& point: points)
    {
        KDroot = insertNode(KDroot, point, 0);
    }

}

KD::~KD() {
    delete KDroot;
}


