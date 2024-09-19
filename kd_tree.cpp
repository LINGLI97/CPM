#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "kd_tree.h"
#include <random>

#include <chrono>



void KDTree::rangeSearchIterative(Node* node, const vector<pair<double, double>>& ranges, vector<Point>& result) {
    if (!node) return;

    struct StackSearchNode {
        Node* node;
        int depth;
    };

    stack<StackSearchNode> searchStack;
    searchStack.push({node, 0});

    while (!searchStack.empty()) {
        auto current = searchStack.top();
        searchStack.pop();

        Node* curNode = current.node;
        int axis = current.depth % dim;

        bool inside = true;
        for (int i = 0; i < dim; ++i) {
            if (curNode->point[i] < ranges[i].first || curNode->point[i] > ranges[i].second) {
                inside = false;
                break;
            }
        }

        if (inside) {
            result.push_back(curNode->point);
        }

        if (curNode->left && curNode->point[axis] >= ranges[axis].first) {
            searchStack.push({curNode->left, current.depth + 1});
        }
        if (curNode->right && curNode->point[axis] <= ranges[axis].second) {
            searchStack.push({curNode->right, current.depth + 1});
        }
    }
}


KDTree::KDTree(vector<Point> &points) {
    if (!points.empty()) {
        dim = points[0].dimension();
        root = buildIterative(points);
    } else {
        root = nullptr;
        dim = 0;
    }
}



Node* KDTree::buildIterative(vector<Point> &points) {
    if (points.empty()) return nullptr;

    struct StackNode {
        Node** nodeRef;
        vector<Point>::iterator start;
        vector<Point>::iterator end;
        int depth;
    };

    stack<StackNode> s;
    Node* tmp = nullptr;
    s.push({&tmp, points.begin(), points.end(), 0});

    while (!s.empty()) {
        StackNode current = s.top();
        s.pop();

        if (current.start >= current.end) continue;

        int axis = current.depth % dim;
        auto mid = current.start + (current.end - current.start) / 2;

        nth_element(current.start, mid, current.end, [axis](const Point& a, const Point& b) {
            return a[axis] < b[axis];
        });

        *current.nodeRef = new Node(*mid);

        s.push({&(*current.nodeRef)->left, current.start, mid, current.depth + 1});
        s.push({&(*current.nodeRef)->right, mid + 1, current.end, current.depth + 1});
    }

    return tmp;
}

vector<Point> KDTree::rangeSearch(const vector<pair<double, double>>& ranges) {
    vector<Point> result;
    if (dim != ranges.size()) {
        cout << "The dimension of range search is not matched with the data points" << endl;
        return result;
    }

    rangeSearchIterative(root, ranges, result);
    return result;
}




KDTree::~KDTree(){
    delete root;
}
//int test() {
//
//    std::vector<Point> points = {
//            {{2.0, 3.0, 3}},
//            {{5.0, 4.0, 4}},
//            {{9.0, 6.0,5}},
//            {{4.0, 7.0, 10}},
//            {{8.0, 1.0, 30}},
//            {{7.0, 2.0, 50}}
//    };
//
//
//
//    KDTree tree(points);
//
//
//    vector<pair<double, double>> ranges = {{-2, 18}, {-1, 15}, {0, 50}};
//
//    vector<Point> result = tree.rangeSearch(ranges);
//
//
//    return 0;
//}
//



vector<Point> generateRandomPoints(int numPoints, int dim, double lowerBound, double upperBound) {
    vector<Point> points;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(lowerBound, upperBound);

    for (int i = 0; i < numPoints; ++i) {
        vector<double> coords;
        for (int j = 0; j < dim; ++j) {
            coords.push_back(dis(gen));
        }
        points.emplace_back(coords);
    }

    return points;
}


vector<pair<double, double>> generateRandomRanges(int dim, double lowerBound, double upperBound) {
    vector<pair<double, double>> ranges;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(lowerBound, upperBound);

    for (int i = 0; i < dim; ++i) {
        double min = dis(gen);
        double max = dis(gen);
        if (min > max) swap(min, max);
        ranges.push_back({min, max});
    }

    return ranges;
}

int test() {
    int numPoints = 10000000;
    int dim = 5;
    double lowerBound = -10.0;
    double upperBound = 10.0;


    vector<Point> points = generateRandomPoints(numPoints, dim, lowerBound, upperBound);

    auto Construction_start = std::chrono::high_resolution_clock::now();

    KDTree tree(points);
    auto Construction_end = std::chrono::high_resolution_clock::now();
    double time_ST = std::chrono::duration_cast < std::chrono::microseconds > (Construction_end - Construction_start).count()*0.000001;


    vector<pair<double, double>> ranges ={{-10,10},{-10,10}, {-10,10},{-10,10},{-10,10} };


    Construction_start = std::chrono::high_resolution_clock::now();
    vector<Point> result = tree.rangeSearch(ranges);
    Construction_end = std::chrono::high_resolution_clock::now();
    double time_ST2 = std::chrono::duration_cast < std::chrono::microseconds > (Construction_end - Construction_start).count()*0.000001;

    std::cout << "Construction time: " << time_ST << "\n";
//
    std::cout << "query time: " << time_ST2 << "\n";

    double ratio = static_cast<double>(result.size()) / points.size();
    cout << "The number of points found is: " << result.size() << endl;
    cout << "The number of total points is: " << points.size() << endl;
    cout << "Ratio is " << ratio << endl;

    return 0;
}