#include <iostream>
#include <string>
#include <stack>
#include <cstring>
#include <unordered_set>
#include <fstream>
#include "sparseSuffixTree.h"

using namespace std;



sparseSuffixTree::sparseSuffixTree(vector<uint64_t> * &final_ssa, vector<uint64_t> * &final_lcp,unsigned char* T, INT &text_size)
{
//    size_t memory_start = memory_usage()*0.001;

    this->T = T;
    this->n = text_size;
    this->root = new sstNode();

    sstNode * current = this->root;

    int d = 0;
    int lcp_value =0;
    for (int i = 0; i < final_ssa->size(); i++) {
        uint64_t suffix_start = (*final_ssa)[i];  // Starting position of current suffix
        if (i != 0){
 // calculate LCP
            lcp_value = (*final_lcp)[i];
        }
        sstNode* currentChild;
        // Walk back to the right node based on the LCP value
        while (current->depth > lcp_value) {
            currentChild = current;
            current = current->parent;
        }

        if (current->depth == lcp_value){
            sstNode* new_node = new sstNode(suffix_start, text_size - suffix_start, T[suffix_start+ current->depth]);
            new_node->parent = current;
            current->addChild(new_node,T[suffix_start+ current->depth]);

            current = new_node;


        } else{


            unsigned char v_label = T[currentChild->start + current->depth];
            unsigned char currentChild_label = T[currentChild->start + lcp_value];
            unsigned char x_label = T[suffix_start + lcp_value];

            //add the branch node v
            sstNode * v = new sstNode( currentChild->start, lcp_value, v_label);
            current->addChild( v, v_label);
            v->addChild( currentChild, currentChild_label);

            current = v;

            // add the other leaf x
            sstNode * x = new sstNode( suffix_start, text_size - suffix_start, x_label);
            current->addChild(x, x_label);
            current = x;
        }
        // Move current to the new node


    }



//    memory = memory_usage()*0.001 - memory_start;

}


void sparseSuffixTree::clearLeaves() {
    if (root == nullptr) {
        return;
    }

    std::stack<sstNode*> nodeStack;
    nodeStack.push(root);

    while (!nodeStack.empty()) {
        sstNode* current = nodeStack.top();
        nodeStack.pop();

        std::unordered_set<sstNode*> empty;
        current->leaves.swap(empty);
        current->heavyLeaf = nullptr;
        for (auto child : current->child) {
            nodeStack.push(child.second);
        }
    }
}






void sparseSuffixTree::initHLD() {
    std::stack<sstNode *> stack;
    stack.push(root);
    INT preorderCounter = 0;
    while (!stack.empty()) {
        sstNode *top = stack.top();
        if (!top->visited) {
            // First visit to a node: add all its child nodes to the stack
            top->visited = true;
            for (auto it = top->child.rbegin(); it!= top->child.rend(); it ++) {
                stack.push(it->second);
            }
            top->preorderId = preorderCounter;
            preorderCounter++;
        } else{
            // Second visit to the node: The size of all child nodes has been calculated. Now calculate the size of the current node.
            if (top->child.empty()){
                top->leaves.insert(top);  // top is leaf
            }


            stack.pop();  // Pop before size calculation to avoid double counting

            INT totalSize = 1;  // Start with 1 to count the current node itself
            sstNode* maxChild = nullptr;
            INT maxSize = 0;

            for (auto &it : top->child) {
                top->leaves.insert(it.second->leaves.begin(),it.second->leaves.end()); //merge leaf nodes for all nodes

                totalSize += it.second->sizeSubtree;  // Sum up the sizes of the subtree from each child
                if (it.second->sizeSubtree > maxSize) {
                    maxSize = it.second->sizeSubtree;
                    maxChild = it.second;  // Track the child with the maximum subtree size
                }
            }
            top->sizeSubtree = totalSize;  // Assign computed size to current node

            if (maxChild) {
                maxChild->heavy = true;  // Mark the child with the maximum subtree as heavy
                if (maxChild->child.empty()){
                    maxChild->heavyLeaf = maxChild;
                }
                top->heavyLeaf=maxChild->heavyLeaf; //pass the heavyleaf upward
            }


            for (auto &it : top->child) {


                if (it.second->heavyLeaf == nullptr){
                    it.second->heavyLeaf = it.second;
                }
                if (it.second!= maxChild){
                    this->lightNodes.insert(it.second);
                }
            }


            top->visited = false; // After second visiting, reset visited status to default for future use


        }

    }

    this->lightNodes.insert(root); // add root at last
}


sstNode *sparseSuffixTree::forward_search(unsigned char *P, INT& pattern_size){

    INT d = 0;
    sstNode * u = this->root;
    sstNode * v = NULL;
    bool match = true;
    while ( match )
    {
        v = u->getChild( P[d] );
        if ( v == NULL )
        {
            match = false;
            break;
        }
        INT i, j;
        for ( i = d, j = d; i < pattern_size && j < v->depth ; i++, j++ )
        {
//            cout<<"P["<<i<<"]"<<P[i]<<endl;
//            cout<<"T["<<v->start+j<<"]"<<this->T[v->start+j]<<endl;

            if ( P[i] != this->T[v->start+j] )
            {
                match = false;
                break;
            }
        }
        d = v->depth;
        u = v;
        if ( d >= pattern_size )
        {
//            INT tmp = u->start+ pattern_size-1;
            return u;

        }
    }
    return nullptr;

}




void sparseSuffixTree::deleteTreeIteratively() {
    std::stack<sstNode*> toDelete;
    toDelete.push(root);

    while (!toDelete.empty()) {
        sstNode* current = toDelete.top();
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




sparseSuffixTree::~sparseSuffixTree() {

//    delete this->root;
    deleteTreeIteratively();


}



