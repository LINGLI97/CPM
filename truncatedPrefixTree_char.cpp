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
                INT maxPhi = 0;
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


//R tree
void truncatedPrefixTree::addPoints(vector<point5> &pointsD1,vector<point5> &pointsD2,vector<point3> &pointsDl,stNode* lightNode){
    std::stack<pfNode *> stack;
    stack.push(root);
    while (!stack.empty()) {
        pfNode *top = stack.top();

        stack.pop();  // Pop before size calculation to avoid double counting

        if (lightNode->parent and top->parent){


            point5 pD1;
            bg::set<0>(pD1, lightNode->preorderId);
            bg::set<1>(pD1, top->parent->depth + 1);
            bg::set<2>(pD1, top->depth);
            bg::set<3>(pD1, lightNode->parent->depth + 1);
            bg::set<4>(pD1, lightNode->depth);
            pointsD1.emplace_back(pD1);

        }
        if (top->parent){
            point5 pD2;
            bg::set<0>(pD2, lightNode->preorderId);
            bg::set<1>(pD2, top->parent->depth + 1);
            bg::set<2>(pD2, top->depth);
            bg::set<3>(pD2, lightNode->depth + 1);
            bg::set<4>(pD2, top->phi);
            pointsD2.emplace_back(pD2);



            point3 pDl;
            bg::set<0>(pDl, top->parent->depth + 1);
            bg::set<1>(pDl, top->depth);
            bg::set<2>(pDl, top->phi);
            pointsDl.emplace_back(pDl);


        }

        for (auto &it: top->child) {
            stack.push(it.second);
        }


    }



}










//range tree
//
//void truncatedPrefixTree::addPoints(std::vector<RangeTree::Point<INT, INT>> &pointsD1, std::vector<RangeTree::Point<INT, INT>> &pointsD2,
//        std::vector<RangeTree::Point<INT, INT>> &pointsDl, stNode *lightNode) {
//
//    std::stack<pfNode *> stack;
//    stack.push(root);
//    while (!stack.empty()) {
//        pfNode *top = stack.top();
//
//        stack.pop();  // Pop before size calculation to avoid double counting
//
//        if (lightNode->parent and top->parent){
//
//            pointsD1.emplace_back(std::vector<INT>{lightNode->preorderId,  top->parent->depth+1,  top->depth,  lightNode->parent->depth +1 , lightNode->depth},0);
//
//        }
//        if (top->parent){
//            pointsD2.emplace_back(std::vector<INT>{lightNode->preorderId,  top->parent->depth+1,  top->depth, lightNode->depth + 1,  top->phi },0);
//
//            pointsDl.emplace_back(std::vector<INT>{ top->parent->depth+1,  top->depth , top->phi},0);
//
//
//        }
//
//        for (auto &it: top->child) {
//            stack.push(it.second);
//        }
//
//
//    }
//
//
//
//
//}



//KD tree


//void truncatedPrefixTree::addPoints(std::vector<Point> &pointsD1, std::vector<Point> &pointsD2,
//                                    std::vector<Point> &pointsDl, stNode *lightNode) {
//
//    std::stack<pfNode *> stack;
//    stack.push(root);
//    while (!stack.empty()) {
//        pfNode *top = stack.top();
//
//        stack.pop();  // Pop before size calculation to avoid double counting
//
//        if (lightNode->parent and top->parent){
//
//            pointsD1.push_back({{lightNode->preorderId,  top->parent->depth+1,  top->depth,  lightNode->parent->depth +1 , lightNode->depth}});
//
//        }
//        if (top->parent){
//            pointsD2.push_back({{lightNode->preorderId,  top->parent->depth+1,  top->depth, lightNode->depth + 1,  top->phi }});
//
//            pointsDl.push_back({{ top->parent->depth+1,  top->depth , top->phi}});
//
//
//        }
//
//        for (auto &it: top->child) {
//            stack.push(it.second);
//        }
//
//
//    }
//
//
//
//
//}

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