#include <iostream>
#include <string>
#include <stack>
#include <cstring>
#include <unordered_set>

#include "sparsePrefixTree.h"

using namespace std;


sparsePrefixTree::sparsePrefixTree(std::vector<pair<INT, INT>> &prefixesStarting,unsigned char * T, INT &text_size, std::unordered_map <INT, INT> &inv_final_ssa_rev, vector<uint64_t> * &final_lcp_rev,sdsl::rmq_succinct_sct<> &rmq ) {
    this->T = T;
    this->root = new spfNode();

    spfNode *current = this->root;
    int d = 0;
    int lce_value = 0;

    for (int i = 0; i < prefixesStarting.size(); i++) {
        int suffix_start = prefixesStarting[i].first;
        if (i != 0) {

            INT L = prefixesStarting[i - 1].first;
            INT R = prefixesStarting[i].first;


            if (L != R){
                INT low = min(inv_final_ssa_rev[L], inv_final_ssa_rev[R]);
                INT high = max(inv_final_ssa_rev[L], inv_final_ssa_rev[R]);

                lce_value = (*final_lcp_rev)[rmq(low + 1, high)];
            } else{

                lce_value =text_size - L;
            }

        }
        spfNode *currentChild;
        while (current->depth > lce_value) {
            currentChild = current;
            current = current->parent;
        }

        if (current->depth == lce_value) {
            spfNode *new_node = new spfNode(suffix_start, text_size - suffix_start, T[suffix_start + current->depth]);
            new_node->parent = current;
            new_node->phi = prefixesStarting[i].second;
            current->addChild(new_node, T[suffix_start + current->depth]);

            current = new_node;
        } else {
            unsigned char v_label = T[currentChild->start + current->depth];
            unsigned char currentChild_label = T[currentChild->start + lce_value];
            unsigned char x_label = T[suffix_start + lce_value];

            spfNode *v = new spfNode(currentChild->start, lce_value, v_label);
            current->addChild(v, v_label);
            v->addChild(currentChild, currentChild_label);

            current = v;

            spfNode *x = new spfNode(suffix_start, text_size - suffix_start, x_label);
            x->phi = prefixesStarting[i].second;
            current->addChild(x, x_label);
            current = x;
        }
    }
}

void sparsePrefixTree::updatePhi() {
    std::stack<spfNode *> stack;
    stack.push(root);
    while (!stack.empty()) {
        spfNode *top = stack.top();
        if (!top->visited) {
            top->visited = true;
            for (auto &it: top->child) {
                stack.push(it.second);
            }
        } else {
            stack.pop();
            if (top->numChild()) {
                int maxPhi = 0;
                for (auto &it : top->child) {
                    if (it.second->phi > maxPhi) {
                        maxPhi = it.second->phi;
                    }
                }
                top->phi = maxPhi;
            }
            top->visited = false;
        }
    }
}

void sparsePrefixTree::deleteTreeIteratively() {
    std::stack<spfNode *> toDelete;
    toDelete.push(root);

    while (!toDelete.empty()) {
        spfNode *current = toDelete.top();
        toDelete.pop();

        for (auto it = current->child.begin(); it != current->child.end();) {
            if (it->second != nullptr) {
                toDelete.push(it->second);
                it->second = nullptr;
            }
            it = current->child.erase(it);
        }
        delete current;
    }
}

void sparsePrefixTree::addPoints(std::vector<Point> &pointsD1, std::vector<Point> &pointsD2, std::vector<Point> &pointsDl, sstNode *lightNode) {
    std::stack<spfNode *> stack;
    stack.push(root);
    while (!stack.empty()) {
        spfNode *top = stack.top();
        stack.pop();

        if (lightNode->parent && top->parent) {
            pointsD1.push_back({{(double)lightNode->preorderId, (double)top->parent->depth + 1, (double)top->depth, (double)lightNode->parent->depth + 1, (double)lightNode->depth}});
        }
        if (top->parent) {
            pointsD2.push_back({{(double)lightNode->preorderId, (double)top->parent->depth + 1, (double)top->depth, (double)lightNode->depth + 1, (double)top->phi}});
            pointsDl.push_back({{(double)top->parent->depth + 1, (double)top->depth, (double)top->phi}});
        }

        for (auto &it: top->child) {
            stack.push(it.second);
        }
    }
}

sparsePrefixTree::~sparsePrefixTree() {
    deleteTreeIteratively();
}
