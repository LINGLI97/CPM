#include "truncatedSuffixTree_char.h"
#include <cassert>
#include <fstream>




truncatedSuffixTree::truncatedSuffixTree(suffixTree *&originalTree, INT &B, vector<INT> &hash_positions){

    this->T = originalTree->T;
    this->hash_positions = &hash_positions;
    root = copyAndTruncate(originalTree->root, B);

}





void truncatedSuffixTree::generateDot(stNode* node, std::ofstream& dotFile, bool suf) {
    if (!node) return;

    std::vector<stNode*> children = node->allChild();
    if (children.empty()) {

//            dotFile << "\"" << node << "\" [shape=ellipse, style=filled, fillcolor=green, label=\" Start: " << node->getStart() << "\"];\n";

    }

    if (!children.empty()){

        INT numChildren = children.size();

        for (INT i = 0; i < numChildren; ++i) {

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

void truncatedSuffixTree::exportSuffixTreeToDot(const std::string& filename,bool suf) {
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

void truncatedSuffixTree::clearLeaves() {


    if (root == nullptr) {
        return;
    }

    std::stack<stNode*> nodeStack;
    nodeStack.push(root);

    while (!nodeStack.empty()) {
        stNode* current = nodeStack.top();
        nodeStack.pop();


        current->leaves_start_depth.clear();      // Remove all elements
        current->leaves_start_depth.shrink_to_fit(); // Attempt to free excess memory

        current->heavyLeaf = nullptr;
//        current->kept_length =
        for (auto child : current->child) {
            nodeStack.push(child.second);
        }
    }

}

stNode *truncatedSuffixTree::forward_search(unsigned char *P, INT &pattern_size) {

    INT d = 0;
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

void truncatedSuffixTree::initHLD() {

    std::stack<stNode *> stack;
    stack.push(root);
    INT preorderCounter = 0;
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
//            if (top->child.empty()){
//                top->leaves.insert(top);  // top is leaf
//            }


            stack.pop();  // Pop before size calculation to avoid double counting

            INT totalSize = 1;  // Start with 1 to count the current node itself
            stNode* maxChild = nullptr;
            INT maxSize = 0;

            for (auto &it : top->child) {
//                top->leaves.insert(it.second->leaves.begin(),it.second->leaves.end()); //merge leaf nodes for all nodes

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


stNode* truncatedSuffixTree::copyAndTruncate(stNode* originalRoot, INT &B) {


    std::stack<stNode *> stack;
    stack.push(originalRoot);
    while (!stack.empty()) {
        stNode *top = stack.top();
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



    stNode* newRoot = new stNode(originalRoot->start, originalRoot->depth, originalRoot->label);
    newRoot->leaves_start_depth = originalRoot->leaves_start_depth;
    // 栈元素包含原始节点、新的节点、从根到该节点的路径长度
    std::stack<pair<stNode*,stNode*>> stackNodes;
    stackNodes.push({originalRoot,newRoot});

    while (!stackNodes.empty()) {
        auto [oldNode, newNode] = stackNodes.top();
        stackNodes.pop();
        // 遍历原始节点的所有子节点
        for ( auto& childPair : oldNode->child) {
            unsigned char charLabel = childPair.first;

            stNode* child = childPair.second;

            //
            if(child->kept_length >= child->depth){

                stNode* newChild = new stNode(child->start, child->depth, charLabel);
                newChild->leaves_start_depth = child->leaves_start_depth;
                // 将新节点添加到新树中
                newNode->addChild(newChild, charLabel);

                // 将子节点压入栈中继续处理
                stackNodes.push({child, newChild});

            } else{

//                assert(oldNode->depth < child->kept_length);
                if(oldNode->depth < child->kept_length){
                    stNode* newChild = new stNode(child->start, child->kept_length, charLabel);
                    newChild->leaves_start_depth = child->leaves_start_depth;

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


//stNode* truncatedSuffixTree::copyAndTruncateB(stNode* originalRoot, INT &B, vector<INT> &hash_positions) {
//
//    stNode* newRoot = new stNode(originalRoot->start, originalRoot->depth, originalRoot->label);
//    // 栈元素包含原始节点、新的节点、从根到该节点的路径长度
//    std::stack<pair<stNode*,stNode*>> stack;
//    stack.push({originalRoot,newRoot});
//
//    while (!stack.empty()) {
//        auto [oldNode, newNode] = stack.top();
//        stack.pop();
//        // 遍历原始节点的所有子节点
//        for ( auto& childPair : oldNode->child) {
//            unsigned char charLabel = childPair.first;
//
//            stNode* child = childPair.second;
//
//            if (charLabel == '#') {
//                // 如果 '#' 是边的标签，不复制这个子树
//                continue;
//            }
//            if (child->depth < B-1 ) {
//
//                stNode* newChild = new stNode(child->start, child->depth, charLabel);
//
//                // 将新节点添加到新树中
//                newNode->addChild(newChild, charLabel);
//
//                // 将子节点压入栈中继续处理
//                stack.push({child, newChild});
//
//            }
////         depth >= B -1
//            if (child->depth > B -2){
//                INT depthValue = B - 1;
//                stNode* newChild = new stNode(child->start, depthValue, charLabel);
//                newNode->addChild(newChild, charLabel);
//            }
//
//
//        }
//    }
//
//    return newRoot;
//}


INT truncatedSuffixTree::find_lower_bound_value(INT &value) {
    auto it = std::lower_bound(hash_positions->begin(), hash_positions->end(), value);

    // return the value of the value
    if (it != hash_positions->end()) {
        return *it;
    }
    return -1;
}

void truncatedSuffixTree::deleteTreeIteratively() {
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


truncatedSuffixTree::~truncatedSuffixTree() {

    deleteTreeIteratively();

}