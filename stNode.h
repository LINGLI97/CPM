
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
    INT start;
    INT depth;
    unsigned char label; // label is the label of edge which links its parent and itself
    INT preorderId;

    stNode * parent;
    stNode * slink;

    stNode();
    stNode(unsigned char &terminate_label);
    stNode( INT& i, INT &d, unsigned char &l );

    void setDepth( INT d);

#ifdef USE_KEPT_LENGTH
    INT kept_length;
#endif

//DFS
    bool heavy;
    bool visited;
    INT sizeSubtree;
    stNode * heavyLeaf;
    vector<pair<INT, INT>> leaves_start_depth;

//    bool leftDiverse;
//    stNode* LCA;





    map<unsigned char, stNode*> child;


    stNode * getChild( unsigned char l );

    void setSLink( stNode * slinkNode );
    void addChild( stNode * childNode,  unsigned char &l );

    void setParent ( stNode * parentNode);
    INT numChild();

    std::vector<stNode*> allChild();


    ~stNode();
};





#endif
