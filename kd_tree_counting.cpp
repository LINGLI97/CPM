#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "kd_tree_counting.h"
#include <random>

#include <chrono>


//void KDTree::rangeSearchIterative(Node* node, const vector<pair<INT, INT>>& ranges, INT &counter) {
//    if (!node) return;
//
//    stack<StackSearchNode> searchStack;
//    searchStack.push({node, 0});
//
//    while (!searchStack.empty()) {
//        auto current = searchStack.top();
//        searchStack.pop();
//
//        Node* curNode = current.node;
//
////            range contains our entire cell
//
//        bool isContained = true;
//        for (INT i = 0; i < dim; ++i) {
//            if (ranges[i].first > curNode->cell[i].first || ranges[i].second < curNode->cell[i].second) {
//                isContained = false;
//                break;
//            }
//        }
//        if (isContained) {
//            counter += curNode->size;
//            continue;
//        }
//
//// no overlap with range
//        bool isDisjoint = false;
//        for (INT i = 0; i < dim; ++i) {
//            if (ranges[i].second < curNode->cell[i].first || ranges[i].first > curNode->cell[i].second) {
//                isDisjoint = true;
//                break;
//            }
//        }
//        if (isDisjoint) {
////            cout<<"isDisjoint"<<endl;
//
//            continue;
//        }
//
////        range partially overlaps cell
//
//        bool inside = true;
//        for (INT i = 0; i < dim; ++i) {
//            if (curNode->point[i] < ranges[i].first || curNode->point[i] > ranges[i].second) {
//                inside = false;
//                break;
//            }
//        }
//        if (inside) {
////            cout<<"counting"<<endl;
//
//            counter++;
//        }
//
//        // push the left and right subtree
//        if (curNode->left) {
//            searchStack.push({curNode->left, current.depth + 1});
//        }
//        if (curNode->right) {
//            searchStack.push({curNode->right, current.depth + 1});
//        }
//    }
//}
//

KDTreeCounting::KDTreeCounting(vector<Point> &points) {
    dim = points[0].dimension();
    root = buildIterative(points);

}

bool KDTreeCounting::isTreeBalancedBySize(Node* node) {
    if (!node) return true;

    int leftSize = (node->left) ? node->left->size : 0;
    int rightSize = (node->right) ? node->right->size : 0;

    // 检查当前节点的平衡性
    if (abs(leftSize - rightSize) > 1) return false;

    // 递归检查左右子树
    return isTreeBalancedBySize(node->left) && isTreeBalancedBySize(node->right);
}



