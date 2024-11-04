#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "kd_tree.h"
#include <random>

#include <chrono>

struct StackSearchNode {
    Node* node;
    INT depth;
};

void KDTree::rangeSearchIterative(Node* node, const vector<pair<INT, INT>>& ranges, INT &counter) {
//    if (!node) return;



    stack<StackSearchNode> searchStack;
    searchStack.push({node, 0});

    while (!searchStack.empty()) {
        auto current = searchStack.top();
        searchStack.pop();

        Node* curNode = current.node;
        INT axis = current.depth % dim;

        bool inside = true;
        for (INT i = 0; i < dim; ++i) {
            if (curNode->point[i] < ranges[i].first || curNode->point[i] > ranges[i].second) {
                inside = false;
                break;
            }
        }

        if (inside) {
           counter++;
        }

        if (curNode->point[axis] >= ranges[axis].first  && curNode->left) {
            searchStack.push({curNode->left, current.depth + 1});
        }
        if (curNode->point[axis] <= ranges[axis].second && curNode->right) {
            searchStack.push({curNode->right, current.depth + 1});
        }
    }
}


KDTree::KDTree(vector<Point> &points) {
    dim = points[0].dimension();
    root = buildIterative(points);

}

struct StackNode {
    Node** nodeRef;
    vector<Point>::iterator start;
    vector<Point>::iterator end;
    INT depth;
};

Node* KDTree::buildIterative(vector<Point> &points) {
//    if (points.empty()) return nullptr;



    stack<StackNode> s;
    Node* tmp = nullptr;
    s.push({&tmp, points.begin(), points.end(), 0});

    while (!s.empty()) {
        StackNode current = s.top();
        s.pop();

        if (current.start >= current.end) continue;

        INT axis = current.depth % dim;
        auto mid = current.start + (current.end - current.start) / 2;

        nth_element(current.start, mid, current.end, [axis](const Point& a, const Point& b){return a[axis] < b[axis];});

        *current.nodeRef = new Node(*mid);

        s.push({&(*current.nodeRef)->left, current.start, mid, current.depth + 1});
        s.push({&(*current.nodeRef)->right, mid + 1, current.end, current.depth + 1});
    }

    return tmp;
}

INT KDTree::rangeSearch(const vector<pair<INT, INT>>& ranges) {
    INT counter =0;
//    if (dim != ranges.size()) {
//        cout << "The dimension of range search is not matched with the data points" << endl;
//        return counter;
//    }

    rangeSearchIterative(root, ranges, counter);
    return counter;
}




KDTree::~KDTree(){
    delete root;
}
//int main() {
//
//    std::vector<Point> points = {
//            {{2, 3, 3}},
//            {{5, 4, 4}},
//            {{9, 6,5}},
//            {{4, 7, 10}},
//            {{8, 1, 30}},
//            {{7, 2, 50}}
//    };
//
//
//
//    KDTree tree(points);
//
//
//    vector<pair<INT, INT>> ranges = {{3, 8}, {-1, 6}, {0, 10}};
//
//    INT result = tree.rangeSearch(ranges);
//    cout<<"# of the counting = "<<result<<endl;
//
//    return 0;
//}


//
//
//vector<Point> generateRandomPoints(INT numPoints, INT dim, INT lowerBound, INT upperBound) {
//    vector<Point> points;
//    random_device rd;
//    mt19937 gen(rd());
//    uniform_real_distribution<> dis(lowerBound, upperBound);
//
//    for (INT i = 0; i < numPoints; ++i) {
//        vector<INT> coords;
//        for (INT j = 0; j < dim; ++j) {
//            coords.push_back(dis(gen));
//        }
//        points.emplace_back(coords);
//    }
//
//    return points;
//}
//



//
//
//int main() {
//    int numPoints = 1000000000;
//    int dim = 5;
//    INT lowerBound = -10;
//    INT upperBound = 10;
//
//    vector<Point> points = generateRandomPoints(numPoints, dim, lowerBound, upperBound);
//
//    auto Construction_start = std::chrono::high_resolution_clock::now();
//    KDTree tree(points);
//    auto Construction_end = std::chrono::high_resolution_clock::now();
//    double time_ST = std::chrono::duration_cast<std::chrono::microseconds>(Construction_end - Construction_start).count() * 0.000001;
//    std::cout << "Construction time: " << time_ST << " seconds\n";
//
//    // 生成2000个随机查询范围
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<> range_dis(lowerBound, upperBound);
//
//    int totalResults = 0;
//    int cnt = 200000;
//    auto Query_start = std::chrono::high_resolution_clock::now();
//
//    for (int i = 0; i < cnt; ++i) {
//        vector<pair<INT, INT>> ranges(dim);
//        for (int j = 0; j < dim; ++j) {
//            INT start = range_dis(gen);
//            INT end = range_dis(gen);
//            ranges[j] = { min(start, end), max(start, end) }; // 确保范围的顺序
//        }
//
//        // 执行范围查询
//        int result = tree.rangeSearch(ranges);
//        totalResults += result;
//    }
//
//    auto Query_end = std::chrono::high_resolution_clock::now();
//    double queryTime = std::chrono::duration_cast<std::chrono::microseconds>(Query_end - Query_start).count() * 0.000001;
//    std::cout << "Total query time for " << cnt<<" queries: " << queryTime << " seconds\n";
//
//    // 计算平均结果和比例
//    double averageResults = static_cast<double>(totalResults) / cnt;
//    double ratio = averageResults / points.size();
//
//    std::cout << "Average number of points found per query: " << averageResults << std::endl;
//    std::cout << "Total points: " << points.size() << std::endl;
//    std::cout << "Average ratio per query: " << ratio << std::endl;
//
//    return 0;
//}