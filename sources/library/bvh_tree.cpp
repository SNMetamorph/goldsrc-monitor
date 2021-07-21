#include "bvh_tree.h"
#include "utils.h"
#include "client_module.h"
#include <cmath>
#include <array>
#include <algorithm>

CBVHTree::CBVHTree(const std::vector<CEntityDescription> *descList)
{
    m_DescList = descList;
}

void CBVHTree::Reset()
{
    m_Nodes.clear();
    m_iRootNodeIndex = -1;
}

void CBVHTree::Build()
{
    CBVHTreeNode rootNode = AppendRootNode(GetRootBoundingBox());
    std::vector<int> rootNodeObjects = GetRootObjectList();
    m_Nodes.reserve(rootNodeObjects.size());
    m_iRootNodeIndex = rootNode.GetIndex();

    m_NodesStack.push(m_iRootNodeIndex);
    m_ObjectListStack.push(rootNodeObjects);
    while (!m_NodesStack.empty())
    {
        CBVHTreeNode &node = m_Nodes[m_NodesStack.top()];
        m_NodesStack.pop();
        ObjectList nodeObjects = m_ObjectListStack.top();
        m_ObjectListStack.pop();
        SplitNode(node, nodeObjects);
    }
    g_pClientEngfuncs->Con_Printf(GetGraphvisDescription().c_str());
}

bool CBVHTree::FindLeaf(const CBoundingBox &box, int &nodeIndex)
{
    std::stack<int> nodesStack;
    nodesStack.push(m_iRootNodeIndex);
    while (!nodesStack.empty())
    {
        int currNode = nodesStack.top();
        CBVHTreeNode &node = m_Nodes[currNode];
        const CBoundingBox &nodeBounds = node.GetBoundingBox();
        nodesStack.pop();
        if (nodeBounds.ContainsPoint(box.GetMins()) && nodeBounds.ContainsPoint(box.GetMaxs()))
        {
            if (node.IsLeaf() && node.GetDescriptionIndex() > 0) {
                const vec3_t diffMin = nodeBounds.GetMins() - box.GetMins();
                const vec3_t diffMax = nodeBounds.GetMaxs() - box.GetMaxs();
                if (diffMin.Length() < 1.f && diffMax.Length() < 1.f) {
                    nodeIndex = currNode;
                    return true;
                }
            }

            int leftChild = node.GetLeftChild();
            int rightChild = node.GetRightChild();
            if (leftChild >= 0) {
                nodesStack.push(leftChild);
            }
            if (rightChild >= 0) {
                nodesStack.push(rightChild);
            }
        }
    }
    return false;
}

void CBVHTree::Visualize(bool textRendering)
{
    if (!IsBuilt())
        return;

    std::stack<int> nodesStack;
    nodesStack.push(m_iRootNodeIndex);
    while (!nodesStack.empty())
    {
        CBVHTreeNode &node = m_Nodes[nodesStack.top()];
        const CBoundingBox &nodeBounds = node.GetBoundingBox();
        nodesStack.pop();

        if (node.GetIndex() == (int)g_pClientEngfuncs->GetClientTime() % m_Nodes.size())
        {
            if (textRendering) {
                Utils::DrawString3D(nodeBounds.GetCenterPoint(), std::to_string(node.GetIndex()).c_str(), 0, 255, 255);
            }
            else {
                Utils::DrawEntityHull(nodeBounds.GetCenterPoint(), vec3_t(0, 0, 0), vec3_t(0, 0, 0), nodeBounds.GetSize());
            }
        }

        if (node.GetLeftChild() >= 0) {
            nodesStack.push(node.GetLeftChild());
        }
        if (node.GetRightChild() >= 0) {
            nodesStack.push(node.GetRightChild());
        }
    }
}

std::string CBVHTree::GetGraphvisDescription()
{
    std::string treeDesc;
    std::stack<int> nodesStack;
    treeDesc += "digraph bvh_tree {\n";

    // mark leaf nodes that has linked descriptions as red boxes
    for (int i = 0; i < m_Nodes.size(); ++i)
    {
        CBVHTreeNode &node = m_Nodes[i];
        if (node.IsLeaf()) 
        {
            treeDesc += std::to_string(i) + " [shape=box]";
            if (node.GetDescriptionIndex() >= 0) {
                treeDesc += " [color=red]";
            }
            treeDesc += '\n';
        }
    }

    nodesStack.push(m_iRootNodeIndex);
    while (!nodesStack.empty())
    {
        CBVHTreeNode &node = m_Nodes[nodesStack.top()];
        int leftChild = node.GetLeftChild();
        int rightChild = node.GetRightChild();
        nodesStack.pop();
        if (leftChild >= 0) {
            nodesStack.push(leftChild);
            treeDesc += std::to_string(node.GetIndex()) + " -> " + std::to_string(leftChild) + "\n";
        }
        if (rightChild >= 0) {
            nodesStack.push(rightChild);
            treeDesc += std::to_string(node.GetIndex()) + " -> " + std::to_string(rightChild) + "\n";
        }
    }
    treeDesc += "}\n";
    return treeDesc;
}

