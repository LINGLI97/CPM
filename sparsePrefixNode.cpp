#include <iostream>
#include <string>
#include <unordered_map>
#include "sparsePrefixNode.h"

using namespace std;

spfNode::spfNode() {
    this->start = 0;
    this->depth = 0;
    this->label = 0;
    this->parent = NULL;
    this->phi = 0;
    this->visited = false;
}

spfNode::spfNode(INT i, INT d, unsigned char l) {
    this->start = i;
    this->depth = d;
    this->label = l;
    this->parent = NULL;
    this->phi = 0;
    this->visited = false;
}

void spfNode::setDepth(INT d) {
    this->depth = d;
}

spfNode *spfNode::getChild(unsigned char l) {
    unordered_map<unsigned char, spfNode*>::iterator it;
    it = this->child.find(l);
    if (it == this->child.end()) {
        return NULL;
    } else {
        return it->second;
    }
}

void spfNode::addChild(spfNode *childNode, unsigned char l) {
    auto it = this->child.find(l);
    if (it == this->child.end()) {
        pair<unsigned char, spfNode*> insertChild(l, childNode);
        this->child.insert(insertChild);
    } else {
        it->second = childNode;
    }
    childNode->label = l;
    childNode->parent = this;
}

void spfNode::setParent(spfNode *parentNode) {
    this->parent = parentNode;
}

INT spfNode::numChild() {
    if (this->child.empty()) {
        return 0;
    }
    return this->child.size();
}

std::vector<spfNode*> spfNode::allChild() {
    std::vector<spfNode*> allChildren;
    for (auto &pair : this->child) {
        allChildren.push_back(pair.second);
    }
    return allChildren;
}

spfNode::~spfNode() {
    // Destructor code if needed, left empty as in the original
}
