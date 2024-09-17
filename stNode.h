
#ifndef CPM_STNODE_H
#define CPM_STNODE_H

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
class stNode
{
public:
    int start;
    int depth;
    unsigned char label; // label is the label of edge which links its parent and itself
    int preorderId;

    stNode * parent;
    stNode * slink;

    stNode();
    explicit stNode(unsigned char terminate_label);
    stNode( int i, int d, unsigned char l );

    void setDepth( int d);


//DFS
    bool heavy;
    bool visited;
    int sizeSubtree;
    stNode * heavyLeaf;
    unordered_set<stNode*> leaves;

//    bool leftDiverse;
//    stNode* LCA;





    map<unsigned char, stNode*> child;


    stNode * getChild( unsigned char l );

    void setSLink( stNode * slinkNode );
    void addChild( stNode * childNode,  unsigned char l );

    void setParent ( stNode * parentNode);
    int numChild();

    std::vector<stNode*> allChild();


    ~stNode();
};





#endif