void CBVHTree::SplitNode(CBVHTreeNode &node, ObjectList nodeObjects)
{
    if (nodeObjects.size() == 1) {
        node.SetDescriptionIndex(nodeObjects[0]);
        return;
    }

    int nodeIndex = node.GetIndex();
    const CBoundingBox &nodeBounds = node.GetBoundingBox();
    vec3_t nodeCenter = nodeBounds.GetCenterPoint();
    static const vec3_t &axisLengths = nodeBounds.GetSize();
    std::vector<int> leftNodeElements[3];
    std::vector<int> rightNodeElements[3];

    for (int i : nodeObjects)
    {
        const CEntityDescription &desc = m_DescList->at(i);
        vec3_t objectCenter = desc.GetBoundingBox().GetCenterPoint();
        for (int j = 0; j < 3; ++j) 
        {
            if (objectCenter[j] < nodeCenter[j]) {
                leftNodeElements[j].push_back(i);
            }
            else {
                rightNodeElements[j].push_back(i);
            }
        }
    }

    // check is it possible to split by any of axes
    bool splitFailed[3];
    for (int i = 0; i < 3; ++i) {
        splitFailed[i] = (leftNodeElements[i].size() == 0) || (rightNodeElements[i].size() == 0);
    }

    // if it isn't possible, just stop because it'll cause infinite loop
    if (splitFailed[0] && splitFailed[1] && splitFailed[2]) {
        return;
    }

    // sort axes accoring to it lengths
    std::array<int, 3> splitOrder = { 0, 1, 2 };
    struct {
        bool operator()(int a, int b) const { return axisLengths[a] < axisLengths[b]; }
    } sortFunc;
    std::sort(splitOrder.begin(), splitOrder.end(), sortFunc);

    std::vector<int> *leftElements = nullptr;
    std::vector<int> *rightElements = nullptr;
    for (int i = 0; i < 3; ++i)
    {
        if (!splitFailed[i]) 
        {
            leftElements = &leftNodeElements[i];
            rightElements = &rightNodeElements[i];
            break;
        }
    }

    int leftNode = AppendNode(CalcNodeBoundingBox(*leftElements), nodeIndex);
    int rightNode = AppendNode(CalcNodeBoundingBox(*rightElements), nodeIndex);

    m_NodesStack.push(leftNode);
    m_NodesStack.push(rightNode);
    m_ObjectListStack.push(*leftElements);
    m_ObjectListStack.push(*rightElements);
}

// Returns appended node index
int CBVHTree::AppendNode(const CBoundingBox &nodeBounds, int parent)
{
    if (parent >= 0) {
        m_Nodes.emplace_back(CBVHTreeNode(m_Nodes.size(), nodeBounds, m_Nodes[parent]));
    }
    else {
        m_Nodes.emplace_back(CBVHTreeNode(m_Nodes.size(), nodeBounds));
    }
    return m_Nodes.size() - 1;
}

CBVHTreeNode &CBVHTree::AppendRootNode(const CBoundingBox &rootNodeBox)
{
    return m_Nodes.emplace_back(CBVHTreeNode(m_Nodes.size(), rootNodeBox));
}

CBoundingBox CBVHTree::CalcNodeBoundingBox(ObjectList nodeObjects, float epsilon)
{
    CBoundingBox nodeBounds;
    const vec3_t fudge = vec3_t(epsilon, epsilon, epsilon);
    for (int i : nodeObjects)
    {
        const CEntityDescription &desc = m_DescList->at(i);
        nodeBounds.CombineWith(desc.GetBoundingBox());
    }
    return CBoundingBox(nodeBounds.GetMins() - fudge, nodeBounds.GetMaxs() + fudge);
}

CBoundingBox CBVHTree::GetRootBoundingBox()
{
    CBoundingBox rootNodeBox;
    for (const CEntityDescription &desc : *m_DescList) {
        rootNodeBox.CombineWith(desc.GetBoundingBox());
    }
    return rootNodeBox;
}

std::vector<int> CBVHTree::GetRootObjectList()
{
    std::vector<int> objectList;
    for (size_t i = 0; i < m_DescList->size(); ++i) {
        objectList.push_back(i);
    }
    return objectList;
}
