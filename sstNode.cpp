#include <iostream>
#include <string>
#include <unordered_map>
#include "sstNode.h"

using namespace std;


sstNode::sstNode()
{
    this->start = 0;
    this->depth = 0;
    this->label = 0;
    this->parent = NULL;
    this->heavyLeaf =NULL;
    this->visited = false;
    this->heavy = false;
    this->sizeSubtree = 0;

}



sstNode::sstNode(unsigned char terminate_label)
{
    this->start = 0;
    this->depth = 0;
    this->label = terminate_label;
    this->parent = NULL;
    this->heavyLeaf =NULL;

    this->visited = false;
    this->heavy = false;
    this-> preorderId = -1;
    this->sizeSubtree = 0;



}

sstNode::sstNode( INT i, INT d, unsigned char l)
{
    this->start = i;
    this->depth = d;
    this->label = l;
    this->parent = NULL;
    this->heavyLeaf =NULL;
    this-> preorderId = -1;
    this->visited = false;
    this->heavy = false;

    this->sizeSubtree = 0;



}


void sstNode::setDepth( INT d){

    this->depth = d;
}



sstNode * sstNode::getChild( unsigned char l )
{

    map<unsigned char, sstNode*>::iterator it;


    it = this->child.find(l);
    if ( it == this->child.end() )
        return NULL;
    else
        return it->second;
}

//
//void sstNode::setSLink( sstNode * slinkNode )
//{
//    this->slink = slinkNode;
//}

void sstNode::addChild( sstNode * childNode, unsigned char l)
{
    auto it = this->child.find( l );
    if ( it == this->child.end() )
    {
        pair <unsigned char, sstNode*> insertChild( l, childNode );
        this->child.insert( insertChild);
    }
    else
    {
        it->second = childNode;
    }
    childNode->label = l;
    childNode->parent = this;
}

void sstNode::setParent( sstNode * parentNode )
{
    this->parent = parentNode;
}


INT sstNode::numChild()
{
    if (this->child.empty()){
        return 0;
    }
    return this->child.size();
}




std::vector<sstNode*> sstNode::allChild() {
    std::vector<sstNode*> allChildren;
    for (auto& pair : this->child) {
        allChildren.push_back(pair.second);
    }
    return allChildren;
}



sstNode::~sstNode()
{

/*
// recursion
    // Delete all child nodes
    for (auto it = this->child.begin(); it != this->child.end(); )
    {

        delete it->second; // Delete child node
        it = this->child.erase(it); // Erase from map and move to next

    }

    // Check if this node is not the root (has a parent)
//    cout<<"Delete the node"<<endl;
    if (this->parent != nullptr)
    {
        // If parent's child map is empty after erasing, delete the parent; otherwise deleting this child from its parent's children map
        if (this->parent->child.empty())
        {
            delete this->parent;
        }
    }

*/

}

