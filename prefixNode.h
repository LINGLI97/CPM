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
    int start;
    int depth;
    unsigned char label; // label is the label of edge which links its parent and itself

    int phi;
    pfNode * parent;

    pfNode();
    pfNode( int i, int d, unsigned char l );

    void setDepth( int d);

    bool visited;


    unordered_map<unsigned char, pfNode*> child;


    pfNode * getChild( unsigned char l );

//    void setSLink( pfNode * slinkNode );
    void addChild( pfNode * childNode,  unsigned char l );

    void setParent ( pfNode * parentNode);
    int numChild();

    std::vector<pfNode*> allChild();

    ~pfNode();
};




#endif