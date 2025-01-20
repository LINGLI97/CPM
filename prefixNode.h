#ifndef CPM_PREFIXNODE_H
#define CPM_PREFIXNODE_H


#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
using namespace std;
#ifdef _USE_64
typedef int64_t INT;
#endif

#ifdef _USE_32
typedef int32_t INT;

#endif
class pfNode
{
public:
    INT start;
    INT depth;
    unsigned char label; // label is the label of edge which links its parent and itself

    INT phi;
    pfNode * parent;

    pfNode();
    pfNode( INT i, INT d, unsigned char l );

    void setDepth( INT d);

    bool visited;

#ifdef USE_KEPT_LENGTH
    INT kept_length;
#endif

    unordered_map<unsigned char, pfNode*> child;


    pfNode * getChild( unsigned char l );

//    void setSLink( pfNode * slinkNode );
    void addChild( pfNode * childNode,  unsigned char l );

    void setParent ( pfNode * parentNode);
    INT numChild();

    std::vector<pfNode*> allChild();

    ~pfNode();
};




#endif