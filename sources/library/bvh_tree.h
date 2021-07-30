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
    double ComputeCost() const;
    bool FindLeaf(const CBoundingBox &box, int &nodeIndex);
    void Visualize(bool textRendering);
    inline const CBVHTreeNode &GetNode(int index) const { return m_Nodes[index]; }

private:
    typedef std::vector<int> ObjectList;
    inline CBVHTreeNode &NodeAt(int index) { return m_Nodes[index]; }

    void PrintReport();
    void BuildTopDown();
    void BuildBottomUp();
    std::vector<int> MergeLevelNodes(std::vector<int> &levelNodes);
    void SplitNode(CBVHTreeNode &node, ObjectList nodeObjects);
    int AppendNode(const CBoundingBox &nodeBounds, int parent = -1);
    CBoundingBox CalcNodeBoundingBox(ObjectList, float epsilon = 0.001f);
    std::string GetGraphvisDescription() const;
    std::vector<int> GetGameObjects();

    int                       m_iRootNodeIndex = -1;
    std::stack<int>           m_NodesStack;
    std::stack<ObjectList>    m_ObjectListStack;
    std::vector<CBVHTreeNode> m_Nodes;
    const std::vector<CEntityDescription> *m_DescList;
};
