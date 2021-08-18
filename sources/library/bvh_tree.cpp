#include "bvh_tree.h"
#include "utils.h"
#include "client_module.h"
#include "cvars.h"
#include <cmath>
#include <array>
#include <cfloat>
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
    BuildBottomUp();
    if ((int)ConVars::gsm_debug->value == 2) {
        PrintReport();
    }
}

bool CBVHTree::FindLeaf(const CBoundingBox &box, int &nodeIndex, int &iterCount)
{
    std::stack<int> nodesStack;
    nodesStack.push(m_iRootNodeIndex);
    iterCount = 0;

    while (!nodesStack.empty())
    {
        int currNode = nodesStack.top();
        CBVHTreeNode &node = m_Nodes[currNode];
        const CBoundingBox &nodeBounds = node.GetBoundingBox();
        nodesStack.pop();

        if (nodeBounds.Contains(box))
        {
            if (node.IsLeaf() && node.GetDescriptionIndex() != -1) 
            {
                const vec3_t difference = nodeBounds.GetCenterPoint() - box.GetCenterPoint();
                const vec3_t diffMin = nodeBounds.GetMins() - box.GetMins();
                const vec3_t diffMax = nodeBounds.GetMaxs() - box.GetMaxs();
                if (diffMin.Length() < 0.2f && diffMax.Length() < 0.2f)
                {
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
        ++iterCount;
    }
    return false;
}

double CBVHTree::ComputeCost() const
{
    double cost = 0.0;
    for (size_t i = 0; i < m_Nodes.size(); ++i)
    {
        const CBVHTreeNode &node = m_Nodes[i];
        if (!node.IsLeaf()) {
            cost += node.GetBoundingBox().GetSurfaceArea();
        }
    }
    return cost / 1000.0;
}

void CBVHTree::Visualize(bool textRendering)
{
    if (m_Nodes.size() < 1)
        return;

    std::stack<int> nodesStack;
    nodesStack.push(m_iRootNodeIndex);
    while (!nodesStack.empty())
    {
        CBVHTreeNode &node = m_Nodes[nodesStack.top()];
        const CBoundingBox &nodeBounds = node.GetBoundingBox();
        const int nodeIndex = node.GetIndex();
        nodesStack.pop();

        if (textRendering) {
            Utils::DrawString3D(nodeBounds.GetCenterPoint(), std::to_string(nodeIndex).c_str(), 0, 255, 255);
        }
        else {
            Utils::DrawCuboid(nodeBounds.GetCenterPoint(), vec3_t(0, 0, 0), vec3_t(0, 0, 0), nodeBounds.GetSize(), Color::GetRandom(nodeIndex));
        }
        
        if (node.GetLeftChild() >= 0) {
            nodesStack.push(node.GetLeftChild());
        }
        if (node.GetRightChild() >= 0) {
            nodesStack.push(node.GetRightChild());
        }
    }
}

std::string CBVHTree::GetGraphvisDescription() const
{
    std::string treeDesc;
    std::stack<int> nodesStack;
    treeDesc += "digraph bvh_tree {\n";

    // mark leaf nodes that has linked descriptions as red boxes
    for (size_t i = 0; i < m_Nodes.size(); ++i)
    {
        const CBVHTreeNode &node = m_Nodes[i];
        if (node.IsLeaf()) 
        {
            treeDesc += std::to_string(i) + " [shape=box]";
            if (node.GetDescriptionIndex() != -1) {
                treeDesc += " [color=red]";
            }
            treeDesc += '\n';
        }
    }

    nodesStack.push(m_iRootNodeIndex);
    while (!nodesStack.empty())
    {
        const CBVHTreeNode &node = m_Nodes[nodesStack.top()];
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

void CBVHTree::BuildBottomUp()
{
    std::vector<int> inputNodes;
    std::vector<int> outputNodes;
    std::vector<int> gameObjects = GetGameObjects();

    // create and initialize leaf nodes
    m_Nodes.reserve(gameObjects.size());
    for (int i : gameObjects)
    {
        const CEntityDescription &desc = m_DescList->at(i);
        int newNode = AppendNode(desc.GetBoundingBox());
        NodeAt(newNode).SetDescriptionIndex(i);
        inputNodes.push_back(newNode);
    }

    while (inputNodes.size() > 0)
    {
        if (inputNodes.size() == 1)
        {
            m_iRootNodeIndex = inputNodes[0];
            return;
        }
        MergeLevelNodes(inputNodes, outputNodes);
        inputNodes = outputNodes;
    }
}

void CBVHTree::MergeLevelNodes(const std::vector<int> &inputNodes, std::vector<int> &outputNodes)
{
    outputNodes.clear();
    for (size_t i = 0; i < inputNodes.size(); ++i)
    {
        int parentIndex;
        CBVHTreeNode &node = NodeAt(inputNodes[i]);
        if (node.GetParent() != -1)
            continue;

        // find best sibling
        double minSurfaceArea = DBL_MAX;
        int siblingIndex = -1;
        for (size_t j = 0; j < inputNodes.size(); ++j)
        {
            CBVHTreeNode &siblingNode = NodeAt(inputNodes[j]);
            if (i != j && siblingNode.GetParent() == -1)
            {
                const CBoundingBox &nodeBounds = node.GetBoundingBox();
                const CBoundingBox &siblingBounds = siblingNode.GetBoundingBox();
                double surfaceArea = nodeBounds.GetUnion(siblingBounds).GetSurfaceArea();
                if (surfaceArea < minSurfaceArea)
                {
                    minSurfaceArea = surfaceArea;
                    siblingIndex = j;
                }
            }
        }

        if (siblingIndex != -1)
        {
            int leftNodeIndex = inputNodes[i];
            int rightNodeIndex = inputNodes[siblingIndex];
            CBoundingBox leftNodeBounds = NodeAt(leftNodeIndex).GetBoundingBox();
            CBoundingBox rightNodeBounds = NodeAt(rightNodeIndex).GetBoundingBox();
            CBoundingBox parentNodeBounds = leftNodeBounds.GetUnion(rightNodeBounds);
            parentIndex = AppendNode(parentNodeBounds);
            NodeAt(parentIndex).SetLeftChild(leftNodeIndex);
            NodeAt(parentIndex).SetRightChild(rightNodeIndex);
            NodeAt(leftNodeIndex).SetParent(parentIndex);
            NodeAt(rightNodeIndex).SetParent(parentIndex);
        }
        else 
        {
            // if siblingIndex == -1 means that there is no free sibling nodes left, and we should 
            // create parent node only with current node sibling, because we haven't second sibling lol
            // anyway this node will be merged somewhere on top levels
            int leftNodeIndex = inputNodes[i];
            parentIndex = AppendNode(NodeAt(leftNodeIndex).GetBoundingBox());
            NodeAt(parentIndex).SetLeftChild(leftNodeIndex);
            NodeAt(leftNodeIndex).SetParent(parentIndex);
        }
        outputNodes.push_back(parentIndex);
    }
}

void CBVHTree::PrintReport()
{
    std::string line;
    std::string treeDesc = GetGraphvisDescription();
    for (size_t offset = 0; offset < treeDesc.size(); offset += 500)
    {
        line.clear();
        line.assign(treeDesc, offset, 500);
        g_pClientEngfuncs->Con_Printf(line.c_str());
    }
    g_pClientEngfuncs->Con_Printf("BVH nodes: %d\nBVH tree cost: %f\n", m_Nodes.size(), ComputeCost());
}

// Not suitable for game objects, tree doesn't covers all game objects with leaf nodes
void CBVHTree::BuildTopDown()
{
    std::vector<int> rootNodeObjects = GetGameObjects();
    int rootNode = AppendNode(CalcNodeBoundingBox(rootNodeObjects));
    m_Nodes.reserve(rootNodeObjects.size());
    m_iRootNodeIndex = rootNode;

    m_NodesStack.push(m_iRootNodeIndex);
    m_ObjectListStack.push(rootNodeObjects);
    while (!m_NodesStack.empty())
    {
        CBVHTreeNode &node = m_Nodes[m_NodesStack.top()];
        ObjectList nodeObjects = m_ObjectListStack.top();
        m_NodesStack.pop();
        m_ObjectListStack.pop();
        SplitNode(node, nodeObjects);
    }
}

void CBVHTree::SplitNode(CBVHTreeNode &node, ObjectList nodeObjects)
{
    const int count = nodeObjects.size();
    if (count == 1)
    {
        node.SetDescriptionIndex(nodeObjects[0]); 
        return;
    }

    int nodeIndex = node.GetIndex();
    const CBoundingBox &nodeBounds = node.GetBoundingBox();
    vec3_t nodeCenter = nodeBounds.GetCenterPoint();
    static vec3_t axisLengths = nodeBounds.GetSize();
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
    bool splitFailed[3] = { false };
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
        bool operator()(int a, int b) const { return axisLengths[a] > axisLengths[b]; }
    } sortFunc;
    std::sort(splitOrder.begin(), splitOrder.end(), sortFunc);

    std::vector<int> *leftElements = nullptr;
    std::vector<int> *rightElements = nullptr;
    for (int i = 0; i < 3; ++i)
    {
        int j = splitOrder[i];
        if (!splitFailed[j]) 
        {
            leftElements = &leftNodeElements[j];
            rightElements = &rightNodeElements[j];
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

std::vector<int> CBVHTree::GetGameObjects()
{
    // skip worldspawn here
    std::vector<int> objectList;
    for (size_t i = 1; i < m_DescList->size(); ++i) {
        objectList.push_back(i);
    }
    return objectList;
}
