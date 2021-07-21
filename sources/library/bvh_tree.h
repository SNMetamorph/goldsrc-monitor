#pragma once
#include "bounding_box.h"
#include "bvh_tree_node.h"
#include "entity_description.h"
#include <vector>
#include <stack>
#include <string>

class CBVHTree
{
public:
    CBVHTree(const std::vector<CEntityDescription> *descList);
    void Reset();
    void Build();
    bool FindLeaf(const CBoundingBox &box, int &nodeIndex);
    void Visualize(bool textRendering);
    std::string GetGraphvisDescription();
    inline const CBVHTreeNode &GetNode(int index) const { return m_Nodes[index]; }

private:
    typedef std::vector<int> ObjectList;

    inline bool IsBuilt() const { return m_Nodes.size() > 0; };
    void SplitNode(CBVHTreeNode &node, ObjectList nodeObjects);
    int AppendNode(const CBoundingBox &nodeBounds, int parent = -1);
    CBVHTreeNode &AppendRootNode(const CBoundingBox &rootNodeBox);
    CBoundingBox CalcNodeBoundingBox(ObjectList, float epsilon = 0.00001f);
    CBoundingBox GetRootBoundingBox();
    std::vector<int> GetRootObjectList();

    int                       m_iRootNodeIndex = -1;
    std::stack<int>           m_NodesStack;
    std::stack<ObjectList>    m_ObjectListStack;
    std::vector<CBVHTreeNode> m_Nodes;
    const std::vector<CEntityDescription> *m_DescList;
};
