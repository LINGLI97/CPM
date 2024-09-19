#include <iostream>
#include <string>
#include <stack>
#include <cstring>
#include <unordered_set>

#include "prefixTree.h"

using namespace std;



prefixTree::prefixTree(std::vector<pair<INT,INT>> prefixesStarting, SA_LCP_LCE & DS)
{


    this->T = DS.T;
    this->prefixesStarting = prefixesStarting;

    this->root = new pfNode();

    pfNode * current = this->root;
    int d = 0;


    int lce_value =0;
    for (int i = 0; i < prefixesStarting.size(); i++) {
        int suffix_start = prefixesStarting[i].first;  // Starting position of current suffix
        if (i != 0){
            lce_value = DS.LCE(prefixesStarting[i-1].first,prefixesStarting[i].first);    // LCP value between current and previous suffix
        }
        pfNode* currentChild;
        // Walk back to the right node based on the LCP value
        while (current->depth > lce_value) {
            currentChild = current;
            current = current->parent;
        }

        if (current->depth == lce_value){
            pfNode* new_node = new pfNode(suffix_start, DS.text_size - suffix_start,T[suffix_start+ current->depth]);
            new_node->parent = current;
            new_node->phi = prefixesStarting[i].second;
            current->addChild(new_node,T[suffix_start+ current->depth]);

            current = new_node;


        } else{


            unsigned char v_label = T[currentChild->start + current->depth];
            unsigned char currentChild_label = T[currentChild->start + lce_value];
            unsigned char x_label = T[suffix_start + lce_value];

            //add the branch node v
            pfNode * v = new pfNode( currentChild->start, lce_value, v_label);
            current->addChild( v, v_label);
            v->addChild( currentChild, currentChild_label);

            current = v;

            // add the other leaf x
            pfNode * x = new pfNode( suffix_start, DS.text_size - suffix_start, x_label);
            x->phi = prefixesStarting[i].second;
            current->addChild(x, x_label);
            current = x;
        }
        // Move current to the new node


    }

}


void prefixTree::updatePhi() {

    std::stack<pfNode *> stack;
    stack.push(root);
    while (!stack.empty()) {
        pfNode *top = stack.top();
        if (!top->visited) {
            // First visit to a node: add all its child nodes to the stack
            top->visited = true;
            for (auto &it: top->child) {
                stack.push(it.second);
            }
        } else{
            // Second visit to the node: The size of all child nodes has been calculated. Now calculate the size of the current node.


            stack.pop();  // Pop before size calculation to avoid double counting

            // top is not leaf
            if (top->numChild()){
                int maxPhi = 0;
                for (auto &it : top->child) {
                    if (it.second->phi> maxPhi) {
                        maxPhi = it.second->phi;
                    }
                }
                top->phi=maxPhi;
            }

            top->visited = false; // After second visiting, reset visited status to default for future use

        }

    }



}

void prefixTree::deleteTreeIteratively() {
    std::stack<pfNode*> toDelete;
    toDelete.push(root);

    while (!toDelete.empty()) {
        pfNode* current = toDelete.top();
        toDelete.pop();

        for (auto it = current->child.begin(); it != current->child.end();) {
            if (it->second != nullptr){
                toDelete.push(it->second);
                it->second = nullptr;

            }
            it = current->child.erase(it);
        }
        delete current;

    }
}
void prefixTree::addPoints(std::vector<Point> &pointsD1,std::vector<Point> &pointsD2,std::vector<Point> &pointsDl,stNode* lightNode, bool flag_Dl) {

    std::stack<pfNode *> stack;
    stack.push(root);
    while (!stack.empty()) {
        pfNode *top = stack.top();

        stack.pop();  // Pop before size calculation to avoid double counting

        if (lightNode->parent and top->parent){
            pointsD1.push_back({{(double)lightNode->preorderId, (double) top->parent->depth+1, (double) top->depth, (double) lightNode->parent->depth +1 ,(double) lightNode->depth}});
        }
        if (top->parent){
            pointsD2.push_back({{(double)lightNode->preorderId, (double) top->parent->depth+1, (double) top->depth,(double) lightNode->depth + 1, (double) top->phi }});
            if (flag_Dl){
                pointsDl.push_back({{(double) top->parent->depth+1, (double) top->depth ,(double) top->phi}});
            }
        }

        for (auto &it: top->child) {
            stack.push(it.second);
        }


    }


}




prefixTree::~prefixTree() {

//    delete this->root;
    deleteTreeIteratively();

}
