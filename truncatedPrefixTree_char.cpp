#include "truncatedPrefixTree_char.h"
#include <cassert>




truncatedPrefixTree::truncatedPrefixTree( prefixTree &originalTree, INT &B, vector<INT> &hash_positions){
    this->hash_positions = &hash_positions;
    root = copyAndTruncate(originalTree.root, B);

}




pfNode* truncatedPrefixTree::copyAndTruncate(pfNode* originalRoot, INT &B) {


    std::stack<pfNode *> stack;
    stack.push(originalRoot);
    while (!stack.empty()) {
        pfNode *top = stack.top();
        if (!top->visited) {
            top->visited = true;
            for (auto &it: top->child) {
                stack.push(it.second);
            }
        } else {
            stack.pop();
            if (top->child.size()){
                //top is a non-leaf node
                //get the minimum between {B-1 and kept_length of all children}
                INT minKeptLength = B;
                for (auto &it : top->child) {
                    if (it.second->kept_length < minKeptLength) {
                        minKeptLength = it.second->kept_length;
                    }
                }
                top->kept_length = minKeptLength;
            } else{
                //top is a leaf
                INT posNextHash = find_lower_bound_value(top->start);
                if (posNextHash!= -1){

                    top->kept_length =  std::min(posNextHash - top->start, B);
                } else{
                    top->kept_length =  std::min(top->depth, B);
                }

            }
            top->visited = false;
        }
    }



    pfNode* newRoot = new pfNode(originalRoot->start, originalRoot->depth, originalRoot->label);

    std::stack<pair<pfNode*,pfNode*>> stackNodes;
    stackNodes.push({originalRoot,newRoot});

    while (!stackNodes.empty()) {
        auto [oldNode, newNode] = stackNodes.top();
        stackNodes.pop();

        for ( auto& childPair : oldNode->child) {
            unsigned char charLabel = childPair.first;

            pfNode* child = childPair.second;

            if(child->kept_length >= child->depth){

                pfNode* newChild = new pfNode(child->start, child->depth, charLabel);
                // 将新节点添加到新树中
                newChild->phi = child->phi;
                newNode->addChild(newChild, charLabel);


                stackNodes.push({child, newChild});

            } else{

                assert(oldNode->depth <= child->kept_length);
                if(oldNode->depth < child->kept_length){
                    pfNode* newChild = new pfNode(child->start, child->kept_length, charLabel);
                    newChild->phi = child->phi;

                    newNode->addChild(newChild, charLabel);

                } else{
                    // if ==
                    continue;

                }

            }


        }
    }

    return newRoot;
}



INT truncatedPrefixTree::find_lower_bound_value(INT &value) {
    auto it = std::lower_bound(hash_positions->begin(), hash_positions->end(), value);

    // return the value of the value
    if (it != hash_positions->end()) {
        return *it;
    }
    return -1;
}



void truncatedPrefixTree::updatePhi() {

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




void truncatedPrefixTree::addPoints(std::vector<Point> &pointsD1, std::vector<Point> &pointsD2,
                                    std::vector<Point> &pointsDl, stNode *lightNode) {

    std::stack<pfNode *> stack;
    stack.push(root);
    while (!stack.empty()) {
        pfNode *top = stack.top();

        stack.pop();  // Pop before size calculation to avoid double counting

        if (lightNode->parent and top->parent){
//            pointsD1.push_back({{top->parent->depth+1, lightNode->parent->depth +1 , lightNode->preorderId,  top->depth,  lightNode->depth}});
            pointsD1.push_back({{ lightNode->depth,  top->depth,  lightNode->preorderId,  lightNode->parent->depth +1 ,top->parent->depth+1}});

//            pointsD1.push_back({{lightNode->preorderId,  top->parent->depth+1,  top->depth,  lightNode->parent->depth +1 , lightNode->depth}});

        }
        if (top->parent){
//            pointsD2.push_back({{lightNode->preorderId,  top->parent->depth+1,  top->depth, lightNode->depth + 1,  top->phi }});
//            pointsD2.push_back({{ top->parent->depth+1,lightNode->depth + 1,   lightNode->preorderId, top->depth,   top->phi }});
            pointsD2.push_back({{    top->phi, top->depth,  lightNode->preorderId,lightNode->depth + 1, top->parent->depth+1 }});

//                pointsDl.push_back({{ top->parent->depth+1,  top->depth , top->phi}});
            pointsDl.push_back({{top->phi,   top->depth , top->parent->depth+1 }});


        }

        for (auto &it: top->child) {
            stack.push(it.second);
        }


    }




}
void truncatedPrefixTree::deleteTreeIteratively() {
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




truncatedPrefixTree::~truncatedPrefixTree() {

    deleteTreeIteratively();

}