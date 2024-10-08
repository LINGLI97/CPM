#ifndef CPM_SPARSEPREFIXNODE_H
#define CPM_SPARSEPREFIXNODE_H

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

class spfNode
{
public:
    INT start;
    INT depth;
    unsigned char label;

    INT phi;
    spfNode *parent;

    spfNode();
    spfNode(INT i, INT d, unsigned char l);

    void setDepth(INT d);

    bool visited;

    unordered_map<unsigned char, spfNode*> child;

    spfNode *getChild(unsigned char l);

    void addChild(spfNode *childNode, unsigned char l);

    void setParent(spfNode *parentNode);
    INT numChild();

    std::vector<spfNode*> allChild();

    ~spfNode();
};

#endif
