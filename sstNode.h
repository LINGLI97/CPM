
#ifndef CPM_SSTNODE_H
#define CPM_SSTNODE_H


#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <unordered_set>
using namespace std;
#ifdef _USE_64
typedef int64_t INT;
#endif

#ifdef _USE_32
typedef int32_t INT;
#endif
class sstNode
{
public:
    INT start;
    INT depth;
    unsigned char label; // label is the label of edge which links its parent and itself
    INT preorderId;

    sstNode * parent;


    sstNode();
    sstNode(unsigned char terminate_label);
    sstNode( INT i, INT d, unsigned char l );

    void setDepth( INT d);


//DFS
    bool heavy;
    bool visited;
    INT sizeSubtree;
    sstNode * heavyLeaf;
    unordered_set<sstNode*> leaves;

//    bool leftDiverse;
//    stNode* LCA;





    map<unsigned char, sstNode*> child;


    sstNode * getChild( unsigned char l );

//    void setSLink( sstNode * slinkNode );
    void addChild( sstNode * childNode,  unsigned char l );

    void setParent ( sstNode * parentNode);
    INT numChild();

    std::vector<sstNode*> allChild();


    ~sstNode();
};






#endif
