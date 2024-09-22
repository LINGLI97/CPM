#include <iostream>
#include <string>
#include <stack>
#include <cstring>
#include <unordered_set>
#include <fstream>
#include "suffixTree.h"

using namespace std;



suffixTree::suffixTree(unsigned char* T, INT &text_size)
{
//    size_t memory_start = memory_usage()*0.001;

    this->T = T;
    this->n = text_size;


    //set the value of terminate_label

    this->root = new stNode();
    this->root->setSLink( this->root);



    stNode * u = this->root;
    int d = 0;


    for ( int i = 0; i < this->n; i++ )
    {
        while (((i+d) < this -> n) && (d == u->depth) && (u->getChild(T[i + d]) != NULL)) {


            u = u->getChild(T[i + d]);
            d = d + 1;
            while ((i + d < this->n) && (u->start +d < this->n) && (d < u->depth) && (T[u->start + d] == T[i + d])) {
                d = d + 1;
            }
        }

        if ( d < u->depth)
        {
            u= createNode (u, d );
        }
        createLeaf( i, u, d);

        if ( u->slink == NULL )
        {
            ComputeSuffixLink( u );
        }
        u = u->slink;

//        d = u->getDepth(); // same as d = max( d-1, 0 );
        d = max( d-1, 0 );


    }
    //the only child of root



//    memory = memory_usage()*0.001 - memory_start;

}

//void suffixTree::getAll_ul(unordered_map<stNode *, stNode *> &result) {
//    std::stack<stNode*> stack;
//    stack.push(root);
//
//    while (!stack.empty()) {
//        stNode* node = stack.top();
//        stack.pop();
//
//        // If the current node is not heavy, it's a light node, start a new heavy path
//        if (!node->heavy) {
//            stNode* current = node;
//            stNode* heavyLeaf = nullptr;
//
//            // Traverse along the heavy path to find the corresponding heavy leaf
//            while (current && current->heavy) {
//                bool hasHeavyChild = false;
//                for (auto &it : current->child) {
//                    if (it.second->heavy) {
//                        current = it.second;
//                        hasHeavyChild = true;
//                        break;
//                    }
//                }
//
//                // If no heavy child, we have found the heavy leaf
//                if (!hasHeavyChild) {
//                    heavyLeaf = current;
//                    break;
//                }
//            }
//
//            // If the light node is also a leaf, it is its own heavy leaf
//            if (node->child.empty()) {
//                heavyLeaf = node;
//            }
//
//            // If a heavy leaf was found, map the light node to this heavy leaf
//            if (heavyLeaf) {
//                result[node] = heavyLeaf;
//            }
//        }
//
//        // Push all children to the stack for further processing
//        for (auto &it : node->child) {
//            stack.push(it.second);
//        }
//    }
//
//}



void suffixTree::clearLeaves() {
    if (root == nullptr) {
        return;
    }

    std::stack<stNode*> nodeStack;
    nodeStack.push(root);

    while (!nodeStack.empty()) {
        stNode* current = nodeStack.top();
        nodeStack.pop();

        std::unordered_set<stNode*> empty;
        current->leaves.swap(empty);
        current->heavyLeaf = nullptr;
        for (auto child : current->child) {
            nodeStack.push(child.second);
        }
    }
}

void suffixTree::ComputeSuffixLink( stNode * u )
{
    int d = u->depth;
    stNode * v = u->parent->slink;

    while ( v->depth < d-1 )
    {
        // go down
        v = v->getChild( this->T[u->start + v->depth + 1] );
    }
    if ( v->depth > d-1 )
    {
        //create a new node as a suffix link node
        v = createNode( v, d-1);
    }
    u->setSLink (v);
//    u-> slink->setDepth(d -1);
}

stNode * suffixTree::createNode(stNode * u, int d )
{

    // add a new node v between p and u
    int i = u->start;
    stNode * p = u->parent;


    stNode * v = new stNode( i, d,  this->T[i+p->depth]);
    v->addChild( u, this->T[i+d]);
    p->addChild( v,  this->T[i+p->depth]);

    return v;
}


void suffixTree::createLeaf( int i, stNode * u, int d )
{
    // create a leaf node connected to u
    // (n+2)(n+1)+1 represents $

    int depth = this->n-i ;
    stNode *leaf  = new stNode(i, depth, this->T[i+d]) ;
    u->addChild( leaf, this->T[i+d]);
//    pos2leaf.insert({i, leaf});


}



void suffixTree::deleteTreeIteratively() {
    std::stack<stNode*> toDelete;
    toDelete.push(root);

    while (!toDelete.empty()) {
        stNode* current = toDelete.top();
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


void suffixTree::initHLD() {
    std::stack<stNode *> stack;
    stack.push(root);
    int preorderCounter = 0;
    while (!stack.empty()) {
        stNode *top = stack.top();
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

            int totalSize = 1;  // Start with 1 to count the current node itself
            stNode* maxChild = nullptr;
            int maxSize = 0;

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


stNode *suffixTree::forward_search(unsigned char *P, INT& pattern_size){

    int d = 0;
    stNode * u = this->root;
    stNode * v = NULL;
    bool match = true;
    while ( match )
    {
        v = u->getChild( P[d] );
        if ( v == NULL )
        {
            match = false;
            break;
        }
        int i, j;
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



void suffixTree::generateDot(stNode* node, std::ofstream& dotFile, bool suf) {
    if (!node) return;

    std::vector<stNode*> children = node->allChild();
    if (children.empty()) {

//            dotFile << "\"" << node << "\" [shape=ellipse, style=filled, fillcolor=green, label=\" Start: " << node->getStart() << "\"];\n";

    }

    if (!children.empty()){

        int numChildren = children.size();

        for (int i = 0; i < numChildren; ++i) {

            dotFile << "\"" << node << "\" -> \"" << children[i] << "\" [label=\""
                    << " label: " << children[i]->label
                    << "\"];\n";

            // 输出节点的颜色（如果是 heavy node，填充红色背景）
            dotFile << "\"" << children[i] << "\" [label=\""
                    << " Heavy: " << (children[i]->heavy ? "Yes" : "No")
                    << " , start: " << children[i]->start
                    << " , string depth: " << children[i]->depth << " \"";

            // 如果是 heavy 节点，将节点标记为红色
            if (!children[i]->heavy) {
                dotFile << ", style=filled, fillcolor=green";
            }

            if (children[i]->heavy) {
                dotFile << ", style=filled, fillcolor=red";
            }
            dotFile << "];\n";

            generateDot(children[i], dotFile, suf);


        }
    }
    if (suf){
        if (node->slink!=NULL){
            dotFile << "\"" << node << "\" -> \"" << node->slink << "\" [label=\"" << "" << "\",color=\"red\"];\n";
        }

    }



}

void suffixTree::exportSuffixTreeToDot(const std::string& filename,bool suf) {
    std::ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        std::cerr << "Unable to open file for writing: " << filename << std::endl;
        return;
    }

    //DOT setting
    dotFile << "digraph SuffixTree {\n";
    dotFile << "node [shape=ellipse, style=filled, fillcolor=lightgrey];\n";
    dotFile << "edge [color=black];\n";
    dotFile << "graph [nodesep=0.5, ranksep=1, splines=polyline];\n";




    generateDot(root, dotFile,suf);  //without leafCount and flag info
//    generateCount(root, dotFile,suf);  //with leafCount and flag info


    dotFile << "}\n";
    dotFile.close();
}


suffixTree::~suffixTree() {

//    delete this->root;
    deleteTreeIteratively();


}
