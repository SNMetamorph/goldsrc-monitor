/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

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
    bool FindLeaf(const CBoundingBox &box, int &nodeIndex, int &iterCount);
    void Visualize(bool textRendering);
    const CBVHTreeNode &GetNode(int index) const { return m_nodes[index]; }

private:
    typedef std::vector<int> ObjectList;
    CBVHTreeNode &NodeAt(int index) { return m_nodes[index]; }

    void PrintReport();
    void BuildTopDown();
    void BuildBottomUp();
    void MergeLevelNodes(const std::vector<int> &inputNodes, std::vector<int> &outputNodes);
    void SplitNode(CBVHTreeNode &node, ObjectList nodeObjects);
    int AppendNode(const CBoundingBox &nodeBounds, int parent = -1);
    CBoundingBox CalcNodeBoundingBox(ObjectList, float epsilon = 0.001f);
    std::string GetGraphvisDescription() const;
    std::vector<int> GetGameObjects();

    int                       m_rootNodeIndex = -1;
    std::stack<int>           m_nodesStack;
    std::stack<ObjectList>    m_objectListStack;
    std::vector<CBVHTreeNode> m_nodes;
    const std::vector<CEntityDescription> *m_descList;
};
