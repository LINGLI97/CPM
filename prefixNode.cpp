#include <iostream>
#include <string>
#include <unordered_map>
#include "prefixNode.h"

using namespace std;


pfNode::pfNode()
{
    this->start = 0;
    this->depth = 0;
    this->label = 0;
    this->parent = NULL;
    this->phi = 0;
    this->visited= false;

#ifdef USE_KEPT_LENGTH
    this->kept_length = -1;
#endif
}


pfNode::pfNode( INT i, INT d, unsigned char l)
{
    this->start = i;
    this->depth = d;
    this->label = l;
    this->parent = NULL;
    this->phi = 0;
    this->visited= false;


#ifdef USE_KEPT_LENGTH
    this->kept_length = -1;
#endif



}


void pfNode::setDepth( INT d){

    this->depth = d;
}



pfNode * pfNode::getChild( unsigned char l )
{

    unordered_map<unsigned char, pfNode*>::iterator it;



    it = this->child.find(l);
    if ( it == this->child.end() )
        return NULL;
    else
        return it->second;
}



void pfNode::addChild( pfNode * childNode, unsigned char l)
{
    auto it = this->child.find( l );
    if ( it == this->child.end() )
    {
        pair <unsigned char, pfNode*> insertChild( l, childNode );
        this->child.insert( insertChild);
    }
    else
    {
        it->second = childNode;
    }
    childNode->label = l;
    childNode->parent = this;
}

void pfNode::setParent(pfNode * parentNode )
{
    this->parent = parentNode;
}


INT pfNode::numChild()
{
    if (this->child.empty()){
        return 0;
    }
    return this->child.size();
}




std::vector<pfNode*> pfNode::allChild() {
    std::vector<pfNode*> allChildren;
    for (auto& pair : this->child) {
        allChildren.push_back(pair.second);
    }
    return allChildren;
}



pfNode::~pfNode() {

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