Node* KDTreeCounting::buildIterative(vector<Point>& points) {



    vector<pair<INT, INT>> initialBounds(dim, {LONG_MAX, LONG_MIN});

    for (const auto& point : points) {
        for (INT i = 0; i < dim; ++i) {
            initialBounds[i].first = std::min(initialBounds[i].first, point[i]);  // 更新最小值
            initialBounds[i].second = std::max(initialBounds[i].second, point[i]); // 更新最大值
        }
    }

    stack<StackNode> stack;
    Node* root = nullptr;

    stack.push({&root, points.begin(), points.end(), 0, initialBounds});

    while (!stack.empty()) {
        auto& current = stack.top();
        Node** nodeRef = current.nodeRef;

        if (!(*nodeRef)) {
            // first time
            if (current.start >= current.end) {
                stack.pop();
                continue;
            }


            INT axis = current.depth % dim;
            auto mid = current.start + (current.end - current.start) / 2;

            nth_element(current.start, mid, current.end, [axis](const Point& a, const Point& b) {
                return a[axis] < b[axis];
            });

            vector<pair<INT, INT>> leftBounds = current.bounds;
            vector<pair<INT, INT>> rightBounds = current.bounds;


            leftBounds[axis].second = (*mid)[axis];
            rightBounds[axis].first = (*mid)[axis];

            // create new node with cell = bounds
            *nodeRef = new Node(*mid, current.bounds);

            stack.push({&(*nodeRef)->right, mid + 1, current.end, current.depth + 1, rightBounds});
            stack.push({&(*nodeRef)->left, current.start, mid, current.depth + 1, leftBounds});
        } else {
            // second time
            Node* curNode = *nodeRef;

            // update the size of subtree
            curNode->size = 1;
            if (curNode->left) {
                curNode->size += curNode->left->size;
//                curNode->cell.insert(curNode->cell.end(),
//                                     curNode->left->cell.begin(), curNode->left->cell.end());  // 合并左子树边界
            }
            if (curNode->right) {
                curNode->size += curNode->right->size;
//                curNode->cell.insert(curNode->cell.end(),
//                                     curNode->right->cell.begin(), curNode->right->cell.end());  // 合并右子树边界
            }

            stack.pop();
        }
    }

    return root;
}
//
//INT KDTreeCounting::rangeSearch(const vector<pair<INT, INT>>& ranges) {
//    INT counter = 0;
//
//    stack<StackSearchNode> searchStack;
//    searchStack.push({root, 0});
//
//    while (!searchStack.empty()) {
//        auto current = searchStack.top();
//        searchStack.pop();
//
//        Node* curNode = current.node;
//
//        // 合并 isContained、isDisjoint 和 inside 的判断
//        bool isContained = true;
//        bool isDisjoint = false;
//        bool inside = true;
//
//        for (INT i = 0; i < dim; ++i) {
//            // 检查完全包含
//            if (ranges[i].first > curNode->cell[i].first || ranges[i].second < curNode->cell[i].second) {
//                isContained = false;
//            }
//
//            // 检查完全无交集
//            if (ranges[i].second < curNode->cell[i].first || ranges[i].first > curNode->cell[i].second) {
//                isDisjoint = true;
//            }
//
//            // 检查当前点是否在范围内
//            if (curNode->point[i] < ranges[i].first || curNode->point[i] > ranges[i].second) {
//                inside = false;
//            }
//
//            // 如果已经判断无交集，直接跳过
//            if (isDisjoint) break;
//        }
//
//        // 如果范围完全包含子树，累加点数并继续
//        if (isContained) {
//            counter += curNode->size;
//            continue;
//        }
//
//        // 如果范围完全无交集，跳过该子树
//        if (isDisjoint) {
//            continue;
//        }
//
//        // 如果当前点在范围内，计数加 1
//        if (inside) {
//            counter++;
//        }
//
//        // 结合 axis 进行左右子树的剪枝
//        INT axis = current.depth % dim;
//        if (ranges[axis].first <= curNode->point[axis] && curNode->left) {
//            searchStack.push({curNode->left, current.depth + 1});
//        }
//        if (ranges[axis].second >= curNode->point[axis] && curNode->right) {
//            searchStack.push({curNode->right, current.depth + 1});
//        }
//    }
//
//    return counter;
//}


INT KDTreeCounting::rangeSearch(const vector<pair<INT, INT>>& ranges) {
    INT counter = 0;

    stack<StackSearchNode> searchStack;
    searchStack.push({root, 0});

    while (!searchStack.empty()) {
        auto current = searchStack.top();
        searchStack.pop();

        Node* curNode = current.node;

        bool isContained = true;
        bool isDisjoint = false;
        bool inside = true;

        for (INT i = 0; i < dim; ++i) {

            const auto& range = ranges[i];
            const auto& cell = curNode->cell[i];

            if (range.second < cell.first || range.first > cell.second) {
                isDisjoint = true;
                break;
            } else if (isContained && (range.first > cell.first || range.second < cell.second)) {
                isContained = false;

            }

            if (inside && (curNode->point[i] < range.first || curNode->point[i] > range.second)) {
                inside = false;
            }


        }

        // 如果完全无交集，跳过子树
        if (isDisjoint) {
            continue;
        }

        // 如果完全包含子树，直接累加点数并跳过子树
        if (isContained) {
            counter += curNode->size;
            continue;
        }

        // 如果当前点在范围内，累加点
        if (inside) {
            counter++;
        }

        // 部分重叠情况，递归左右子树
        INT axis = current.depth % dim;
        if (curNode->left && ranges[axis].first <= curNode->point[axis]) {


            searchStack.push({curNode->left, current.depth + 1});
        }

        if (curNode->right && ranges[axis].second >= curNode->point[axis]) {
            searchStack.push({curNode->right, current.depth + 1});
        }
    }

    return counter;
}



KDTreeCounting::~KDTreeCounting(){
    delete root;
}
